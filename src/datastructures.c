//
// Created by jglrxavpok on 12/12/18.
//
#include "datastructures.h"
#include <stdlib.h>
#include <stdio.h>

array_list* NewArrayList(size_t element_size) {
    array_list* list = calloc(1, sizeof(array_list));
    list->element_size = element_size;
    return list;
}

array_list* AddToArrayList(array_list* list, void* element) {
    if(list->current_capacity == list->current_size) {
        list->backing_array = realloc(list->backing_array, list->element_size*(list->current_capacity+10));
    }
    list->backing_array[list->current_size++] = element;
    printf("add %li at %li in %li\n", (uint64_t)element, list->current_size, (uint64_t)list);
    return list;
}

void* GetInArrayList(array_list* list, size_t index) {
    printf("retrieve at %li in %li\n", index, (uint64_t)list);
    return list->backing_array[index];
}

stack* NewStack() {
    return calloc(1, sizeof(stack));
}

void PushStack(stack* stack, void* element) {
    if(stack->current_node == NULL) { // if stack empty
        stack->current_node = calloc(1, sizeof(stacknode));
        stack->current_node->data = element;
    } else {
        stacknode* new_node = calloc(1, sizeof(stacknode));
        new_node->data = element;
        new_node->parent = stack->current_node;
        stack->current_node = new_node;
    }
}

void* PopStack(stack* stack) {
    if(stack->current_node == NULL)
        return NULL;
    void* data = stack->current_node->data;
    stack->current_node = stack->current_node->parent;
    return data;
}

void* PeekStack(stack* stack) {
    if(stack->current_node == NULL)
        return NULL;
    return stack->current_node->data;
}