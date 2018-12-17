//
// Created by jglrxavpok on 12/12/18.
//

#ifndef EZPROFILE_DATASTRUCTURES_H
#define EZPROFILE_DATASTRUCTURES_H
#include <inttypes.h>
#include <stdlib.h>

struct vector {
    size_t capacity;
    size_t size;
    uint64_t members[];
};

struct array_list {
    void** backing_array;
    size_t current_size;
    size_t current_capacity;
    size_t element_size;
} typedef array_list;

struct stacknode {
    struct stacknode* parent;
    void* data;
} typedef stacknode;

struct stack {
    stacknode* current_node;
} typedef stack;

array_list* NewArrayList(size_t element_size);

array_list* AddToArrayList(array_list* list, void* element);

void* GetInArrayList(array_list* list, size_t index);

stack* NewStack();
void PushStack(stack* stack, void* element);
void* PopStack(stack* stack);
void* PeekStack(stack* stack);
#endif //EZPROFILE_DATASTRUCTURES_H
