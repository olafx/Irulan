#pragma once
#include <cstdlib>
#include <new>
#include "Base.hh"

namespace Irulan
{   namespace Dynamic
    {

//  Dynamic 'Array' has runtime size and heap allocated data.

template <typename ...Properties>
struct Array : Base<Properties...>
{

public:

    using Base<Properties...>::layout,
          Base<Properties...>::axis,
          typename Base<Properties...>::size_type,
          typename Base<Properties...>::value_type;
    static constexpr size_t order = Base<Properties...>::dims[0];



private:

    using Base<Properties...>::combinations;
    using DeepInitList = typename Base<Properties...>::template DeepInitList<order, value_type>::type;



private:

    size_type dims[order];
    value_type *data;



private:

    //  Calculate the data size, which depends on 'Layout' and 'dims'.

    template <typename ...Dims>
    static size_t size(Dims... dims) noexcept
    {   if constexpr (layout == conventional)
            return (dims * ...);
        if constexpr (layout == packed)
            return combinations(order + [](auto a, auto... b){ return a; }(dims...) - 1, order);
    }



private:

    //  Calculates 1D memory index based on nD 'Array' index where n is 'order'.
    //  Currently only implemented for 'Layout<conventional>'.

    template <size_t level, typename I, typename ...J>
    auto index(I i, J... j) const noexcept
    {   if constexpr (sizeof...(j) == 0)
            return i;
        else
            return index<level + 1>(j...) * (*this)[level] + i;
    }

    //  Calculates 1D memory index based on nD 'Array' index. Behavior is 'Axis' property dependent if n < 'order'.
    //  Currently only implemented for 'Layout<conventional>'.

    template <size_t level, typename ...I>
    auto index_offset(I... i) const noexcept
    {   if constexpr (level == 0)
            return index<0>(i...);
        else
            return index_offset<level - 1>(0, i...);
    }



public:

    //  Malloc is used for data memory allocation due to need for future CUDA compatibility.

    template <typename ...Dims>
    Array(Dims... dims)
        : dims {static_cast<size_type>(dims)...},
          data {static_cast<decltype(data)>(malloc(sizeof(decltype(*data)) * size(dims...)))}
    {   if (data == NULL)
            throw std::bad_alloc {};
    }



public:

    //  Dimension operator.

    template <typename I, std::enable_if_t<std::is_integral_v<I>>* = nullptr>
    const size_type& operator[](I i) const noexcept
    {   return dims[i];
    }
    template <typename I, std::enable_if_t<std::is_integral_v<I>>* = nullptr>
    size_type& operator[](I i) noexcept
    {   return dims[i];
    }



public:

    //  Raw data access.

    value_type *operator()() noexcept
    {   return data;
    }



public:

    //  Indexing.
    //  Currently only implemented for 'Layout<conventional>'.

    template <typename ...I, std::enable_if_t<(std::is_integral_v<I> && ...)>* = nullptr>
    value_type& operator()(I... i) noexcept
    {   return (*this)()[index_offset<order - sizeof...(i)>(i...)];
    }

    template <typename ...I, std::enable_if_t<(std::is_integral_v<I> && ...)>* = nullptr>
    const value_type& operator()(I... i) const noexcept
    {   return (*this)()[index_offset<order - sizeof...(i)>(i...)];
    }



public:

    //  Assignment via a value will set all data to this value.

    Array& operator=(const value_type& value) noexcept
    {   for (size_type i = 0; i < (*this)[order - 1]; i++)
            (*this)(i) = value;
        return *this;
    }

    //  Assignment via a 'DeepInitList' does not require said list to be full, and may use the previously defined value
    //  assignment operator (in case the list's order is less than 'order').

    Array& operator=(const DeepInitList& list) noexcept
    {   for (size_type i = 0; i < list.size(); i++)
            (*this)(i) = list.begin()[i];
        return *this;
    }
};

    }
}
