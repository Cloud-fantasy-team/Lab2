#include<pthread.h>
#include <semaphore.h>
#include<vector>
#include <iostream>
#include <string.h>

#include "sudoku.h"
#ifndef DONE_THEEAD_H
#define DONE_THEEAD_H

extern int total_solved ;
extern int total ;
extern int total_mission ;
extern int next_mission ;

struct file
{
    /* data */
    char file_name[128] ;
};


extern std::vector<file> file_list ;
extern int file_now ;
extern int file_size ;

extern pthread_mutex_t total_m ;
extern pthread_mutex_t total_s_m ;
extern pthread_mutex_t next_mission_m ;
extern pthread_mutex_t file_input_m ;
extern bool done_all ;

struct mission{
    bool sovle = false ;
    char puzzle[128] ;
    int flag ;
};

struct args{
    bool (*solve_name)(char* , int) ;
    mission *m ;
    int which_space ;
};

extern std::vector<mission> mission_queue ; //vector does not support multi threads
extern std::vector<pthread_t> thread_queue ;
extern std::vector<args> args_queue ;
// extern mission mission_queue[1005] ;
// extern args args_queue[1005] ;

void* input_mission_file(void *arg) ;
void output_mission() ;
void make_mission(FILE *fp, bool (*solve)(char * , int)) ;
void* solve_sudoku(void *arg) ;
void* solve_thread(void *arg) ;


#endif
