#pragma once
#include <cstddef>
#include <array>
#include <type_traits>
#include "Type.hh"

namespace Irulan
{

// Base class of Array.

template <typename ...Properties>
struct Base
{

protected:

    //  Extract array properties from Properties variadic template.
    //  Properties are of some type defined in Type.hh, e.g. Layout. Each of these types has a base, e.g. LayoutBase that is
    //  given as Base to Extractor. (This way e.g. Layout<conventional> can be seen to belong to LayoutBase.)
    //  If the right property isn't found in Properties, default to Default (e.g. if the conventional layout should be
    //  the default, call Extractor<LayoutBase, Layout<conventional>>).
    //  Having multiple properties of the same type is not allowed, which can be verified at compile time.

    template <typename Base, typename Default>
    struct Extractor
    {
        //  Find a property in Properties of the right type, i.e. one that inherits from Base.
        //  The 1st template argument is for enabling and should be void. The rest hold the remaining properties.

        //  Case for when the property can't be found.
        //  The std::true_type inheritance reflects that there are no duplicate properties of Base.

        template <typename ...>
        struct Get
            : std::true_type
        {   //  The search result is Default.
            using type = Default;
        };

        //  Case for when the property has been found to be A.
        //  The std::false_type inheritance reflects that no more properties inheriting from Base are allowed.

        template <typename A, typename ...B>
        struct Get<std::enable_if_t<std::is_base_of_v<Base, A>>, A, B...>
            : std::false_type
        {   static_assert(Get<void, B...> {}, "multiple properties of the same type not allowed");
            //  The search result is A.
            using type = A;
        };

        //  Case for when the property has been found to not be A.
        //  The boolean type inheritance is carried through recursively from the inside out.

        template <typename A, typename ...B>
        struct Get<std::enable_if_t<!std::is_base_of_v<Base, A>>, A, B...>
            : std::conditional_t<Get<void, B...> {}, std::true_type, std::false_type>
        {   //  The search result is carried through recursively from the inside out.
            using type = typename Get<void, B...>::type;
        };

        //  The result of the extractor is the result of the getter.
        using type = typename Get<void, Properties...>::type;
    };

    //  Extract the size type already, because it's needed in the definition of the next extractor.

    using size_type = typename Extractor<SizeTypeBase, SizeType<size_t>>::type::type;
    static constexpr LayoutEnum layout {Extractor<LayoutBase, Layout<conventional>>::type::value};

    //  Shape extraction needs its own specialization because this property works differently from the rest.
    //  The Default type reflects the default type of the data, which is not the shape.
    //  Having multiple shape properties is not allowed, which can be verified at compile time.

    template <typename Default>
    struct Extractor<ShapeBase, Default>
    {
        //  Search for the shape property, which is an array type. Defaults to a 1st order array of length 1.
        //  The 1st template argument is for enabling and should be void. The rest hold the remaining properties in Properties.

        //  Case for when the array type can't be found.
        //  The std::true_type inheritance reflects that there are no duplicate array type properties.

        template <typename ...>
        struct Get
            : std::true_type
        {   //  The search result is Default, and the shape is a 1st order array of length 1.
            using value_type = Default;
            static constexpr std::array<size_type, 1> dims {1};
        };

        //  Case for when the array type has been found to be A.
        //  The std::false_type inheritance reflects that no other array type properties are allowed.

        template <typename A, typename ...B>
        struct Get<std::enable_if_t<std::is_array_v<A>>, A, B...>
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

            static_assert(Get<void, B...> {}, "multiple properties of the same type not allowed");

            //  Deduce the data type, which is the element type of the multidimensional array type. This is a search result.
            using value_type = std::remove_all_extents_t<A>;
            static_assert(std::is_trivially_default_constructible_v<value_type>,
                "data type must be trivially default constructible to maintain predictable memory layout");
            //  The other search result is the shape.
            static constexpr std::array dims {Dims<void, A>::value};
        };

        //  Case for when the array type has been found to not be A.
        //  The boolean type inheritance is carried through recursively from the inside out.

        template <typename A, typename ...B>
        struct Get<std::enable_if_t<!std::is_array_v<A>>, A, B...>
            : std::conditional_t<Get<void, B...> {}, std::true_type, std::false_type>
        {   //  The search result is carried through recursively from the inside out.
            using value_type = typename Get<void, B...>::value_type;
            static constexpr std::array dims {Get<void, B...>::dims};
        };

        //  The result of the extractor is the result of the getter.
        using value_type = typename Get<void, Properties...>::value_type;
        static constexpr std::array dims {Get<void, Properties...>::dims};
    };



protected:

    //  Using the extractor.

    static constexpr AxisEnum   axis     {Extractor<AxisBase, Axis<column>>          ::type::value};
    static constexpr bool       allocate {Extractor<AllocateBase, Allocate<true>>    ::type::value};
    static constexpr std::array dims     {Extractor<ShapeBase, double>::dims};
    using value_type = typename Extractor<ShapeBase, double>::value_type;



protected:

    //  Defining a nested version of std::initializer_list.
    //  Used for setting values of Array for e.g. initialization.

    template <size_t depth, typename T>
    struct DeepInitList
    {   using type = std::initializer_list<typename DeepInitList<depth - 1, T>::type>;
    };

    template <typename T>
    struct DeepInitList<0, T>
    {   using type = T;
    };



protected:

    //  Combinations function (n choose k) needed for the size of packed Arrays.

    template <typename T>
    static constexpr T combinations(T n, T k) noexcept
    {   T result = 1;
        for (T i = 1; i <= k; i++)
            result = (n - i + 1) * result / i;
        return result;
    }
};

}
