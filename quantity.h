/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2014 André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_QUANTITY_H_
#define AM_NUMERIC_QUANTITY_H_


#include <cstdint>
#include <cmath>
#include <cassert>

#include "traits.h"
#include "limits.h"
#include "narrowing.h"
#include "conversion.h"


namespace am {
namespace num {


/*****************************************************************************
 *
 * @brief   represents an integral quantity "safe unsigned int"
 * @details value is either in range [0,max] or infinity
 *
 *
 *****************************************************************************/
template<class IntT>
class quantity
{
    static_assert(is_integral<IntT>::value && !is_unsigned<IntT>::value,
        "quantity<T>: T has to be a signed integral number type");

public:
    //---------------------------------------------------------------
    using value_type = IntT;
    using numeric_type = value_type;


    //---------------------------------------------------------------
    constexpr
    quantity(value_type v = zero_val()) noexcept :
        v_(v >= zero_val() ? v : zero_val())
    {}
    //-----------------------------------------------------
    template<class T, class = typename
        std::enable_if<is_number<decay_t<T>>::value>::type>
    constexpr
    quantity(T&& v) noexcept :
        v_(v < zero_val() ? zero_val() :
            (v > max_val()
                ? infty_val()
                : value_type(std::forward<T>(v)) ))
    {
         static_assert(is_integral<decay_t<T>>::value,
            "quantity<T>(x) : x has to be an integral number");

         AM_CHECK_NARROWING(value_type,T)
    }

    //-----------------------------------------------------
    constexpr
    quantity(const quantity& source) noexcept :
        v_(source.v_)
    {}
    //-----------------------------------------------------
    constexpr
    quantity(quantity&& source) noexcept :
        v_(std::move(source.v_))
    {}

    //---------------------------------------------------------------
    quantity&
    operator = (const quantity& source) noexcept {
        v_ = source.v_;
        return *this;
    }
    //-----------------------------------------------------
    quantity&
    operator = (quantity&& source) noexcept {
        v_ = std::move(source.v_);
        return *this;
    }

    //-----------------------------------------------------
    template<class T, class = typename
        std::enable_if<is_number<decay_t<T>>::value>::type>
    quantity&
    operator = (T&& v) noexcept
    {
        static_assert(is_integral<decay_t<T>>::value,
            "quantity<T> = x : x has to be an integral number");

        AM_CHECK_NARROWING(value_type,T)

        v_ = (v >= zero_val()) ? value_type(std::forward<T>(v)) : zero_val();

        return *this;
    }


    //---------------------------------------------------------------
    constexpr
    const value_type&
    value() const noexcept {
        return v_;
    }
    //-----------------------------------------------------
    template<class T, class = typename
        std::enable_if<is_number<T>::value>::type>
    explicit
    operator T() const noexcept
    {
        return (isinf(*this)) ? numeric_limits<T>::infinity() : T(v_);
    }


    //---------------------------------------------------------------
    static constexpr quantity
    zero() noexcept {
        return quantity(zero_val());
    }
    //-----------------------------------------------------
    static constexpr quantity
    max() noexcept {
        return quantity(max_val);
    }
    //-----------------------------------------------------
    static constexpr quantity
    infinity() noexcept {
        return quantity{0,0};
    }

    //-----------------------------------------------------
    inline friend
    constexpr bool
    isinf(const quantity& q) noexcept {
        return (q.v_ < zero_val());
    }


    //---------------------------------------------------------------
    quantity&
    operator += (const quantity& c) noexcept
    {
        if(!isinf(*this)) {
            if(isinf(c))
                *this = infinity();
            else
                v_ += c.v_;
        }
        return *this;
    }

    //-----------------------------------------------------
    template<class OtherIntT>
    quantity&
    operator += (const quantity<OtherIntT>& c) noexcept
    {
        AM_CHECK_NARROWING(value_type,OtherIntT)

        if(!isinf(*this)) {
            if(isinf(c))
                *this = infinity();
            else
                v_ += value_type(c);
        }
        return *this;
    }

    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<decay_t<T>>::value>::type>
    quantity&
    operator += (T&& v) noexcept
    {
        static_assert(is_integral<decay_t<T>>::value,
            "quantity += x : x has to be an integral number");

        AM_CHECK_NARROWING(value_type,T)

        if(!isinf(*this)) {
            v_ += value_type(std::forward<T>(v));
        }
        return *this;
    }


    //---------------------------------------------------------------
    quantity&
    operator -= (const quantity& c) noexcept
    {
        if(isinf(*this)) {
            if(isinf(c))
                v_ = zero_val();
        }
        else {
            if(!isinf(c) && (v_ > c.v_))
                v_ -= c.v_;
            else
                v_ = zero_val();
        }
        return *this;
    }

    //-----------------------------------------------------
    template<class OtherIntT>
    quantity&
    operator -= (const quantity<OtherIntT>& c) noexcept
    {
        AM_CHECK_NARROWING(value_type,OtherIntT)

        if(isinf(*this)) {
            if(isinf(c))
                v_ = zero_val();
        }
        else {
            if(!isinf(c) && (v_ > c.v_))
                v_ -= value_type(c);
            else
                v_ = zero_val();
        }
        return *this;
    }

    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<decay_t<T>>::value>::type>
    quantity&
    operator -= (T&& v) noexcept
    {
        static_assert(is_integral<decay_t<T>>::value,
            "quantity += x : x has to be an integral number");

        AM_CHECK_NARROWING(value_type,T)

        if(!isinf(*this)) {
            if(v_ > v)
                v_ -= value_type(std::forward<T>(v));
            else
                v_ = zero_val();
        }
        return *this;
    }


    //---------------------------------------------------------------
    quantity&
    operator *= (const quantity& c) noexcept
    {
        if(!isinf(*this)) {
            if(isinf(c))
                *this = infinity();
            else
                v_ *= c.v_;
        }
        return *this;
    }

    //-----------------------------------------------------
    template<class OtherIntT>
    quantity&
    operator *= (const quantity<OtherIntT>& c) noexcept
    {
        AM_CHECK_NARROWING(value_type,OtherIntT)

        if(!isinf(*this)) {
            if(isinf(c))
                *this = infinity();
            else
                v_ *= value_type(c);
        } else {
            if(c == c.zero_val()) v_ = zero_val();
        }
        return *this;
    }

    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<decay_t<T>>::value>::type>
    quantity&
    operator *= (T&& v) noexcept
    {
        static_assert(is_integral<decay_t<T>>::value,
            "quantity *= x : x has to be an integral number");

        AM_CHECK_NARROWING(value_type,T)

        if(!isinf(*this)) {
            if(v > zero_val())
                v_ *= value_type(std::forward<T>(v));
            else
                v_ = zero_val();
        }
        return *this;
    }


    //---------------------------------------------------------------
    quantity&
    operator ++ () noexcept {
        if(!isinf(*this)) ++v_;
        return *this;
    }
    //-----------------------------------------------------
    quantity
    operator ++ (int) noexcept {
        auto old = *this;
        ++*this;
        return old;
    }
    //-----------------------------------------------------
    quantity&
    operator -- () noexcept {
        if(v_ > zero()) --v_; else v_ = zero_val();
        return *this;
    }
    //-----------------------------------------------------
    quantity
    operator -- (int) noexcept {
        auto old = *this;
        ++*this;
        return old;
    }

private:
    //---------------------------------------------------------------
    static constexpr value_type
    zero_val() {
        return value_type(0);
    }
    static constexpr value_type
    infty_val() {
        return value_type(-1);
    }
    static constexpr value_type
    max_val() {
        return numeric_limits<value_type>::max();
    }


    //---------------------------------------------------------------
    /// @brief special ctor for infinity initialization
    constexpr
    quantity(int,int) noexcept :
        v_(infty_val())
    {}

    //---------------------------------------------------------------
    value_type v_;

};






/*****************************************************************************
 *
 *
 *
 *
 *****************************************************************************/

template<class IntT>
inline constexpr quantity<signed_t<decay_t<IntT>>>
make_quantity(IntT&& x)
{
    static_assert(is_integral<IntT>::value,
        "make_quantity(x): x has to be an integral number");

//    assert(x <= numeric_limits<signed_t<decay_t<IntT>>>::max());

    return quantity<signed_t<decay_t<IntT>>>{make_signed(std::forward<IntT>(x))};
}



namespace literals {

//-------------------------------------------------------------------
constexpr quantity<long long int>
operator "" _qty (unsigned long long int x)
{
    return (x > static_cast<unsigned long long int>(
            std::numeric_limits<long long int>::max()))
               ? quantity<long long int>::infinity()
               : quantity<long long int>(static_cast<long long int>(x));
}

} // namespace literals






/*****************************************************************************
 *
 *
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
quantity<common_numeric_t<T1,T2>>
operator + (const quantity<T1>& a, const quantity<T2>& b) noexcept
{
    using cv_t = common_numeric_t<T1,T2>;
    using res_t = quantity<cv_t>;

    return (isinf(a) || isinf(b))
        ? res_t::infinity()
        : res_t{cv_t(a) + cv_t(b)};
}

//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
quantity<common_numeric_t<T1,T2>>
operator - (const quantity<T1>& a, const quantity<T2>& b) noexcept
{
    using cv_t = common_numeric_t<T1,T2>;
    using res_t = quantity<cv_t>;

    return (isinf(a))
        ? (isinf(b)) ? res_t::zero() : res_t::infinity()
        : (isinf(b)) ? res_t::zero() : res_t{cv_t(a) - cv_t(b)};
}

//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
quantity<common_numeric_t<T1,T2>>
operator * (const quantity<T1>& a, const quantity<T2>& b) noexcept
{
    using cv_t = common_numeric_t<T1,T2>;
    using res_t = quantity<cv_t>;

    return (isinf(a))
        ? ( (isinf(b))
            ? res_t::infinity()
            : ((b == b.zero()) ? res_t::zero() : res_t::infinity())
        )
        : ( (isinf(b))
            ? ((a == a.zero()) ? res_t::zero() : res_t::infinity())
            : res_t{cv_t(a) * cv_t(b)}
        );
}






/*****************************************************************************
 *
 *
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
operator == (const quantity<T1>& a, const quantity<T2>& b) noexcept
{
    return (a.value() == b.value());
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator == (const quantity<T1>& a, const T2& b) noexcept
{
    return (a.value() == b);
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator == (const T2& b, const quantity<T1>& a) noexcept
{
    return (b == a.value());
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
operator != (const quantity<T1>& a, const quantity<T2>& b) noexcept
{
    return (a.value() != b.value());
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator != (const quantity<T1>& a, const T2& b) noexcept
{
    return (a.value() != b);
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator != (const T2& b, const quantity<T1>& a) noexcept
{
    return (b != a.value());
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
operator <= (const quantity<T1>& a, const quantity<T2>& b) noexcept
{
    return isinf(b) || (!isinf(a) && (a.value() <= b.value()));
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_number<T2>::value>>
inline constexpr bool
operator <= (const quantity<T1>& a, const T2& b) noexcept
{
    using std::isinf;

    return isinf(b) || (!isinf(a) && (a.value() <= b.value()));
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_number<T2>::value>>
inline constexpr bool
operator <= (const T2& b, const quantity<T1>& a) noexcept
{
    using std::isinf;

    return isinf(b) || (!isinf(a) && (a.value() <= b));
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
operator <  (const quantity<T1>& a, const quantity<T2>& b) noexcept
{
    return !isinf(a) && (isinf(b) || (a.value() < b.value()) );
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_number<T2>::value>>
inline constexpr bool
operator <  (const quantity<T1>& a, const T2& b) noexcept
{
    using std::isinf;

    return !isinf(a) && (isinf(b) || (a.value() < b) );
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_number<T2>::value>>
inline constexpr bool
operator <  (const T2& b, const quantity<T1>& a) noexcept
{
    using std::isinf;

    return !isinf(a) && (isinf(b) || (a.value() < b) );
}


//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
operator >= (const quantity<T1>& a, const quantity<T2>& b) noexcept
{
    return isinf(a) || (!isinf(b) && (a.value() >= b.value()));
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_number<T2>::value>>
inline constexpr bool
operator >= (const quantity<T1>& a, const T2& b) noexcept
{
    using std::isinf;

    return isinf(a) || (!isinf(b) && (a.value() >= b.value()));
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_number<T2>::value>>
inline constexpr bool
operator >= (const T2& b, const quantity<T1>& a) noexcept
{
    using std::isinf;

    return isinf(a) || (!isinf(b) && (a.value() >= b.value()));
}


//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
operator >  (const quantity<T1>& a, const quantity<T2>& b) noexcept
{
    return !isinf(b) && (isinf(a) || (a.value() > b.value()) );
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_number<T2>::value>>
inline constexpr bool
operator >  (const quantity<T1>& a, const T2& b) noexcept
{
    using std::isinf;

    return !isinf(b) && (isinf(a) || (a.value() > b) );
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_number<T2>::value>>
inline constexpr bool
operator >  (const T2& b, const quantity<T1>& a) noexcept
{
    using std::isinf;

    return !isinf(b) && (isinf(a) || (a.value() > b) );
}






/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class T>
struct numeric_limits<quantity<T>> :
    public numeric_limits<T>
{
    static constexpr quantity<T>
    infinity() {return quantity<T>::infinity(); }

    static constexpr quantity<T>
    lowest() {return quantity<T>::zero(); }

    static constexpr quantity<T>
    min() {return quantity<T>::zero(); }

    static constexpr quantity<T>
    max() {return quantity<T>::max(); }
};






/*****************************************************************************
 *
 * I/O
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class Ostream, class Int>
inline Ostream&
operator << (Ostream& os, const quantity<Int>& c)
{
    if(isinf(c))
        os << "inf";
    else
        os << std::intmax_t(c);

    return os;
}

//---------------------------------------------------------
template<class Ostream, class Int>
inline Ostream&
print(Ostream& os, const quantity<Int>& c)
{
    if(isinf(c))
        os << "oo";
    else
        os << "#" << std::intmax_t(c);

    return os;
}


}  // namespace num
}  // namespace am


#endif
