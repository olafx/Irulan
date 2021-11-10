#include "../include/Irulan/Dynamic.h"

#include <iostream>

int main()
{   using namespace Irulan;

    Dynamic::Array<double[3], Allocate<false>> A {3, 4, 5};
    double *a;
    if (A.allocate)
        return EXIT_FAILURE;
    if (A() != NULL)
        return EXIT_FAILURE;
    A() = a;
    if (a != A())
        return EXIT_FAILURE;
}
