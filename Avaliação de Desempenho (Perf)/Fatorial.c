#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

uint64_t fatorial_i(uint32_t n){
    uint64_t i, r = 1;
    for(i = 2; i <= n; i++){
        r = r * i;
    }
    return r;
}

uint64_t fatorial_r(uint32_t n){
    if (n <= 1) return 1;
    else return n * fatorial_r(n-1);
}

int main(){
    uint64_t fi, fr, i;

    for(i = 0; i < 1000000; i++){
        fi = fatorial_i(15);
        fr = fatorial_r(15);
    }

    printf("fi = %" PRIu64 ", fr = %" PRIu64 "\n", fi, fr);
}

//Flat profile:

// Each sample counts as 0.01 seconds.
//   %   cumulative   self              self     total
//  time   seconds   seconds    calls  ns/call  ns/call  name
// 100.00      0.02     0.02  1000000    20.00    20.00  fatorial_r
//   0.00      0.02     0.00  1000000     0.00     0.00  fatorial_i

//  %         the percentage of the total running time of the
// time       program used by this function.

// cumulative a running sum of the number of seconds accounted
//  seconds   for by this function and those listed above it.

//  self      the number of seconds accounted for by this
// seconds    function alone.  This is the major sort for this
//            listing.

// calls      the number of times this function was invoked, if
//            this function is profiled, else blank.

//  self      the average number of milliseconds spent in this
// ms/call    function per call, if this function is profiled,
//            else blank.

//  total     the average number of milliseconds spent in this
// ms/call    function and its descendents per call, if this
//            function is profiled, else blank.

// resultado.txt
