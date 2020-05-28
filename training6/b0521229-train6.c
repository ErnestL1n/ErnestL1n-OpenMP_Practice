//author:  B0521229  Ernest Lin 
//for homework6
//Please write a OpenMP program to sort a list with 100,000,000 numbers, each between 0 and 99999 using bucket sort technique.
//randomly generate numbers between 0~99999



/*-----------------------------------And compare complexity to Train3 Mpi case-------------------------------------*/

#include<stdio.h>
#include<stdlib.h>
#include<omp.h>     //for OpenMP
#include<time.h>
#include<math.h>    //for sqrt
#include<string.h>  //for memset

#define N 100000000
#define randLarge 100000


//the needed header file must be included
#include<stdio.h>
#include<stdlib.h>   //for random function
#include<time.h>
#include<mpi.h>
#include<math.h>


//for qsort
int compare(const void*,const void*);

typedef struct{
	int number_of_elements;
	int index;        //0~number_of_elements-1
	int startpos;     //startposition of final array
}bucket;



int main(int argc,char *argv[]){
	
	
	
	        /****************just modify number of threads you want to use here***************************/
			 omp_set_num_threads(16);			 
	
	double start=0.0,end; 
	int *origin_array=(int*)malloc(sizeof(int)*N);
	int *final_array=(int*)malloc(sizeof(int)*N);
	itn numberofthreads;
	int number_of_buckets;
	int size_of_local_bucket;
	int workload;
	int final_index;
	int i;
	
		
			 
	 /*parallel directive=>create threads*/
	#pragma omp parallel{
		
		
	numberofthreads=omp_get_num_threads();
		
	#pragma omp	master{
		
	printf("\n\n--------------------There are %d threads created--------------------\n\n",numberofthreads);
		
	number_of_buckets=numberofthreads;   //set to equal to number of threads
		
	//array contains all local bucket For example:4 threads there are 4*4 local buckets
	//reset members to 0
	bucket* buckets=(bucket*)calloc(numberofthreads*number_of_buckets,sizeof(bucket));
	
	
	int global_boucket_count[number_of_buckets]; //number of elements in global buckets
	int global_boucket_start[number_of_buckets]; //starting position of each bucket in final array
	
	//reset to 0
	memset(global_boucket_count,0,sizeof(int)*number_of_buckets);
	memset(global_boucket_start,0,sizeof(int)*number_of_buckets);
		
	size_of_local_bucket=randLarge/number_of_buckets;
	
	workload=N/numberofthreads;
	
	
	}
	
	#pragma omp barrier
	
	start=omp_get_wtime();
	
	//generate numbers
	#pragma omp for private(i) schedule(static,workload)
		for(i=0;i<N;i++)
			origin_array[i]=rand()%randLarge;
		
	#pragma omp barrier
	
	
	int my_id = omp_get_thread_num();
	int previous_index;
	int local_index;           //local_index is the same among all threads
	int buckets_array_index;   //for buckets array
	
	
	//distributed date into local buckets among threads
	#pragma omp for private(i,local_index)
	    for(i=0;i<N;i++){
			local_index=origin_array[i]/size_of_local_bucket;
		
		buckets_array_index=local_index+my_id*number_of_buckets;
		buckets[buckets_array_index].number_of_elements++;
		}
	
	//use to count number of elements in global bucket
	int sum_of_local_elements=0;
	
	#pragma omp for private(i)
		for(i=my_id;i<numberofthreads*number_of_buckets;i+=number_of_buckets)
			sum_of_local_elements+=buckets[i].number_of_elements;
			
	global_boucket_count[my_id]=sum_of_local_elements;
	
	
	#pragma omp barrier

	
}


int compare(const void *num1, const void *num2){
	int a1=*(int*)num1;
	int a2=*(int*)num2;
	if(a1<a2)return -1;
	else if(a1==a2)return 0;
	else return 1;
}


