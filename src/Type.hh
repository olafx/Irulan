#pragma once

namespace Irulan
{

enum LayoutEnum {conventional, packed};
enum AxisEnum   {column};

struct LayoutBase   { virtual ~LayoutBase()   = 0; };
struct AxisBase     { virtual ~AxisBase()     = 0; };
struct ShapeBase    { virtual ~ShapeBase()    = 0; };
struct SizeTypeBase { virtual ~SizeTypeBase() = 0; };

template <LayoutEnum layout>  struct Layout   : LayoutBase   { static constexpr auto value {layout}; };
template <AxisEnum axis>      struct Axis     : AxisBase     { static constexpr auto value {axis}; };
template <typename size_type> struct SizeType : SizeTypeBase { typedef size_type type; };

}
