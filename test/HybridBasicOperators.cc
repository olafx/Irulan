#include "../src/Hybrid.hh"

int main()
{   using namespace Irulan;

    {   Hybrid::Array<float[3][4][5]> A;
        if (A[0] != 3 || A[1] != 4 || A[2] != 5)
            return EXIT_FAILURE;
        auto a = A();
        A = 1;
        if (a != A())
            return EXIT_FAILURE;
        A = {{{1}}};
        if (a != A())
            return EXIT_FAILURE;
    }
}
