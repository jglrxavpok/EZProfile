//
// Created by jglrxavpok on 19/12/18.
//

#ifndef EZPROFILE_ARGUMENTS_H
#define EZPROFILE_ARGUMENTS_H
#include <argp.h>

enum computation_mode {
    inclusive, exclusive, both
} typedef computation_mode;

struct arguments {
    char* trace_file;
    computation_mode computation_mode;
    int thread_index;
    char* thread_name;
} typedef arguments;

arguments* parse_arguments(int argc, char** argv);

#endif //EZPROFILE_ARGUMENTS_H
