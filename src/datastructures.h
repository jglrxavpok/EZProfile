//
// Created by jglrxavpok on 12/12/18.
//

#ifndef EZPROFILE_DATASTRUCTURES_H
#define EZPROFILE_DATASTRUCTURES_H
struct vector {
    size_t capacity;
    size_t size;
    uint64_t members[];
};

struct user_data {
    struct vector *locations;
    struct vector *strings;
    struct vector *names;
};

#endif //EZPROFILE_DATASTRUCTURES_H
