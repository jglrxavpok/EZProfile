//
// Created by jglrxavpok on 12/12/18.
//

#ifndef EZPROFILE_CALLBACKS_H
#define EZPROFILE_CALLBACKS_H

#include <otf2/otf2.h>

/**
 * NOT TO BE CALLED UNDER NORMAL CIRCUMSTANCES
 *
 * Called when the OTF2 reader enters a region
 * @param location the location of the region
 * @param time the timestamp of the entry
 * @param userData the user data passed by OTF2
 * @param attributes the attributes of the region
 * @param region the region
 * @return an error code (OTF2_CALLBACK_SUCCESS if everything's right)
 */
OTF2_CallbackCode
RegionEntryCallback(OTF2_LocationRef location,
                    OTF2_TimeStamp time,
                    void *userData,
                    OTF2_AttributeList *attributes,
                    OTF2_RegionRef region);

/**
 * NOT TO BE CALLED UNDER NORMAL CIRCUMSTANCES
 *
 * Called when the OTF2 reader exits a region
 * @param location the location of the region
 * @param time the timestamp of the exit
 * @param userData the user data passed by OTF2
 * @param attributes the attributes of the region
 * @param region the region
 * @return an error code (OTF2_CALLBACK_SUCCESS if everything's right)
 */
OTF2_CallbackCode
RegionExitCallback(OTF2_LocationRef location,
                   OTF2_TimeStamp time,
                   void *userData,
                   OTF2_AttributeList *attributes,
                   OTF2_RegionRef region);

/**
 * NOT TO BE CALLED UNDER NORMAL CIRCUMSTANCES
 *
 * Called when the OTF2 reader reads a String inside the definition file
 * @param userData the user data passed by OTF2
 * @param self the string reference
 * @param string the actual string
 * @return an OTF2 error code
 */
OTF2_CallbackCode
SetStringCallback(void *userData, OTF2_StringRef self, const char *string);

/**
 * NOT TO BE CALLED UNDER NORMAL CIRCUMSTANCES
 *
 * Called when the OTF2 reader finds a region registry inside the definition file
 * @param userData the user data passed by OTF2
 * @param self the region reference
 * @param name the name of the region
 * @param canonicalName the canonical name of the region
 * @param description the description of the region
 * @param regionRole the role of the region
 * @param paradigm the paradigm of the region
 * @param regionFlags
 * @param sourceFile the source file the region comes from
 * @param beginLineNumber the starting line
 * @param endLineNumber the last line
 * @return an OTF2 error code
 */
OTF2_CallbackCode
GlobalDefitionRegionRegisterCallback(
        void *userData,
        OTF2_RegionRef self,
        OTF2_StringRef name,
        OTF2_StringRef canonicalName,
        OTF2_StringRef description,
        OTF2_RegionRole regionRole,
        OTF2_Paradigm paradigm,
        OTF2_RegionFlag regionFlags,
        OTF2_StringRef sourceFile,
        uint32_t beginLineNumber,
        uint32_t endLineNumber
);

/**
 * NOT TO BE CALLED UNDER NORMAL CIRCUMSTANCES
 *
 * Called when the OTF2 reader finds a region registry inside the definition file
 * @param userData the user data passed by OTF2
 * @param location the location reference
 * @param name the name of the location
 * @param locationType the location type
 * @param numberOfEvents the number of events inside this location
 * @param locationGroup the group of the location
 * @return an OTF2 error code
 */
OTF2_CallbackCode
GlobDefLocation_Register(void *userData,
                         OTF2_LocationRef location,
                         OTF2_StringRef name,
                         OTF2_LocationType locationType,
                         uint64_t numberOfEvents,
                         OTF2_LocationGroupRef locationGroup);

#endif //EZPROFILE_CALLBACKS_H
