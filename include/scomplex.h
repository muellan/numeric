/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/

#ifndef AMLIB_NUMERIC_SPLIT_COMPLEX_H_
#define AMLIB_NUMERIC_SPLIT_COMPLEX_H_

#include <cmath>
#include <cfloat>

#include "constants.h"
#include "narrowing.h"
#include "equality.h"


namespace am {
namespace num {


/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class> class scomplex;

template<class>
struct is_scomplex :
    std::false_type
{};

template<class T>
struct is_scomplex<scomplex<T>> :
    std::true_type
{};




/*************************************************************************//***
 *
 * @brief
 * represents a split complex number (aka real tessarine) r + j * d where
 * r is the real part,
 * d is the imaginary part and
 * j is the split complex imaginary unit with j*j = +1
 *
 *****************************************************************************/
template<class NumberType>
class scomplex
{
public:

    static_assert(is_number<NumberType>::value,
        "scomplex<T>: T must be a number type");

    static_assert(!is_scomplex<NumberType>::value,
        "scomplex<T>: T must not be a scomplex<> type itself");


    //---------------------------------------------------------------
    // TYPES
    //---------------------------------------------------------------
    using value_type      = NumberType;
    using numeric_type    = value_type;


    //---------------------------------------------------------------
    // CONSTRUCTION / DESTRUCTION
    //---------------------------------------------------------------

    //-----------------------------------------------------
    /// @brief default constructor
    constexpr
    scomplex() = default;

    //-----------------------------------------------------
    /// @brief
    template<class T, class = typename std::enable_if<
        !is_scomplex<decay_t<T>>::value &&
        is_number<decay_t<T>>::value>::type>
    explicit constexpr
    scomplex(T&& a):
        r_(std::forward<T>(a)), i_(value_type(0))
    {
        AM_CHECK_NARROWING(value_type,T)
    }

    //-----------------------------------------------------
    /// @brief
    constexpr
    scomplex(value_type real, value_type imag):
        r_(std::move(real)), i_(std::move(imag))
    {}

    //-----------------------------------------------------
    /// @brief
    template<class T1, class T2, class = typename std::enable_if<
        !is_scomplex<decay_t<T1>>::value &&
        !is_scomplex<decay_t<T2>>::value &&
        is_number<decay_t<T1>,decay_t<T2>>::value>::type>
    explicit constexpr
    scomplex(T1&& real, T2&& imag):
        r_(std::forward<T1>(real)), i_(std::forward<T2>(imag))
    {
        AM_CHECK_NARROWING2(value_type,T1,T2)
    }

    //-----------------------------------------------------
    constexpr
    scomplex(const scomplex&) = default;

    //-----------------------------------------------------
    constexpr
    scomplex(scomplex&&) = default;

    //-----------------------------------------------------
    template<class T>
    explicit constexpr
    scomplex(const scomplex<T>& src):
        r_(src.real()), i_(src.imag())
    {
        AM_CHECK_NARROWING(value_type,T)
    }
    //-----------------------------------------------------
    template<class T>
    explicit constexpr
    scomplex(scomplex<T>&& src):
        r_(std::move(src.real())), i_(std::move(src.imag()))
    {
        AM_CHECK_NARROWING(value_type,T)
    }


    //---------------------------------------------------------------
    // ASSIGNMENT
    //---------------------------------------------------------------
    scomplex&
    operator = (const scomplex&) = default;
    //-----------------------------------------------------
    scomplex&
    operator = (scomplex&&) = default;

    //-----------------------------------------------------
    template<class T>
    scomplex&
    operator = (const scomplex<T>& d)
    {
        AM_CHECK_NARROWING(value_type,T)

        r_ = d.r_;
        i_ = d.i_;
        return *this;
    }
    //-----------------------------------------------------
    template<class T>
    scomplex&
    operator = (scomplex<T>&& d)
    {
        AM_CHECK_NARROWING(value_type,T)

        r_ = std::move(d.r_);
        i_ = std::move(d.i_);
        return *this;
    }

    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        !is_scomplex<decay_t<T>>::value &&
        is_number<decay_t<T>>::value>::type>
    scomplex&
    operator = (T&& n)
    {
        AM_CHECK_NARROWING(value_type,T)

        r_ = std::forward<T>(n);
        i_ = value_type(0);
        return *this;
    }


    //---------------------------------------------------------------
    // ELEMENT ACCESS
    //---------------------------------------------------------------
    constexpr const value_type&
    real() const noexcept {
        return r_;
    }

    //-----------------------------------------------------
    constexpr const value_type&
    imag() const noexcept {
        return i_;
    }


    //---------------------------------------------------------------
    scomplex&
    conjugate()
    {
        i_ = -i_;
        return *this;
    }

    //---------------------------------------------------------
    scomplex&
    negate() noexcept
    {
        r_ = -r_;
        i_ = -i_;
        return *this;
    }


    //---------------------------------------------------------------
    // scomplex (op)= number
    //---------------------------------------------------------------
    template<class T, class = typename std::enable_if<
        !is_scomplex<T>::value && is_number<T>::value>::type>
    scomplex&
    operator += (const T& v)
    {
        AM_CHECK_NARROWING(value_type,T)

        r_ += v;
        return *this;
    }
    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        !is_scomplex<T>::value && is_number<T>::value>::type>
    scomplex&
    operator -= (const T& v)
    {
        AM_CHECK_NARROWING(value_type,T)

        r_ -= v;
        return *this;
    }
    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        !is_scomplex<T>::value && is_number<T>::value>::type>
    scomplex&
    operator *= (const T& v)
    {
        AM_CHECK_NARROWING(value_type,T)

        r_ *= v;
        i_ *= v;
        return *this;
    }
    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        !is_scomplex<T>::value && is_number<T>::value>::type>
    scomplex&
    operator /= (const T& v)
    {
        AM_CHECK_NARROWING(value_type,T)

        r_ /= v;
        i_ /= v;
        return *this;
    }


    //---------------------------------------------------------------
    // increment / decrement
    //---------------------------------------------------------------
    scomplex&
    operator ++ () {
        ++r_;
        return *this;
    }
    //-----------------------------------------------------
    scomplex&
    operator -- () {
        --r_;
        return *this;
    }

    //-----------------------------------------------------
    scomplex
    operator ++ (int) {
        auto old = *this;
        ++*this;
        return old;
    }
    //-----------------------------------------------------
    scomplex
    operator -- (int) {
        auto old = *this;
        --*this;
        return old;
    }


    //---------------------------------------------------------------
    // scomplex (op)= scomplex with different value_type
    //---------------------------------------------------------------
    template<class T>
    scomplex&
    operator += (const scomplex<T>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

        r_ += o.real();
        i_ += o.imag();
        return *this;
    }
    //-----------------------------------------------------
    template<class T>
    scomplex&
    operator -= (const scomplex<T>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

        r_ -= o.real();
        i_ -= o.imag();
        return *this;
    }
    //-----------------------------------------------------
    template<class T>
    scomplex&
    operator *= (const scomplex<T>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

        auto ro = r_;
        r_ = (r_ * o.r_) + (i_ * o.i_);
        i_ = (ro * o.i_) + (i_ * o.r_);
        return *this;
    }
    //-----------------------------------------------------
    template<class T>
    scomplex&
    operator /= (const scomplex<T>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

        auto abso_inv = value_type(1) / abs(o);
        auto ro = r_;
        r_ = abso_inv * ( (r_ * o.r_) - (i_ * o.i_) );
        i_ = abso_inv * ( (i_ * o.r_) - (ro   * o.i_) );
        return *this;
    }

    //-----------------------------------------------------
    template<class T>
    scomplex&
    times_conj(const scomplex<T>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

        auto ro = r_;
        r_ = (r_ * o.r_) - (i_ * o.i_);
        i_ = (i_ * o.r_) - (ro * o.i_);
        return *this;
    }
    //-----------------------------------------------------
    template<class T>
    scomplex&
    conj_times(const scomplex<T>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

        auto ro = r_;
        r_ = (r_ * o.r_) - (i_ * o.i_);
        i_ = (ro * o.i_) - (i_ * o.r_);
        return *this;
    }


private:
    value_type r_;
    value_type i_;
};




/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class T1, class T2, class = typename std::enable_if<
    !is_scomplex<decay_t<T1>>::value &&
    is_number<decay_t<T1>>::value &&
    !is_scomplex<decay_t<T2>>::value &&
    is_number<decay_t<T2>>::value>::type>
inline
scomplex<common_numeric_t<decay_t<T1>,decay_t<T2>>>
make_scomplex(T1&& a, T2&& b)
{
    return scomplex<common_numeric_t<decay_t<T1>,decay_t<T2>>>
        {std::forward<T1>(a), std::forward<T2>(b)};
}

//---------------------------------------------------------------
template<class T, class = typename
    std::enable_if<!is_scomplex<decay_t<T>>::value &&
    is_number<decay_t<T>>::value,T>::type>
inline
scomplex<decay_t<T>>
make_scomplex(T&& x)
{
    return scomplex<decay_t<T>>{std::forward<T>(x), T(0)};
}




/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class Istream, class T>
inline Istream&
operator >> (Istream& is, scomplex<T>& x)
{
    return (is >> x.real() >> x.imag());
}

//---------------------------------------------------------
template<class Ostream, class T>
inline Ostream&
operator << (Ostream& os, const scomplex<T>& x)
{
    return (os << x.real() << " " << x.imag() );
}

//---------------------------------------------------------
template<class T, class Ostream>
inline Ostream&
print(Ostream& os, const scomplex<T>& d)
{
    return (os << "(" << d.real() << "," << d.imag() << ")" );
}




/*****************************************************************************
 *
 * ACCESS
 *
 *****************************************************************************/
template<class T>
inline constexpr auto
real(const scomplex<T>& d) noexcept -> decltype(d.real())
{
    return d.real();
}



//-------------------------------------------------------------------
template<class T>
inline constexpr auto
imag(const scomplex<T>& d) noexcept -> decltype(d.imag())
{
    return d.imag();
}



//-------------------------------------------------------------------
template<class T>
inline constexpr
scomplex<T>
conj(const scomplex<T>& x)
{
    return scomplex<T>{x.real(), -x.imag()};
}




/*****************************************************************************
 *
 * COMPARISON
 *
 *****************************************************************************/
template<class T1, class T2>
inline bool
operator == (const scomplex<T1>& a, const scomplex<T2>& b)
{
    return ((a.real() == b.real()) && (a.imag() == b.imag()));
}

//---------------------------------------------------------
template<class T1, class T2>
inline bool
operator != (const scomplex<T1>& a, const scomplex<T2>& b)
{
    return ((a.real() != b.real()) || (a.imag() != b.imag()));
}


//-------------------------------------------------------------------
template<class T1, class T2, class T3 = common_numeric_t<T1,T2>>
inline constexpr bool
approx_equal(
    const scomplex<T1>& a, const scomplex<T2>& b,
    const T3& tol = tolerance<T3>::value())
{
    return (approx_equal(a.real(), b.real(), tol) &&
            approx_equal(a.imag(), b.imag(), tol) );
}

//---------------------------------------------------------
template<class T>
inline constexpr bool
approx_1(const scomplex<T>& x, const T& tol = tolerance<T>::value())
{
    return (
        approx_1(x.real(), tol) &&
        approx_0(x.imag(), tol) );
}

//---------------------------------------------------------
template<class T>
inline constexpr bool
approx_0(const scomplex<T>& x, const T& tol = tolerance<T>::value())
{
    return (
        approx_0(x.real(), tol) &&
        approx_0(x.imag(), tol) );
}



//-------------------------------------------------------------------
// COMPARISON WITH INTEGERS OR REAL NUMBERS
//-------------------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_scomplex<T2>::value && is_number<T2>::value>::type>
inline bool
operator > (const scomplex<T1>& x, const T2& r)
{
    return (x.real() > r);
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_scomplex<T2>::value && is_number<T2>::value>::type>
inline bool
operator > (const T2& r, const scomplex<T1>& x)
{
    return (r > x.real());
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_scomplex<T2>::value && is_number<T2>::value>::type>
inline bool
operator >= (const scomplex<T1>& x, const T2& r)
{
    return (x.real() >= r);
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_scomplex<T2>::value && is_number<T2>::value>::type>
inline bool
operator >= (const T2& r, const scomplex<T1>& x)
{
    return (r >= x.real());
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_scomplex<T2>::value && is_number<T2>::value>::type>
inline bool
operator < (const scomplex<T1>& x, const T2& r)
{
    return (x.real() < r);
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_scomplex<T2>::value && is_number<T2>::value>::type>
inline bool
operator < (const T2& r, const scomplex<T1>& x)
{
    return (r < x.real());
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_scomplex<T2>::value && is_number<T2>::value>::type>
inline bool
operator <= (const scomplex<T1>& x, const T2& r)
{
    return (x.real() <= r);
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_scomplex<T2>::value && is_number<T2>::value>::type>
inline bool
operator <= (const T2& r, const scomplex<T1>& x)
{
    return (r <= x.real());
}




/*****************************************************************************
 *
 * ARITHMETIC
 *
 *****************************************************************************/
template<class T1, class T2>
inline constexpr
scomplex<common_numeric_t<T1,T2>>
operator + (const scomplex<T1> x, const scomplex<T2>& y)
{
    return scomplex<common_numeric_t<T1,T2>>{
        x.real() + y.real(), x.imag() + y.imag()};
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_scomplex<T2>::value && is_number<T2>::value>::type>
inline constexpr
scomplex<common_numeric_t<T1,T2>>
operator + (const scomplex<T1> x, const T2& y)
{
    return scomplex<common_numeric_t<T1,T2>>
        {x.real() + y, x.imag() + y};
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_scomplex<T2>::value && is_number<T2>::value>::type>
inline constexpr
scomplex<common_numeric_t<T1,T2>>
operator + (const T2& y, const scomplex<T1> x)
{
    return scomplex<common_numeric_t<T1,T2>>
        {y + x.real(), y + x.imag()};
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
scomplex<common_numeric_t<T1,T2>>
operator - (const scomplex<T1> x, const scomplex<T2>& y)
{
    return scomplex<common_numeric_t<T1,T2>>{
        x.real() - y.real(), x.imag() - y.imag()};
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_scomplex<T2>::value && is_number<T2>::value>::type>
inline constexpr
scomplex<common_numeric_t<T1,T2>>
operator - (const scomplex<T1> x, const T2& y)
{
    return scomplex<common_numeric_t<T1,T2>>
        {x.real() - y, x.imag() - y};
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_scomplex<T2>::value && is_number<T2>::value>::type>
inline constexpr
scomplex<common_numeric_t<T1,T2>>
operator - (const T2& y, const scomplex<T1> x)
{
    return scomplex<common_numeric_t<T1,T2>>
        {y - x.real(), y - x.imag()};
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
scomplex<common_numeric_t<T1,T2>>
operator * (const scomplex<T1> x, const scomplex<T2>& y)
{
    return scomplex<common_numeric_t<T1,T2>>{
        (x.real() * y.real()) + (x.imag() * y.imag()),
        (x.real() * y.imag()) + (x.imag() * y.real()),
    };
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_scomplex<T2>::value &&is_number<T2>::value>::type>
inline constexpr
scomplex<common_numeric_t<T1,T2>>
operator * (const scomplex<T1> x, const T2& y)
{
    return scomplex<common_numeric_t<T1,T2>>
        {x.real() * y, x.imag() * y};
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_scomplex<T2>::value &&is_number<T2>::value>::type>
inline constexpr
scomplex<common_numeric_t<T1,T2>>
operator * (const T2& y, const scomplex<T1> x)
{
    return scomplex<common_numeric_t<T1,T2>>
        {y * x.real(), y * x.imag()};
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
scomplex<common_numeric_t<T1,T2>>
operator / (const scomplex<T1> x, const scomplex<T2>& y)
{
    using res_t = common_numeric_t<T1,T2>;
    auto absy = res_t(1) / abs(y);

    return scomplex<res_t>{
        absy * ( (x.real() * y.real()) - (x.imag() * y.imag()) ),
        absy * ( (x.imag() * y.real()) - (x.real() * y.imag()) )
    };
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_scomplex<T2>::value &&is_number<T2>::value>::type>
inline constexpr
scomplex<common_numeric_t<T1,T2>>
operator / (const scomplex<T1> x, const T2& y)
{
    return scomplex<common_numeric_t<T1,T2>>
        {x.real() / y, x.imag() / y};
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_scomplex<T2>::value &&is_number<T2>::value>::type>
inline constexpr
scomplex<common_numeric_t<T1,T2>>
operator / (const T2& y, const scomplex<T1> x)
{
    return scomplex<common_numeric_t<T1,T2>>
        {y / x.real(), y / x.imag()};
}



//-------------------------------------------------------------------
template<class T>
inline constexpr
scomplex<T>
operator - (const scomplex<T> x)
{
    return scomplex<T>{-x.real(), -x.imag()};
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline scomplex<common_numeric_t<T1,T2>>
times_conj(const scomplex<T1>& x, const scomplex<T2>& y)
{
    return scomplex<common_numeric_t<T1,T2>>{
        (x.real() * y.real()) - (x.imag() * y.imag()),
        (x.imag() * y.real()) - (x.real() * y.imag()),
    };
}

//---------------------------------------------------------
template<class T1, class T2>
inline scomplex<common_numeric_t<T1,T2>>
conj_times(const scomplex<T1>& x, const scomplex<T2>& y)
{
    return scomplex<common_numeric_t<T1,T2>>{
        (x.real() * y.real()) - (x.imag() * y.imag()),
        (x.real() * y.imag()) - (x.imag() * y.real()),
    };
}




/*****************************************************************************
 *
 * FUNCTIONS
 *
 * @note f(r + e*d) = f(r) + d*f'(r) * e
 *       where e is the scomplex unit, r the real part and d the scomplex part
 *
 *****************************************************************************/
template<class T>
inline
scomplex<T>
ceil(const scomplex<T>& x)
{
    using std::ceil;
    return scomplex<T>{ceil(x.real()), ceil(x.imag())};
}

//---------------------------------------------------------
template<class T>
inline
scomplex<T>
floor(const scomplex<T>& x)
{
    using std::floor;
    return scomplex<T>{floor(x.real()), floor(x.imag())};
}



//-------------------------------------------------------------------
/// @brief absolute value
template<class T>
inline T
abs(const scomplex<T>& x)
{
    using std::sqrt;
    return sqrt(((x.real() * x.real()) - (x.imag() * x.imag())));
}

//---------------------------------------------------------
/// @brief magnitude squared
template<class T>
inline scomplex<T>
abs2(const scomplex<T>& x)
{
    return ((x.real() * x.real()) - (x.imag() * x.imag()));
}



//-------------------------------------------------------------------
// ROOTS
//-------------------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//sqrt(const scomplex<T>& x)
//{
//    //TODO
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//cbrt(const scomplex<T>& x)
//{
//    //TODO
//}



//-------------------------------------------------------------------
// EXPONENTIATION
//-------------------------------------------------------------------
//template<class T1, class T2>
//inline
//scomplex<common_numeric_t<T1,T2>>
//pow(const scomplex<T1> b, const scomplex<T2>& e)
//{
//
//    //TODO
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//pow(const scomplex<T> b, const T& e)
//{
//    //TODO
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//exp(const scomplex<T>& x)
//{
//    //TODO
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//exp2(const scomplex<T>& x)
//{
//    //TODO
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//expm1(const scomplex<T>& x)
//{
//    //TODO
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//ldexp(const scomplex<T>& a, const scomplex<int>&b)
//{
//    //TODO
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//frexp(const scomplex<T>& a, scomplex<int>* b)
//{
//    //TODO
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//modf(const scomplex<T>& a, scomplex<T>* b)
//{
//    //TODO
//}



//-------------------------------------------------------------------
// LOGARITHMS
//-------------------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//log(const scomplex<T>& x)
//{
//    //TODO
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//log10(const scomplex<T>& x)
//{
//    //TODO
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//log2(const scomplex<T>& x)
//{
//    //TODO
//}

//---------------------------------------------------------
/// @brief logarithm to floating-point basis (FLT_RADIX)
//template<class T>
//inline
//scomplex<T>
//logb(const scomplex<T>& x)
//{
//    //TODO
//}

//---------------------------------------------------------
/// @brief logarithm + 1
//template<class T>
//inline
//scomplex<T>
//log1p(const scomplex<T>& x)
//{
//    //TODO
//}

//---------------------------------------------------------
/// @brief logarithm to any base
//template<class T>
//inline
//scomplex<T>
//log_base(const T& base, const scomplex<T>& x)
//{
//    //TODO
//}



//-------------------------------------------------------------------
// TRIGONOMETRIC
//-------------------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//sin(const scomplex<T>& x)
//{
//    //TODO
//    return scomplex<T>{};
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//cos(const scomplex<T>& x)
//{
//    //TODO
//    return scomplex<T>{};
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//tan(const scomplex<T>& x)
//{
//    //TODO
//    return scomplex<T>{};
//}




//-------------------------------------------------------------------
// INVERSE TRIGONOMETRIC
//-------------------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//asin(const scomplex<T>& x)
//{
//    //TODO
//    return scomplex<T>{};
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//acos(const scomplex<T>& x)
//{
//    //TODO
//    return scomplex<T>{};
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//atan(const scomplex<T>& x)
//{
//    //TODO
//    return scomplex<T>{};
//}



//-------------------------------------------------------------------
// HYPERBOLIC
//-------------------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//sinh(const scomplex<T>& x)
//{
//    //TODO
//    return scomplex<T>{};
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//cosh(const scomplex<T>& x)
//{
//    //TODO
//    return scomplex<T>{};
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//tanh(const scomplex<T>& x)
//{
//    //TODO
//    return scomplex<T>{};
//}




//-------------------------------------------------------------------
// INVERSE HYPERBOLIC
//-------------------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//asinh(const scomplex<T>& x)
//{
//    //TODO
//    return scomplex<T>{};
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//acosh(const scomplex<T>& x)
//{
//    //TODO
//    return scomplex<T>{};
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//atanh(const scomplex<T>& x)
//{
//    //TODO
//    return scomplex<T>{};
//}



//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
///@brief  error function
//template<class T>
//inline scomplex<T>
//erf(const scomplex<T>& x)
//{
//    //TODO
//    return scomplex<T>{};
//}

//---------------------------------------------------------
///@brief complementary error function
//template<class T>
//inline scomplex<T>
//erfc(const scomplex<T>& x)
//{
//    //TODO
//    return scomplex<T>{};
//}



//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
template<class T>
inline bool
isfinite(const scomplex<T>& x)
{
    using std::isfinite;
    return (isfinite(x.real()) && isfinite(x.imag()));
}

//---------------------------------------------------------
template<class T>
inline bool
isinf(const scomplex<T>& x)
{
    using std::isinf;
    return (isinf(x.real()) || isinf(x.imag()));
}

//---------------------------------------------------------
template<class T>
inline bool
isnan(const scomplex<T>& x)
{
    using std::isnan;
    return (isnan(x.real()) || isnan(x.imag()));
}

//---------------------------------------------------------
template<class T>
inline bool
isnormal(const scomplex<T>& x)
{
    using std::isnormal;
    return (isnormal(x.real()) && isnormal(x.imag()));
}




/*****************************************************************************
 *
 * TRAITS SPECIALIZATIONS
 *
 *****************************************************************************/
template<class T>
struct is_number<scomplex<T>> : std::true_type {};

template<class T>
struct is_number<scomplex<T>&> : std::true_type {};

template<class T>
struct is_number<scomplex<T>&&> : std::true_type {};

template<class T>
struct is_number<const scomplex<T>&> : std::true_type {};

template<class T>
struct is_number<const scomplex<T>> : std::true_type {};


//-------------------------------------------------------------------
template<class T>
struct is_floating_point<scomplex<T>> :
    std::integral_constant<bool, is_floating_point<T>::value>
{};



//-------------------------------------------------------------------
template<class T, class T2>
struct common_numeric_type<scomplex<T>,T2>
{
    using type = scomplex<common_numeric_t<T,T2>>;
};
//---------------------------------------------------------
template<class T, class T2>
struct common_numeric_type<T2,scomplex<T>>
{
    using type = scomplex<common_numeric_t<T,T2>>;
};
//---------------------------------------------------------
template<class T1, class T2>
struct common_numeric_type<scomplex<T1>,scomplex<T2>>
{
    using type = scomplex<common_numeric_t<T1,T2>>;
};


namespace detail {

//-------------------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, scomplex<To>, From> :
    public is_non_narrowing_helper<true,To,From>
{};

//---------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, scomplex<To>, scomplex<From> > :
    public is_non_narrowing_helper<true,To,From>
{};

//---------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, To, scomplex<From> > :
    public is_non_narrowing_helper<true,To,From>
{};

}  // namespace detail


}  // namespace num
}  // namespace am

#endif
