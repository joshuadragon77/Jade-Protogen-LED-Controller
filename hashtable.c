#include "hashtable.h"


void fnv1aHash(const uint8_t * data, const unsigned int dataLength, uint64_t * hashOutput){
    *hashOutput = 14695981039346656037ul;

    for (unsigned int i = 0;i<dataLength;i++){
        *hashOutput = *hashOutput ^ data[i];
        *hashOutput = *hashOutput * 1099511628211;
    }
}

void initHashableTable(HashableTable * table, const unsigned int length){
    table->valueTable = (void ** )malloc(sizeof(void *) * length);
    table->keyTable = (char ** )malloc(sizeof(char *) * length);
    table->tableSize = length;
    
    for (unsigned int index = 0;index<length;index++){
        table->valueTable[index] = 0;
        table->keyTable[index] = 0;
    }
}

int hasValueFromHashableTable(HashableTable * table, const char * key){
    uint64_t hash;

    fnv1aHash((uint8_t *)key, strlen(key), &hash);

    unsigned long index = hash % table->tableSize; 

    for (;index<table->tableSize;index ++){
        if (table->keyTable[index] == 0){
            return -1;
        }
        if (strcmp(table->keyTable[index], key) == 0){
            return 0;
        }else{
            index ++;
        }
    };

    return -1;
}

int getValueFromHashableTable(HashableTable * table, const char * key, void ** value){
    uint64_t hash;

    fnv1aHash((uint8_t *)key, strlen(key), &hash);

    unsigned long index = hash % table->tableSize; 

    *value = 0;

    for (;index<table->tableSize;index ++){
        if (table->keyTable[index] == 0){
            return -1;
        }
        if (strcmp(table->keyTable[index], key) == 0){
            *value = table->valueTable[index];
            return 0;
        }else{
            index ++;
        }
    };

    return -1;
}

int setValueToHashableTable(HashableTable * table, const char * key, void * value){
    uint64_t hash;

    unsigned int keyLength = strlen(key);
    fnv1aHash((uint8_t *)key, keyLength, &hash);

    unsigned long index = hash % table->tableSize; 

    for (;index<table->tableSize;index ++){
        if (table->keyTable[index] == 0){
            table->keyTable[index] = (char * )malloc(keyLength);
            strcpy(table->keyTable[index], key);

            table->valueTable[index] = value;
            return 0;
        }else{
            index ++;
        }
    };

    return -1;
}

int main_b(){

    // uint64_t hash = 0;

    // fnv1aHash((const uint8_t *)"hello", 5, &hash);

    HashableTable table;

    initHashableTable(&table, 100);

    setValueToHashableTable(&table, "Hello", (char *)"Bitch");

    setValueToHashableTable(&table, "Bye", (char *)"Dick");

    char * value;

    printf("Status: %d\n", getValueFromHashableTable(&table, "Bye", (void **)&value));
    printf("Content: %s\n", value);

    return 0;
}