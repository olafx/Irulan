#pragma once
#include <cstdlib>
#include <new>
#include "Base.hh"

namespace Irulan
{   namespace Dynamic
    {

//  Dynamic::Array has runtime size and heap allocated data.

template <typename ...Properties>
struct Array : Base<Properties...>
{

public:

    using Base_ = Base<Properties...>;

    using Base_::layout,
          Base_::axis,
          Base_::allocate,
          typename Base_::size_type,
          typename Base_::value_type;
    static constexpr size_t order = Base_::dims[0];



private:

    using Base_::combinations;
    using DeepInitList = typename Base_::template DeepInitList<order, value_type>::type;



private:

    size_type dims[order];
    value_type *data;



private:

    //  Calculate the data size, which depends on Layout and dims.

    template <typename ...Dims>
    static size_t size(Dims... dims) noexcept
    {   if constexpr (sizeof...(dims) == 0)
            return 0;
        else if constexpr (layout == conventional)
            return (dims * ...);
        else if constexpr (layout == packed)
            return combinations(order + [](auto a, auto... b){ return a; }(dims...) - 1, order);
    }



private:

    //  Calculates 1D memory index based on order-dimensional Array index.
    //  Currently only implemented for Layout<conventional>.

    template <size_t level, typename I, typename ...J>
    auto index(I i, J... j) const noexcept
    {   if constexpr (sizeof...(j) == 0)
            return i;
        else
            return index<level + 1>(j...) * (*this)[level] + i;
    }

    //  Calculates 1D memory index based on n-dimensional Array index. Behavior is Axis property dependent if n < order.
    //  Currently only implemented for Layout<conventional>.

    template <size_t level, typename ...I>
    auto index_offset(I... i) const noexcept
    {   if constexpr (level == 0)
            return index<0>(i...);
        else
            return index_offset<level - 1>(0, i...);
    }



public:

    template <typename ...Dims,
        bool allocate_delayed = allocate, std::enable_if_t<allocate_delayed>* = nullptr>
    Array(Dims... dims)
        : dims {static_cast<size_type>(dims)...},
          data {new value_type[size(dims...)]}
    {
    }

    template <typename ...Dims,
        bool allocate_delayed = allocate, std::enable_if_t<!allocate_delayed>* = nullptr>
    Array(Dims... dims) noexcept
        : dims {static_cast<size_type>(dims)...},
          data {NULL}
    {
    }



    ~Array() noexcept
    {   if constexpr (allocate)
            delete[] data;
    }



public:

    //  Dimension operator.

    template <typename I,
        typename = std::enable_if_t<std::is_integral_v<I>>>
    size_type& operator[](I i) noexcept
    {   return dims[i];
    }

    template <typename I,
        typename = std::enable_if_t<std::is_integral_v<I>>>
    const size_type& operator[](I i) const noexcept
    {   return dims[i];
    }



public:

    //  Raw data access. For Arrays with Allocate<false>, the raw pointer can be assigned.

    template <bool allocate_delayed = allocate, std::enable_if_t<allocate_delayed>* = nullptr>
    value_type *operator()() noexcept
    {   return data;
    }

    template <bool allocate_delayed = allocate, std::enable_if_t<allocate_delayed>* = nullptr>
    const value_type *operator()() const noexcept
    {   return data;
    }


    template <bool allocate_delayed = allocate, std::enable_if_t<!allocate_delayed>* = nullptr>
    value_type *& operator()() noexcept
    {   return data;
    }

    template <bool allocate_delayed = allocate, std::enable_if_t<!allocate_delayed>* = nullptr>
    const value_type *& operator()() const noexcept
    {   return data;
    }



public:

    //  Indexing.
    //  Currently only implemented for Layout<conventional>.

    template <typename ...I,
        typename = std::enable_if_t<(std::is_integral_v<I> && ...)>>
    value_type& operator()(I... i) noexcept
    {   return (*this)()[index_offset<order - sizeof...(i)>(i...)];
    }

    template <typename ...I,
        typename = std::enable_if_t<(std::is_integral_v<I> && ...)>>
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

    //  Assignment via a DeepInitList does not require said list to be full, and may use the previously defined value assignment
    //  operator (in case the list's order is less than Array order).

    Array& operator=(const DeepInitList& list) noexcept
    {   for (size_type i = 0; i < list.size(); i++)
            (*this)(i) = list.begin()[i];
        return *this;
    }
};

    }
}
