/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/

#ifndef AMLIB_NUMERIC_CONSTANT_H_
#define AMLIB_NUMERIC_CONSTANT_H_


#include "traits.h"


namespace am {
namespace num {


/*****************************************************************************
 *
 * @brief represents an immutable constant that keeps its initial value
 *
 *
 *
 *****************************************************************************/
template<class T>
class constant
{
public:
    //---------------------------------------------------------------
    using value_type = T;
    using numeric_type = value_type;


    //---------------------------------------------------------------
    /// @brief construction is only allowed with a concrete value
    template<class V, class = typename
        std::enable_if<!std::is_same<decay_t<V>,constant<T>>::value>::type>
    explicit constexpr
    constant(V&& value):
        v_(std::forward<T>(value))
    {}

    //-----------------------------------------------------
    constant(const constant&) = default;
    constant(constant&&) = default;


    //---------------------------------------------------------------
    constant& operator = (const constant&) = delete;
    constant& operator = (constant&&) = delete;


    //---------------------------------------------------------------
    constexpr const value_type&
    value() const noexcept {
        return v_;
    }

    //-----------------------------------------------------
    /// @brief allow implicit conversions to wrapped type
    constexpr
    operator const value_type& () const noexcept {
        return v_;
    }


private:
    value_type v_;

};






/*****************************************************************************
 *
 *
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T>
inline constexpr bool
is_constant(const constant<T>&) noexcept {
    return true;
}

//-----------------------------------------------------
template<class T>
inline constexpr bool
is_constant(const T&) noexcept {
    return false;
}



//-------------------------------------------------------------------
template<class T>
inline constexpr constant<decay_t<T>>
make_constant(T&& x)
{
    return constant<decay_t<T>>{std::forward<T>(x)};
}



}  // namespace num
}  // namespace am


#endif
