#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>


#define MAX_FILE_COUNT 3000


char* getFilename(char* buf){
    char* filename = malloc(256*sizeof(char));
    int index=0;
    for (int i = 9; i < strlen(buf); ++i) {
        filename[index]=buf[i];
        index++;
    }
    if (filename[index-1]=='\n'){
        filename[index-1]='\0';
    }else {
        filename[index] = '\0';
    }
    return filename;
}

//Поиск файла

char *pathcat(const char *path1, const char *path2){
    int res_path_len = strlen(path1) + strlen(path2) + 2;
    char *res_path = malloc(res_path_len*sizeof(char));
    sprintf(res_path, "%s/%s", path1 ,path2);
    return res_path;
}

char *find_file(const char *dir_name, const char *filename, char** allFiles, int* index, bool* found){
    char *full_path_file = NULL;
    DIR *dir = opendir(dir_name);
    if(dir){
        struct dirent *de = readdir(dir);
        while (de){
            if(de->d_type == DT_REG && !strcmp(de->d_name, filename)){

                full_path_file = pathcat(dir_name, filename);

                FILE *file=fopen(full_path_file, "r");
                char buf[256];
                if (file){
                    while ((fgets(buf, 256, file))!=NULL){
                        if (*found== true)
                            break;

                        if (strcmp(buf, "Minotaur")==0){
                            allFiles[*index]=full_path_file;
                            (*index)++;
                            *found = true;
                            break;
                        }else{
                            char* nameNextFile = getFilename(buf);
                            if (strcmp(nameNextFile, "")==0){
                                nameNextFile=NULL;
                                break;
                            }
                            if (nameNextFile!=NULL){
                                if (*index>0){
                                    if (strcmp(allFiles[*index-1], full_path_file)!=0){
                                        allFiles[*index] = full_path_file;
                                        (*index)++;
                                    }
                                }else{
                                    allFiles[*index] = full_path_file;
                                    (*index)++;
                                }
                                find_file("./labyrinth", nameNextFile, allFiles, index, found);
                            }
                        }
                    }
                    fclose(file);
                }

            }else if (de->d_type == DT_DIR &&
                      strcmp(de->d_name, ".") != 0 &&
                      strcmp(de->d_name, "..") != 0){

                char *new_dir = pathcat(dir_name, de->d_name);

                full_path_file = find_file(new_dir, filename, allFiles, index, found);
                free(new_dir);
            }
            if(full_path_file)
                break;
            /*if (*found==true){
                closedir(dir);
            }*/
            de = readdir(dir);
        }
        closedir(dir);
    }else
        printf("Failed to open %s directory\n", dir_name);
    return full_path_file;
}


int main(){
    char** allFiles = malloc(MAX_FILE_COUNT*sizeof(char));
    int index=0;
    bool found=false;
    find_file("./labyrinth", "file.txt", allFiles, &index, &found);

    FILE *res = fopen("result.txt", "w");
    for (int i = 0; i < index; ++i) {
        fprintf(res, "%s\n", allFiles[i]);
    }
    fclose(res);
    free(allFiles);
    return 0;
}