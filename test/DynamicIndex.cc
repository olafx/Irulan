#include "../include/Irulan/Dynamic.h"

#include <iostream>

int main()
{   using namespace Irulan;

    {   Dynamic::Array<float[3]> A {3, 4, 5};
        
        if (A() != (float *) &A(0)    ||
            A() != (float *) &A(0, 0) ||
            A() != &A(0, 0, 0))
            return EXIT_FAILURE;

        A(0) = A(1);
        A(0, 0) = A(0, 1);
        A(0, 0, 0) = A(0, 0, 1) = 0;

        float *prev = A() - 1;
        for (size_t k = 0; k < A[2]; k++)
            for (size_t j = 0; j < A[1]; j++)
                for (size_t i = 0; i < A[0]; i++)
                {   if (&A(i, j, k) != prev + 1)
                        return EXIT_FAILURE;
                    prev = &A(i, j, k);
                } 
    }

    {   Dynamic::Array<int[2], Layout<packed_inc>> A {4};
        int *old = A() - 1;
        for (size_t j = 0; j < A[0]; j++)
            for (size_t i = 0; i <= j; i++)
            {   if (old + 1 != &A(i, j))
                    return EXIT_FAILURE;
                old = &A(i, j);
            }
    }

    {   Dynamic::Array<int[2], Layout<packed_dec>> A {4};
        int *old = A() - 1;
        for (size_t j = 0; j < A[0]; j++)
            for (size_t i = j; i < A[0]; i++)
            {   if (old + 1 != &A(i, j))
                    return EXIT_FAILURE;
                old = &A(i, j);
            }
    }

    {   Dynamic::Array<int[2]> A {4, 5};
        if (&A(1) != &A(0, 1))
            return EXIT_FAILURE;
    }

    {   Dynamic::Array<int[2], Layout<packed_inc>> A {4};
        if (&A(1) != &A(0, 1))
            return EXIT_FAILURE;
    }
}
