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
          typename Base<Properties...>::size_type,
          typename Base<Properties...>::value_type;
    static constexpr size_t order = Base<Properties...>::dims.size();



private:

    using DeepInitList = typename Base<Properties...>::template DeepInitList<order, value_type>::type;



private:

    Static::Array<Properties...> *data;



public:

    Array()
        : data {new Static::Array<Properties...>}
    {
    }

    Array(const Array& other)
        : data {new Static::Array<Properties...>}
    {   *data = *other.data;
    }

    Array(const DeepInitList& list)
        : data {new Static::Array<Properties...>}
    {   *data = list;
    }

    ~Array()
    {   delete data;
    }



public:

    //  Dimension operator.

    template <typename I>
    constexpr const size_type& operator[](I i) const noexcept
    {   return (*data)[i];
    }



public:

    //  Raw data access.

    value_type *operator()() noexcept
    {   return (*data)();
    }



public:

    //  Indexing.
    //  Currently only implemented for Layout<conventional>.

    template <typename ...I>
    auto& operator()(I... i) noexcept
    {   return (*data)(i...);
    }

    template <typename ...I>
    const auto& operator()(I... i) const noexcept
    {   return (*data)(i...);
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
