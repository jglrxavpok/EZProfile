#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <otf2/otf2.h>
#include <inttypes.h>
#include "datastructures.h"
#include "callbacks.h"

int main() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("CWD: %s\n", cwd);
    OTF2_Reader* reader = OTF2_Reader_Open(
            //"../tests/test_stdio/eztrace_log.otf2"
            //"../tests/taurus_west_class_a_full_4/traces.otf2"
            //"../tests/scorep_smxv-omp-dynamic_large/traces.otf2"
            "../tests/taurus_west_class_a_full_4/traces.otf2"
            );
    OTF2_Reader_SetSerialCollectiveCallbacks(reader);

    uint64_t number_of_locations;
    OTF2_Reader_GetNumberOfLocations(reader,
                                     &number_of_locations);
    struct vector *locations = malloc(sizeof(*locations)
                                      + number_of_locations
                                        * sizeof(*locations->members));
    locations->capacity = number_of_locations;
    locations->size = 0;

    uint64_t nStrings = 1024;
    struct vector *strings = malloc(sizeof(*strings) + nStrings * sizeof(*strings->members));
    strings->size = 0;
    strings->capacity = nStrings;

    struct vector *names = malloc(sizeof(*names) + nStrings * sizeof(*names->members));
    names->size = 0;
    names->capacity = nStrings;


    struct user_data *userData = malloc(sizeof(struct user_data));
    userData->locations = locations;
    userData->strings = strings;
    userData->names = names;

    OTF2_GlobalDefReader *global_def_reader = OTF2_Reader_GetGlobalDefReader(reader);

    OTF2_GlobalDefReaderCallbacks *global_def_callbacks = OTF2_GlobalDefReaderCallbacks_New();
    OTF2_GlobalDefReaderCallbacks_SetLocationCallback(global_def_callbacks,
                                                      &GlobDefLocation_Register);
    OTF2_GlobalDefReaderCallbacks_SetRegionCallback(global_def_callbacks,
                                                    &GlobalDefinitionRegionRegisterCallback);
    OTF2_GlobalDefReaderCallbacks_SetStringCallback(global_def_callbacks,
                                                    &SetStringCallback);
    OTF2_Reader_RegisterGlobalDefCallbacks(reader,
                                           global_def_reader,
                                           global_def_callbacks,
                                           userData);
    OTF2_GlobalDefReaderCallbacks_Delete(global_def_callbacks);

    uint64_t definitions_read = 0;
    OTF2_Reader_ReadAllGlobalDefinitions(reader,
                                         global_def_reader,
                                         &definitions_read);

    // everything has been stored, select the ones we want to read

    for (size_t i = 0; i < locations->size; i++) {
        OTF2_Reader_SelectLocation(reader, locations->members[i]);
    }

    bool successful_open_def_files =
            OTF2_Reader_OpenDefFiles(reader) == OTF2_SUCCESS;
    OTF2_Reader_OpenEvtFiles(reader);

    for (size_t i = 0; i < locations->size; i++) {
        if (successful_open_def_files) {
            OTF2_DefReader *def_reader =
                    OTF2_Reader_GetDefReader(reader, locations->members[i]);
            if (def_reader) {
                uint64_t def_reads = 0;
                OTF2_Reader_ReadAllLocalDefinitions(reader,
                                                    def_reader,
                                                    &def_reads);
                OTF2_Reader_CloseDefReader(reader, def_reader);
            }
        }
        OTF2_EvtReader *evt_reader =
                OTF2_Reader_GetEvtReader(reader, locations->members[i]);
    }

    if (successful_open_def_files) {
        OTF2_Reader_CloseDefFiles(reader);
    }

    OTF2_GlobalEvtReader *global_evt_reader = OTF2_Reader_GetGlobalEvtReader(reader);

    // register callbacks

    OTF2_GlobalEvtReaderCallbacks *event_callbacks = OTF2_GlobalEvtReaderCallbacks_New();
    OTF2_GlobalEvtReaderCallbacks_SetEnterCallback(event_callbacks,
                                                   &RegionEntryCallback);
    OTF2_GlobalEvtReaderCallbacks_SetLeaveCallback(event_callbacks,
                                                   &RegionExitCallback);
    OTF2_Reader_RegisterGlobalEvtCallbacks(reader,
                                           global_evt_reader,
                                           event_callbacks,
                                           userData);
    OTF2_GlobalEvtReaderCallbacks_Delete(event_callbacks);

    // read all

    uint64_t events_read = 0;
    OTF2_Reader_ReadAllGlobalEvents(reader,
                                    global_evt_reader,
                                    &events_read);

    // everything is read, close the file

    OTF2_Reader_CloseGlobalEvtReader(reader, global_evt_reader);
    OTF2_Reader_CloseEvtFiles(reader);

    OTF2_Reader_Close(reader);
    free(locations);
    return EXIT_SUCCESS;
}