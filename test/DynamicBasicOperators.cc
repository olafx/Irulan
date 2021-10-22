#include "../include/Dynamic.hh"

int main()
{   using namespace Irulan;

    {   Dynamic::Array<float[3]> A {3, 4, 5};
        if (A[0] != 3 || A[1] != 4 || A[2] != 5)
            return EXIT_FAILURE;
        auto a = A();
        A = 1;
        if (a != A())
            return EXIT_FAILURE;
    }
}
