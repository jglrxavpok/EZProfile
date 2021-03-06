#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <otf2/otf2.h>
#include <inttypes.h>
#include "datastructures.h"
#include "callbacks.h"
#include "ezprofile.h"
#include "arguments.h"

void readOTF2File(arguments* args) {
    OTF2_Reader* reader = OTF2_Reader_Open(args->trace_file);
    OTF2_Reader_SetSerialCollectiveCallbacks(reader);

    uint64_t number_of_locations;
    OTF2_Reader_GetNumberOfLocations(reader,
                                     &number_of_locations);

    ezdata* ezdata = NewEZData(number_of_locations, args->computation_mode, args->thread_index, args->thread_name);

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
                                           ezdata);
    OTF2_GlobalDefReaderCallbacks_Delete(global_def_callbacks);

    uint64_t definitions_read = 0;
    OTF2_Reader_ReadAllGlobalDefinitions(reader,
                                         global_def_reader,
                                         &definitions_read);

    // everything has been stored, select the ones we want to read

    for (size_t i = 0; i < ezdata->locations->size; i++) {
        OTF2_Reader_SelectLocation(reader, RetrieveLocation(ezdata, i));
    }

    bool successful_open_def_files =
            OTF2_Reader_OpenDefFiles(reader) == OTF2_SUCCESS;
    OTF2_Reader_OpenEvtFiles(reader);

    for (size_t i = 0; i < ezdata->locations->size; i++) {
        if (successful_open_def_files) {
            OTF2_DefReader *def_reader =
                    OTF2_Reader_GetDefReader(reader, RetrieveLocation(ezdata, i));
            if (def_reader) {
                uint64_t def_reads = 0;
                OTF2_Reader_ReadAllLocalDefinitions(reader,
                                                    def_reader,
                                                    &def_reads);
                OTF2_Reader_CloseDefReader(reader, def_reader);
            }
        }
        OTF2_EvtReader *evt_reader =
                OTF2_Reader_GetEvtReader(reader, RetrieveLocation(ezdata, i));
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
                                           ezdata);
    OTF2_GlobalEvtReaderCallbacks_Delete(event_callbacks);

    // read all

    uint64_t events_read = 0;
    OTF2_Reader_ReadAllGlobalEvents(reader,
                                    global_evt_reader,
                                    &events_read);

    // everything is read, close the file

    OTF2_Reader_CloseGlobalEvtReader(reader, global_evt_reader);
    OTF2_Reader_CloseEvtFiles(reader);


    PrintEZResults(ezdata);

    OTF2_Reader_Close(reader);
    free(ezdata);
}

int main(int argc, char** argv) {
    arguments* args = parse_arguments(argc, argv);
    if(!args) { // failed to parse arguments
        return -1;
    }
    readOTF2File(
            args
            //"../tests/test_stdio/eztrace_log.otf2"
            //"../tests/taurus_west_class_a_full_4/traces.otf2"
            //"../tests/scorep_smxv-omp-dynamic_large/traces.otf2"
            //"../tests/taurus_west_class_a_full_4/traces.otf2"
    );
    return EXIT_SUCCESS;
}