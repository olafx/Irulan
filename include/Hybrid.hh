#pragma once
#include <cstdlib>
#include <new>
#include "Static.hh"

namespace Irulan
{   namespace Hybrid
    {

//  Hybrid::Array has compile time size and heap allocated data.

template <typename ...Properties>
struct Array : Base<Properties...>
{

public:

    using Base<Properties...>::layout,
          Base<Properties...>::axis,
          Base<Properties...>::allocate,
          typename Base<Properties...>::size_type,
          typename Base<Properties...>::value_type;
    static constexpr size_t order = Base<Properties...>::dims.size();



private:

    using DeepInitList = typename Base<Properties...>::template DeepInitList<order, value_type>::type;



private:

    Static::Array<Properties...> *data;



public:

    //  Constructors with allocation.

    template <bool allocate_delayed = allocate, std::enable_if_t<allocate_delayed>* = nullptr>
    Array()
        : data {new Static::Array<Properties...>}
    {
    }

    template <bool allocate_delayed = allocate, std::enable_if_t<allocate_delayed>* = nullptr>
    Array(const Array& other)
        : data {new Static::Array<Properties...>}
    {   *data = *other.data;
    }

    template <bool allocate_delayed = allocate, std::enable_if_t<allocate_delayed>* = nullptr>
    Array(const DeepInitList& list)
        : data {new Static::Array<Properties...>}
    {   *data = list;
    }



    //  Constructor without allocation.

    template <bool allocate_delayed = allocate, std::enable_if_t<!allocate_delayed>* = nullptr>
    Array() noexcept
        : data {NULL}
    {
    }

    template <bool allocate_delayed = allocate, std::enable_if_t<!allocate_delayed>* = nullptr>
    Array(value_type *data) noexcept
        : data {reinterpret_cast<Static::Array<Properties...> *>(data)}
    {
    }



    ~Array()
    {   if constexpr (allocate)
            delete data;
    }



public:

    //  Dimension operator.

    template <typename I>
    constexpr const size_type& operator[](I i) const noexcept
    {   return (*data)[i];
    }



public:

    //  Raw data access. For Arrays with Allocate<false>, the raw pointer can be assigned.

    template <bool allocate_delayed = allocate, std::enable_if_t<allocate_delayed>* = nullptr>
    value_type *operator()() noexcept
    {   return (*data)();
    }

    template <bool allocate_delayed = allocate, std::enable_if_t<allocate_delayed>* = nullptr>
    const value_type *operator()() const noexcept
    {   return (*data)();
    }


    template <bool allocate_delayed = allocate, std::enable_if_t<!allocate_delayed>* = nullptr>
    value_type *& operator()() noexcept
    {   return reinterpret_cast<value_type *&>(data);
    }

    template <bool allocate_delayed = allocate, std::enable_if_t<!allocate_delayed>* = nullptr>
    const value_type *& operator()() const noexcept
    {   return reinterpret_cast<value_type *&>(data);
    }



public:

    //  Indexing.
    //  Currently only implemented for Layout<conventional>.

    template <typename I, typename ...J>
    auto& operator()(I i, J... j) noexcept
    {   return (*data)(i, j...);
    }

    template <typename I, typename ...J>
    const auto& operator()(I i, J... j) const noexcept
    {   return (*data)(i, j...);
    }



public:

    //  Assignment via a value will set all data to this value.

    Array& operator=(const value_type& value) noexcept
    {   *data = value;
        return *this;
    }

    //  Assignment via a DeepInitList does not require said list to be full, and may use the previously defined value
    //  assignment operator (in case the list's order is less than Array order).

    Array& operator=(const DeepInitList& list) noexcept
    {   *data = list;
        return *this;
    }

};

    }
}
