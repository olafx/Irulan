#include "../src/Dynamic.hh"

int main()
{   using namespace Irulan;

    {   Dynamic::Array<float[1]> A {3};
    }

    {   Dynamic::Array<float[3]> A {3, 4, 5};
    }

    {   Dynamic::Array<float[3], Layout<packed>> A {3};
    }
}
