#pragma once
#include <new>
#include "Static.hh"

namespace Irulan
{   namespace Hybrid
    {

template <typename ...Properties>
struct Array : Base<Properties...>
{

public:
    using Base<Properties...>::layout,
          Base<Properties...>::axis,
          typename Base<Properties...>::size_type,
          typename Base<Properties...>::value_type;
    static constexpr size_t order {Base<Properties...>::dims.size()};
private:
    using deep_init_list = typename Base<Properties...>::template DeepInitList<order, size_type>::type;

private:
    Static::Array<Properties...> *data;

public:
    Array()
        : data {static_cast<decltype(data)>(malloc(sizeof(decltype(*data))))}
    {   if (data == NULL)
            throw std::bad_alloc {};
    }
    Array(const Array& other)
        : data {static_cast<decltype(data)>(malloc(sizeof(decltype(*data))))}
    {   if (data == NULL)
            throw std::bad_alloc {};
        else
            *data = *other.data;
    }
    Array(const deep_init_list& list)
        : data {static_cast<decltype(data)>(malloc(sizeof(decltype(*data))))}
    {   if (data == NULL)
            throw std::bad_alloc {};
        else
            *data = list;
    }

public:
    template <typename I>
    const size_type& operator[](I i) const noexcept
    {   return (*data)[i];
    }

public:
    value_type *operator()() noexcept
    {   return (*data)();
    }

public:
    template <typename ...I>
    auto& operator()(I... i) noexcept
    {   return (*data)(i...);
    }
    template <typename ...I>
    const auto& operator()(I... i) const noexcept
    {   return (*data)(i...);
    }

public:
    Array& operator=(const value_type& value) noexcept
    {   for (size_type i {}; i < (*this)[order - 1]; i++)
            (*this)(i) = value;
        return *this;
    }
    Array& operator=(const deep_init_list& list) noexcept
    {   *data = list;
        return *this;
    }
};

    }
}
