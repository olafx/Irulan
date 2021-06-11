# Irulan

**C++17 multidimensional array (tensor) library.**

Focuses on data, not math. Provides various layouts from various libraries (BLAS, LAPACK, etc.), and is designed to be used in parallel with these libraries.

**Proof of concept, and very much WIP.**



## Types

### Static

Compile time size, direct storage (stack). (`std::array` equivalent.)

```C++
Static::Array<int8_t[3][4][5]> A;
constexpr Static::Array<float[2][2]> B {{1, 2}, {3, 4}};
```

For regular dense tensors, stores a C-style array of lower order tensors.

```C++
Static::Array<int8_t[3][4]> C = A(4);
```

### Hybrid

Compile time size, heap storage. (`new std::array` equivalent.)

```C++
Hybrid::Array<int8_t[2][3]> A {{1, 2}, {3}, {5, 6}};
```

### Dynamic

Run time size. Compile time order. (`std::vector` equivalent, without the dynamic allocation.)

```C++
Dynamic::Array<int8_t[3]> A {3840, 3840, 3};
```

Dimensions are stored on stack in the object.



## Property System

Properties are templated. They can be given in any order. All have implicit defaults.

```C++
Static:Array<int[3]> A;
Static:Array<int[3], Layout<conventional>> B;
Static:Array<Layout<conventional>, int[3]> C;
// all the same
```



## Access

### Indexing

Elements are accessed by Fortran-style indexing (except 0-based).

```C++
Static::Tensor<float[3][4]> A;
A(0, 1) = 3;
```

If the 'wrong' number indices is given, it will correctly interpret this and return for example the beginning of a column, independent of the particular layout.

```C++
A(3);
// points to 4th column, not 4th element
// A(0, 3), not A(3, 0)
```

### Dimensions

The square bracket operator is used.

```C++
Hybrid:Array<int8_t[3][4][5]> A;
A[1]; // 4
```

### Pointer

In the spirit of working in parallel with other libraries, a raw pointer can easily be returned to send to e.g. some CBLAS function.

```C++
Dynamic::Array<double[2]> A {64, 64};
A(); // raw pointer
```



## Axis

Currently only column major storage is supported, and is implicit.

```C++
Dynamic::Array<float[1000][1000], Axis<column>> A;
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

*Partially implemented; size is correct and generalized to any number of dimensions, indexing is incomplete.*

```C++
Dynamic::Array<float[3], Layout<packed>> A {3}; // size 10
```

## Initializer Lists

Initializer lists are used not only for initialization, but also for assignment.

```C++
Hybrid::Array<int[2][2][2]> A {{{1, 2}, {3, 4}}, {{5, 6}, {7, 8}}};
A(1, 1) = {-1, -2};
```
