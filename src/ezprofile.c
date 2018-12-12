//
// Created by jglrxavpok on 12/12/18.
//

#include <otf2/otf2.h>
#include <stdio.h>
#include "ezprofile.h"

ezdata* NewEZData(computation_mode mode, uint8_t thread_count, size_t location_count) {
    ezdata* data = calloc(1, sizeof(ezdata));
    data->thread_count = thread_count;
    data->mode = mode;

    data->strings = hash_init();
    data->region_names = hash_init();
    data->locations_names = hash_init();
    data->locations = malloc(sizeof(*data->locations) + location_count*sizeof(*data->locations->members));
    data->locations->size = 0;
    data->locations->capacity = location_count;

    data->threads = calloc(thread_count, sizeof(thread_data));
    return data;
}

char* RetrieveRegionName(ezdata* data, OTF2_RegionRef region) {
    return hash_get(data->strings, (size_t) hash_get(data->region_names, region));
}

char* RetrieveLocationName(ezdata* data, OTF2_LocationRef location) {
    return hash_get(data->strings, (size_t) hash_get(data->locations_names, location));
}

OTF2_LocationRef RetrieveLocation(ezdata* data, size_t index) {
    return (OTF2_LocationRef) data->locations->members[index];
}