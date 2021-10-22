#include "../include/Hybrid.hh"

int main()
{   using namespace Irulan;

    {   Hybrid::Array<float[3][4][5]> A;
        
        if (A() != (float *) &A(0)    ||
            A() != (float *) &A(0, 0) ||
            A() != &A(0, 0, 0))
            return EXIT_FAILURE;

        A(0) = A(1);
        A(0, 0) = A(0, 1);
        A(0, 0, 0) = A(0, 0, 1) = 0;

        float *prev {A() - 1};
        for (size_t k {}; k < A[2]; k++)
            for (size_t j {}; j < A[1]; j++)
                for (size_t i {}; i < A[0]; i++)
                {   if (&A(i, j, k) != prev + 1)
                        return EXIT_FAILURE;
                    prev = &A(i, j, k);
                }
                    
    }
}
