#ifndef HASHTABLE
#define HASHTABLE

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "string.h"

typedef struct _HashableTable{
    void ** valueTable;
    char ** keyTable;

    unsigned int tableSize;
} HashableTable;

void initHashableTable(HashableTable * table, const unsigned int length);

int getValueFromHashableTable(HashableTable * table, const char * key, void ** value);

int setValueToHashableTable(HashableTable * table, const char * key, void * value);

int hasValueFromHashableTable(HashableTable * table, const char * key);

int main_b(void);

#endif