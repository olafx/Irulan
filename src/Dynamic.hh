#pragma once
#include <cstdlib>
#include <new>
#include "Base.hh"

namespace Irulan
{   namespace Dynamic
    {

template <typename ...Properties>
struct Array : Base<Properties...>
{

public:
    using Base<Properties...>::layout,
          Base<Properties...>::axis,
          typename Base<Properties...>::size_type,
          typename Base<Properties...>::value_type;
    static constexpr size_t order {Base<Properties...>::dims[0]};
private:
    using Base<Properties...>::combinations;
    using deep_init_list = typename Base<Properties...>::template DeepInitList<order, size_type>::type;

private:
    size_type dims[order];
    value_type *data;

private:
    template <typename Dim, typename ...Dims>
    static size_t size(Dim dim, Dims... dims) noexcept
    {   if constexpr (layout == conventional)
            return dim * (dims * ...);
        if constexpr (layout == packed)
            return combinations(order + dim - 1, order);
    }

private:
    template <size_t level, typename I, typename ...J>
    auto index(I i, J... j) const noexcept
    {   if constexpr (sizeof...(j) == 0)
            return i;
        else
            return index<level + 1>(j...) * (*this)[level] + i;
    }
    template <size_t level, typename ...I>
    auto index_offset(I... i) const noexcept
    {   if constexpr (level == 0)
            return index<0>(i...);
        else
            return index_offset<level - 1>(0, i...);
    }

public:
    template <typename ...Dims>
    Array(Dims... dims)
        : dims {static_cast<size_type>(dims)...},
          data {static_cast<decltype(data)>(malloc(sizeof(decltype(*data)) * size(dims...)))}
    {   if (data == NULL)
            throw std::bad_alloc {};
    }

public:
    template <typename I, std::enable_if_t<std::is_integral_v<I>>* = nullptr>
    const size_type& operator[](I i) const noexcept
    {   return dims[i];
    }
    template <typename I, std::enable_if_t<std::is_integral_v<I>>* = nullptr>
    size_type& operator[](I i) noexcept
    {   return dims[i];
    }

public:
    value_type *operator()() noexcept
    {   return data;
    }

public:
    template <typename ...I, std::enable_if_t<(std::is_integral_v<I> && ...)>* = nullptr>
    value_type& operator()(I... i) noexcept
    {   return (*this)()[index_offset<order - sizeof...(i)>(i...)];
    }
    template <typename ...I, std::enable_if_t<(std::is_integral_v<I> && ...)>* = nullptr>
    const value_type& operator()(I... i) const noexcept
    {   return (*this)()[index_offset<order - sizeof...(i)>(i...)];
    }

public:
    Array& operator=(const value_type& value) noexcept
    {   for (size_type i {}; i < (*this)[order - 1]; i++)
            (*this)(i) = value;
        return *this;
    }
    Array& operator=(const deep_init_list& list) noexcept
    {   for (size_type i {}; i < list.size(); i++)
            (*this)(i) = list.begin()[i];
        return *this;
    }
};

    }
}
