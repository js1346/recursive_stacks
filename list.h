#ifndef LIST_H
#define LIST_H

#include "rstack.h"

typedef enum
{
    stack,
    num
} whichtype;

typedef struct Node
{

    whichtype type;
    union
    {
        uint64_t num;
        rstack_t *stack;
    } data;
    struct Node *next;

} Node;

// adds node with stack/value, returns 0/-1 if correct or not
// sets errno
int push_stack(Node **l, rstack_t *rs);
int push_value(Node **l, uint64_t value);

// pop given node(now), changes now to now->next
void pop(Node **prev, Node **now);

// moves a to b and b to b->next
void move(Node **a, Node **b);

// free every node in list|not free stacks
void kill(Node **l);

#endif // LINKED_LIST_H
