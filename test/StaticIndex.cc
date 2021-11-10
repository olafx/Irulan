#include "../include/Irulan/Static.h"

int main()
{   using namespace Irulan;

    {   Static::Array<float[3][4][5]> A;
        
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

    {   Static::Array<int[4][4], Layout<packed_inc>> A;
        int *old = A() - 1;
        for (size_t j = 0; j < A[0]; j++)
            for (size_t i = 0; i <= j; i++)
            {   if (old + 1 != &A(i, j))
                    return EXIT_FAILURE;
                old = &A(i, j);
            }
    }

    {   Static::Array<int[4][4], Layout<packed_dec>> A;
        int *old = A() - 1;
        for (size_t j = 0; j < A[0]; j++)
            for (size_t i = j; i < A[0]; i++)
            {   if (old + 1 != &A(i, j))
                    return EXIT_FAILURE;
                old = &A(i, j);
            }
    }

    {   Static::Array<int[4][5]> A;
        if (reinterpret_cast<int *>(&A(1)) != &A(0, 1))
            return EXIT_FAILURE;
    }

    {   Static::Array<int[4][4], Layout<packed_inc>> A;
        if (&A(1) != &A(0, 1))
            return EXIT_FAILURE;
    }
}
