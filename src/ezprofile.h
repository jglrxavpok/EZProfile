//
// Created by jglrxavpok on 12/12/18.
//

#ifndef EZPROFILE_EZPROFILE_H
#define EZPROFILE_EZPROFILE_H

#include "hashmap/hashmap.h"
#include <stdint.h>
#include "datastructures.h"
#include "arguments.h"

/**
 * Represents the data of a single function in a single thread
 */
struct function_data {
    /**
     * OTF2 name of the function
     */
    OTF2_RegionRef reference;

    /**
     * The time spent in this function, including sub-function calls
     */
    uint64_t inclusive_time_spent;
    uint64_t min_inclusive_time;
    uint64_t max_inclusive_time;
    uint64_t call_count;
    double average_inclusive_time;
    /**
     * The time spent in this function, excluding sub-function calls
     */
    uint64_t exclusive_time_spent;

    stack* entry_times;
    stack* exclusive_entry_times;
    uint64_t last_entry_time_exclusive;
} typedef function_data;

/**
 * Represents the data of a single thread
 */
struct thread_data {
    struct hashmap* function_data;

    function_data* current_function;
    stack* callstack;
} typedef thread_data;

/**
 * Represents the data of the entire program
 */
struct ezdata {
    struct hashmap* thread_names;
    struct hashmap* region_names;
    struct hashmap* strings;
    struct vector* locations;
    thread_data** threads;
    struct hashmap* thread_indices;
    size_t thread_count;
    size_t last_thread_index;

    computation_mode computation_mode;

    // filtering options
    char* thread_name;
    int thread_index;
} typedef ezdata;

// ezdata
ezdata* NewEZData(size_t thread_count, computation_mode computation_mode, int thread_index, char* thread_name);

char* RetrieveRegionName(ezdata* data, OTF2_RegionRef region);

char* RetrieveThreadName(ezdata *data, OTF2_LocationRef location);

OTF2_LocationRef RetrieveLocation(ezdata* data, size_t index);

thread_data* GetThreadData(ezdata* data, OTF2_LocationRef thread);

void PrintEZResults(ezdata* data);

// thread_data
thread_data* NewThreadData();

function_data* GetOrCreateFunctionData(thread_data* thread, OTF2_RegionRef function);
/**
 * Used to compute time spent in a function
 */
void EnterThreadFunction(ezdata* data, thread_data* thread, OTF2_RegionRef function, OTF2_TimeStamp time);
void ExitThreadFunction(ezdata* data, thread_data* thread, OTF2_RegionRef function, OTF2_TimeStamp time);

// function_data
void EnterFunction(function_data* function, OTF2_TimeStamp time, bool subcall);
void ExitFunction(function_data* function, OTF2_TimeStamp time, bool subcall);

#endif //EZPROFILE_EZPROFILE_H
