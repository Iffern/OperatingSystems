#ifndef SYSYOPY_LIB_H
#define difflib_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct MainTable{
    int index;
    struct BlockTable** tableOfBlocks;
};

struct BlockTable{
    int edit_num;
    char** editingBlocks;
};

struct MainTable* createMainTable(int size);
struct BlockTable* createBlock(int size);
void diffFiles(const char* first_file,const char* second_file,const char* tmp_file);
int parseTmpFile(const char* tmp_file, struct MainTable* mainTab);
int numberOfOperations(struct MainTable* mainTab, int index);
void removeEditingOperation(struct MainTable* mainTab, int blockIdx, int operationIdx);
void removeOperationBlock(struct MainTable* mainTab, int index);

#endif

