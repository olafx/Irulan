#include "../include/Irulan/Dynamic.h"

int main()
{   using namespace Irulan;

    {   Dynamic::Array<float[1]> A {3};
    }

    {   Dynamic::Array<float[3]> A {3, 4, 5};
    }

    {   Dynamic::Array<float[3], Layout<packed_inc>> A {3};
    }

    {   Dynamic::Array<float[3], Allocate<false>> A {3, 4, 5};
    }
}
