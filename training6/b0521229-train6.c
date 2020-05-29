//author:  B0521229  Ernest Lin 
//for homework6
//Please write a OpenMP program to sort a list with 100,000,000 numbers, each between 0 and 99999 using bucket sort technique.
//randomly generate numbers between 0~99999

/* bug fixed 2020-5-29 */

/*--------------------------------------And compare complexity to Train3 Mpi case--------------------------------------------*/

#include<stdio.h>
#include<stdlib.h>
#include<omp.h>     //for OpenMP
#include<time.h>
#include<math.h>    //for sqrt
#include<string.h>  //for memset

#define N 100000000        
#define randLarge 100000 




/********************************just modify number of threads you want to use here*******************************************/
#define threads 16




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
	
	
	
	        
	omp_set_num_threads(threads);			 
	
	double start=0.0,end; 
	int *origin_array=(int*)calloc(N,sizeof(int));
	int *final_array=(int*)calloc(N,sizeof(int));
	int numberofthreads=threads;
	int number_of_buckets=numberofthreads;   //set to equal to number of threads
	int size_of_local_bucket;
	int final_index;
	int i;
	
		
		
	//array contains all local bucket For example:4 threads there are 4*4 local buckets ,thus local buckets are no.0~15
	//reset members to 0
	bucket* buckets=(bucket*)calloc(numberofthreads*number_of_buckets,sizeof(bucket));
	
	
	//4 threads=>4 large buckets
	int global_boucket_count[number_of_buckets]; //number of elements in global(large) buckets
	int global_boucket_start[number_of_buckets]; //starting position of each bucket in final array
	
	//reset to 0
	memset(global_boucket_count,0,sizeof(int)*number_of_buckets);
	memset(global_boucket_start,0,sizeof(int)*number_of_buckets);
		
	size_of_local_bucket=randLarge/number_of_buckets;
	
	
	
	/*                ---------------------------    step1      --------------------------------              */   
	//generate numbers,not in parallel part
	
	for(i=0;i<N;i++)
			origin_array[i]=rand()%randLarge;
	
	/*parallel directive=>create threads*/
	
	#pragma omp parallel
	{
		
	//local variable !!  my bug appears here....................Don't be lazy to declare another variable
	int j;
	
	#pragma omp	master
	printf("\n\n--------------------There are %d threads created--------------------\n\n",numberofthreads);
	
	#pragma omp barrier
	
	
	
	/*                                            Times starts                                                */
	start=omp_get_wtime();
	
	int my_id = omp_get_thread_num();
	int previous_index;
	int local_index;           //local_index is the same among all threads
	int buckets_array_index;   //for buckets array
	
	
	
	
	/*                ---------------------------    step2      --------------------------------              */
	//distributed date into local buckets among threads

	#pragma omp for private(i,local_index) 
	    for(i=0;i<N;i++){
			local_index=origin_array[i]/size_of_local_bucket;
		if (local_index > number_of_buckets-1)
                local_index = number_of_buckets-1;
		buckets_array_index=local_index+my_id*number_of_buckets;
		buckets[buckets_array_index].number_of_elements++;
		}


	//use to count number of elements in global bucket
	int sum_of_local_elements=0;
	
		for(j=my_id;j<numberofthreads*number_of_buckets;j+=numberofthreads)
			sum_of_local_elements+=buckets[j].number_of_elements;
			
	global_boucket_count[my_id]=sum_of_local_elements;

	#pragma omp barrier



	/*                ---------------------------    step3      --------------------------------              */
	//Merge small buckets to large bucket
	
	//take example for 4 threads
	//first loop is done by the master thread 0
    //which initializes the starting position for both local and global buckets in final array
	
    #pragma omp master
	{   //handle buckets[0~2]
		for(j=1;j<number_of_buckets;j++){
			global_boucket_start[j]=global_boucket_start[j-1]+global_boucket_count[j-1];
			buckets[j].startpos=buckets[j-1].startpos+ global_boucket_count[j-1];
			buckets[j].index   =buckets[j-1].index   + global_boucket_count[j-1];
	}
 	}
	
	#pragma omp barrier   //wait for master thread
	
	
	
	
	
	
	
	/*                ---------------------------    step4      --------------------------------              */
	//count number of elements in global bucket
	//second loop is done in parallel,since thread j can set up the right starting position in final array for each local bucket
	//thread 0  buckets:4  8 12
	//thread 1  buckets:5  9 13
   	//thread 2  buckets:6 10 14
    //thread 3  buckets:7 11 15
	
	for (j=my_id+number_of_buckets; j< number_of_buckets*numberofthreads; j+=numberofthreads){
		int prevoius_index  = j-number_of_buckets;
		buckets[j].startpos = buckets[prevoius_index].startpos + buckets[prevoius_index].number_of_elements;
		buckets[j].index    = buckets[prevoius_index].index    + buckets[prevoius_index].number_of_elements;	
	}
	
	#pragma omp barrier   
	
	
	
	
	
	
	
	
	/*                ---------------------------    step5      --------------------------------              */
	//now we can write data into final array, since we have all positions
	#pragma omp for private(i, final_index) 
    for (i=0; i< N ;i++){
        int temp = origin_array[i]/size_of_local_bucket;
		if (temp > number_of_buckets-1)
                temp = number_of_buckets-1;
		
        int n = temp+ my_id*number_of_buckets;
        final_index = buckets[n].index++;
        final_array[final_index] = origin_array[i];
    }
	
	
	#pragma omp for private(i) 
    for(i=0; i<number_of_buckets; i++)
        qsort(final_array+global_boucket_start[i], global_boucket_count[i], sizeof(int), compare);
	
	
	
	
	/*                                            Times ends                                                 */
	end=omp_get_wtime();
	
	
	printf("                Thread %3d  took %8.6f unit times\n",omp_get_thread_num(),end-start);
	
	
	
	/*                                            Check results                                              */
	
	
}

		char sorted = 'Y';
		
		for(i = 0; i < N - 2; i++){
		if(final_array[i] > final_array[i+1]){
				sorted = 'N';
				break;
			}			
		}
		if(sorted == 'Y'){
			printf("\nSORTING is CORRECT\n");
		}else{
			printf("\nSORTING is NOT CORRECT\n");
		}
		printf("\n-----------------------------------------------------------------\n");

    free(buckets);
	free(origin_array);
	free(final_array);
	
}
int compare(const void *num1, const void *num2){
	int a1=*(int*)num1;
	int a2=*(int*)num2;
	if(a1<a2)return -1;
	else if(a1==a2)return 0;
	else return 1;
}