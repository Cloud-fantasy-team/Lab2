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

int main(){
sem_t problemEmpty;    //信号量 
sem_t problemFull;
sem_t problemMutex;
sem_t filenameEmpty;
sem_t filenameFull;
sem_t filenameMutex;
//     使用时参考这个形式 sem_wait(&filenameEmpty); sem_wait(&filenameBufferMutex);
          
//        sem_post(&filenameBufferMutex);   sem_post(&filenameBufferFull);     wait和post都是原子操作  可以直接用 
	
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
