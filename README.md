# Irulan

**C++17 multidimensional array (tensor) data structure library.**

**This project is WIP!**

I wrote this library to make numerical codes easier to write in CUDA/C++.

It provides tensor data structures, completely recursively defined, and of various layouts as to work in parallel with CBLAS, cuBLAS, LAPACK, etc.

What sets this library apart from other tensor libraries is twofold:
- Total control over data.
- No math.

So it's really a tool for data oriented design.

```C++
using namespace Irulan;
```



## Types

### Static

Compile time size, direct storage (stack).

```C++
Static::Array<int[3][4][5][6]> A;
```

For regular dense tensors, this stores a C-style array of lower order tensors, so you get nice indexed types.

```C++
Static::Array<int[3][4]> C = A(0, 0);
```

### Dynamic

Run time size. Compile time order.

```C++
Dynamic::Array<int[3]> A {3840, 3840, 3};
```

Dimensions are stored on stack in the object. Data is stored on heap.



## Property System

Properties are templated. They can be given in any order. All have implicit defaults.

```C++
Static::Array<int[3]> {};
Static::Array<int[3], Layout<conventional>> {};
Static::Array<Layout<conventional>, SizeType<size_t>, int[3], Allocate<true>> {};
//  each of these can safely be reinterpret casted to the type of another
```



## Access

### Indexing

Elements are accessed by Fortran-style bracket indexing (except 0-based).

```C++
Static::Array<float[3][4]> A;
A(0, 1) = 3;
```

If less indicides are given that the array's order, it will correctly interpret this and return for example the beginning of a column.

```C++
Static::Array<int[2][3]> A;
Dynamic::Array<int[2]> B {2, 3};

Static::Array<int[2]> A1 = A(1);
int *B1 = &B(1);

// B1 points to the 2nd column, not 2nd element, so A1 and B1 are consistent
```

### Dimensions

The square bracket operator is used to get dimensions.

```C++
Static::Array<int[3][4][5]> A;
A[1]; // 4
```

For dynamic tensors the dimension can also be set (i.e. the tensor can be reinterpreted).
It's designed to be used like this, but there is no safety, it will never reallocate.

```C++
Dynamic::Array<int[3]> A {16, 16, 3};
A[0] = 4; A[1] = 6; A[2] = 24;
// works fine since 4 * 6 * 24 <= 16 * 16 * 3
```

### Pointer

In the spirit of working in parallel with other libraries, a raw pointer can easily be returned.

```C++
#include <fftw3.h>
#include <vtkNew>
#include <vtkImageImport>
Dynamic::Array<fftw_complex[3]> in {512, 512, 256}, out {512, 512, 256};
auto plan = fftw_plan_dft_3d(in[0], in[1], in[2], in(), out(), FFTW_FORWARD, FFTW_ESTIMATE);
...
vtkNew<vtkImageImport> image_import;
image_import->SetWholeExtent(0, out[0] - 1, 0, out[1] - 1, 0, out[2] - 1);
image_import->SetNumberOfScalarComponents(in.order);
image_import->SetImportVoidPointer(static_cast<void *>(out()));
...
```

Dynamic tensors can be used to wrap existing data.

```C++
Dynamic::Array<float[3], Allocate<false>> A {256, 256, 256};
// A() == NULL
float *a = new float[A[0] * A[1] * A[2]];
A() = a;
```



## Axis

Currently only column major storage is supported, and is implicit.

```C++
Dynamic::Array<float[2], Axis<column>> A {512, 512};
```



## Layouts

The goal is to ultimately support all tensor layouts by major linear algebra libraries.

### Conventional

The implicit, standard, dense layout.

```C++
Dynamic::Array<float[3], Layout<conventional>> A {3, 4, 5}; // size 60
```

### Packed

Packed storage, used in e.g. Hermitian matrices and triangular matrices. Only defined for symmetric storage.

Comes in two flavors: `packed_inc` and `packed_dec`. The difference lies in how they're supposed to be indexed.

Increasing packed is for e.g. upper triangular column major storage; the first major piece of data is the 1st column, containing only one element.

Decreasing packed is for e.g. lower triangular column major storage; the first major piece of data is the 1st column, which is the column with the largest length.

*Understand once more that this is a datastructure library, not a math library. For symmetric matrices the value at `(i, j)` is the same as that at `(j, i)`, but for complex Hermitian matrices it isn't. Both use packed storage. Naturally then, don't expect `(i, j)` to point to `(j, i)`. Only the indexes in the generalized upper triangle for `packed_inc` and lower triangle for `packed_dec` have meaning (assuming column major axis, for row major it's flipped). This also means that the indexing math used is perfectly optimal as it doesn't have to account for each half space.*

```C++
Dynamic::Array<float[3], Layout<packed_inc>> A {3}; // allocates 10 floats
A(0, 1, 0); // this is part of the generalized upper triangle. no matter the order, no matter the application, this compiles to the perfect index equation
A(1, 0, 0); // this is not part of the generalized upper triangle, so this access is incorrect usage
```



## Initializer Lists

Initializer lists are used not only for initialization, but also for assignment.

```C++
Static::Array<int[2][2][2]> A {{{1, 2}, {3, 4}}, {{5, 6}, {7, 8}}};
A(1, 1) = {-1, -2};
```

This does not exist for our convenience. It exists to be able to make a `constexpr` tensor. (That's why it's not available in `Dynamic::Array`.)

```C++
constexpr Static::Array<int[2][2]> B {{1, 2}, {3, 4}};
```



## Size Type

Even the element type of the array that stores the dimensions of a `Dynamic::Array` can be specified. By default, the type is `size_t`. (For `Static::Array` too but that doesn't really do anything.)

```C++
sizeof(Dynamic::Array<float[3]>); // 32 bytes on my system
sizeof(Dynamic::Array<float[3], SizeType<uint32_t>>); // 24 bytes on my system
```
