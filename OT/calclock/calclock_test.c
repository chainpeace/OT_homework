#include "calclock.h"
#include <stdio.h>

unsigned long long a_time;
unsigned long long a_count;

unsigned long long b_time;
unsigned long long b_count;

unsigned long long c_time;
unsigned long long c_count;

int mem[1000];
void init_mem(){
	for(int i = 0; i < 1000; i++){
		mem[i] = 0;
	}
	return;
}


int fibo_c(int n){
	struct timespec local_time[2];
	clock_gettime(CLOCK_REALTIME, &local_time[0]);
	if (n<=0){
		mem[0] = 0;
		// clock_gettime(CLOCK_REALTIME, &local_time[1]);
		// calclock(local_time, &c_time, &c_count);
		return mem[0];
		}
	else if(n==1){
		mem[1] = 1;
		// clock_gettime(CLOCK_REALTIME, &local_time[1]);
		// calclock(local_time, &c_time, &c_count);
		return mem[1];
		}
	else{
		if(mem[n] != 0){
			// clock_gettime(CLOCK_REALTIME, &local_time[1]);
			// calclock(local_time, &c_time, &c_count);
			return mem[n];
		}
		else{
			mem[n] = fibo_c(n-1) + fibo_c(n-2);
			clock_gettime(CLOCK_REALTIME, &local_time[1]);
			calclock(local_time, &c_time, &c_count);
			return mem[n];
		}
	}
}

int fibo_b(int n){
	int a = 0;
	int b = 1;
	int tmp;
	struct timespec local_time[2];
	clock_gettime(CLOCK_REALTIME, &local_time[0]);


	if(n<=0){
		clock_gettime(CLOCK_REALTIME, &local_time[1]);
		calclock(local_time, &b_time, &b_count);
		
		return a;
	}
	else if(n==1){
		clock_gettime(CLOCK_REALTIME, &local_time[1]);
		calclock(local_time, &b_time, &b_count);
		
		return b;
	}
	else{
		for(int i = 0; i < n-1; i++){
			tmp = a;
			a = b;
			b = tmp + b;	
		}
		clock_gettime(CLOCK_REALTIME, &local_time[1]);
		calclock(local_time, &b_time, &b_count);
		return b; 
	}
}


int fibo_a_internal(int n){
	if(n<=0)
		return 0;
	else if(n==1)
		return 1;
	else
		return fibo_a_internal(n-1) + fibo_a_internal(n-2); 
}

int fibo_a(int n){
	int ret;
	struct timespec local_time[2];
	//getrawmonotonic(&local_time[0]);
	clock_gettime(CLOCK_REALTIME, &local_time[0]);
	ret = fibo_a_internal(n);
	//getrawmonotonic(&local_time[1]);
	clock_gettime(CLOCK_REALTIME, &local_time[1]);
	calclock(local_time, &a_time, &a_count);
	return ret;
}

int main(){
	int a = 0;
	int b = 0;
	int c = 0;
	init_mem();

	for(int i = 0; i < 46; i++){
		a = fibo_a(i);
		b = fibo_b(i);
		c = fibo_c(i);
		printf("%dth fibo # : %d, %d, %d\n", i, a, b, c); 
	}

	printf("fibo_a time: %llu ms, count : %llu\n", a_time/1000000, a_count); 
	printf("fibo_b time: %llu ns, count : %llu\n", b_time, b_count); 
	printf("fibo_c time: %llu ns, count : %llu\n", c_time, c_count); 
	return 0;
}

