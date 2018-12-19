//
// Created by jglrxavpok on 12/12/18.
//

#include <otf2/otf2.h>
#include <stdio.h>
#include <string.h>
#include "hashmap/hashmap.h"
#include "ezprofile.h"

ezdata* NewEZData(size_t thread_count, computation_mode computation_mode, int thread_index, char* thread_name) {
    ezdata* data = calloc(1, sizeof(ezdata));
    data->thread_index = thread_index;
    data->thread_name = thread_name;
    data->thread_count = thread_count;
    data->computation_mode = computation_mode;

    data->strings = hash_init();
    data->region_names = hash_init();
    data->thread_names = hash_init();
    data->thread_indices = hash_init();
    data->locations = malloc(sizeof(*data->locations) + thread_count*sizeof(*data->locations->members));
    data->locations->size = 0;
    data->locations->capacity = thread_count;

    data->threads = calloc(thread_count, sizeof(thread_data));
    for(size_t i = 0;i<thread_count;i++) {
        data->threads[i] = NewThreadData();
    }
    return data;
}

char* RetrieveRegionName(ezdata* data, OTF2_RegionRef region) {
    return hash_get(data->strings, (size_t) hash_get(data->region_names, region));
}

char* RetrieveThreadName(ezdata *data, OTF2_LocationRef location) {
    return hash_get(data->strings, (size_t) hash_get(data->thread_names, location));
}

OTF2_LocationRef RetrieveLocation(ezdata* data, size_t index) {
    return (OTF2_LocationRef) data->locations->members[index];
}

void printThreadHeader(ezdata* data, thread_data* thread) {
    printf("%-20s\t%-20s",
            "Function name", "Call count");
    if(data->computation_mode == both || data->computation_mode == exclusive) {
        printf("\t%-20s", "Exclusive time (s)");
    }
    if(data->computation_mode == both || data->computation_mode == inclusive) {
        printf("\t%-20s\t%-20s\t%-20s\t%-20s", "Inclusive time (s)", "Min inclusive time", "Max inclusive time", "Avg inclusive time");
    }
    printf("\n");
}

void printFunctionData(ezdata* data, function_data* function) {
    printf("%-20s\t%-20li", (const char*)RetrieveRegionName(data, function->reference), function->call_count);
    if(data->computation_mode == both || data->computation_mode == exclusive) {
        printf("\t%-20f", function->exclusive_time_spent/1000000.0);
    }
    if(data->computation_mode == both || data->computation_mode == inclusive) {
        printf("\t%-20f\t%-20f\t%-20f\t%-20f",
                function->inclusive_time_spent/1000000.0,
                function->min_inclusive_time/1000000.0, function->max_inclusive_time/1000000.0, function->average_inclusive_time/1000000.0);
    }
    printf("\n");
}

void PrintEZResults(ezdata* data) {
    printf("Profile:\n");
    for(size_t thread_index = 0;thread_index<data->locations->size;thread_index++) {
        if(data->thread_index >= 0) {
            if(data->thread_index != thread_index)
                continue;
        }
        OTF2_LocationRef thread_ref = data->locations->members[thread_index];
        thread_data* thread = GetThreadData(data, thread_ref);
        if(data->thread_name) {
            if(strcmp(data->thread_name, RetrieveThreadName(data, thread_ref)) != 0) {
                continue;
            }
        }
        printf("=== Profile for thread #%li (%s) (%li) ===\n", thread_index, RetrieveThreadName(data, thread_ref), thread_ref);
        printThreadHeader(data, thread);
        size_t function_count;
        function_data*** functions = (function_data*** ) hash_flat(thread->function_data, sizeof(function_data), &function_count);
        for(size_t func_index = 0;func_index<function_count;func_index++) {
            function_data* function = *functions[func_index*sizeof(function_data)];
            if(function == NULL)
                continue;
            printFunctionData(data, function);
        }
        printf("\n");
    }
}

thread_data* GetThreadData(ezdata* data, OTF2_LocationRef thread) {
    size_t* indexPtr = hash_get(data->thread_indices, thread);
    if(indexPtr == NULL) {
        size_t* new_index = malloc(sizeof(size_t));
        *new_index = data->last_thread_index++;
        hash_put(data->thread_indices, thread, new_index);
        indexPtr = new_index;
    }
    return data->threads[*indexPtr];
}

// thread_data
thread_data* NewThreadData() {
    thread_data* data = calloc(1, sizeof(thread_data));
    data->function_data = hash_init();
    data->callstack = NewStack();
    return data;
}

function_data* GetOrCreateFunctionData(thread_data* thread, OTF2_RegionRef function) {
    function_data* data = hash_get(thread->function_data, function);
    if(data == NULL) {
        data = calloc(1, sizeof(function_data));
        data->reference = function;
        data->entry_times = NewStack();
        data->exclusive_entry_times = NewStack();
        data->min_inclusive_time = UINT64_MAX;
        hash_put(thread->function_data, function, data);
    }
    return data;
}

/**
 * Used to compute time spent in a function
 */

void EnterThreadFunction(ezdata* data, thread_data* thread, OTF2_RegionRef function, OTF2_TimeStamp time) {
    function_data* function_data = GetOrCreateFunctionData(thread, function);
    EnterFunction(function_data, time, false);
    if(thread->current_function != NULL) {
        ExitFunction(thread->current_function, time, true); // subcall
    }
    PushStack(thread->callstack, function_data);
    thread->current_function = function_data;
}

void ExitThreadFunction(ezdata* data, thread_data* thread, OTF2_RegionRef function, OTF2_TimeStamp time) {
    function_data* function_data = GetOrCreateFunctionData(thread, function);
    ExitFunction(function_data, time, false); // actual exit
    PopStack(thread->callstack);
    thread->current_function = PeekStack(thread->callstack);
    if(thread->current_function != NULL) {
        EnterFunction(thread->current_function, time, true);
    }
}

// function_data
void EnterFunction(function_data* function, OTF2_TimeStamp time, bool subcall) {
    OTF2_TimeStamp* start = malloc(sizeof(OTF2_TimeStamp));
    *start = time;
    PushStack(function->exclusive_entry_times, start);
    if(!subcall) {
        PushStack(function->entry_times, start);
    }
}

void ExitFunction(function_data* function, OTF2_TimeStamp time, bool subcall) {
    if(!subcall) {
        OTF2_TimeStamp start = * (OTF2_TimeStamp*)PopStack(function->entry_times);
        OTF2_TimeStamp elapsed_time = time-start;
        if(elapsed_time < function->min_inclusive_time) {
            function->min_inclusive_time = elapsed_time;
        }
        if(elapsed_time > function->max_inclusive_time) {
            function->max_inclusive_time = elapsed_time;
        }
        uint64_t n = function->call_count;
        if(n == 0) {
            function->average_inclusive_time = elapsed_time;
        } else {
            // recompute average with previous average
            function->average_inclusive_time = (function->average_inclusive_time * n + elapsed_time) / (double)(n+1);
        }
        function->call_count++;
        function->inclusive_time_spent += elapsed_time;
    }

    OTF2_TimeStamp entry_time = *(OTF2_TimeStamp *)PopStack(function->exclusive_entry_times);
    function->exclusive_time_spent += time-entry_time;
}
