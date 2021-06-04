#include "../src/Hybrid.hh"

int main()
{   using namespace Irulan;

    {   Hybrid::Array<float[3]> A;
    }

    {   Hybrid::Array<float[3][4][5]> A;
    }

    {   Hybrid::Array<float[3][3][3], Layout<packed>> A;
    }

    {   Hybrid::Array<float[2]> A {1, 2};
    }

    {   Hybrid::Array<float[2][2]> A {{1,2}, {3,4}};
    }
}
