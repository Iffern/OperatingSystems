#include <ctype.h>
#include "difflib.h"

const int MAX_CHARS=200;

struct MainTable* createMainTable(int size){
struct MainTable* main=malloc(sizeof(struct MainTable));
main->tableOfBlocks=(struct BlockTable**) calloc(size,sizeof(struct BlockTable*));
main->index=-1;
return main;
}

struct BlockTable* createBlock(int size){
struct BlockTable* block = malloc(sizeof(struct BlockTable));
block->edit_num=-1;
block->editingBlocks=(char**)calloc(size,sizeof(char*));
return block;
}


void diffFiles(const char* first_file,const char* second_file,const char* tmp_file){
const int size = strlen(first_file)+strlen(second_file)+strlen(tmp_file)+strlen("diff")+10;
char* command = calloc(size,sizeof(char));
strcpy(command, "diff ");
strcat(command, first_file);
strcat(command, " ");
strcat(command, second_file);
strcat(command, " > ");
strcat(command, tmp_file);
system(command);

free(command);
}

int parseTmpFile(const char* tmp_file, struct MainTable* mainTab){
FILE* tmp = fopen(tmp_file,"r");
if(tmp==NULL) fprintf(stderr,"Failed to open temporary file %s",tmp_file);

char *line_buf = NULL;
size_t line_buf_size = 0;
int block_couter = 0;
while(getline(&line_buf, &line_buf_size, tmp)!=-1){
    if(line_buf[0] != '>' && line_buf[0] != '<' && line_buf[0] != '-') block_couter++;
}
struct BlockTable* table = createBlock(block_couter);

fclose(tmp);
tmp=fopen(tmp_file,"r");

*line_buf=NULL;
line_buf_size=0;
while(getline(&line_buf, &line_buf_size, tmp)!=-1){
    if(line_buf[0] != '>' && line_buf[0] != '<' && line_buf[0] != '-'){
        table->edit_num+=1;
        table->editingBlocks[table->edit_num]=(char*)calloc(MAX_CHARS,sizeof(char));
        strcpy(table->editingBlocks[table->edit_num],line_buf);
}           else{
                strcat(table->editingBlocks[table->edit_num],line_buf);
}
}

free(line_buf);

fclose(tmp);
mainTab->tableOfBlocks[++mainTab->index] = table;
return mainTab->index;
}

int numberOfOperations(struct MainTable* mainTab, int index){
if(mainTab->index<index || index<0){
    fprintf(stderr,"Podany indeks bloku operacji edycyjnych nie istnieje\n");
    return -1;
}
    else{
        return mainTab->tableOfBlocks[index]->edit_num+1;
}
}

void removeEditingOperation(struct MainTable* mainTab, int blockIdx, int operationIdx){
    if(mainTab->index<blockIdx || blockIdx<0){
        fprintf(stderr,"Podany indeks bloku operacji edycyjnych nie istnieje\n");
}
        else{
            struct BlockTable* tmp=mainTab->tableOfBlocks[blockIdx];
                if(tmp->edit_num<operationIdx || operationIdx<0){
                fprintf(stderr,"Podany indeks operacji edycyjnej w bloku o indeksie %d nie istnieje\n",blockIdx);
}
                else{
                free(tmp->editingBlocks[operationIdx]);
                for(int i=operationIdx;i<tmp->edit_num;i++) tmp->editingBlocks[i]=tmp->editingBlocks[i+1];
                }
                }
}

void removeOperationBlock(struct MainTable* mainTab, int index){
if(mainTab->index<index || index<0){
fprintf(stderr,"Podany indeks bloku operacji edycyjnych nie istnieje\n");
}
else{
free(mainTab->tableOfBlocks[index]);
for(int i=index;i<mainTab->index;i++) mainTab->tableOfBlocks[i]=mainTab->tableOfBlocks[i+1];
}
}

