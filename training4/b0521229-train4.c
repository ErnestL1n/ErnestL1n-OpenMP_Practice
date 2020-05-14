//author:  B0521229  Ernest Lin 
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
	
	
	//range 1-10
	for(int i=0;i<N;i++)
		array[i]=(rand()%10)+1;
	

	double start=0.0,end;
	
	
	long sum=0;
	
	
	         /****************just modify number of threads you want to use here***************************/
	#pragma omp parallel num_threads(1)
	{
		
		start=omp_get_wtime();
		
	#pragma omp for
		for(int i=0;i<N;i++){
			#pragma omp atomic  //Guarantee sum operation is atomic ,not being revised 
			sum+=array[i];
		}
		
		
		end=omp_get_wtime();
		
		
	
	printf("  Thread %3d  took %8.6f unit times\n",omp_get_thread_num(),end-start);
	
	
	}
	
	
	
	free(array);
	
	return 0;
	
	
	
	
}