#include<pthread.h>
#include <semaphore.h>
#include<vector>
#include <iostream>
#include <string.h>



#include "sudoku.h"
#include "done_theead.h"

int total_solved = 0;
int total = 0;
int total_mission = 0;
int next_mission = 0;

std::vector<file> file_list ;
int file_now = 0 ;
int file_size = 0 ;

pthread_mutex_t total_m ;
pthread_mutex_t total_s_m ;
pthread_mutex_t next_mission_m ;
pthread_mutex_t file_input_m ;
bool done_all = false ;

std::vector<mission> mission_queue ; //vector does not support multi threads
std::vector<pthread_t> thread_queue ;
std::vector<args> args_queue ;
// mission mission_queue[1005] ;
// args args_queue[1005] ;


void* input_mission_file(void *arg){
    char file_name[128] ;
    while(1){
        std::cin.getline(file_name, 128) ;
        if(strcmp(file_name, "") == 0){
            continue ;
        }
        else {
            file file_tmp ;
            pthread_mutex_unlock(&file_input_m) ;
            char base[128]="./TestData/";
            strcat(base,file_name);
            strcpy(file_name,base);
            file_list.push_back(file_tmp) ;
            file_size++ ;
            pthread_mutex_unlock(&file_input_m) ;
            strcpy(file_list[file_size-1].file_name, file_name) ;
        }
    }
}

void output_mission(){
    for(int i = 0 ; i < mission_queue.size() ; i++){
      printf("%s", mission_queue[i].puzzle) ;
    }
}


// vector version to make missions
void make_mission(FILE *fp, bool (*solve)(char * , int)){
    mission_queue.clear() ;
    total_mission = 0;
    next_mission = 0;

    char puzzle[128] ;
    while (fgets(puzzle, sizeof puzzle, fp) != NULL) {
        total++ ;
        total_mission++ ;
        if (strlen(puzzle) >= N) {
            mission tmp ;
            tmp.sovle = false ;
            tmp.flag = total ;
            strcpy(tmp.puzzle, puzzle) ;
            mission_queue.push_back(tmp) ;

            args tmp_args ;
            args_queue.push_back(tmp_args) ;
        }
    }
}


#if (!DONE_THEEAD && !DONE_THEEAD_2)
void* solve_sudoku(void *arg){
    args *tmp_args = (args*)arg ;
    bool (*solve)(char * , int) = tmp_args->solve_name ;
    if (strlen((tmp_args->m)->puzzle) >= N) {
        // ++total;
        if (solve((tmp_args->m)->puzzle, tmp_args->which_space)) {
            (tmp_args->m)->sovle = true ;
            ++total_solved;
        }
        else {
            printf("No: %s", (tmp_args->m)->puzzle);
        }
    }
}
#endif

#if (DONE_THEEAD && !DONE_THEEAD_2)
void* solve_sudoku(void *arg){
    args* tmp_args = (args*)arg ;
    bool (*solve)(char * , int) = tmp_args->solve_name ;
    if (strlen((tmp_args->m)->puzzle) >= N) {
        if (solve((tmp_args->m)->puzzle, tmp_args->which_space)) {
            pthread_mutex_lock(&total_s_m) ;
            (tmp_args->m)->sovle = true ;
            ++total_solved;
            pthread_mutex_unlock(&total_s_m) ;
        }
        else {
            printf("No: %s", (tmp_args->m)->puzzle);
        }
    }
}
#endif

#if (!DONE_THEEAD && DONE_THEEAD_2)
void* solve_sudoku(void* arg){
    args* tmp_args = (args*)arg ;
    bool (*solve)(char * , int) = tmp_args->solve_name ;
    if (strlen((tmp_args->m)->puzzle) >= N) {
        if (solve((tmp_args->m)->puzzle, tmp_args->which_space)) {
            pthread_mutex_lock(&total_s_m) ;
            (tmp_args->m)->sovle = true ;
            ++total_solved;
            pthread_mutex_unlock(&total_s_m) ;
        }
        else {
            printf("No: %s", (tmp_args->m)->puzzle);
        }
    }
}
void* solve_thread(void* arg){
    args *tmp_arg = (args *)arg ;
    bool (*solve)(char * , int) = tmp_arg->solve_name ;
    while (next_mission < total_mission)
    {
        pthread_mutex_lock(&next_mission_m) ;
        int i = next_mission ;
        next_mission++ ;
        pthread_mutex_unlock(&next_mission_m) ;
        args_queue[i].solve_name = solve ;
        args_queue[i].m = &mission_queue[i] ;
        args_queue[i].which_space = 0 ;
        solve_sudoku(&args_queue[i]) ;
    }
}
#endif
