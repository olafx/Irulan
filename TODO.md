Fix all static arrays from needing to be 0 initialized even if a full initializer list is given. The issue is that the initializer
list must be in the constructor initializer list somehow, because otherwise it won't qualify as constexpr. There are also issues
with C++ accepting initializer lists as constexpr at all, so this is quite tricky to get to work at all let alone elegantly. This
will require lots of testing with simplified cases.

Try to find a way to remove the order from the template of Dynamic::Arrays while keeping order compile time. This essentially
means the type of the Dynamic::Array will be dependent on constexpr properties of its constructor arguments. In theory this is
possible, but it's extremely difficult to do. This will also require lots of testing with simplified cases.

Generalize packed matrix indexing to packed tensor indexing theoretically. Once I understand the math, I should be able to implement it too.

Add compile time checks for misuse where possible.

Constructors and operator= for similar, yet different tensors.

Constructors and operator= between tensors of different types.

Row major support. This is quite tricky since a lot of features silently assume column major support, but I've been noting which ones do.

Aligned allocation support for Hybrid::Array and Dynamic::Array. Needs complete client control.

CUDA host, device, and shared allocation support for Hybrid::Array and Dynamic::Array.

Implement more of the BLAS, LAPACK, and LINPACK layouts.
