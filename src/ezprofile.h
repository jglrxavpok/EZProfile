//
// Created by jglrxavpok on 12/12/18.
//

#ifndef EZPROFILE_EZPROFILE_H
#define EZPROFILE_EZPROFILE_H

#include "hashmap/hashmap.h"
#include <stdint.h>
#include "datastructures.h"
/**
 * Represents how ezprofile should compile results: time spent in functions *including* in sub-routines or *excluding* ?
 */
enum computation_mode {
    INCLUSIVE,
    EXCLUSIVE,
} typedef computation_mode;

/**
 * Represents the data of a single thread
 */
struct thread_data {

} typedef thread_data;

/**
 * Represents the data of the entire program
 */
struct ezdata {
    struct hashmap* locations_names;
    struct hashmap* region_names;
    struct hashmap* strings;
    struct vector* locations;
    computation_mode mode;
    thread_data* threads;
    uint8_t thread_count;
} typedef ezdata;

ezdata* NewEZData(computation_mode mode, uint8_t thread_count, size_t location_count);

char* RetrieveRegionName(ezdata* data, OTF2_RegionRef region);

char* RetrieveLocationName(ezdata* data, OTF2_LocationRef location);

OTF2_LocationRef RetrieveLocation(ezdata* data, size_t index);
#endif //EZPROFILE_EZPROFILE_H
