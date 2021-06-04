#pragma once
#include <cstddef>
#include <array>
#include <type_traits>
#include "Type.hh"

namespace Irulan
{

template <typename ...Properties>
struct Base
{

public:
    template <typename Base, typename Default>
    struct Extractor
    {   template <typename ...>
        struct Find : std::true_type
        {   typedef Default type;
        };
        template <typename A, typename ...B>
        struct Find<std::enable_if_t<std::is_base_of_v<Base, A>>, A, B...> : std::false_type
        {   static_assert(Find<void, B...> {}, "multiple properties of the same type not allowed");
            typedef A type;
        };
        template <typename A, typename B, typename ...C>
        struct Find<std::enable_if_t<!std::is_base_of_v<Base, A>>, A, B, C...>
            : std::conditional_t<Find<void, B, C...> {}, std::true_type, std::false_type>
        {   typedef typename Find<void, B, C...>::type type;
        };
        typedef typename Find<void, Properties...>::type type;
    };

    typedef typename Extractor<SizeTypeBase, SizeType<size_t>>::type::type size_type;

    template <typename Default>
    struct Extractor<ShapeBase, Default>
    {   template <typename ...>
        struct Find : std::true_type
        {   typedef Default value_type;
            static constexpr std::array dims {(size_type) 1};
        };
        template <typename A, typename ...B>
        struct Find<std::enable_if_t<std::is_array_v<A>>, A, B...> : std::false_type
        {   template <typename, typename C>
            struct RemovePointers
            {   typedef C type;
                static constexpr size_t count {};
            };
            template <typename C>
            struct RemovePointers<std::enable_if_t<std::is_pointer_v<C>>, C>
            {   typedef RemovePointers<void, std::remove_pointer_t<C>> removed;
                typedef typename removed::type type;
                static constexpr size_t count {removed::count + 1};
            };
            template <typename, typename shape, size_type ...a>
            struct Dims
            {   static constexpr std::array value {a...};
            };
            template<typename shape, size_type ...a>
            struct Dims<std::enable_if_t<std::is_array_v<shape>>, shape, a...>
            {   static constexpr std::array value {Dims<void, std::remove_extent_t<shape>, a..., std::extent_v<shape>>::value};
            };
            static_assert(Find<void, B...> {}, "multiple properties of the same type not allowed");
            typedef std::remove_all_extents_t<A> raw_type;
            typedef typename RemovePointers<void, raw_type>::type value_type;
            static constexpr std::array dims {Dims<void, A>::value};
        };
        template <typename A, typename B, typename ...C>
        struct Find<std::enable_if_t<!std::is_array_v<A>>, A, B, C...>
            : std::conditional_t<Find<void, B, C...> {}, std::true_type, std::false_type>
        {   typedef typename Find<void, B, C...>::value_type value_type;
            static constexpr std::array dims {Find<void, B, C...>::dims};
        };
        typedef typename Find<void, Properties...>::value_type value_type;
        static constexpr std::array dims {Find<void, Properties...>::dims};
    };

public:
    static constexpr LayoutEnum layout {Extractor<LayoutBase, Layout<conventional>>::type::value};
    static constexpr AxisEnum axis {Extractor<AxisBase, Axis<column>>::type::value};
    static constexpr std::array dims {Extractor<ShapeBase, double>::dims};
    typedef typename Extractor<ShapeBase, double>::value_type value_type;

public:
    template <size_t depth, typename T>
    struct DeepInitList
    {   typedef std::initializer_list<typename DeepInitList<depth - 1, T>::type> type;
    };
    template <typename T>
    struct DeepInitList<0, T>
    {   typedef T type;
    };

public:
    template <typename T>
    static constexpr T combinations(T n, T k) noexcept
    {   T result {1};
        for (T i {1}; i <= k; i++)
            result = (n - i + 1) * result / i;
        return result;
    }
};

}
