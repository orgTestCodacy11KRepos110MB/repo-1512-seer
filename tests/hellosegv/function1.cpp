#include "/peak/src/Seer/hellosegv/function1.h"
#include <iostream>
#include <unistd.h>
#include <signal.h>


void function1 (const std::string& text) {

    int i = 42;

    sleep(2); // Simulate doing work.

    std::cout << text << i << std::endl;

    //raise(SIGSEGV);

    int* x = 0;

    *x = 2112;

    sleep(3); // Simulate doing work.
}

