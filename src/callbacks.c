//
// Created by jglrxavpok on 12/12/18.
//
#include "callbacks.h"
#include "datastructures.h"
#include "ezprofile.h"
#include <inttypes.h>
#include <stdio.h>

OTF2_CallbackCode
RegionEntryCallback(OTF2_LocationRef location,
                    OTF2_TimeStamp time,
                    void *userData,
                    OTF2_AttributeList *attributes,
                    OTF2_RegionRef region) {
    ezdata* data = userData;

    EnterThreadFunction(data, GetThreadData(data, location), region, time);
    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
RegionExitCallback(OTF2_LocationRef location,
                   OTF2_TimeStamp time,
                   void *userData,
                   OTF2_AttributeList *attributes,
                   OTF2_RegionRef region) {
    ezdata* data = userData;

    ExitThreadFunction(data, GetThreadData(data, location), region, time);
    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
SetStringCallback(void *userData, OTF2_StringRef self, const char *string) {
    ezdata* data = userData;
    hash_put(data->strings, self, (void*)string);
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
    ezdata* data = userData;
    hash_put(data->region_names, self, (void*)name);

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
GlobDefLocation_Register(void *userData,
                         OTF2_LocationRef location,
                         OTF2_StringRef name,
                         OTF2_LocationType locationType,
                         uint64_t numberOfEvents,
                         OTF2_LocationGroupRef locationGroup) {
    ezdata* data = userData;
    hash_put(data->thread_names, location, (void*)name);
    GetThreadData(data, location);
    data->locations->members[data->locations->size++] = location;
    return OTF2_CALLBACK_SUCCESS;
}