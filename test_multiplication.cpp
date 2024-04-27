#include <iostream>
#include <chrono>
#include <omp.h>

int main() {

    #pragma omp parallel for
    
    for ( int i = 0 ; i < 4 ; i++ ) {
        std::cout << "Thread " << omp_get_thread_num() << " is working on iteration " << i << std::endl;
    }
    return 0;
}