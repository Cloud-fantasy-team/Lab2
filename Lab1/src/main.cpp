#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>

using namespace std;
string getTime()
{
    time_t timekk;
    time (&timekk); //获取time_t类型的当前时间
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",localtime(&timekk) );//对日期和时间进行格式化
    return tmp;
}

void thread_function filenameReader() {
  for (int i = 0; i < 100; i++)
    std::cout << "thread function excuting" << std::endl;//添加读入 
}

/*任务结构体 直接在dancinglinck里文件流输出结果 不用考虑返回结果数组了*/
struct job{
	int board[81]; //题目
};

/*任务池大小*/
#define POOL_SIZE 1000	//这个需要整体实现完之后进行调参
/*任务单元大小*/ 
#define JOB_UNIT_SIZE 100	//输入线程or解题线程以这个大小为单位一次性读取or消费这么多个题目
/*信号量数值上限*/
#define SEM_MAXIMUM 10	//这个值为任务池大小/任务单元大小

struct job[POOL_SIZE];	//任务池


/*信号量	*/	
//！！！	注意！！！  任务池中当前任务数量n = poolFull.currNum * JOB_UNIT_SIZE     
sem_t pollEmpty;//任务池空 
//--> 读取线程 当pollEmpty大于0时直接返回（空位足够） 等于0则睡眠（塞不下了）
sem_t pollFull;	//任务池满
//--> 解题线程 当poolFull大于0时直接返回（任务足够） 等于0则睡眠（没任务）

//任务池锁
pthread_mutex_t jobPoolMutex=PTHREAD_MUTEX_INITIALIZER;

//分段式读入可能需要的位置标记
//int flag=0;


/*
感觉暂时用不到这些
sem_t problemMutex;		
sem_t filenameEmpty;
sem_t filenameFull;
sem_t filenameMutex; 
*/

int sem_init __P ((sem_t *__sem, int __pshared, unsigned int __value));

int main(){
		//初始化两个信号量
		sem_init(&poolEmpty,0,SEM_MAXIMUM);
		sem_init(&poolFull,0,0);
//     使用时参考这个形式 
//		sem_wait(&pollEmpty); 
//		sem_wait(&pollFull);  
          
//      sem_post(&pollEmpty);
//		sem_post(&pollFull);     
//		wait和post都是原子操作  可以直接用 
	
    string   Stime = getTime();   //起始时间   
    cout << Stime << endl;
    
    pthread_t filenameReader;    // 所有定义  多文件各文件名字读入线程 
    pthread_t problemReader;    //问题读入线程 
    pthread_t problemSolver;     //解决题目线程 
    pthread_t resultPrinter;     //IO输出线程 

    problemSemInit();        // 初始化 
    resultSemInit();

    pthread_create(&filenameReader, NULL, filenameReadThread, NULL);  //创建各类线程   （这里面的参数根据你们自己写代码的函数可具体修改） 
    pthread_create(&problemReader, NULL, problemReadThread, NULL);  
    pthread_create(&problemSolver, NULL, solveManager, NULL);
    pthread_create(&resultPrinter, NULL, answerPrinter, NULL);

    pthread_join(filenameReader, NULL);
    pthread_join(problemReader, NULL);
    pthread_join(problemSolver, NULL);
    pthread_join(resultPrinter, NULL);   
    
    
    string  Etime=getTime()
    cout << Etime << endl;      //末尾时间 
    system("pause");
    return 0;
}
