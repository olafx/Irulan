#include "../include/Hybrid.hh"

int main()
{   using namespace Irulan;

    {   Hybrid::Array<float[2][2]> A {{1,2}, {3}};
        if (A(0,0) != 1 || A(1,0) != 2 || A(0,1) != 3)
            return EXIT_FAILURE;
    }

    {   Hybrid::Array<float[2][2]> A;
        auto a = A();
        A = {{1, 2}, {3}};
        if (a != A())
            return EXIT_FAILURE;
        if (A(0,0) != 1 || A(1,0) != 2 || A(0,1) != 3)
            return EXIT_FAILURE;
    }
}
