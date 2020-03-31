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
char* Int2String(int num,char *str)//10进制 
{
	int i = 0;//指示填充str 
	if(num<0)//如果num为负数，将num变正 
	{
		num = -num;
		str[i++] = '-';
	} 
	//转换 
	do
	{
		str[i++] = num%10+48;//取num最低位 字符0~9的ASCII码是48~57；简单来说数字0+48=48，ASCII码对应字符'0' 
		num /= 10;//去掉最低位	
	}while(num);//num不为0继续循环
	
	str[i] = '\0';
	
	//确定开始调整的位置 
	int j = 0;
	if(str[0]=='-')//如果有负号，负号不用调整 
	{
		j = 1;//从第二位开始调整 
		++i;//由于有负号，所以交换的对称轴也要后移1位 
	}
	//对称交换 
	for(;j<i/2;j++)
	{
		//对称交换两端的值 其实就是省下中间变量交换a+b的值：a=a+b;b=a-b;a=a-b; 
		str[j] = str[j] + str[i-1-j];
		str[i-1-j] = str[j] - str[i-1-j];
		str[j] = str[j] - str[i-1-j];
	} 
	
	return str;//返回转换后的值 
}

int main(){
	int POOL_SIZE =1;
	int JOB_UNIT_SIZE =2;
	int NUM_OF_WORK_THREAD =3;
	int TEST_NO =4;
	char outFileName[50];
	//输入文件名
	//scanf("%s",outFileName);
	char tmp2[5]="PS=";
	//拼接输出文件名
	strcat(outFileName,tmp2);	//POOL_SIZE
	//char tmp[50]="";
	//Int2String(POOL_SIZE,tmp);
	
	//strcat(outFileName,tmp);
	/*	
	strcat(outFileName,"_JUS=");	//JOB_UNIT_SIZE
	snprintf(tmp, sizeof(tmp), "%d", JOB_UNIT_SIZE);
	strcat(outFileName,tmp);
	
	strcat(outFileName,"_NT=");	//NUM_OF_WORK_THREAD
	snprintf(tmp, sizeof(tmp), "%d", NUM_OF_WORK_THREAD);
	strcat(outFileName,tmp);
	
	strcat(outFileName,"_");	//TEST_NO
	snprintf(tmp, sizeof(tmp), "%d", TEST_NO);
	strcat(outFileName,tmp);
	*/
	printf("%s",outFileName);
}
