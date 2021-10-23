#pragma once
#include "Base.hh"

namespace Irulan
{   namespace Static
    {

//  Static 'Array' has compile time size and stack allocated data.

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
    static constexpr size_t order = dims.size();



private:

    using DeepInitList = typename Base<Properties...>::template DeepInitList<order, size_type>::type;



private:

    //  The elements of 'Array' can be seen as lower order 'Array's. These have the same properties, except for the shape
    //  property. Create an 'Array' type with the same 'Properties', except set the shape to the given 'shape' in 'Modify',
    //  which is contained in 'ShapeModifier'.

    //  Recursion case for when 'B' is not the shape property.

    template <typename, typename B, typename ...A>
    struct ShapeModifier
    {   template <typename shape, typename ...C>
        struct Modify
        {   typedef typename ShapeModifier<void, A...>::template Modify<shape, C..., B>::type type;
        };
    };

    //  Recursion case for when 'B' is the shape property.

    template <typename B, typename ...A>
    struct ShapeModifier<std::enable_if_t<std::is_array_v<B>>, B, A...>
    {   template <typename shape, typename ...C>
        struct Modify
        {   typedef typename ShapeModifier<void, A...>::template Modify<shape, C..., shape>::type type;
        };
    };

    //  Base case for when 'B' is not the shape property.

    template <typename Enabled, typename B>
    struct ShapeModifier<Enabled, B>
    {   template <typename shape, typename ...C>
        struct Modify
        {   typedef Array<C..., B> type;
        };
    };

    //  Base case for when 'B' is the shape property.

    template <typename B>
    struct ShapeModifier<std::enable_if_t<std::is_array_v<B>>, B>
    {   template <typename shape, typename ...C>
        struct Modify
        {   typedef Array<C..., shape> type;
        };
    };



private:

    //  Create the shape of 'Array's this 'Array' can be seen to contain.
    //  The result depends on whether storage is row or column major. At the moment only column major is supported.

    template <size_t length, typename T, typename = void>
    struct CreateShape
    {   typedef typename CreateShape<length, T[dims[length - 1 - std::rank_v<T>]]>::type type;
    };

    template <size_t length, typename T>
    struct CreateShape <length, T, std::enable_if_t<std::rank_v<T> == length>>
    {   typedef T type;
    };



private:

    //  Create the 'Data' type; the type of the data this 'Array' stores.
    //  This depends on properties like 'Layout' and 'Axis'.

    //  Unused default.

    template <size_t order, LayoutEnum layout, typename = void>
    struct Data
    {};

    //  'Layout<conventional>' recursion case.

    template <size_t order>
    struct Data<order, conventional, std::enable_if_t<order != 1>>
    {   typename ShapeModifier<void, Properties...>::template
            Modify<typename CreateShape<order - 1, value_type>::type>::type value[dims[order - 1]];
    };

    //  'Layout<conventional>' base case.

    template <size_t order>
    struct Data<order, conventional, std::enable_if_t<order == 1>>
    {   value_type value[dims[0]];
    };

    //  'Layout<packed>' case.

    template <size_t order>
    struct Data<order, packed>
    {   value_type value[combinations(order + dims[0] - 1, order)];
    };

    Data<order, layout> data;



public:

    Array() noexcept = default;

    Array(const Array& other) noexcept = default;

    //  The 'DeepInitList' constructor works via the assignment operator overload.

    constexpr Array(const DeepInitList& list) noexcept : data {}
    {   *this = list;
    }



public:

    //  Dimension operator.

    template <typename I, std::enable_if_t<std::is_integral_v<I>>* = nullptr>
    constexpr const size_type& operator[](I i) const noexcept
    {   return dims[i];
    }



public:

    //  Raw data access.

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

    //  Indexing.
    //  Currently only implemented for 'Layout<conventional>'.

    template <typename I, typename ...J, std::enable_if_t<std::is_integral_v<I>>* = nullptr>
    constexpr auto& operator()(I i, J... j) noexcept
    {   if constexpr (layout == conventional)
        {   if constexpr (sizeof...(j) == 0)
                return data.value[i];
            else
                return (*this)(j...)(i);
        }
        if constexpr (layout == packed)
            //  TODO
            return;
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
            //  TODO
            return;
    }



public:

    //  Assignment via a value will set all data to this value.

    constexpr Array& operator=(const value_type& value) noexcept
    {   for (size_type i = 0; i < (*this)[order - 1]; i++)
            (*this)(i) = value;
        return *this;
    }

    //  Assignment via a 'DeepInitList' does not require said list to be full, and may use the previously defined value
    //  assignment operator (in case the list's order is less than 'order').

    constexpr Array& operator=(const DeepInitList& list) noexcept
    {   for (size_type i = 0; i < list.size(); i++)
            (*this)(i) = list.begin()[i];
        return *this;
    }

};

    }
}
