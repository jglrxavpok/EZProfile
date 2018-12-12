//
// Created by jglrxavpok on 12/12/18.
//
#include "callbacks.h"
#include "datastructures.h"
#include <inttypes.h>
#include <stdio.h>

OTF2_CallbackCode
RegionEntryCallback(OTF2_LocationRef location,
                    OTF2_TimeStamp time,
                    void *userData,
                    OTF2_AttributeList *attributes,
                    OTF2_RegionRef region) {
    struct vector *strings = ((struct user_data *) userData)->strings;
    struct vector *names = ((struct user_data *) userData)->names;
    printf("Entering region %u at location %" PRIu64 " at time %" PRIu64 " (%s).\n",
           region, location, time, ((char *) strings->members[names->members[region]]));

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
RegionExitCallback(OTF2_LocationRef location,
                   OTF2_TimeStamp time,
                   void *userData,
                   OTF2_AttributeList *attributes,
                   OTF2_RegionRef region) {
    struct vector *strings = ((struct user_data *) userData)->strings;
    struct vector *names = ((struct user_data *) userData)->names;
    printf("Leaving region %u at location %" PRIu64 " at time %" PRIu64 " (%s).\n",
           region, location, time, ((char *) strings->members[names->members[region]]));

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
SetStringCallback(void *userData, OTF2_StringRef self, const char *string) {
    struct vector *strings = ((struct user_data *) userData)->strings;
    struct vector *names = ((struct user_data *) userData)->names;
    strings->members[self] = (uint64_t) string;
    printf(">> %i = %s\n", self, string);
    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
GlobalDefinitionRegionRegisterCallback(void *userData,
                                       OTF2_RegionRef self,
                                       OTF2_StringRef name,
                                       OTF2_StringRef canonicalName,
                                       OTF2_StringRef description,
                                       OTF2_RegionRole regionRole,
                                       OTF2_Paradigm paradigm,
                                       OTF2_RegionFlag regionFlags,
                                       OTF2_StringRef sourceFile,
                                       uint32_t beginLineNumber,
                                       uint32_t endLineNumber) {
    struct vector *names = ((struct user_data *) userData)->names;
    names->members[self] = canonicalName;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
GlobDefLocation_Register(void *userData,
                         OTF2_LocationRef location,
                         OTF2_StringRef name,
                         OTF2_LocationType locationType,
                         uint64_t numberOfEvents,
                         OTF2_LocationGroupRef locationGroup) {
    struct vector *locations = ((struct user_data *) userData)->locations;
    if (locations->size == locations->capacity) {
        return OTF2_CALLBACK_INTERRUPT;
    }
    locations->members[locations->size++] = location;

    return OTF2_CALLBACK_SUCCESS;
}