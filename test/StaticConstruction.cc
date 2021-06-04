#include "../src/Static.hh"

int main()
{   using namespace Irulan;

    {   Static::Array<float[3]> A;
    }

    {   Static::Array<float[3][4][5]> A;
    }

    {   Static::Array<float[3][3][3], Layout<packed>> A;
    }

    {   constexpr Static::Array<float[3][4][5]> A {};
    }

    {   Static::Array<float[2]> A {1, 2};
    }

    {   Static::Array<float[2][2]> A {{1,2}, {3,4}};
    }

    {   constexpr Static::Array<float[2][2]> A {{1,2}, {3,4}};
    }
}
