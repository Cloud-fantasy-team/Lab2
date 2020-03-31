#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <sys/time.h>
#include <time.h>


/*任务池大小*/
#define POOL_SIZE 1000	//这个需要整体实现完之后进行调参
/*任务单元大小*/ 
#define JOB_UNIT_SIZE 100	//输入线程or解题线程以这个大小为单位一次性读取or消费这么多个题目
/*信号量数值上限*/
#define SEM_MAXIMUM 10	//这个值为任务池大小/任务单元大小
/*解题线程数量*/
#define NUM_OF_WORK_THREAD 5


//计算时间差
double time_diff(struct timeval x , struct timeval y)
{
  double x_ms , y_ms , diff;
  x_ms = (double)x.tv_sec*1000000 + (double)x.tv_usec;
  y_ms = (double)y.tv_sec*1000000 + (double)y.tv_usec;
  diff = (double)y_ms - (double)x_ms;
  if(diff<0)
  {
    fprintf(stderr, "ERROR! time_diff<0\n");
    exit(1);
  }
  return diff;
}


/*任务结构体 直接在dancinglinck里文件流输出结果 不用考虑返回结果数组了*/
struct job{
	int puzzleNo;	//题目在输入文件中的编号  在第n行就是n
	int board[81]; //题目
};


typedef struct {
  long int result;
} ThreadParas;

/*任务池*/
struct job JOB_POOL[POOL_SIZE];	

//输入文件
FILE* fp;

/*信号量	*/	//！！！	注意！！！  任务池中当前任务数量n = poolFull.currNum * JOB_UNIT_SIZE     

sem_t poolEmpty;		//任务池空 
						//--> 读取线程 当pollEmpty大于0时直接返回（空位足够） 等于0则睡眠（塞不下了）
sem_t poolFull;			//任务池满
						//--> 解题线程 当poolFull大于0时直接返回（任务足够） 等于0则睡眠（没任务）

//任务池锁
pthread_mutex_t jobPoolMutex=PTHREAD_MUTEX_INITIALIZER;

//分段式读入可能需要的位置标记
//int flag=0;


void getFileSource(){
printf("input the file name: \n");
	char fileName[20];
	//输入文件名
	scanf("%s",fileName);
	fp = fopen(fileName, "r");	//  ./sudoku test1 a  test1是输入文件名
	
	/*	char puzzle[81];
	while (fgets(puzzle, sizeof puzzle, fp) != NULL) {	//IO操作 从输入文件中按行读入puzzle
  		printf(puzzle);		//打印一下读入的数独字符串
  	}  */
}

int count=0;
int consumed=0;
//读题线程函数
void* problemReadThread(void *arg){
	
	//不断尝试读取任务
	while(1){
		//试图通过信号量poolEmpty进入临界区
		sem_wait(&poolEmpty);
		//获得任务池锁
		pthread_mutex_lock(&jobPoolMutex);
	 
		count++;
		if(consumed >= 100){ exit(0);}
		printf("[%u]: 生产者进入临界区 生产：count=%d\n",pthread_self(),count);
	 
		//释放锁
		pthread_mutex_unlock(&jobPoolMutex);
		sem_post(&poolFull); 
	}
}

//解题线程函数
void* problemSolveThread(void *i){

	//试图一直消费
	while(1){
		//试图通过信号量poolEmpty进入临界区
		sem_wait(&poolFull);
		//获得任务池锁
		pthread_mutex_lock(&jobPoolMutex);
		 
		count--;
		consumed++;
		printf("[%u]: 消费者%d进入临界区 消费：count=%d  consumed:%d\n",pthread_self(),(unsigned long)i,count,consumed);
		 
		//释放锁
		 pthread_mutex_unlock(&jobPoolMutex);
		sem_post(&poolEmpty); 
	}
}


int main(){
  struct timeval tvGenStart,tvEnd;
  
  
  	//等待输入流
  	getFileSource();
	//初始化两个信号量
	sem_init(&poolEmpty,0,SEM_MAXIMUM);
	sem_init(&poolFull,0,0); 
	
	//初始化线程
    pthread_t problemReader;    							//读题线程 
    pthread_t problemSolvers[NUM_OF_WORK_THREAD];			//解题线程
    pthread_t resultPrinter;     							//IO输出线程   
    	
    ThreadParas thPara[NUM_OF_WORK_THREAD];					//线程参数结构体数组
    
 	gettimeofday(&tvGenStart,NULL);	//记录起始时间
 
  
    pthread_create(&problemReader, NULL, problemReadThread, NULL);  //创建各类线程   （这里面的参数根据你们自己写代码的函数可具体修改）
    for(int i=0;i<NUM_OF_WORK_THREAD;i++){
    	pthread_create(&problemSolvers[i], NULL, problemSolveThread, (void*)i);
    }



    pthread_join(problemReader, NULL);
     for(int i=0;i<NUM_OF_WORK_THREAD;i++){
    	pthread_join(problemSolvers[i], NULL);
    }
    
  

  printf("Generating input jobs ...\n");

	for(int i=0;i<1000000;i++){}
  gettimeofday(&tvEnd,NULL);
  printf("Generating input jobs done. Spend %.5lf s to finish.",time_diff(tvGenStart,tvEnd)/1E6);
    return 0;
}
