#include "rstack.h"
#include "list.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>

// conventions variable-cammelCase, functions-are_with_underscores
// braces in next lines,
//  one lined if's without braces, unless complex expressions

struct rstack
{
    Node *head;
    bool isVisited;
    bool isDestroyed;
};

static Node *user = nullptr;

// next function's goal is only to free memory,not analise relations btw. nodes

static void rstack_kill(rstack_t *rs)
{
    kill(&rs->head);
    free(rs);
}

rstack_t *rstack_new()
{
    rstack_t *toReturn = malloc(sizeof(rstack_t));

    if (toReturn == nullptr)
    {
        errno = ENOMEM;
        return toReturn;
    }

    (toReturn->head) = nullptr;
    (toReturn->isVisited) = false;
    (toReturn->isDestroyed) = false;

    if (push_stack(&user, toReturn) != 0)
    {
        rstack_kill(toReturn);
        toReturn = nullptr;
    }

    return toReturn;
}

static void make_visited(bool x, rstack_t *rs)
{
    if (rs == nullptr)
        return;
    if (rs->isVisited == x)
        return;

    rs->isVisited = x;

    Node *now = rs->head;
    Node *prev = nullptr;

    while (now != nullptr)
    {
        if (now->type == stack)
        {
            make_visited(x, now->data.stack);
        }
        move(&prev, &now);
    }
}

// we iterate along global list, recursively mark visited stack's
// we can get to from stacks not marked destroyed, destroy others

static void mark_and_sweep()
{
    Node *prev = nullptr;
    Node *now = user;

    while (now != nullptr)
    {
        if (now->data.stack->isDestroyed == false)
        {
            make_visited(true, now->data.stack);
        }
        move(&prev, &now);
    }

    prev = nullptr;
    now = user;

    while (now != nullptr)
    {
        if (now->data.stack->isVisited == false)
        {
            rstack_kill(now->data.stack);
            if (prev == nullptr)
            {
                pop(&prev, &now);
                user = now;
            }
            else
                pop(&prev, &now);
        }
        else
        {
            move(&prev, &now);
        }
    }

    prev = nullptr;
    now = user;

    while (now != nullptr)
    {
        make_visited(false, now->data.stack);
        move(&prev, &now);
    }
}

void rstack_delete(rstack_t *rs)
{
    if (rs == nullptr)
        return;

    rs->isDestroyed = true;
    mark_and_sweep();
}

int rstack_push_value(rstack_t *rs, uint64_t value)
{
    if (rs == nullptr)
    {
        errno = EINVAL;
        return -1;
    }

    if (push_value(&rs->head, value) != 0)
        return -1; // errno set in the list

    return 0;
}

int rstack_push_rstack(rstack_t *rs1, rstack_t *rs2)
{
    if (rs1 == nullptr || rs2 == nullptr)
    {
        errno = EINVAL;
        return -1;
    }

    if (push_stack(&rs1->head, rs2) != 0)
        return -1; // errno set in list

    return 0;
}

void rstack_pop(rstack_t *rs)
{
    if (rs == nullptr || rs->head == nullptr)
        return;

    Node *temp = rs->head;

    if (temp->type == stack)
    {
        pop(nullptr, &rs->head);
        mark_and_sweep();
        return;
    }

    pop(nullptr, &rs->head);
}

// iterate along list, if got number we return false
// if got stack, enter recursively
// we mark entered stacks visited to not revisit same paths
// after recursive function we set back to false

static bool empty_helper(rstack_t *rs)
{
    if (rs == nullptr || rs->head == nullptr)
        return true;
    if (rs->isVisited == true)
        return true;

    rs->isVisited = true;
    bool toReturn = true;
    Node *prev = nullptr;
    Node *now = rs->head;

    while (now != nullptr && toReturn == true)
    {
        if (now->type == num)
        {
            toReturn = false;
        }
        else
        {
            toReturn = empty_helper(now->data.stack);
            move(&prev, &now);
        }
    }

    return toReturn;
}

bool rstack_empty(rstack_t *rs)
{
    bool toReturn = empty_helper(rs);

    make_visited(false, rs);

    return toReturn;
}

// iterate along list, if it got number, leave recursion,
// if got stack, enter recursively
// if we meet cycle without number we return empty and check further

static result_t front_helper(rstack_t *rs)
{
    result_t toReturn = {.flag = false, .value = 1};

    if (rs == nullptr || rs->head == nullptr)
        return toReturn;
    if (rs->isVisited == true)
        return toReturn;

    rs->isVisited = true;
    Node *prev = nullptr;
    Node *now = rs->head;

    while (now != nullptr && toReturn.flag == false)
    {
        if (now->type == num)
        {
            toReturn.flag = true;
            toReturn.value = now->data.num;
        }
        else
        {
            toReturn = front_helper(now->data.stack);
            move(&prev, &now);
        }
    }

    return toReturn;
}

result_t rstack_front(rstack_t *rs)
{
    result_t toReturn = front_helper(rs);

    make_visited(false, rs);

    return toReturn;
}

// function to help in read

static rstack_t *leave(bool toClose, FILE *file, int error, rstack_t *rs)
{
    if (toClose == true)
    {
        fclose(file);
        errno = error;
    }

    pop(nullptr, &user);
    rstack_kill(rs);
    return nullptr;
}

// standard parsing with buffer
// numbers are in uint64_t, so their size is 2^64~10^20-10^25
// therefore 100 chars are enough to get whole input

rstack_t *rstack_read(char const *path)
{
    if (path == nullptr)
    {
        errno = EINVAL;
        return nullptr;
    }

    rstack_t *toReturn = rstack_new();

    if (toReturn == nullptr)
        return nullptr;

    FILE *file = fopen(path, "r");

    if (file == nullptr)
        return leave(false, file, errno, toReturn);

    uint64_t getStrtoumax;
    char buffer[100];

    while (fscanf(file, "%99s", buffer) == 1)
    {
        errno = 0;
        char *endptr;

        for (int i = 0; buffer[i] != '\0'; i++)
            if (!isdigit((unsigned char)buffer[i]))
                return leave(true, file, EINVAL, toReturn);

        getStrtoumax = (uint64_t)strtoull(buffer, &endptr, 10);

        if (errno == ERANGE || endptr == buffer || *endptr != '\0')
            return leave(true, file, EINVAL, toReturn);

        if (rstack_push_value(toReturn, getStrtoumax) != 0)
            return leave(true, file, ENOMEM, toReturn);
    }

    if (!feof(file))
        return leave(true, file, EIO, toReturn);

    fclose(file);
    return toReturn;
}

/* firstly we enter last node possible, if got number, write it to file
if got stack mark it visited, enter recursively its list and mark non visited
we return negative value if we need to stop writing
-2 if cycle (visit stack being visited), -1 if error in fprintf(e.g memory)*/

static int write_helper(FILE *file, Node *el)
{
    if (el == nullptr)
        return 0;

    if (el->next != nullptr)
    {
        int x = write_helper(file, el->next);
        if (x < 0)
        {
            return x;
        }
    }

    if (el->type == num)
    {
        if (fprintf(file, "%lu\n", el->data.num) < 0)
        {
            return -1;
        }
    }
    else
    {
        if (el->data.stack->isVisited == true)
        {
            return -2;
        }
        else
        {
            el->data.stack->isVisited = true;
            int x = write_helper(file, el->data.stack->head);
            if (x != 0)
            {
                return x;
            }
            el->data.stack->isVisited = false;
        }
    }

    return 0;
}

// if helper returns -2, process does not return error

int rstack_write(char const *path, rstack_t *rs)
{
    if (path == nullptr || rs == nullptr)
    {
        errno = EINVAL;
        return -1;
    }

    FILE *file = fopen(path, "w");

    if (file == nullptr)
        return -1; // three lines above there is set to errno=enoent

    rs->isVisited = true;

    int toReturn = write_helper(file, rs->head);

    make_visited(false, rs);

    fclose(file);

    if (toReturn == -2)
        toReturn = 0;

    return toReturn;
}
