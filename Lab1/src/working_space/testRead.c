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
using namespace std;

struct job_t{
	int puzzleNo;	//题目在输入文件中的编号  在第n行就是n
	int board[81]; //题目
};
queue<struct job_t> JOB_POOL;		//改用队列这一数据结构
int main(){
	//输入文件
	FILE* fp;
	char fileName[20];
	//输入文件名
	scanf("%s",fileName);
	fp = fopen(fileName, "r");	//  ./sudoku test1 a  test1是输入文件名
	char puzzle[85];
/*	while (fgets(puzzle, sizeof puzzle, fp) != NULL) {	
  		printf(puzzle);		//打印一下读入的数独字符串
  		char tmp;	
		scanf("%c",&tmp);
		if(puzzle[0]=='5') break;
		
    }	
    
    printf("break...\n");
     //fseek(fp, 5L*4,SEEK_SET); 
     while (fgets(puzzle, sizeof puzzle, fp) != NULL) {	
  		printf(puzzle);		//打印一下读入的数独字符串
  		char tmp;	
		scanf("%c",&tmp);
		
    } */
  /*  if (fgets(puzzle, sizeof puzzle, fp) != NULL) {	
  		printf(puzzle);		//打印一下读入的数独字符串
		job_t newJob;		//构造一个新任务			
		newJob.puzzleNo=1;		//获取当前任务在文件中的行数
		for(int i=0;i<81;i++){	//把题目从字符串转成int型
			newJob.board[i]=puzzle[i]-'0';
		}
		JOB_POOL.push(newJob);
    }	
    
    job_t t = JOB_POOL.front();
	printf("获取任务成功:\n %d\n",t.puzzleNo);
	for(int i=0;i<81;i++){
		printf("%d",t.board[i]);
	} */
	
	int i=1;
	int j=i++;
	printf("j=%d i=%d\n",j,i);
	
	
}
