#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <sys/time.h>
#include <time.h>
#include <queue>
#include <vector>

#include "sudoku.h"
using namespace std;

/*任务池大小*/
int POOL_SIZE = 1000;
//#define POOL_SIZE 1000	//这个需要整体实现完之后进行调参
/*任务单元大小*/ 
int JOB_UNIT_SIZE = 10;
//#define JOB_UNIT_SIZE 10	//输入线程or解题线程以这个大小为单位一次性读取or消费这么多个题目
/*信号量数值上限*/
int SEM_MAXIMUM = 100;
//#define SEM_MAXIMUM 100	//这个值为任务池大小/任务单元大小
/*解题线程数量*/
int NUM_OF_WORK_THREAD = 2;
//#define NUM_OF_WORK_THREAD 2

bool (*solve)(int,job_t&) = solve_sudoku_dancing_links_2;
int total_solved = 0;


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


/*任务结构体 直接在dancinglinck里文件流输出结果 不用考虑返回结果数组了
struct job_t{
	int puzzleNo;	//题目在输入文件中的编号  在第n行就是n
	int board[81]; //题目
};
*/

typedef struct {
  long int result;
} ThreadParas;

/*任务池*/
//struct job JOB_POOL[POOL_SIZE];	
queue<struct job_t> JOB_POOL;		//改用队列这一数据结构
priority_queue<struct job_t,vector<struct job_t>,cmp> SOLVE_POOL;  //答案队列->sudoku.h
//任务池锁
pthread_mutex_t jobPoolMutex=PTHREAD_MUTEX_INITIALIZER;
//解题计数变量锁
pthread_mutex_t totalSolvedCountMutex=PTHREAD_MUTEX_INITIALIZER;
//输入文件
FILE* fp;
long int jobReadCount=0;	//记录本文件已经读入了多少个任务


/*信号量	*/				//任务池中当前任务数量n = poolFull.currNum * JOB_UNIT_SIZE     

sem_t poolEmpty;		//任务池空 
						//--> 读取线程 当pollEmpty大于0时直接返回（空位足够） 等于0则睡眠（塞不下了）
sem_t poolFull;			//任务池满
						//--> 解题线程 当poolFull大于0时直接返回（任务足够） 等于0则睡眠（没任务）



//分段式读入可能需要的位置标记	用不上
//int last_few_jobs_flag=0;		//flag为0表示最后一次读入的任务数量是JOB_UNIT_SIZE 为1表示已经到末尾不足JOB_UNIT_SIZE个任务了


void getFileSource(){
printf("input the file name: \n");
	char fileName[20];
	//输入文件名
	scanf("%s",fileName);
	fp = fopen(fileName, "r");	//  ./sudoku test1 a  test1是输入文件名
}



//读题线程函数
void* problemReadThread(void *arg){
	
	//不断尝试读取任务
	while(1){
		//试图通过信号量poolEmpty进入临界区
		sem_wait(&poolEmpty);
		//获得任务池锁
		pthread_mutex_lock(&jobPoolMutex);
	 
	 	//以JOB_UNIT_SIZE为目标从文件中读取 如果没读到这么多 那么就修改flag为1
	 	char puzzle[85];
	 	int readInCount=0;
	 	//IO操作 从输入文件中按行读入puzzle
		while (fgets(puzzle, sizeof puzzle, fp) != NULL) {	
	  		//printf(puzzle);		//打印一下读入的数独字符串
	  		if (strlen(puzzle) >= 81) {		
				job_t newJob;				//构造一个新任务			
				newJob.puzzleNo=jobReadCount++;//获取当前任务在文件中的行数 之后迭代计数
				for(int i=0;i<81;i++){		//把题目从字符串转成int型
					newJob.board[i]=puzzle[i]-'0';
				}
				JOB_POOL.push(newJob);	//新job进入队列
				printf("新job进入队列 puzzleNo: %d\n",newJob.puzzleNo);
				readInCount++;			//本次读入任务计数迭代
				//成功读完一个单位数量的任务
				if(readInCount>=JOB_UNIT_SIZE){ 
					printf("newJob.puzzleNo=%d readInCount=%d JOB_UNIT_SIZE=%d\n",newJob.puzzleNo,readInCount,JOB_UNIT_SIZE);
					break;
				}		
			}	
  		}
  		//释放锁
		pthread_mutex_unlock(&jobPoolMutex);
		printf("生产者释放锁\n");
		//poolFull.post 相当于任务池又新增了1个单位的任务 具体数量<=JOB_UNIT_SIZE
		sem_post(&poolFull); 
		
  		//不完整的单位  意味着文件已经全都读完了 这一部分是零头  
  		if(readInCount<JOB_UNIT_SIZE){
  			//任务已经读取完毕 那么读题线程该歇逼了
  			printf("读题线程结束\n");
  			return 0;
  			//解题线程总是会试图从队列头部拿出JOB_UNIT_SIZE个任务
  			//当没有这么多的时候 有多少拿多少
  		} 
	}
}

//解题线程函数
void* problemSolveThread(void *threadNo){

	//试图一直消费
	while(1){
		//试图通过信号量poolEmpty进入临界区
		sem_wait(&poolFull);
		//获得任务池锁
		pthread_mutex_lock(&jobPoolMutex);
		 
		//试图获取JOB_UNIT_SIZE个任务
		job_t t[JOB_UNIT_SIZE];
		int jobGetCount=0;
		while(1){
			if(JOB_POOL.size()<=0){
				//这时候队列如果为空了  说明此时文件中所有任务都已经被拿完了	
				//任务池不满JOB_UNIT_SIZE时，有可能是任务全部完成，有可能是还存在不满JOB_UNIT_SIZE的任务
				//将任务池剩余的任务处理

				for(int i=0;i<jobGetCount;i++){
					printf("[%u]: 消费者%d正在解题 puzzleNO: %d\n",pthread_self(),(unsigned long)threadNo,t[i].puzzleNo);
					//如果求解成功返回了true
				  	if (solve(0,t[i])) {		
				  	//成功求解计数增加
				  		pthread_mutex_lock(&totalSolvedCountMutex);
						++total_solved;
						SOLVE_POOL.push(t[i]);
						pthread_mutex_unlock(&totalSolvedCountMutex);
				  	}
				  	else {//solve返回了false 表示无解
						printf("No solution:\n\t");	//该题目无解 并打印对应题目
						for(int j=0;j<81;j++){
							printf("%d",t[i].board[j]);
						}
						printf("\n");
				  	}
				}
			  	if(jobGetCount>0){
					printf("[%u]: 消费者%d最后的零头任务解题完毕！！！\n",pthread_self(),(unsigned long)threadNo);
					printf("[%u]: 消费者%d退出  通知其他在等待的解题线程退出\n",pthread_self(),(unsigned long)threadNo);	
				}
				else{
					printf("[%u]: 消费者%d发现任务队列已空 退出并通知其他在等待的解题线程退出\n",pthread_self(),(unsigned long)threadNo);
				}
				//释放锁，并且将信号量poolFull增加，使得沉睡的消费者线程苏醒并退出
				pthread_mutex_unlock(&jobPoolMutex);
				sem_post(&poolFull); 

				return 0;
			}
			
			t[jobGetCount++] = JOB_POOL.front();
			JOB_POOL.pop();
			

			//读满了JOB_UNIT_SIZE个
			if(jobGetCount>=JOB_UNIT_SIZE){
				break;
			}
		}
		//释放锁 让其他线程进入临界区获取任务
		pthread_mutex_unlock(&jobPoolMutex);
		sem_post(&poolEmpty);
		
		//真正的解题过程
		for(int i=0;i<jobGetCount;i++){
			printf("[%u]: 消费者%d正在解题 puzzleNO: %d\n",pthread_self(),(unsigned long)threadNo,t[i].puzzleNo);
			//如果求解成功返回了true
			if (solve(0,t[i])) {		
				//成功求解计数增加
				pthread_mutex_lock(&totalSolvedCountMutex);
				++total_solved;
				SOLVE_POOL.push(t[i]);
				pthread_mutex_unlock(&totalSolvedCountMutex);
			}
			else {//solve返回了false 表示无解
				printf("No solution:\n\t");	//该题目无解 并打印对应题目
				for(int j=0;j<81;j++){
					printf("%d",t[i].board[j]);
				}
				printf("\n");
			}
		} 
	}
}


int main(int argc, char* argv){
  	struct timeval tvStart,tvEnd;
	
  	if (argv[1] != NULL){
		//POOL_SIZE = (int)(argv[1] - '0');
		POOL_SIZE = atoi(argv[1]);
	}
	if (argv[2] != NULL){
		//JOB_UNIT_SIZE = (int)(argv[2] - '0');
		JOB_UNIT_SIZE = atoi(argv[2]);
		SEM_MAXIMUM = POOL_SIZE/JOB_UNIT_SIZE;
	}
 	if (argv[3] != NULL){
		//NUM_OF_WORK_THREAD = (int)(argv[3] - '0');
		NUM_OF_WORK_THREAD = atoi(argv[3]);
	}
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
    
 	gettimeofday(&tvStart,NULL);	//记录起始时间
 
  
    pthread_create(&problemReader, NULL, problemReadThread, NULL);  //创建各类线程   （这里面的参数根据你们自己写代码的函数可具体修改）
    for(int i=0;i<NUM_OF_WORK_THREAD;i++){
    	pthread_create(&problemSolvers[i], NULL, problemSolveThread, (void*)i);
    }



    pthread_join(problemReader, NULL);
    for(int i=0;i<NUM_OF_WORK_THREAD;i++){
    	pthread_join(problemSolvers[i], NULL);
    }


  	gettimeofday(&tvEnd,NULL);
  	printf("Process finished. Spend %.5lf s to finish.",time_diff(tvStart,tvEnd)/1E6);
	printf("total solved:%d\n",total_solved);
	for(;!SOLVE_POOL.empty();SOLVE_POOL.pop()){
		job_t jobb = SOLVE_POOL.top();
		printf("job ID:%d ",jobb.puzzleNo);
		for(int i = 0; i < N; ++i){
			printf("%d",jobb.board[i]);
		}
		printf("\n");
	}
	//printf("POOL_SIZE: %d JOB_UNIT_SIZE: %d SEM_MAXIMUM: %d NUM_OF_WORK_THREAD: %d",POOL_SIZE, JOB_UNIT_SIZE, SEM_MAXIMUM, NUM_OF_WORK_THREAD);
    return 0;
}
