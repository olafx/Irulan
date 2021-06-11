#pragma once
#include "Base.hh"

namespace Irulan
{   namespace Static
    {

template <typename ...Properties>
struct Array : public Base<Properties...>
{

private:
    using Base<Properties...>::dims,
          Base<Properties...>::combinations;
public:
    using Base<Properties...>::layout,
          Base<Properties...>::axis,
          typename Base<Properties...>::size_type,
          typename Base<Properties...>::value_type;
    static constexpr size_t order {dims.size()};
private:
    using deep_init_list = typename Base<Properties...>::template DeepInitList<order, size_type>::type;

private:
    template <typename, typename B, typename ...A>
    struct ShapeModifier
    {   template <typename shape, typename ...C>
        struct Modify
        {   typedef typename ShapeModifier<void, A...>::template Modify<shape, C..., B>::type type;
        };
    };
    template <typename B, typename ...A>
    struct ShapeModifier<std::enable_if_t<std::is_array_v<B>>, B, A...>
    {   template <typename shape, typename ...C>
        struct Modify
        {   typedef typename ShapeModifier<void, A...>::template Modify<shape, C..., shape>::type type;
        };
    };
    template <typename Enabled, typename B>
    struct ShapeModifier<Enabled, B>
    {   template <typename shape, typename ...C>
        struct Modify
        {   typedef Array<C..., B> type;
        };
    };
    template <typename B>
    struct ShapeModifier<std::enable_if_t<std::is_array_v<B>>, B>
    {   template <typename shape, typename ...C>
        struct Modify
        {   typedef Array<C..., shape> type;
        };
    };

private:
    template <size_t length, typename T, typename = void>
    struct CreateShape
    {   typedef typename CreateShape<length, T[dims[length - 1 - std::rank_v<T>]]>::type type;
    };
    template <size_t length, typename T>
    struct CreateShape <length, T, std::enable_if_t<std::rank_v<T> == length>>
    {   typedef T type;
    };

private:
    template <size_t order, LayoutEnum layout, typename = void>
    struct Data
    {};
    template <size_t order>
    struct Data<order, conventional, std::enable_if_t<order != 1>>
    {   typename ShapeModifier<void, Properties...>::template
            Modify<typename CreateShape<order - 1, value_type>::type>::type value[dims[order - 1]];
    };
    template <size_t order>
    struct Data<order, conventional, std::enable_if_t<order == 1>>
    {   value_type value[dims[0]];
    };
    template <size_t order>
    struct Data<order, packed>
    {   value_type value[combinations(order + dims[0] - 1, order)];
    };
    Data<order, layout> data;

public:
    Array() noexcept = default;
    Array(const Array& other) noexcept = default;
    constexpr Array(const deep_init_list& list) noexcept : data {}
    {   *this = list;
    }

public:
    template <typename I, std::enable_if_t<std::is_integral_v<I>>* = nullptr>
    constexpr const size_type& operator[](I i) const noexcept
    {   return dims[i];
    }

public:
    constexpr value_type *operator()() noexcept
    {   if constexpr (layout == conventional)
        {   if constexpr (order == 1)
                return data.value;
            else
                return data.value[0]();
        }
        if constexpr (layout == packed)
            return data.value;
    }
    constexpr const value_type *operator()() const noexcept
    {   if constexpr (layout == conventional)
        {   if constexpr (order == 1)
                return data.value;
            else
                return data.value[0]();
        }
        if constexpr (layout == packed)
            return data.value;
    }

public:
    template <typename I, typename ...J, std::enable_if_t<std::is_integral_v<I>>* = nullptr>
    constexpr auto& operator()(I i, J... j) noexcept
    {   if constexpr (layout == conventional)
        {   if constexpr (sizeof...(j) == 0)
                return data.value[i];
            else
                return (*this)(j...)(i);
        }
        if constexpr (layout == packed)
            return; // TODO
    }
    template <typename I, typename ...J, std::enable_if_t<std::is_integral_v<I>>* = nullptr>
    constexpr const auto& operator()(I i, J... j) const noexcept
    {   if constexpr (layout == conventional)
        {   if constexpr (sizeof...(j) == 0)
                return data.value[i];
            else
                return (*this)(j...)(i);
        }
        if constexpr (layout == packed)
            return; // TODO
    }

public:
    constexpr Array& operator=(const value_type& value) noexcept
    {   for (size_type i {}; i < (*this)[order - 1]; i++)
            (*this)(i) = value;
        return *this;
    }
    constexpr Array& operator=(const deep_init_list& list) noexcept
    {   for (size_type i {}; i < list.size(); i++)
            (*this)(i) = list.begin()[i];
        return *this;
    }
};

    }
}
