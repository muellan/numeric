/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2014 André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_ROUNDED_H_
#define AM_NUMERIC_ROUNDED_H_


#include <cmath>
#include <cstdint>
#include <cfloat>

#include "limits.h"
#include "traits.h"
#include "narrowing.h"


namespace am {
namespace num {



/*****************************************************************************
 *
 *
 * ROUNDING METHODS
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
struct round_to_nearest_int
{
    template<class T>
    constexpr T
    operator () (const T& x) const noexcept {
        using std::nearbyint;
        return (is_integral<T>::value) ? x : nearbyint(x);
    }
};



//-------------------------------------------------------------------
template<class T>
struct round_to_nearest
{
    round_to_nearest(T unit = T(0) ) :
        unit_(std::move(unit))
    {
        if(unit_ <= T(0)) {
            unit_ = is_integral<T>::value
                       ? T(1)
                       : numeric_limits<T>::epsilon();
        }
    }

    T operator () (T x) const noexcept {
        using std::remainder;
        return (x - remainder(x, unit_));
    }

private:
    T unit_;
};








/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class,class> class rounded;

template<class>
struct is_rounded :
    std::false_type
{};

template<class T, class R>
struct is_rounded<rounded<T,R>> :
    std::true_type
{};






/*****************************************************************************
 *
 * @brief  represents a number that is restricted to an interval
 *
 * @tparam NumberType        value type
 * @tparam RoundingMethod
 *
 * @details
 * inherits from RoundingMethod to make use of emtpy base class optimization
 *
 *****************************************************************************/
template<
    class NumberType,
    class RoundingMethod
>
class rounded :
    private RoundingMethod
{
public:

    static_assert(is_number<NumberType>::value,
        "rounded<T,R>: T must be a number type");

    static_assert(!is_rounded<NumberType>::value,
        "rounded<T,R>: T must not be a rounded<> type itself");


    //---------------------------------------------------------------
    using value_type      = NumberType;
    using numeric_type    = value_type;
    using rounding_method = RoundingMethod;


    //---------------------------------------------------------------
    /// @brief default constructor
    constexpr
    rounded() = default;

    //-----------------------------------------------------
    /// @brief
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    explicit constexpr
    rounded(T&& v, rounding_method rp = rounding_method()) :
        rounding_method(std::move(rp)),
        v_(corrected(value_type(std::forward<T>(v))))
    {
        AM_CHECK_NARROWING(value_type,T)
    }
    //-----------------------------------------------------
    /// @brief
    explicit constexpr
    rounded(rounding_method rp) :
        rounding_method(std::move(rp)),
        v_()
    {}
    //-----------------------------------------------------
    /// @brief
    template<class T, class R>
    explicit constexpr
    rounded(const rounded<T,R>& v, rounding_method bp = rounding_method()) :
        rounding_method(std::move(bp)),
        v_(corrected(value_type(v)))
    {
        AM_CHECK_NARROWING(value_type,T)
    }

    //-----------------------------------------------------
    constexpr
    rounded(const rounded&) = default;

    //-----------------------------------------------------
    constexpr
    rounded(rounded&&) = default;

    //-----------------------------------------------------
    template<class T, class R>
    explicit constexpr
    rounded(const rounded<T,R>& src):
        rounding_method(),
        v_(corrected(value_type(src)))
    {
        AM_CHECK_NARROWING(value_type,T)
    }
    //-----------------------------------------------------
    template<class T, class R>
    explicit constexpr
    rounded(rounded<T,R>&& src):
        rounding_method(),
        v_(corrected(value_type(std::move(src))))
    {
        AM_CHECK_NARROWING(value_type,T)
    }


    //---------------------------------------------------------------
    // ASSIGNMENT
    //---------------------------------------------------------------
    rounded&
    operator = (const rounded&) = default;
    //-----------------------------------------------------
    rounded&
    operator = (rounded&&) = default;

    //-----------------------------------------------------
    template<class T, class R, class = typename std::enable_if<
        is_number<T>::value>::type>
    rounded&
    operator = (const rounded<T,R>& b)
    {
        AM_CHECK_NARROWING(value_type,T)

        v_ = corrected(value_type(b));

        return *this;
    }
    //-----------------------------------------------------
    template<class T, class R, class = typename std::enable_if<
        is_number<T>::value>::type>
    rounded&
    operator = (rounded<T,R>&& b)
    {
        AM_CHECK_NARROWING(value_type,T)

        v_ = corrected(value_type(std::move(b)));

        return *this;
    }

    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    rounded&
    operator = (T&& v)
    {
        AM_CHECK_NARROWING(value_type,T)

        v_ = corrected(value_type(std::forward<T>(v)));

        return *this;
    }


    //---------------------------------------------------------------
    constexpr const value_type&
    value() const noexcept {
        return v_;
    }

    //-----------------------------------------------------
    constexpr
    operator value_type () const noexcept
    {
        return v_;
    }


    //---------------------------------------------------------------
    // rounded (op)= number
    //---------------------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    rounded&
    operator += (const T& v)
    {
        AM_CHECK_NARROWING(value_type,T)

        v_ = corrected(v_ + value_type(v));

        return *this;
    }
    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    rounded&
    operator -= (const T& v)
    {
        AM_CHECK_NARROWING(value_type,T)

        v_ = corrected(v_ - value_type(v));

        return *this;
    }
    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    rounded&
    operator *= (const T& v)
    {
        AM_CHECK_NARROWING(value_type,T)

        v_ = corrected(v_ * value_type(v));

        return *this;
    }
    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    rounded&
    operator /= (const T& v)
    {
        AM_CHECK_NARROWING(value_type,T)

        v_ = corrected(v_ /= value_type(v));

        return *this;
    }


    //---------------------------------------------------------------
    // increment / decrement
    //---------------------------------------------------------------
    rounded&
    operator ++ ()
    {
        v_ = corrected(v_ + 1);
        return *this;
    }
    //-----------------------------------------------------
    rounded&
    operator -- ()
    {
        v_ = corrected(v_ - 1);
        return *this;
    }

    //-----------------------------------------------------
    rounded
    operator ++ (int)
    {
        auto old = *this;
        ++*this;
        return old;
    }
    //-----------------------------------------------------
    rounded
    operator -- (int)
    {
        auto old = *this;
        --*this;
        return old;
    }


    //---------------------------------------------------------------
    // rounded (op)= rounded with different value_type
    //---------------------------------------------------------------
    template<class T, class R>
    rounded&
    operator += (const rounded<T,R>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

        v_ = corrected(v_ + o.value());

        return *this;
    }
    //-----------------------------------------------------
    template<class T, class R>
    rounded&
    operator -= (const rounded<T,R>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

        v_ = corrected(v_ - o.value());

        return *this;
    }
    //-----------------------------------------------------
    template<class T, class R>
    rounded&
    operator *= (const rounded<T,R>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

        v_ = corrected(v_ * o.value());

        return *this;
    }
    //-----------------------------------------------------
    template<class T, class R>
    rounded&
    operator /= (const rounded<T,R>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

        v_ = corrected(v_ / o.value());

        return *this;
    }
    //-----------------------------------------------------
    template<class T, class R>
    rounded&
    operator %= (const rounded<T,R>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

        v_ = corrected(v_ % o.value());

        return *this;
    }


private:
    value_type
    corrected(value_type v) {
        return rounding_method::operator()(std::move(v));
    }

    value_type v_;

};






/*****************************************************************************
 *
 *
 * DERIVED TYPES
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T>
using rounded_to_nearest = rounded<T,round_to_nearest<T>>;


//-------------------------------------------------------------------
template<class T>
using rounded_to_nearest_int = rounded<T,round_to_nearest_int>;








/*****************************************************************************
 *
 *
 * FACTORIES
 *
 *
 *****************************************************************************/


//-------------------------------------------------------------------
template<class T1, class T2, class = typename std::enable_if<
    is_number<decay_t<T1>>::value &&
    is_number<decay_t<T2>>::value &&
    !is_rounded<decay_t<T1>>::value &&
    !is_rounded<decay_t<T2>>::value>::type>
inline constexpr
rounded_to_nearest<common_numeric_t<decay_t<T1>,decay_t<T2>>>
make_rounded_to_nearest(T1&& x, T2&& unit)
{
    return rounded_to_nearest<common_numeric_t<decay_t<T1>,decay_t<T2>>>{
        std::forward<T1>(x), std::move(unit)};
}








/*****************************************************************************
 *
 *
 *
 *
 *
 *****************************************************************************/


//-------------------------------------------------------------------
// I/O
//-------------------------------------------------------------------
template<class Istream, class T, class R>
inline Istream&
operator >> (Istream& is, rounded<T,R>& x)
{
    T v;
    is >> v;
    x = v;
    return is;
}

//---------------------------------------------------------
template<class Ostream, class T, class R>
inline Ostream&
operator << (Ostream& os, const rounded<T,R>& x)
{
    return (os << x.value());
}

//---------------------------------------------------------
template<class Ostream, class T, class R>
inline Ostream&
print(Ostream& os, const rounded<T,R>& x)
{
    return (os << x.value());
}






/*****************************************************************************
 *
 *
 * COMPARISON
 *
 *
 *****************************************************************************/


//-------------------------------------------------------------------
template<class T1, class B1, class T2, class B2,
    class T3 = common_numeric_t<T1,T2>>
inline constexpr bool
approx_equal(const rounded<T1,B1>& a, const rounded<T2,B2>& b,
    const T3& tol = tolerance<T3>::value())
{
    return approx_equal(a.value(), b.value(), tol);
}

//---------------------------------------------------------
template<class T, class B>
inline constexpr bool
approx_1(const rounded<T,B>& x, const T& tol = tolerance<T>::value())
{
    return approx_1(x.value(), tol);
}

//---------------------------------------------------------
template<class T, class B>
inline constexpr bool
approx_0(const rounded<T,B>& x, const T& tol = tolerance<T>::value())
{
    return approx_0(x.value(), tol);
}



//-------------------------------------------------------------------
template<class T1, class R, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_rounded<T2>::value>::type>
inline bool
operator == (const rounded<T1,R>& x, const T2& r)
{
    return (x.value() == r);
}
//---------------------------------------------------------
template<class T1, class R, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_rounded<T2>::value>::type>
inline bool
operator == (const T2& r, const rounded<T1,R>& x)
{
    return (r == x.value());
}
//---------------------------------------------------------
template<class T1, class R1, class T2, class R2>
inline bool
operator == (const rounded<T1,R1>& a, const rounded<T2,R2>& b)
{
    return (a.value() == b.value());
}



//-------------------------------------------------------------------
template<class T1, class R, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_rounded<T2>::value>::type>
inline bool
operator != (const rounded<T1,R>& x, const T2& r)
{
    return (x.value() != r);
}
//---------------------------------------------------------
template<class T1, class R, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_rounded<T2>::value>::type>
inline bool
operator != (const T2& r, const rounded<T1,R>& x)
{
    return (r != x.value());
}
//---------------------------------------------------------
template<class T1, class R1, class T2, class R2>
inline bool
operator != (const rounded<T1,R1>& a, const rounded<T2,R2>& b)
{
    return (a.value() != b.value());
}



//-------------------------------------------------------------------
template<class T1, class R, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_rounded<T2>::value>::type>
inline bool
operator > (const rounded<T1,R>& x, const T2& r)
{
    return (x.value() > r);
}
//---------------------------------------------------------
template<class T1, class R, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_rounded<T2>::value>::type>
inline bool
operator > (const T2& r, const rounded<T1,R>& x)
{
    return (r > x.value());
}
//---------------------------------------------------------
template<class T1, class R1, class T2, class R2>
inline bool
operator > (const rounded<T1,R1>& a, const rounded<T2,R2>& b)
{
    return (a.value() > b.value());
}


//---------------------------------------------------------
template<class T1, class R, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_rounded<T2>::value>::type>
inline bool
operator >= (const rounded<T1,R>& x, const T2& r)
{
    return (x.value() >= r);
}
//---------------------------------------------------------
template<class T1, class R, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_rounded<T2>::value>::type>
inline bool
operator >= (const T2& r, const rounded<T1,R>& x)
{
    return (r >= x.value());
}
//---------------------------------------------------------
template<class T1, class R1, class T2, class R2>
inline bool
operator >= (const rounded<T1,R1>& a, const rounded<T2,R2>& b)
{
    return (a.value() >= b.value());
}


//---------------------------------------------------------
template<class T1, class R, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_rounded<T2>::value>::type>
inline bool
operator < (const rounded<T1,R>& x, const T2& r)
{
    return (x.value() < r);
}
//---------------------------------------------------------
template<class T1, class R, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_rounded<T2>::value>::type>
inline bool
operator < (const T2& r, const rounded<T1,R>& x)
{
    return (r < x.value());
}
//---------------------------------------------------------
template<class T1, class R1, class T2, class R2>
inline bool
operator < (const rounded<T1,R1>& a, const rounded<T2,R2>& b)
{
    return (a.value() < b.value());
}


//---------------------------------------------------------
template<class T1, class R, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_rounded<T2>::value>::type>
inline bool
operator <= (const rounded<T1,R>& x, const T2& r)
{
    return (x.value() <= r);
}
//---------------------------------------------------------
template<class T1, class R, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_rounded<T2>::value>::type>
inline bool
operator <= (const T2& r, const rounded<T1,R>& x)
{
    return (r <= x.value());
}
//---------------------------------------------------------
template<class T1, class R1, class T2, class R2>
inline bool
operator <= (const rounded<T1,R1>& a, const rounded<T2,R2>& b)
{
    return (a.value() <= b.value());
}






/*****************************************************************************
 *
 *
 * ARITHMETIC
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
// ADDITION
//-------------------------------------------------------------------
template<class T1, class R1, class T2, class R2>
inline constexpr
common_numeric_t<rounded<T1,R1>, rounded<T2,R2>>
operator + (const rounded<T1,R1>& x, const rounded<T2,R2>& y)
{
    return common_numeric_t<rounded<T1,R1>, rounded<T2,R2>>
               {x.value() + y.value()};
}

//---------------------------------------------------------
template<class T1, class R, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_rounded<T2>::value>::type>
inline constexpr
rounded<common_numeric_t<T1,T2>,R>
operator + (const rounded<T1,R>& x, const T2& y)
{
    return rounded<common_numeric_t<T1,T2>,R>{x.value() + y};
}
//---------------------------------------------------------
template<class T1, class R, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_rounded<T2>::value>::type>
inline constexpr
rounded<common_numeric_t<T1,T2>,R>
operator + (const T2& y, const rounded<T1,R>& x)
{
    return rounded<common_numeric_t<T1,T2>,R>{y + x.value()};
}



//-------------------------------------------------------------------
// SUBTRACTION
//-------------------------------------------------------------------
template<class T1, class R1, class T2, class R2>
inline constexpr
common_numeric_t<rounded<T1,R1>, rounded<T2,R2>>
operator - (const rounded<T1,R1>& x, const rounded<T2,R2>& y)
{
    return common_numeric_t<rounded<T1,R1>, rounded<T2,R2>>
               {x.value() - y.value()};
}

//---------------------------------------------------------
template<class T1, class R, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_rounded<T2>::value>::type>
inline constexpr
rounded<common_numeric_t<T1,T2>,R>
operator - (const rounded<T1,R>& x, const T2& y)
{
    return rounded<common_numeric_t<T1,T2>,R>{x.value() - y};
}
//---------------------------------------------------------
template<class T1, class R, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_rounded<T2>::value>::type>
inline constexpr
rounded<common_numeric_t<T1,T2>,R>
operator - (const T2& y, const rounded<T1,R>& x)
{
    return rounded<common_numeric_t<T1,T2>,R>{y - x.value()};
}



//-------------------------------------------------------------------
// MULTIPLIPCATION
//-------------------------------------------------------------------
template<class T1, class R1, class T2, class R2>
inline constexpr
common_numeric_t<rounded<T1,R1>, rounded<T2,R2>>
operator * (const rounded<T1,R1>& x, const rounded<T2,R2>& y)
{
    return common_numeric_t<rounded<T1,R1>, rounded<T2,R2>>
               {x.value() * y.value()};
}

//---------------------------------------------------------
template<class T1, class R, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_rounded<T2>::value>::type>
inline constexpr
rounded<common_numeric_t<T1,T2>,R>
operator * (const rounded<T1,R>& x, const T2& y)
{
    return rounded<common_numeric_t<T1,T2>,R>{x.value() * y};
}
//---------------------------------------------------------
template<class T1, class R, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_rounded<T2>::value>::type>
inline constexpr
rounded<common_numeric_t<T1,T2>,R>
operator * (const T2& y, const rounded<T1,R>& x)
{
    return rounded<common_numeric_t<T1,T2>,R>{y * x.value()};
}



//-------------------------------------------------------------------
// DIVISION
//-------------------------------------------------------------------
template<class T1, class R1, class T2, class R2>
inline constexpr
common_numeric_t<rounded<T1,R1>, rounded<T2,R2>>
operator / (const rounded<T1,R1>& x, const rounded<T2,R2>& y)
{
    return common_numeric_t<rounded<T1,R1>, rounded<T2,R2>>
               {x.value() / y.value()};
}

//---------------------------------------------------------
template<class T1, class R, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_rounded<T2>::value>::type>
inline constexpr
rounded<common_numeric_t<T1,T2>,R>
operator / (const rounded<T1,R>& x, const T2& y)
{
    return rounded<common_numeric_t<T1,T2>,R>{x.value() / y};
}
//---------------------------------------------------------
template<class T1, class R, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_rounded<T2>::value>::type>
inline constexpr
rounded<common_numeric_t<T1,T2>,R>
operator / (const T2& y, const rounded<T1,R>& x)
{
    return rounded<common_numeric_t<T1,T2>,R>{y / x.value()};
}



//-------------------------------------------------------------------
// MODULO
//-------------------------------------------------------------------
template<class T1, class R1, class T2, class R2>
inline constexpr
common_numeric_t<rounded<T1,R1>, rounded<T2,R2>>
operator % (const rounded<T1,R1>& x, const rounded<T2,R2>& y)
{
    return common_numeric_t<rounded<T1,R1>, rounded<T2,R2>>
               {x.value() % y.value()};
}

//---------------------------------------------------------
template<class T1, class R, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_rounded<T2>::value>::type>
inline constexpr
rounded<common_numeric_t<T1,T2>,R>
operator % (const rounded<T1,R>& x, const T2& y)
{
    return rounded<common_numeric_t<T1,T2>,R>{x.value() % y};
}
//---------------------------------------------------------
template<class T1, class R, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_rounded<T2>::value>::type>
inline constexpr
rounded<common_numeric_t<T1,T2>,R>
operator % (const T2& y, const rounded<T1,R>& x)
{
    return rounded<common_numeric_t<T1,T2>,R>{y % x.value()};
}



//-------------------------------------------------------------------
// INVERSION
//-------------------------------------------------------------------
template<class T, class R>
inline constexpr
rounded<T,R>
operator - (const rounded<T,R>& x)
{
    return rounded<T,R>{-x.value()};
}







/*****************************************************************************
 *
 *
 *
 * MATH FUNCTIONS
 *
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T, class R>
inline bool
isfinite(const rounded<T,R>& x)
{
    using std::isfinite;
    return isfinite(x.value());
}

//---------------------------------------------------------
template<class T, class R>
inline bool
isinf(const rounded<T,R>& x)
{
    using std::isinf;
    return isinf(x.value());
}

//---------------------------------------------------------
template<class T, class R>
inline bool
isnan(const rounded<T,R>& x)
{
    using std::isnan;
    return isnan(x.value());
}

//---------------------------------------------------------
template<class T, class R>
inline bool
isnormal(const rounded<T,R>& x)
{
    using std::isnormal;
    return isnormal(x.value());
}

//---------------------------------------------------------
template<class T, class R>
inline rounded<T,R>
abs(const rounded<T,R>& x)
{
    using std::abs;
    return rounded<T,R>(abs(x.value()));
}








/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class T, class R>
class numeric_limits<rounded<T,R>>
{
    using val_t = rounded<T,R>;

public:
    static constexpr bool is_specialized = true;

    static constexpr val_t
    min() {return val_t(numeric_limits<T>::min()); }

    static constexpr val_t
    max() {return val_t(numeric_limits<T>::max()); }

    static constexpr val_t
    lowest() {return val_t(numeric_limits<T>::lowest()); }

    static constexpr int digits = numeric_limits<T>::digits;
    static constexpr int digits10 = numeric_limits<T>::digits10;
    static constexpr int max_digits10 = numeric_limits<T>::max_digits10;
    static constexpr bool is_signed = numeric_limits<T>::is_signed;
    static constexpr bool is_integer = numeric_limits<T>::is_integer;
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

    static constexpr bool has_infinity = numeric_limits<T>::has_infinity;
    static constexpr bool has_quiet_NaN = numeric_limits<T>::has_quiet_NaN;
    static constexpr bool has_signaling_NaN = numeric_limits<T>::has_signaling_NaN;
    static constexpr std::float_denorm_style has_denorm = numeric_limits<T>::has_denorm;
    static constexpr bool has_denorm_loss = numeric_limits<T>::has_denorm_loss;

    static constexpr val_t
    infinity() noexcept {return val_t(numeric_limits<T>::infinity()); }

    static constexpr val_t
    quiet_NaN() noexcept {return val_t(numeric_limits<T>::quiet_NaN()); }

    static constexpr val_t
    signaling_NaN() noexcept {return val_t(numeric_limits<T>::signaling_NaN()); }

    static constexpr val_t
    denorm_min() noexcept {return val_t(numeric_limits<T>::denorm_min()); }

    static constexpr bool is_iec559 = numeric_limits<T>::is_iec559;
    static constexpr bool is_bounded = numeric_limits<T>::is_bounded;
    static constexpr bool is_modulo = numeric_limits<T>::is_modulo;

    static constexpr bool traps = numeric_limits<T>::traps;
    static constexpr bool tinyness_before = numeric_limits<T>::tinyness_before;
    static constexpr std::float_round_style round_style = numeric_limits<T>::round_style;
};






/*****************************************************************************
 *
 * DEFAULT COMMON ROUNDING POLICY
 *
 *****************************************************************************/

//-------------------------------------------------------------------
/// @brief default behaviour: use the method thats less restrictive
//-------------------------------------------------------------------
template<class, class>
struct common_rounding_method {};

//---------------------------------------------------------
template<class R>
struct common_rounding_method<R,R> {
    using type = R;
};

//---------------------------------------------------------
template<class T1, class T2>
struct common_rounding_method<round_to_nearest<T1>, round_to_nearest<T2>> {
    using type = round_to_nearest<common_numeric_t<T1,T2>>;
};

//---------------------------------------------------------
template<class T>
struct common_rounding_method<round_to_nearest<T>, round_to_nearest_int> {
    using type = round_to_nearest<T>;
};

//---------------------------------------------------------
template<class T>
struct common_rounding_method<round_to_nearest_int, round_to_nearest<T>> {
    using type = round_to_nearest<T>;
};


//-------------------------------------------------------------------
template<class R1, class R2>
using common_rounding_method_t = typename common_rounding_method<R1,R2>::type;






/*****************************************************************************
 *
 *
 * TRAITS SPECIALIZATIONS
 *
 *
 *****************************************************************************/


//-------------------------------------------------------------------
template<class T, class R>
struct is_number<rounded<T,R>> : std::true_type {};

template<class T, class R>
struct is_number<rounded<T,R>&> : std::true_type {};

template<class T, class R>
struct is_number<rounded<T,R>&&> : std::true_type {};

template<class T, class R>
struct is_number<const rounded<T,R>&> : std::true_type {};

template<class T, class R>
struct is_number<const rounded<T,R>> : std::true_type {};



//-------------------------------------------------------------------
template<class T, class R>
struct is_floating_point<rounded<T,R>> :
    std::integral_constant<bool, is_floating_point<T>::value>
{};



//-------------------------------------------------------------------
template<class T1, class R, class T2>
struct common_numeric_type<rounded<T1,R>,T2>
{
    using type = rounded<common_numeric_t<T1,T2>,R>;
};
//---------------------------------------------------------
template<class T1, class R, class T2>
struct common_numeric_type<T2,rounded<T1,R>>
{
    using type = rounded<common_numeric_t<T1,T2>,R>;
};
//---------------------------------------------------------
/// @brief
template<class T1, class R1, class T2, class R2>
struct common_numeric_type<rounded<T1,R1>,rounded<T2,R2>>
{
    using type = rounded<
        common_numeric_t<T1,T2>,
        common_rounding_method_t<R1,R2>>;
};


namespace detail {

//-------------------------------------------------------------------
template<class To, class R, class From>
struct is_non_narrowing_helper<true, rounded<To,R>, From> :
    public is_non_narrowing_helper<true,To,From>
{};

//---------------------------------------------------------
template<class To, class R1, class From, class R2>
struct is_non_narrowing_helper<true, rounded<To,R1>, rounded<From,R2> > :
    public is_non_narrowing_helper<true,To,From>
{};

//---------------------------------------------------------
template<class To, class From, class R>
struct is_non_narrowing_helper<true, To, rounded<From,R> > :
    public is_non_narrowing_helper<true,To,From>
{};


}  // namespace detail



}  // namespace num
}  // namespace am


#endif
