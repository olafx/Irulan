#pragma once

namespace Irulan
{

//  Properties for the array template are mostly given as templated types. These templates sometimes hold enum values, sometimes
//  types. Sometimes the properties aren't directly given as a type (e.g. the shape).

enum LayoutEnum {conventional, packed};
enum AxisEnum   {column};

//  Properties are given a base type they inherit from. This way different specific properties of the same property
//  (e.g. Layout<conventional> and Layout<packed>) can both be interpreted as being of the same property (e.g. Layout).
//  Objects of these base types should never be instantiated, so they're made purely virtual.

struct ShapeBase    { virtual ~ShapeBase()    = 0; };
struct LayoutBase   { virtual ~LayoutBase()   = 0; };
struct AxisBase     { virtual ~AxisBase()     = 0; };
struct SizeTypeBase { virtual ~SizeTypeBase() = 0; };

//  The property classes with their templated specific property must hold this specific property as a field for access.

template <LayoutEnum layout>  struct Layout   : LayoutBase   { static constexpr auto value {layout}; };
template <AxisEnum axis>      struct Axis     : AxisBase     { static constexpr auto value {axis}; };
template <typename size_type> struct SizeType : SizeTypeBase { typedef size_type type; };

}
