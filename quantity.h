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
#include <iostream>

#include "limits.h"
#include "narrowing.h"
#include "conversion.h"


namespace am {
namespace num {


/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class> class quantity;

template<class>
struct is_quantity :
    std::false_type
{};

template<class T>
struct is_quantity<quantity<T>> :
    std::true_type
{};






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
    template<class T, class = typename std::enable_if<
        !is_quantity<decay_t<T>>::value &&
        is_number<decay_t<T>>::value>::type>
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
    //-----------------------------------------------------
    template<class T>
    constexpr
    quantity(const quantity<T>& v) noexcept :
        v_((isinf(v) || (v > max_val())
            ? infty_val()
            : value_type(v) ))
    {
         AM_CHECK_NARROWING(value_type,T)
    }

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
    template<class T>
    quantity&
    operator = (const quantity<T>& v) noexcept
    {
        AM_CHECK_NARROWING(value_type,T)

        v_ = (isinf(v) || v > max_val())
                ? infty_val()
                : value_type(v);

        return *this;
    }

    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        !is_quantity<decay_t<T>>::value &&
        is_number<decay_t<T>>::value>::type>
    quantity&
    operator = (T&& v) noexcept
    {
        static_assert(is_integral<decay_t<T>>::value,
            "quantity<T> = x : x has to be an integral number");

        AM_CHECK_NARROWING(value_type,T)

        v_ = v < zero_val() ? zero_val() :
            (v > max_val()
                ? infty_val()
                : value_type(std::forward<T>(v)) );

        return *this;
    }


    //---------------------------------------------------------------
    template<class T, class = typename
        std::enable_if<is_number<T>::value>::type>
    explicit
    operator T() const noexcept
    {
        return (isinf(*this) ? max_infinity<T>::value()
                             : T(v_));
    }

    //-----------------------------------------------------
    value_type
    value() const noexcept
    {
        return (isinf(*this) ? max_infinity<value_type>::value()
                             : v_);
    }


    //---------------------------------------------------------------
    static constexpr quantity
    zero() noexcept {
        return quantity(zero_val());
    }
    //-----------------------------------------------------
    static constexpr quantity
    max() noexcept {
        return quantity(max_val());
    }
    //-----------------------------------------------------
    static constexpr quantity
    infinity() noexcept {
        return quantity{0,0};
    }


    //---------------------------------------------------------------
    inline friend
    constexpr bool
    isinf(const quantity& q) noexcept {
        return (q.v_ < zero_val());
    }
    //-----------------------------------------------------
    inline friend
    constexpr bool
    isfinite(const quantity& q) noexcept {
        return (q.v_ > zero_val());
    }


    //---------------------------------------------------------------
    quantity&
    operator += (const quantity& c) noexcept
    {
        if(!isinf(*this)) {
            if(isinf(c)) {
                *this = infinity();
            } else {
                if((max_val() - v_) > c.v_) {
                    v_ = max_val();
                } else {
                    v_ += c.v_;
                }
            }
        }
        return *this;
    }

    //-----------------------------------------------------
    template<class T>
    quantity&
    operator += (const quantity<T>& c) noexcept
    {
        AM_CHECK_NARROWING(value_type,T)

        if(!isinf(*this)) {
            const auto vc = value_type(c);

            if(isinf(c) || (vc > max_val()))
                *this = infinity();
            else {
                if((max_val() - v_) > vc) {
                    v_ = max_val();
                } else {
                    v_ += vc;
                }
            }
        }
        return *this;
    }

    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        !is_quantity<decay_t<T>>::value &&
        is_number<decay_t<T>>::value>::type>
    quantity&
    operator += (T&& v) noexcept
    {
        static_assert(is_integral<decay_t<T>>::value,
            "quantity += x : x has to be an integral number");

        AM_CHECK_NARROWING(value_type,T)

        if(!isinf(*this)) {
            if(v > max_val()) {
                *this = infinity();
            } else {
                const auto vc = value_type(std::forward<T>(v));;
                if((max_val() - v_) > vc) {
                    v_ = max_val();
                } else {
                    v_ += vc;
                }
            }
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
    template<class T>
    quantity&
    operator -= (const quantity<T>& c) noexcept
    {
        AM_CHECK_NARROWING(value_type,T)

        if(isinf(*this)) {
            if(isinf(c) || (value_type(c) > max_val()))
                v_ = zero_val();
        }
        else {
            if(!isinf(c) && (v_ > c.v_)) {
                v_ -= value_type(c);
            } else {
                v_ = zero_val();
            }
        }
        return *this;
    }

    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        !is_quantity<decay_t<T>>::value &&
        is_number<decay_t<T>>::value>::type>
    quantity&
    operator -= (T&& v) noexcept
    {
        static_assert(is_integral<decay_t<T>>::value,
            "quantity += x : x has to be an integral number");

        AM_CHECK_NARROWING(value_type,T)

        if(!isinf(*this)) {
            if(v_ > v) {
                v_ -= value_type(std::forward<T>(v));
            } else {
                v_ = zero_val();
            }
        }
        return *this;
    }


    //---------------------------------------------------------------
    quantity&
    operator *= (const quantity& c) noexcept
    {
        if(!isinf(*this)) {
            if(isinf(c) || (c.v_ > max_val())) {
                *this = infinity();
            } else {
                if(c.v_ > (max_val() / v_)) {
                    v_ = max_val();
                } else {
                    v_ *= c.v_;
                }
            }
        }
        return *this;
    }

    //-----------------------------------------------------
    template<class T>
    quantity&
    operator *= (const quantity<T>& c) noexcept
    {
        AM_CHECK_NARROWING(value_type,T)

        if(!isinf(*this)) {
            const auto cv = value_type(c);
            if(isinf(c) || (cv > max_val())) {
                *this = infinity();
            } else {
                if(cv > (max_val() / v_)) {
                    v_ = max_val();
                } else {
                    v_ *= cv;
                }
            }
        } else {
            if(c == c.zero_val()) v_ = zero_val();
        }
        return *this;
    }

    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        !is_quantity<decay_t<T>>::value &&
        is_number<decay_t<T>>::value>::type>
    quantity&
    operator *= (T&& v) noexcept
    {
        static_assert(is_integral<decay_t<T>>::value,
            "quantity *= x : x has to be an integral number");

        AM_CHECK_NARROWING(value_type,T)

        if(!isinf(*this)) {
            if(v > zero_val()) {
                if(v <= max_val()) {
                    const auto cv = value_type(std::forward<T>(v));
                    if(cv > (max_val() / v_)) {
                        v_ = max_val();
                    } else {
                        v_ *= cv;
                    }
                } else {
                    v_ = max_val();
                }
            } else {
                v_ = zero_val();
            }
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
 * ARITHMETIC
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
// +
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
quantity<common_numeric_t<T1,T2>>
operator + (const quantity<T1>& a, const quantity<T2>& b) noexcept
{
    using std::isinf;

    using cv_t = common_numeric_t<T1,T2>;
    using res_t = quantity<cv_t>;

    return (isinf(a) || isinf(b))
        ? res_t::infinity()
        : ( (cv_t(res_t::max()) - cv_t(a)) < cv_t(b)
                ? res_t::max()
                : res_t{cv_t(a) + cv_t(b)});
}

//---------------------------------------------------------
template<class T1, class T2, class = typename std::enable_if<
    !is_quantity<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr common_numeric_t<T1,T2>
operator + (const quantity<T1>& a, const T2& b) noexcept
{
    using std::isinf;

    using cv_t = common_numeric_t<T1,T2>;

    return (isinf(a) || isinf(b))
        ? max_infinity<cv_t>::value()
        : ( (numeric_max<cv_t>::value() - cv_t(a)) < cv_t(b)
               ? numeric_max<cv_t>::value()
               : cv_t(a) + cv_t(b));
}
//---------------------------------------------------------
template<class T1, class T2, class = typename std::enable_if<
    !is_quantity<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator + (const T2& b, const quantity<T1>& a) noexcept
{
    using std::isinf;

    using cv_t = common_numeric_t<T1,T2>;

    return (isinf(a) || isinf(b))
        ? max_infinity<cv_t>::value()
        : ( (numeric_max<cv_t>::value() - cv_t(b)) < cv_t(a)
               ? numeric_max<cv_t>::value()
               : cv_t(b) + cv_t(a));
}



//-------------------------------------------------------------------
// -
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
quantity<common_numeric_t<T1,T2>>
operator - (const quantity<T1>& a, const quantity<T2>& b) noexcept
{
    using std::isinf;

    using cv_t = common_numeric_t<T1,T2>;
    using res_t = quantity<cv_t>;

    return (isinf(a))
        ? (isinf(b)) ? res_t::zero() : res_t::infinity()
        : (isinf(b)) ? res_t::zero() :
            (cv_t(b) > cv_t(a)
               ? res_t::zero()
               : res_t{cv_t(a) - cv_t(b)});
}

//---------------------------------------------------------
template<class T1, class T2, class = typename std::enable_if<
    !is_quantity<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr common_numeric_t<T1,T2>
operator - (const quantity<T1>& a, const T2& b) noexcept
{
    using std::isinf;

    using cv_t = common_numeric_t<T1,T2>;

    return (isinf(a))
        ? (isinf(b)) ? cv_t(0) : max_infinity<cv_t>::value()
        : (isinf(b)) ? cv_t(0) :
            (cv_t(b) > cv_t(a)
               ? cv_t(0)
               : cv_t(a) - cv_t(b));
}
//---------------------------------------------------------
template<class T1, class T2, class = typename std::enable_if<
    !is_quantity<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator - (const T2& b, const quantity<T1>& a) noexcept
{
    using std::isinf;

    using cv_t = common_numeric_t<T1,T2>;

    return (isinf(b))
        ? (isinf(a)) ? cv_t(0) : max_infinity<cv_t>::value()
        : (isinf(a)) ? cv_t(0) :
            (cv_t(a) > cv_t(b)
               ? cv_t(0)
               : cv_t(b) - cv_t(a));
}



//-------------------------------------------------------------------
// *
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
quantity<common_numeric_t<T1,T2>>
operator * (const quantity<T1>& a, const quantity<T2>& b) noexcept
{
    using std::isinf;

    using cv_t = common_numeric_t<T1,T2>;
    using res_t = quantity<cv_t>;

    return (isinf(a))
        ? ( (isinf(b))
            ? res_t::infinity()
            : ((b == b.zero()) ? res_t::zero() : res_t::infinity())
        )
        : ( (isinf(b))
            ? ((a == a.zero()) ? res_t::zero() : res_t::infinity())
            : ((a == a.zero() || b == b.zero())
                  ? res_t::zero()
                  : ((cv_t(a) > (cv_t(res_t::max()) / cv_t(b)))
                      ? res_t::max()
                      : res_t{cv_t(a) * cv_t(b)} )
              )
        );
}

//---------------------------------------------------------
template<class T1, class T2, class = typename std::enable_if<
    !is_quantity<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr common_numeric_t<T1,T2>
operator * (const quantity<T1>& a, const T2& b) noexcept
{
    using std::isinf;

    using cv_t = common_numeric_t<T1,T2>;

    return (isinf(a))
        ? ( (isinf(b))
            ? max_infinity<cv_t>::value()
            : ((b == T2(0)) ? cv_t(0) : max_infinity<cv_t>::value())
        )
        : ( (isinf(b))
            ? ((a == a.zero()) ? cv_t(0) : max_infinity<cv_t>::value())
            : ((a == a.zero() || b == T2(0))
                  ? cv_t(0)
                  : ((cv_t(a) > (numeric_max<cv_t>::value() / cv_t(b)))
                      ? numeric_max<cv_t>::value()
                      : cv_t(a) * cv_t(b) )
              )
        );
}
//---------------------------------------------------------
template<class T1, class T2, class = typename std::enable_if<
    !is_quantity<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator * (const T2& b, const quantity<T1>& a) noexcept
{
    using std::isinf;

    using cv_t = common_numeric_t<T1,T2>;

    return (isinf(a))
        ? ( (isinf(b))
            ? max_infinity<cv_t>::value()
            : ((b == T2(0)) ? cv_t(0) : max_infinity<cv_t>::value())
        )
        : ( (isinf(b))
            ? ((a == a.zero()) ? cv_t(0) : max_infinity<cv_t>::value())
            : ((a == a.zero() || b == T2(0))
                  ? cv_t(0)
                  : ((cv_t(a) > (numeric_max<cv_t>::value() / cv_t(b)))
                      ? numeric_max<cv_t>::value()
                      : cv_t(a) * cv_t(b) )
              )
        );
}






/*****************************************************************************
 *
 *
 * COMPARISONS
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
operator == (const quantity<T1>& a, const quantity<T2>& b) noexcept
{
    return isinf(a) ? isinf(b) : (isinf(b) ? false : (T1(a) == T2(b)) );
}

//---------------------------------------------------------
template<class T1, class T2, class = typename std::enable_if<
    !is_quantity<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator == (const quantity<T1>& a, const T2& b) noexcept
{
    using std::isinf;

    return isinf(a) ? isinf(b) : (isinf(b) ? false : (T1(a) == b) );
}
//---------------------------------------------------------
template<class T1, class T2, class = typename std::enable_if<
    !is_quantity<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator == (const T2& b, const quantity<T1>& a) noexcept
{
    using std::isinf;

    return isinf(a) ? isinf(b) : (isinf(b) ? false : (T1(a) == b) );
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
operator != (const quantity<T1>& a, const quantity<T2>& b) noexcept
{
    return !(a == b);
}

//---------------------------------------------------------
template<class T1, class T2, class = typename std::enable_if<
    !is_quantity<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator != (const quantity<T1>& a, const T2& b) noexcept
{
    return !(a == b);
}
//---------------------------------------------------------
template<class T1, class T2, class = typename std::enable_if<
    !is_quantity<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator != (const T2& b, const quantity<T1>& a) noexcept
{
    return !(a == b);
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
operator <= (const quantity<T1>& a, const quantity<T2>& b) noexcept
{
    return isinf(b) || (!isinf(a) && (T1(a) <= T2(b)));
}

//---------------------------------------------------------
template<class T1, class T2, class = typename std::enable_if<
    !is_quantity<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator <= (const quantity<T1>& a, const T2& b) noexcept
{
    using std::isinf;

    return isinf(b) || (!isinf(a) && (T1(a) <= b));
}
//---------------------------------------------------------
template<class T1, class T2, class = typename std::enable_if<
    !is_quantity<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator <= (const T2& b, const quantity<T1>& a) noexcept
{
    using std::isinf;

    return isinf(b) || (!isinf(a) && (b <= T1(a)));
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
operator <  (const quantity<T1>& a, const quantity<T2>& b) noexcept
{
    return !isinf(a) && (isinf(b) || (T1(a) < T2(b)));
}

//---------------------------------------------------------
template<class T1, class T2, class = typename std::enable_if<
    !is_quantity<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator <  (const quantity<T1>& a, const T2& b) noexcept
{
    using std::isinf;

    return !isinf(a) && (isinf(b) || (T1(a) < b));
}
//---------------------------------------------------------
template<class T1, class T2, class = typename std::enable_if<
    !is_quantity<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator <  (const T2& b, const quantity<T1>& a) noexcept
{
    using std::isinf;

    return !isinf(a) && (isinf(b) || (b < T1(a)) );
}


//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
operator >= (const quantity<T1>& a, const quantity<T2>& b) noexcept
{
    return isinf(a) || (!isinf(b) && (T1(a) >= T2(b)));
}

//---------------------------------------------------------
template<class T1, class T2, class = typename std::enable_if<
    !is_quantity<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator >= (const quantity<T1>& a, const T2& b) noexcept
{
    using std::isinf;

    return isinf(a) || (!isinf(b) && (T1(a) >= b));
}
//---------------------------------------------------------
template<class T1, class T2, class = typename std::enable_if<
    !is_quantity<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator >= (const T2& b, const quantity<T1>& a) noexcept
{
    using std::isinf;

    return isinf(a) || (!isinf(b) && (b >= T1(a)));
}


//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
operator >  (const quantity<T1>& a, const quantity<T2>& b) noexcept
{
    return !isinf(b) && (isinf(a) || (T1(a) > T2(b)) );
}

//---------------------------------------------------------
template<class T1, class T2, class = typename std::enable_if<
    !is_quantity<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator >  (const quantity<T1>& a, const T2& b) noexcept
{
    using std::isinf;

    return !isinf(b) && (isinf(a) || (T1(a) > b) );
}
//---------------------------------------------------------
template<class T1, class T2, class = typename std::enable_if<
    !is_quantity<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator >  (const T2& b, const quantity<T1>& a) noexcept
{
    using std::isinf;

    return !isinf(b) && (isinf(a) || (b > T1(a)) );
}






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







/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class T>
class numeric_limits<quantity<T>>
{
    using val_t = quantity<T>;

public:
    static constexpr bool is_specialized = true;

    static constexpr val_t
    min() {return val_t::zero(); }

    static constexpr val_t
    max() {return val_t::max(); }

    static constexpr val_t
    lowest() {return val_t::zero(); }

    static constexpr int digits = numeric_limits<T>::digits;
    static constexpr int digits10 = numeric_limits<T>::digits10;
    static constexpr int max_digits10 = numeric_limits<T>::max_digits10;
    static constexpr bool is_signed = false;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = numeric_limits<T>::is_exact;
    static constexpr int radix = numeric_limits<T>::radix;

    static constexpr val_t
    tolerance() noexcept { return num::tolerance<T>::value(); }

    static constexpr val_t
    round_error() noexcept { return numeric_limits<T>::round_error(); }

    static constexpr int min_exponent   = numeric_limits<T>::min_exponent;
    static constexpr int min_exponent10 = numeric_limits<T>::min_exponent10;
    static constexpr int max_exponent   = numeric_limits<T>::max_exponent;
    static constexpr int max_exponent10 = numeric_limits<T>::max_exponent10;

    static constexpr bool has_infinity = true;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;
    static constexpr std::float_denorm_style has_denorm = std::denorm_absent;
    static constexpr bool has_denorm_loss = false;

    static constexpr val_t
    infinity() noexcept {return val_t::infinity(); }

    static constexpr val_t
    quiet_NaN() noexcept {return val_t(numeric_limits<T>::quiet_NaN()); }

    static constexpr val_t
    signaling_NaN() noexcept {return val_t(numeric_limits<T>::signaling_NaN()); }

    static constexpr val_t
    denorm_min() noexcept {return val_t(numeric_limits<T>::denorm_min()); }

    static constexpr bool is_iec559 = numeric_limits<T>::is_iec559;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = numeric_limits<T>::is_modulo;

    static constexpr bool traps = numeric_limits<T>::traps;
    static constexpr bool tinyness_before = numeric_limits<T>::tinyness_before;
    static constexpr std::float_round_style round_style = numeric_limits<T>::round_style;
};








/*****************************************************************************
 *
 *
 * TRAITS SPECIALIZATIONS
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T>
struct is_number<quantity<T>> : std::true_type {};

template<class T>
struct is_number<quantity<T>&> : std::true_type {};

template<class T>
struct is_number<quantity<T>&&> : std::true_type {};

template<class T>
struct is_number<const quantity<T>&> : std::true_type {};

template<class T>
struct is_number<const quantity<T>> : std::true_type {};



//-------------------------------------------------------------------
template<class T>
struct is_floating_point<quantity<T>> :
    std::integral_constant<bool, is_floating_point<T>::value>
{};



//-------------------------------------------------------------------
template<class T, class T2>
struct common_numeric_type<quantity<T>,T2>
{
    using type = quantity<common_numeric_t<T,T2>>;
};
//---------------------------------------------------------
template<class T, class T2>
struct common_numeric_type<T2,quantity<T>>
{
    using type = quantity<common_numeric_t<T,T2>>;
};
//---------------------------------------------------------
template<class T1, class T2>
struct common_numeric_type<quantity<T1>,quantity<T2>>
{
    using type = quantity<common_numeric_t<T1,T2>>;
};


namespace detail {

//-------------------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, quantity<To>, From> :
    public is_non_narrowing_helper<true,To,From>
{};

//---------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, quantity<To>, quantity<From> > :
    public is_non_narrowing_helper<true,To,From>
{};

//---------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, To, quantity<From> > :
    public is_non_narrowing_helper<true,To,From>
{};


}  // namespace detail








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



}  // namespace num
}  // namespace am


#endif
