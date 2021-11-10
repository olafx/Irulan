/*
    MIT License

    Copyright (c) 2021 Olaf Willocx

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#pragma once

namespace Irulan
{

//  Properties for the Array template are mostly given as templated types. These templates sometimes hold enum values, sometimes
//  types of their own. Sometimes the properties aren't directly given as a type (e.g. the shape).
//  The property classes with their templated specific property must hold this specific property as a field for accessibility.
//  Properties are given a base type they inherit from. This way different specific properties of the same property
//  (e.g. Layout<conventional> and Layout<packed_inc>) can both be interpreted as being of the same property (e.g. Layout).



//  Shape property is special and only needs a base to distinguish it from other properties.

struct ShapeBase
{
};



//  The data layout, e.g. conventional for general matrices and packed for symmetric matrices. Layout does not refer to matrix
//  type, e.g. symmetric and square triangular matrices both have a packed layout. The packed_inc type is for packed storage
//  with an increasing number of elements per section, and packed_dec for decreasing. They're effectively the same, only the
//  indexing is different. The former is used for e.g. upper triangular storage for column major matrices, but also lower
//  triangular storage for row major matrices.

enum LayoutEnum {conventional, packed_inc, packed_dec};

struct LayoutBase
{
};

template <LayoutEnum layout>
struct Layout : LayoutBase
{   static constexpr LayoutEnum value {layout};
};



//  The axis refers to column or row major storage, i.e. the axis on which data appends in memory.

enum AxisEnum {column};

struct AxisBase
{
};

template <AxisEnum axis>
struct Axis : AxisBase
{   static constexpr AxisEnum value {axis};
};



//  The allocate property is used to distinguish the usual Array's that hold data from ones that wrap existing data.

struct AllocateBase
{
};

template <bool allocate>
struct Allocate : AllocateBase
{   static constexpr bool value {allocate};
};



//  The size type property specifies the type to use for specifying the Array's shape.

struct SizeTypeBase {};

template <typename size_type>
struct SizeType : SizeTypeBase
{   using type = size_type;
};

}
