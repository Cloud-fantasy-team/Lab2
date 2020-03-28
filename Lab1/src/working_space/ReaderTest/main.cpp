#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fstream>
using namespace std;

struct job{
	char board[81]; //题目
};


int main(){
    job job[1000];
    FILE *f; 
    char file[20];
    char filename[20];
    cin>>file;
    int i;
    for(i=2; i<20; i++){
        filename[i-2]=file[i];
    }
    cout<<filename<<endl;
    f=fopen(filename,"r");
    int count=0;
    for(i=0; (fgets(job[i].board,(sizeof job[i].board)+2,f) != NULL) && (i<1000); i++){
        cout<<i<<":"<<job[i].board<<endl;
        count++;
        cout<<"number of topic:"<<count<<endl;
    }
    count=i;
    cout<<"total number of topic:"<<count<<endl;
    cout<<endl;
    return 0;
}