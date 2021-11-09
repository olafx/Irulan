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

private:

    using Base_ = Base<Properties...>;
    using Base_::combinations;



public:

    using Base_::layout,
          Base_::axis,
          Base_::allocate,
          typename Base_::size_type,
          typename Base_::value_type;
    static constexpr size_t order = Base_::dims[0];



private:

    //  Some early compile time checks for incorrect use.

    static_assert(Base_::dims.size() == 1, "dynamic arrays template their order, and should only have one");



private:

    size_type dims[order];
    value_type *data;



private:

    //  Compile time checks for incorrect use.

    template <typename ...Dims>
    constexpr void dims_validity(Dims... dims) noexcept
    {   static_assert((std::is_integral_v<Dims> && ...), "dimension types must be integral");
        static_assert((std::is_convertible_v<Dims, size_type> && ...), "dimension types must be convertible to size type");
        if constexpr (layout == conventional)
            static_assert(sizeof...(dims) == order, "number of given dimensions must equal order");
        else if constexpr (layout == packed)
            static_assert(sizeof...(dims) == 1, "packed arrays have equal sides so need only one dimension");
    }

    template <typename ...I>
    static constexpr void index_validity(I... i) noexcept
    {   static_assert((std::is_integral_v<I> && ...), "index types must be integral");
        static_assert(sizeof...(i) <= order, "must have at most as many indexes as order");
    }



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
    {   dims_validity(dims...);
    }

    template <typename ...Dims,
        bool allocate_delayed = allocate, std::enable_if_t<!allocate_delayed>* = nullptr>
    Array(Dims... dims) noexcept
        : dims {static_cast<size_type>(dims)...},
          data {NULL}
    {   dims_validity(dims...);
    }



    ~Array() noexcept
    {   if constexpr (allocate)
            delete[] data;
    }



public:

    //  Dimension operator.

    template <typename I>
    size_type& operator[](I i) noexcept
    {   index_validity(i);
        return dims[i];
    }

    template <typename I>
    const size_type& operator[](I i) const noexcept
    {   index_validity(i);
        return dims[i];
    }



public:

    //  Raw data access. For Arrays with Allocate<false>, the raw pointer can be assigned.

    template <bool allocate_delayed = allocate, typename = std::enable_if_t<allocate_delayed>>
    value_type *operator()() noexcept
    {   return data;
    }

    template <bool allocate_delayed = allocate, typename = std::enable_if_t<allocate_delayed>>
    const value_type *operator()() const noexcept
    {   return data;
    }


    value_type *& operator()() noexcept
    {   return data;
    }

    const value_type *& operator()() const noexcept
    {   return data;
    }



public:

    //  Indexing.
    //  Currently only implemented for Layout<conventional>.

    template <typename ...I>
    value_type& operator()(I... i) noexcept
    {   index_validity(i...);
        return (*this)()[index_offset<order - sizeof...(i)>(i...)];
    }

    template <typename ...I>
    const value_type& operator()(I... i) const noexcept
    {   index_validity(i...);
        return (*this)()[index_offset<order - sizeof...(i)>(i...)];
    }
};

    }
}
