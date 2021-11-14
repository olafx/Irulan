#include "../include/Irulan/Dynamic.h"

int main()
{   using namespace Irulan;

    {   Dynamic::Array<double[1], EfficientShape<true>> A {10};
        if (sizeof(A) != sizeof(double *))
            return EXIT_FAILURE;
    }

    {   Dynamic::Array<double[3], EfficientShape<true>> A {2, 3, 4};
        if (sizeof(A) != sizeof(double *) + 2 * sizeof(decltype(A)::size_type))
            return EXIT_FAILURE;
        if (A[0] != 2 || A[1] != 3)
            return EXIT_FAILURE;
    }

    {   Dynamic::Array<double[1], EfficientShape<true>> A;
    }
}
