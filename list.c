#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "list.h"

int push_stack(Node **l, rstack_t *rs)
{
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == nullptr)
    {
        errno = ENOMEM;
        return -1;
    }

    new_node->type = stack;
    new_node->data.stack = rs;
    new_node->next = (*l);
    (*l) = new_node;

    return 0;
}

int push_value(Node **l, uint64_t value)
{
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == nullptr)
    {
        errno = ENOMEM;
        return -1;
    }

    new_node->type = num;
    new_node->data.num = value;
    new_node->next = (*l);
    (*l) = new_node;

    return 0;
}

void pop(Node **prev, Node **now)
{
    if ((*now) == nullptr)
    {
        return;
    }

    // if prev is nullptr,we assume head is first node, then
    if (prev == nullptr || (*prev) == nullptr)
    {
        Node *temp = (*now)->next;
        free(*now);
        (*now) = temp;
    }
    else
    {
        if ((*prev)->next == (*now)) // we check correctnes of nodes
        {
            Node *temp = (*now)->next;
            (*prev)->next = (*now)->next;
            free(*now);
            (*now) = temp;
        }
    }
}

void move(Node **a, Node **b)
{
    if (b == nullptr || (*b) == nullptr)
        return;

    (*a) = (*b);
    (*b) = (*b)->next;
}

void kill(Node **l)
{
    if (l == nullptr)
        return;

    while ((*l) != nullptr)
        pop(nullptr, l);
}
