/*
    MIT License

    Copyright (c) 2021 Olaf Willocx

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#pragma once
#include "Base.h"

namespace Irulan
{   namespace Dynamic
    {

//  Dynamic::Array has runtime size and heap allocated data.

template <typename ...Properties>
struct Array : Base<Properties...>
{

private:

    using Base_ = Base<Properties...>;



public:

    using Base_::layout,
          Base_::axis,
          Base_::allocate,
          Base_::efficient_shape,
          typename Base_::size_type,
          typename Base_::value_type;
    static constexpr std::size_t order = Base_::dims[0];



private:

    static constexpr std::size_t stored_order = Base_::dims[0] - (efficient_shape ? 1 : 0);



private:

    //  Some early compile time checks for incorrect use.

    static_assert(Base_::dims.size() == 1, "dynamic arrays template their order, and should only have one");



private:

    template <size_t n_dims>
    struct Data
    {
        value_type *data;
        size_type dims[n_dims];

        //  Have to prevent potentially writing the last dim in constructor to dims if EfficientShape is enabled.

        template <size_t current = 0, typename Dim, typename ...Dims>
        void set_dims(Dim dim, Dims... dims_)
        {   if constexpr (current != stored_order)
            {   dims[current] = dim;
                set_dims<current + 1>(dims_...);
            }
        }

        template <size_t current = 0>
        void set_dims()
        {
        }
    };

    template <>
    struct Data<0>
    {   value_type *data;

        template <typename ...Dims>
        void set_dims(Dims...)
        {
        }
    };

    Data<order - (efficient_shape ? 1 : 0)> data;



private:

    //  Compile time checks for incorrect use.

    template <typename ...Dims>
    constexpr void dims_validity(Dims... dims) noexcept
    {   static_assert((std::is_integral_v<Dims> && ...), "dimension types must be integral");
        static_assert((std::is_convertible_v<Dims, size_type> && ...), "dimension types must be convertible to size type");
        if constexpr (layout == conventional)
            static_assert(sizeof...(dims) <= order, "number of given dimensions should be at most order");
        else if constexpr (layout == packed_inc || layout == packed_dec)
            static_assert(sizeof...(dims) <= 1, "packed arrays have equal sides so need only one dimension");
    }

    template <typename ...I>
    static constexpr void index_validity(I... i) noexcept
    {   static_assert((std::is_integral_v<I> && ...), "index types must be integral");
        static_assert(sizeof...(i) <= order, "must have at most as many indexes as order");
    }



private:

    //  Calculate the data size, which depends on Layout and dims.

    template <typename ...Dims>
    static std::size_t size(Dims... dims) noexcept
    {   if constexpr (sizeof...(dims) == 0)
            return 0;
        else if constexpr (layout == conventional)
            return (dims * ...);
        else if constexpr (layout == packed_inc || layout == packed_dec)
            return Base_::combinations(order + [](auto a, auto... b){ return a; }(dims...) - 1, order);
    }



private:

    //  Calculate 1D memory index based on order-dimensional Array index.

    template <std::size_t level, typename I, typename ...J>
    auto index(I i, J... j) const noexcept
    {   if constexpr (layout == conventional)
        {   if constexpr (sizeof...(j) == 0)
                return i;
            else
                return i + index<level + 1>(j...) * (*this)[level];
        }
        else if constexpr (layout == packed_inc)
        {   return Base_::PackedIndexing::template C_inc<0>(i, j...);
        }
        else if constexpr (layout == packed_dec)
        {   return Base_::PackedIndexing::template C_dec<0>((*this)[0], i, j...);
        }
    }



public:

    template <typename ...Dims,
        bool allocate_delayed = allocate, std::enable_if_t<allocate_delayed>* = nullptr>
    Array(Dims... dims)
        : data {new value_type[size(dims...)]}
    {   dims_validity(dims...);
        data.set_dims(dims...);
    }

    template <typename ...Dims,
        bool allocate_delayed = allocate, std::enable_if_t<!allocate_delayed>* = nullptr>
    Array(Dims... dims) noexcept
        : data {NULL}
    {   dims_validity(dims...);
        data.set_dims(dims...);
    }



    ~Array() noexcept
    {   if constexpr (allocate)
            delete[] data.data;
    }



public:

    //  Dimension operator.

    template <typename I>
    size_type& operator[](I i) noexcept
    {   index_validity(i);
        return data.dims[i];
    }

    template <typename I>
    const size_type& operator[](I i) const noexcept
    {   index_validity(i);
        return data.dims[i];
    }



public:

    //  Raw data access. For Arrays with Allocate<false>, the raw pointer can be assigned.

    template <bool allocate_delayed = allocate, typename = std::enable_if_t<allocate_delayed>>
    value_type *operator()() noexcept
    {   return data.data;
    }

    template <bool allocate_delayed = allocate, typename = std::enable_if_t<allocate_delayed>>
    const value_type *operator()() const noexcept
    {   return data.data;
    }


    template <bool allocate_delayed = allocate, typename = std::enable_if_t<!allocate_delayed>>
    value_type *&operator()() noexcept
    {   return data.data;
    }

    template <bool allocate_delayed = allocate, typename = std::enable_if_t<!allocate_delayed>>
    const value_type * const& operator()() const noexcept
    {   return data.data;
    }



public:

    //  Indexing.

    template <typename ...I>
    value_type& operator()(I... i) noexcept
    {   index_validity(i...);
        if constexpr (sizeof...(i) != order)
            return (*this)(0, i...);
        else
            return (*this)()[index<0>(i...)];
    }

    template <typename ...I>
    const value_type& operator()(I... i) const noexcept
    {   index_validity(i...);
        if constexpr (sizeof...(i) != order)
            return (*this)(0, i...);
        else
            return (*this)()[index<0>(i...)];
    }
};

    }
}
