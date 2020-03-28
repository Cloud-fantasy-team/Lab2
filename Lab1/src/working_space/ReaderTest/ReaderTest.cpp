/*
本文件为读题程序的测试程序，实现的功能有：
1.一次性读取固定数量的题目（数量不足时读取剩余所有题目）
2.讲读题程序封装，可以传参
*/

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <iostream>

#include <thread>

using namespace std;

/*任务池大小*/
#define POOL_SIZE 500	//这个需要整体实现完之后进行调参



struct job{
	int board[81]; //题目
};


void translate(int board[],char j[]){
    for(int i=0;i<81;i++){
        board[i]=j[i]-'0';
    }
}

void threadReader(FILE* f,job job[]) {      //一次性读入1000个题目或者将剩余题目全部读取
  int i;
  char j[81];
//   std::cout<<"size is:"<<sizeof j<<std::endl;
  for(i=0; (fgets(j,(sizeof j)+2,f) != NULL) && (i<POOL_SIZE); i++){
        // std::cout<<"char is:"<<j<<std::endl;     //调试输出读入的题目
        // //count++;
        // //std::cout<<"number of topic:"<<i<<std::endl;
        translate(job[i].board,j);
        // std::cout<<"int is:";
        // for(int j=0;j<81;j++) cout<<job[i].board[j];  //输出转换后的int型题目
        // cout<<endl;
    }
  std::cout<<"total number of topic:"<<i<<std::endl;
}



int main(int argc, char* argv[])
{

  job job[POOL_SIZE];  //创建1000个保存题目的结构体
  char file[20];  //输入的文件
  char filename[20];  //文件名
  std::cin>>file;
  for(int i=2; i<20; i++){           //获取文件名
        filename[i-2]=file[i];
    }
  FILE *f=fopen(filename,"r");
  std::thread read(threadReader, f, std::ref(job));
  read.join();
//   cout<<"thread successfully exit!"<<endl;




  return 0;
}
