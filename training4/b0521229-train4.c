//author:Ernest Lin 
//for homework4 A MOTIVATING EXAMPLE 
//OpenMP program
//Please declare an array with 100,000,000 elements
//the value (range: 1-10) of each elements is randomly generated
//and then write a OpenMP program to calculate the sum by using share memory programming. 
//(1, 2, 4, 8, 16, 32 processors)


#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include<time.h>


#define N 100000000


int main(int argc,char *argv[]){
	
	int *array=(int*)malloc(sizeof(int)*N);
	
	int numberofthreads;
	
	int i;
	
	
	//range 1-10
	for(i=0;i<N;i++)
		array[i]=(rand()%10)+1;
	

	double start=0.0,end;
	
	long sum=0;
	
	
	//for load balancing
	long chunk=1000000;
	
	         /****************just modify number of threads you want to use here***************************/
			 omp_set_num_threads(32);
			 
			 
			 
    /*parallel directive=>create threads*/
	#pragma omp parallel 
	{
		numberofthreads=omp_get_num_threads();
		
	#pragma omp	master
	printf("\n\n--------------------There are %d threads created--------------------\n\n",numberofthreads);
		
		start=omp_get_wtime();
		
	/*------GUIDED: Similar to DYNAMIC except chunk size decreases over time (better load balancing)------*/	
	#pragma omp for private(i) \
	                    schedule(guided,chunk) nowait reduction(+:sum)
						/*using reduction to guarantee atomicity and increase performance*/
		for(i=0;i<N;i++)
			sum+=array[i];
		
		
		/*
		for(i=0;i<N;i++)
			sum+=(i+1);
		//use to check sum is correct
		*/
		
		end=omp_get_wtime();
		
		
	
	printf("                Thread %3d  took %8.6f unit times\n",omp_get_thread_num(),end-start);
	
	
	}
	
	
	
	free(array);
	
	return 0;
	
	
	
	
}
