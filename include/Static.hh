#pragma once
#include "Base.hh"

namespace Irulan
{   namespace Static
    {

//  Static::Array has compile time size and stack allocated data.

template <typename ...Properties>
struct Array : public Base<Properties...>
{

private:

    using Base_ = Base<Properties...>;
    using Base_::dims;



public:

    using Base_::layout,
          Base_::axis,
          typename Base_::size_type,
          typename Base_::value_type;
    static constexpr size_t order = dims.size();



private:

    //  Some early compile time checks for incorrect use.

    static_assert((layout != packed_inc && layout != packed_dec) || []()
    {   for (size_t i = 1; i < order; i++)
            if (dims[0] != dims[i])
                return false;
        return true;
    }(), "packed arrays should have equal sides");



private:

    using DeepInitList = typename Base_::template DeepInitList<order, value_type>::type;



private:

    //  Create the Data type, which is the type of the data this Array stores.
    //  This depends on properties like Layout and Axis.


    //  Unused default.

    template <size_t order, LayoutEnum layout,
        typename = void>
    struct Data
    {
    };


    //  Layout<conventional> recursion case.

    template <size_t order>
    struct Data<order, conventional,
        std::enable_if_t<order != 1>>
    {
        //  The elements of Array can be seen as lower order Arrays. These have the same properties, except for the shape property.
        //  Create an Array type with the same Properties, except set the shape to the one given by ShapeModifier::Set.

        //  Recursion case for when B is not the shape property.

        template <typename, typename B, typename ...A>
        struct ShapeModifier
        {   template <typename shape, typename ...C>
            struct Set
            {   using type = typename ShapeModifier<void, A...>::template Set<shape, C..., B>::type;
            };
        };

        //  Recursion case for when B is the shape property.

        template <typename B, typename ...A>
        struct ShapeModifier<std::enable_if_t<std::is_array_v<B>>, B, A...>
        {   template <typename shape, typename ...C>
            struct Set
            {   using type = typename ShapeModifier<void, A...>::template Set<shape, C..., shape>::type;
            };
        };

        //  Base case for when B is not the shape property.

        template <typename Enabled, typename B>
        struct ShapeModifier<Enabled, B>
        {   template <typename shape, typename ...C>
            struct Set
            {   using type = Array<C..., B>;
            };
        };

        //  Base case for when B is the shape property.

        template <typename B>
        struct ShapeModifier<std::enable_if_t<std::is_array_v<B>>, B>
        {   template <typename shape, typename ...C>
            struct Set
            {   using type = Array<C..., shape>;
            };
        };

        //  Create the shape of the Array this Array can be seen to contain.

        template <size_t length, typename T, typename = void>
        struct CreateShape
        {   using type = typename CreateShape<length, T[dims[length - 1 - std::rank_v<T>]]>::type;
        };

        template <size_t length, typename T>
        struct CreateShape<length, T, std::enable_if_t<std::rank_v<T> == length>>
        {   using type = T;
        };

        using ShapeModifier_ = ShapeModifier<void, Properties...>;
        using ElemShape = typename CreateShape<order - 1, value_type>::type;
        using ElemType = typename ShapeModifier_::template Set<ElemShape>::type;

        ElemType value[dims[order - 1]];
    };


    //  Layout<conventional> base case.

    template <size_t order>
    struct Data<order, conventional,
        std::enable_if_t<order == 1>>
    {   value_type value[dims[0]];
    };


    //  Layout<packed_inc> case.

    template <size_t order>
    struct Data<order, packed_inc>
    {   value_type value[Base_::combinations(order + dims[0] - 1, order)];
    };

    //  Layout<packed_dec> case.

    template <size_t order>
    struct Data<order, packed_dec>
    {   value_type value[Base_::combinations(order + dims[0] - 1, order)];
    };


    Data<order, layout> data;



private:

    //  Compile time checks for incorrect use.

    template <typename ...I>
    static constexpr void index_validity(I... i) noexcept
    {   static_assert((std::is_integral_v<I> && ...), "index types must be integral");
        static_assert(sizeof...(i) <= order, "must have at most as many indexes as order");
    }



public:

    Array() noexcept = default;

    Array(const Array& other) noexcept = default;

    constexpr Array(const DeepInitList& list) noexcept
        : data {}
    {   *this = list;
    }



public:

    //  Dimension operator.

    template <typename I>
    constexpr const size_type& operator[](I i) const noexcept
    {   index_validity(i);
        return dims[i];
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
        else if constexpr (layout == packed_inc || layout == packed_dec)
            return data.value;
    }

    constexpr const value_type *operator()() const noexcept
    {   if constexpr (layout == conventional)
        {   if constexpr (order == 1)
                return data.value;
            else
                return data.value[0]();
        }
        else if constexpr (layout == packed_inc || layout == packed_dec)
            return data.value;
    }



public:

    //  Indexing.
    //  Currently only implemented for Layout<conventional>.

    template <typename I, typename ...J>
    constexpr auto& operator()(I i, J... j) noexcept
    {   index_validity(i, j...);
        if constexpr (layout == conventional)
        {   if constexpr (sizeof...(j) == 0)
                return data.value[i];
            else
                return (*this)(j...)(i);
        }
        else if constexpr ((layout == packed_inc || layout == packed_dec) && sizeof...(j) + 1 != order)
            return (*this)(0, i, j...);
        else if constexpr (layout == packed_inc)
            return data.value[Base_::PackedIndexing::template C_inc<0>(i, j...)];
        else if constexpr (layout == packed_dec)
            return data.value[Base_::PackedIndexing::template C_dec<0>((*this)[0], i, j...)];
    }

    template <typename I, typename ...J>
    constexpr const auto& operator()(I i, J... j) const noexcept
    {   index_validity(i, j...);
        if constexpr (layout == conventional)
        {   if constexpr (sizeof...(j) == 0)
                return data.value[i];
            else
                return (*this)(j...)(i);
        }
        else if constexpr ((layout == packed_inc || layout == packed_dec) && sizeof...(j) + 1 != order)
            return (*this)(0, i, j...);
        else if constexpr (layout == packed_inc)
            return data.value[Base_::PackedIndexing::template C_inc<0>(i, j...)];
        else if constexpr (layout == packed_dec)
            return data.value[Base_::PackedIndexing::template C_dec<0>((*this)[0], i, j...)];
    }



public:

    //  Assignment via a DeepInitList does not require said list to be full, and may use the previously defined value assignment
    //  operator (in case the list's order is less than Array's order).

    constexpr Array& operator=(const DeepInitList& list) noexcept
    {   for (size_type i = 0; i < list.size(); i++)
            (*this)(i) = list.begin()[i];
        return *this;
    }

};

    }
}
