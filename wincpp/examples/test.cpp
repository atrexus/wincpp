#include <iostream>

#include "process.hpp"

int main()
{
    // print the process id and name of each process

    const auto process = wincpp::process::current();
    return 0;
}