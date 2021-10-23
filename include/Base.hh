#pragma once
#include <cstddef>
#include <array>
#include <type_traits>
#include "Type.hh"

namespace Irulan
{

// Base class of 'Array's.

template <typename ...Properties>
struct Base
{

public:

    //  Extract array properties from 'Properties' variadic template.
    //  Properties are of some type defined in 'Type.hh', e.g. 'Layout'. Each of these types has a base, e.g. 'LayoutBase' that
    //  is given as 'Base' to 'Extractor'. (This way e.g. 'Layout<conventional>' can be seen to belong to 'LayoutBase'.)
    //  If the right property isn't found in 'Properties', default to 'Default' (e.g. if the conventional layout should be
    //  the default, call Extractor<LayoutBase, Layout<conventional>>).
    //  Having multiple properties of the same type is not allowed, which can be verified at compile time.

    template <typename Base, typename Default>
    struct Extractor
    {
        //  Find a property in 'Properties' of the right type, i.e. one that inherits from 'Base'.
        //  The 1st template argument is for enabling and should be void. The rest hold the remaining properties.

        //  Case for when the property can't be found.
        //  The 'std::true_type' inheritance reflects that there are no duplicate properties of 'Base'.

        template <typename ...>
        struct Find
            : std::true_type
        {   //  The search result is Default.
            typedef Default type;
        };

        //  Case for when the property has been found to be 'A'.
        //  The 'std::false_type' inheritance reflects that no more properties inheriting from 'Base' are allowed.

        template <typename A, typename ...B>
        struct Find<std::enable_if_t<std::is_base_of_v<Base, A>>, A, B...>
            : std::false_type
        {   static_assert(Find<void, B...> {}, "multiple properties of the same type not allowed");
            //  The search result is A.
            typedef A type;
        };

        //  Case for when the property has been found to not be 'A'.
        //  The boolean type inheritance is carried through recursively from the inside out.

        template <typename A, typename ...B>
        struct Find<std::enable_if_t<!std::is_base_of_v<Base, A>>, A, B...>
            : std::conditional_t<Find<void, B...> {}, std::true_type, std::false_type>
        {   //  The search result is carried through recursively from the inside out.
            typedef typename Find<void, B...>::type type;
        };

        //  The result of the extractor is the result of the finder.
        typedef typename Find<void, Properties...>::type type;
    };

    //  Extract the size type already, because it's needed in the definition of the next extractor.

    typedef typename Extractor<SizeTypeBase, SizeType<size_t>>::type::type size_type;

    //  Shape extraction needs its own specialization because this property works differently from the rest.
    //  The 'Default' type reflects the default type of the data, which is not the shape.
    //  Having multiple shape properties is not allowed, which can be verified at compile time.

    template <typename Default>
    struct Extractor<ShapeBase, Default>
    {
        //  Search for the shape property, which is an array type. Defaults to a 1st order array of length 1.
        //  The 1st template argument is for enabling and should be void. The rest hold the remaining properties in 'properties'.

        //  Case for when the array type can't be found.
        //  The 'std::true_type' inheritance reflects that there are no duplicate array type properties.

        template <typename ...>
        struct Find
            : std::true_type
        {   //  The search result is Default, and the shape is a 1st order array of length 1.
            typedef Default value_type;
            static constexpr std::array<size_type, 1> dims {1};
        };

        //  Case for when the array type has been found to be 'A'.
        //  The 'std::false_type' inheritance reflects that no other array type properties are allowed.

        template <typename A, typename ...B>
        struct Find<std::enable_if_t<std::is_array_v<A>>, A, B...>
            : std::false_type
        {
            //  Deduce the shape, which is the shape of the multidimensional array type.
            //  The 1st template argument is for enabling and should be void. The rest holds shape read so far.

            //  Case for when the full shape has been read.

            template <typename, typename shape, size_type ...a>
            struct Dims
            {   //  The result is an array containing the full shape.
                static constexpr std::array value {a...};
            };

            //  Case for when there still is a remaining part to the shape.

            template<typename shape, size_type ...a>
            struct Dims<std::enable_if_t<std::is_array_v<shape>>, shape, a...>
            {   //  The result is carried through recursively from the inside out.
                static constexpr std::array value {Dims<void, std::remove_extent_t<shape>, a..., std::extent_v<shape>>::value};
            };

            static_assert(Find<void, B...> {}, "multiple properties of the same type not allowed");

            //  Deduce the data type, which is the element type of the multidimensional array type. This is a search result.
            typedef std::remove_all_extents_t<A> value_type;
            //  The other search result is the shape.
            static constexpr std::array dims {Dims<void, A>::value};
        };

        //  Case for when the array type has been found to not be 'A'.
        //  The boolean type inheritance is carried through recursively from the inside out.

        template <typename A, typename ...B>
        struct Find<std::enable_if_t<!std::is_array_v<A>>, A, B...>
            : std::conditional_t<Find<void, B...> {}, std::true_type, std::false_type>
        {   //  The search result is carried through recursively from the inside out.
            typedef typename Find<void, B...>::value_type value_type;
            static constexpr std::array dims {Find<void, B...>::dims};
        };

        //  The result of the extractor is the result of the finder.
        typedef typename Find<void, Properties...>::value_type value_type;
        static constexpr std::array dims {Find<void, Properties...>::dims};
    };



public:

    //  Using the extractor.

    static constexpr LayoutEnum layout {Extractor<LayoutBase, Layout<conventional>>::type::value};
    static constexpr AxisEnum   axis   {Extractor<AxisBase,           Axis<column>>::type::value};
    static constexpr std::array dims   {Extractor<ShapeBase,                double>::dims};
    typedef typename Extractor<ShapeBase, double>::value_type value_type;



public:

    //  Defining a nested version of std::initializer_list.
    //  Used for setting values of 'Array's for e.g. initialization.

    template <size_t depth, typename T>
    struct DeepInitList
    {   typedef std::initializer_list<typename DeepInitList<depth - 1, T>::type> type;
    };

    template <typename T>
    struct DeepInitList<0, T>
    {   typedef T type;
    };



public:

    //  Combinations function (n choose k) needed for the size of packed 'Array's.
    //  (Not optimized for large 'k' since it's never large in practice.)

    template <typename T>
    static constexpr T combinations(T n, T k) noexcept
    {   T result = 1;
        for (T i = 1; i <= k; i++)
            result = (n - i + 1) * result / i;
        return result;
    }
};

}