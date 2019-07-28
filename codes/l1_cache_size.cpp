// Program to calculate L1 cache line size, compile in g++ -O1
// Platform : Intel Xeon(R) CPU E5-2667 0 @ 2.90GHz
// OS: Ubuntu 12.04
// Author : Yancy Chien

#include <iostream>
#include <string>
#include <sys/time.h>
#include <cstdlib>
using namespace std;

#define ARRAY_SIZE (256 * 1024) // arbitary array size, must in 2^N to let module work

void access_array(char* arr, int steps)
{
    const int loop_cnt = 1024 * 1024 * 32; // arbitary loop count
    int idx = 0;
    for (int i = 0; i < loop_cnt; i++)
    {
        arr[idx] += 10;
        idx = (idx + steps) & (ARRAY_SIZE - 1); // if use %, the latency will be too high to see the gap
    }
}

int main(int argc, char** argv){
    double cpu_us_used;
    struct timeval  start, end;
    
    for(int step = 1 ; step <= ARRAY_SIZE ; step *= 2){
        char* arr = new char[ARRAY_SIZE];
        for(int i = 0 ; i < ARRAY_SIZE ; i++){
            arr[i] = 0;
        }
        
        gettimeofday(&start, NULL); // get start clock
        
        access_array(arr, step);
        
        gettimeofday(&end, NULL);   // get end clock
        
        cpu_us_used = 1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);
          
        cout << step << " , " << cpu_us_used << endl;
        
        delete[] arr;
    }
    return 0;
}