#pragma once

namespace Irulan
{

//  Properties for the Array template are mostly given as templated types. These templates sometimes hold enum values, sometimes
//  types of their own. Sometimes the properties aren't directly given as a type (e.g. the shape).
//  The property classes with their templated specific property must hold this specific property as a field for accessibility.
//  Properties are given a base type they inherit from. This way different specific properties of the same property
//  (e.g. Layout<conventional> and Layout<packed>) can both be interpreted as being of the same property (e.g. Layout).



//  Shape property is special and only needs a base to distinguish it from other properties.

struct ShapeBase
{
};



//  The data layout, e.g. conventional for general matrices and packed for symmetric matrices. Layout does not refer to matrix
//  type, e.g. symmetric and square triangular matrices both have a packed layout.

enum LayoutEnum {conventional, packed};

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
