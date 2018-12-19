//
// Created by jglrxavpok on 19/12/18.
//

#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "arguments.h"

static int
parse_opt(int key, char* arg, struct argp_state* state) {
    arguments* arguments = state->input;
    switch (key) {
        case 'c':
            if(strcmp(arg, "inclusive") == 0) {
                arguments->computation_mode = inclusive;
            } else if(strcmp(arg, "exclusive") == 0) {
                arguments->computation_mode = exclusive;
            } else if(strcmp(arg, "both") == 0) {
                arguments->computation_mode = both;
            } else {
                fprintf(stderr, "Invalid option: %s\n", arg);
                return ARGP_KEY_ERROR;
            }
            break;
        case 'i':
            arguments->thread_index = atoi(arg);
            break;
        case 't':
            arguments->thread_name = arg;
            break;
        case ARGP_KEY_ARG:
            if (state->arg_num >= 1)
            {
                argp_usage(state);
            }
            arguments->trace_file = arg;
            break;
        case ARGP_KEY_END:
            if (state->arg_num < 1)
            {
                argp_usage (state);
            }
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

arguments* parse_arguments(int argc, char** argv) {
    char* args_docs = "FILE";
    arguments* arguments = malloc(sizeof(arguments));
    arguments->computation_mode = both;
    arguments->thread_name = NULL;
    arguments->thread_index = -1;
    arguments->trace_file = NULL;
    struct argp_option options[] = {
            { "compute", 'c', "exclusive|inclusive|both", 0, "Compute exclusive time, inclusive time or both (default)" },
            { "thread", 't', "STRING", 0, "Get the result of all threads sharing the given name" },
            { "threadindex", 'i', "NUM", 0, "Get the result of a single thread (via its index)" },
            {0}
    };
    struct argp argp = { options, parse_opt, args_docs };
    if(! argp_parse(&argp, argc, argv, 0, 0, arguments)) {
        return arguments;
    }
    return NULL;
}