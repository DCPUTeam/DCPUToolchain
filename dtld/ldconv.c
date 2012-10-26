/**

    File:       ldconv.c

    Project:    DCPU-16 Toolchain
    Component:  Linker

    Authors:    James Rhodes

    Description:    Converts object format structures to linked lists
            so that they can be easily modified and cloned.

**/

#include <stdio.h>
#include <stdlib.h>
#include <bstring.h>
#include <simclist.h>
#include <debug.h>
#include <lprov.h>
#include "ldconv.h"

size_t lconv_entry_meter(const void* el)
{
    return sizeof(struct lconv_entry);
}

int lconv_entry_comparator(const void* a, const void* b)
{
    struct lconv_entry* ea = (struct lconv_entry*)a;
    struct lconv_entry* eb = (struct lconv_entry*)b;
    if (ea->address > eb->address)
        return -1;
    else if (ea->address == eb->address)
        return 0;
    else
        return 1;
}

int lconv_entry_seeker(const void* e, const void* name)
{
    struct lconv_entry* el = (struct lconv_entry*)e;
    if (el->label == NULL) return 0;
    if (biseq(el->label, (bstring)name))
        return 1;
    else
        return 0;
}

list_t* list_create()
{
    list_t* list = malloc(sizeof(list_t));
    list_init(list);
    list_attributes_copy(list, &lconv_entry_meter, 1);
    list_attributes_comparator(list, &lconv_entry_comparator);
    list_attributes_seeker(list, &lconv_entry_seeker);
    return list;
}

long allocated_strings = 0;

list_t* list_convert(struct lprov_entry* first)
{
    struct lconv_entry* entry;
    list_t* list = malloc(sizeof(list_t));
    list_init(list);
    list_attributes_copy(list, &lconv_entry_meter, 1);
    list_attributes_comparator(list, &lconv_entry_comparator);
    list_attributes_seeker(list, &lconv_entry_seeker);
    while (first != NULL)
    {
        entry = malloc(sizeof(struct lconv_entry));
        entry->address = first->address;
        entry->bin = NULL;
        entry->label = bfromcstr(first->label);
        list_append(list, entry);
        free(entry);
        first = first->next;
    }
    return list;
}

struct lprov_entry* list_revert(list_t* list)
{
    struct lprov_entry* first = NULL;
    struct lprov_entry* previous = NULL;
    struct lprov_entry* current = NULL;
    struct lconv_entry* entry = NULL;

    if (list == NULL)
        return NULL;

    list_iterator_start(list);
    while (list_iterator_hasnext(list))
    {
        entry = list_iterator_next(list);
        current = malloc(sizeof(struct lprov_entry));
        current->address = entry->address;
        current->label = bstr2cstr(entry->label, '0');
        current->next = NULL;
        if (previous != NULL)
            previous->next = current;
        if (first == NULL)
            first = current;
        previous = current;
    }
    list_iterator_stop(list);

    return first;
}

void list_prov_destroy(list_t* list)
{
    struct lconv_entry* entry = NULL;

    if (list == NULL)
        return;

    list_iterator_start(list);
    while (list_iterator_hasnext(list))
    {
        entry = list_iterator_next(list);
        bdestroy(entry->label);

        // No need to free entries as list_attributes_copy
        // has the copy_data option set to 1.  We also don't
        // free the bin name, as we don't own that.
    }
    list_iterator_stop(list);
    list_clear(list);
    list_destroy(list);
    free(list);
}

list_t* list_clone(list_t* original)
{
    list_t* list = malloc(sizeof(list_t));
    list_init(list);
    list_attributes_copy(list, &lconv_entry_meter, 1);
    list_attributes_comparator(list, &lconv_entry_comparator);
    list_attributes_seeker(list, &lconv_entry_seeker);
    list_iterator_start(original);
    while (list_iterator_hasnext(original))
        list_append(list, list_iterator_next(original));
    list_iterator_stop(original);
    return list;
}