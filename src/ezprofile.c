//
// Created by jglrxavpok on 12/12/18.
//

#include <otf2/otf2.h>
#include <stdio.h>
#include "hashmap/hashmap.h"
#include "ezprofile.h"

ezdata* NewEZData(size_t thread_count) {
    ezdata* data = calloc(1, sizeof(ezdata));
    data->thread_count = thread_count;

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

void PrintEZResults(ezdata* data) {
    printf("Profile:\n");
    for(size_t thread_index = 0;thread_index<data->thread_count;thread_index++) {
        printf("Profile for thread #%li (%s)\n", thread_index, RetrieveThreadName(data, thread_index));
        printf("%-20s | %-20s | %-20s\n", "Function name", "Inclusive time (s)", "Exclusive time(s)");
        thread_data* thread = GetThreadData(data, thread_index);
        size_t function_count;
        function_data*** functions = (function_data*** ) hash_flat(thread->function_data, sizeof(function_data), &function_count);
        for(size_t func_index = 0;func_index<function_count;func_index++) {
            function_data* function = *functions[func_index*sizeof(function_data)];
            if(function == NULL)
                continue;
            printf("%-20s | %-20f | %-20f (%li)\n",
                    (const char*)RetrieveRegionName(data, function->reference), function->inclusive_time_spent/1000000.0, function->exclusive_time_spent/1000000.0, function->reference);
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
    if(subcall) {
        function->last_entry_time_exclusive = time;
    } else {
        function->last_entry_time_inclusive = time;
    }
}

void ExitFunction(function_data* function, OTF2_TimeStamp time, bool subcall) {
    OTF2_TimeStamp start;
    if(!subcall) {
        start = function->last_entry_time_inclusive;
        uint64_t dt = time-start;
        function->inclusive_time_spent += dt;
        function->last_entry_time_inclusive = time;
    }
    start = function->last_entry_time_exclusive;
    uint64_t dt = time-start;
    function->exclusive_time_spent += dt;
    function->last_entry_time_exclusive = time;
}
