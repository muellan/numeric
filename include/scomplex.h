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
    using value_type    = NumberType;
    using numeric_type  = value_type;


    //---------------------------------------------------------------
    /// @brief default constructor
    constexpr
    scomplex() = default;

    explicit constexpr
    scomplex(const value_type& a):
        r_{a}, i_(value_type(0))
    {}

    constexpr
    scomplex(const value_type& realPart, const value_type& imagPart):
        r_{realPart}, i_{imagPart}
    {}

    constexpr
    scomplex(value_type&& realPart, value_type&& imagPart) noexcept :
        r_{std::move(realPart)}, i_{std::move(imagPart)}
    {}


    constexpr
    scomplex(const scomplex&) = default;

    constexpr
    scomplex(scomplex&&) = default;


    //---------------------------------------------------------------
    scomplex&
    operator = (const scomplex&) = default;
    
    scomplex&
    operator = (scomplex&&) = default;


    scomplex&
    operator = (const value_type& num)
    {
        r_ = num;
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
    scomplex&
    operator += (const value_type& v) {
        r_ += v;
        return *this;
    }
    //-----------------------------------------------------
    scomplex&
    operator -= (const value_type& v) {
        r_ -= v;
        return *this;
    }
    //-----------------------------------------------------
    scomplex&
    operator *= (const value_type& v) {
        r_ *= v;
        i_ *= v;
        return *this;
    }
    //-----------------------------------------------------
    scomplex&
    operator /= (const value_type& v)
    {
        r_ /= v;
        i_ /= v;
        return *this;
    }


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
    scomplex&
    operator += (const scomplex& o)
    {
        r_ += o.real();
        i_ += o.imag();
        return *this;
    }
    //-----------------------------------------------------
    scomplex&
    operator -= (const scomplex& o)
    {
        r_ -= o.real();
        i_ -= o.imag();
        return *this;
    }
    //-----------------------------------------------------
    scomplex&
    operator *= (const scomplex& o)
    {
        auto ro = r_;
        r_ = (r_ * o.r_) + (i_ * o.i_);
        i_ = (ro * o.i_) + (i_ * o.r_);
        return *this;
    }
    //-----------------------------------------------------
    scomplex&
    operator /= (const scomplex& o)
    {
        auto abso_inv = value_type(1) / abs(o);
        auto ro = r_;
        r_ = abso_inv * ( (r_ * o.r_) - (i_ * o.i_) );
        i_ = abso_inv * ( (i_ * o.r_) - (ro   * o.i_) );
        return *this;
    }

    //-----------------------------------------------------
    scomplex&
    times_conj(const scomplex& o)
    {
        auto ro = r_;
        r_ = (r_ * o.r_) - (i_ * o.i_);
        i_ = (i_ * o.r_) - (ro * o.i_);
        return *this;
    }
    //-----------------------------------------------------
    scomplex&
    conj_times(const scomplex& o)
    {
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
template<class T1, class T2, class = std::enable_if_t<
    !is_scomplex<T1>::value &&
    is_number<T1>::value &&
    !is_scomplex<T2>::value &&
    is_number<T2>::value>>
inline auto
make_scomplex(const T1& a, const T2& b)
{
    using T = common_numeric_t<T1,T2>;
    return scomplex<T>{T(a), T(b)};
}

//---------------------------------------------------------------
template<class T, class = std::enable_if_t<
    !is_scomplex<T>::value && is_number<T>::value,T>>
inline auto
make_scomplex(const T& x)
{
    return scomplex<T>{x, T(0)};
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
real(const scomplex<T>& d) noexcept
{
    return d.real();
}



//-------------------------------------------------------------------
template<class T>
inline constexpr auto
imag(const scomplex<T>& d) noexcept
{
    return d.imag();
}



//-------------------------------------------------------------------
template<class T>
inline constexpr auto
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
    const T3& tol = tolerance<T3>)
{
    return (approx_equal(a.real(), b.real(), tol) &&
            approx_equal(a.imag(), b.imag(), tol) );
}

//---------------------------------------------------------
template<class T>
inline constexpr bool
approx_1(const scomplex<T>& x, const T& tol = tolerance<T>)
{
    return ( approx_1(x.real(), tol) && approx_0(x.imag(), tol) );
}

//---------------------------------------------------------
template<class T>
inline constexpr bool
approx_0(const scomplex<T>& x, const T& tol = tolerance<T>)
{
    return ( approx_0(x.real(), tol) && approx_0(x.imag(), tol) );
}



//-------------------------------------------------------------------
// COMPARISON WITH INTEGERS OR REAL NUMBERS
//-------------------------------------------------------------------
template<class T1, class T2, class = 
    std::enable_if_t<!is_scomplex<T2>::value && is_number<T2>::value>>
inline bool
operator > (const scomplex<T1>& x, const T2& r)
{
    return (x.real() > r);
}
//---------------------------------------------------------
template<class T1, class T2, class = 
    std::enable_if_t<!is_scomplex<T2>::value && is_number<T2>::value>>
inline bool
operator > (const T2& r, const scomplex<T1>& x)
{
    return (r > x.real());
}

//---------------------------------------------------------
template<class T1, class T2, class = 
    std::enable_if_t<!is_scomplex<T2>::value && is_number<T2>::value>>
inline bool
operator >= (const scomplex<T1>& x, const T2& r)
{
    return (x.real() >= r);
}
//---------------------------------------------------------
template<class T1, class T2, class = 
    std::enable_if_t<!is_scomplex<T2>::value && is_number<T2>::value>>
inline bool
operator >= (const T2& r, const scomplex<T1>& x)
{
    return (r >= x.real());
}

//---------------------------------------------------------
template<class T1, class T2, class = 
    std::enable_if_t<!is_scomplex<T2>::value && is_number<T2>::value>>
inline bool
operator < (const scomplex<T1>& x, const T2& r)
{
    return (x.real() < r);
}
//---------------------------------------------------------
template<class T1, class T2, class = 
    std::enable_if_t<!is_scomplex<T2>::value && is_number<T2>::value>>
inline bool
operator < (const T2& r, const scomplex<T1>& x)
{
    return (r < x.real());
}

//---------------------------------------------------------
template<class T1, class T2, class = 
    std::enable_if_t<!is_scomplex<T2>::value && is_number<T2>::value>>
inline bool
operator <= (const scomplex<T1>& x, const T2& r)
{
    return (x.real() <= r);
}
//---------------------------------------------------------
template<class T1, class T2, class = 
    std::enable_if_t<!is_scomplex<T2>::value && is_number<T2>::value>>
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
inline constexpr auto
operator + (const scomplex<T1> x, const scomplex<T2>& y)
{
    using T = common_numeric_t<T1,T2>;
    return scomplex<T>{ T(x.real()) + T(y.real()), 
                        T(x.imag()) + T(y.imag())};
}

//---------------------------------------------------------
template<class T1, class T2, class = 
    std::enable_if_t<!is_scomplex<T2>::value && is_number<T2>::value>>
inline constexpr auto
operator + (const scomplex<T1> x, const T2& y)
{
    using T = common_numeric_t<T1,T2>;
    return scomplex<T>{ T(x.real()) + T(y), T(x.imag()) + T(y) };
}
//---------------------------------------------------------
template<class T1, class T2, class = 
    std::enable_if_t<!is_scomplex<T2>::value && is_number<T2>::value>>
inline constexpr auto
operator + (const T2& y, const scomplex<T1> x)
{
    using T = common_numeric_t<T1,T2>;
    return scomplex<T>{ T(y) + T(x.real()), T(y) + T(x.imag()) };
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr auto
operator - (const scomplex<T1> x, const scomplex<T2>& y)
{
    using T = common_numeric_t<T1,T2>;
    return scomplex<T>{ T(x.real()) - T(y.real()), 
                        T(x.imag()) - T(y.imag()) };
}

//---------------------------------------------------------
template<class T1, class T2, class = 
    std::enable_if_t<!is_scomplex<T2>::value && is_number<T2>::value>>
inline constexpr auto
operator - (const scomplex<T1> x, const T2& y)
{
    using T = common_numeric_t<T1,T2>;
    return scomplex<T>{ T(x.real()) - T(y), T(x.imag()) - T(y) };
}
//---------------------------------------------------------
template<class T1, class T2, class = 
    std::enable_if_t<!is_scomplex<T2>::value && is_number<T2>::value>>
inline constexpr auto
operator - (const T2& y, const scomplex<T1> x)
{
    using T = common_numeric_t<T1,T2>;
    return scomplex<T>{ T(y) - T(x.real()), T(y) - T(x.imag()) };
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr auto
operator * (const scomplex<T1> x, const scomplex<T2>& y)
{
    using T = common_numeric_t<T1,T2>;
    return scomplex<T>{
        (T(x.real()) * T(y.real())) + (T(x.imag()) * T(y.imag())),
        (T(x.real()) * T(y.imag())) + (T(x.imag()) * T(y.real())),
    };
}

//---------------------------------------------------------
template<class T1, class T2, class = 
    std::enable_if_t<!is_scomplex<T2>::value &&is_number<T2>::value>>
inline constexpr auto
operator * (const scomplex<T1> x, const T2& y)
{
    using T = common_numeric_t<T1,T2>;
    return scomplex<T>{ T(x.real()) * T(y), T(x.imag()) * T(y) };
}
//---------------------------------------------------------
template<class T1, class T2, class = 
    std::enable_if_t<!is_scomplex<T2>::value &&is_number<T2>::value>>
inline constexpr auto
operator * (const T2& y, const scomplex<T1> x)
{
    using T = common_numeric_t<T1,T2>;
    return scomplex<T>{ T(y) * T(x.real()), T(y) * T(x.imag()) };
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr auto
operator / (const scomplex<T1> x, const scomplex<T2>& y)
{
    using T = common_numeric_t<T1,T2>;
    auto absy = T(1) / T(abs(y));

    return scomplex<T>{
        absy * ( (T(x.real()) * T(y.real())) - (T(x.imag()) * T(y.imag())) ),
        absy * ( (T(x.imag()) * T(y.real())) - (T(x.real()) * T(y.imag())) )
    };
}

//---------------------------------------------------------
template<class T1, class T2, class = 
    std::enable_if_t<!is_scomplex<T2>::value &&is_number<T2>::value>>
inline constexpr auto
operator / (const scomplex<T1> x, const T2& y)
{
    using T = common_numeric_t<T1,T2>;
    return scomplex<T>{ T(x.real()) / T(y), T(x.imag()) / T(y) };
}
//---------------------------------------------------------
template<class T1, class T2, class = 
    std::enable_if_t<!is_scomplex<T2>::value &&is_number<T2>::value>>
inline constexpr auto
operator / (const T2& y, const scomplex<T1> x)
{
    using T = common_numeric_t<T1,T2>;
    return scomplex<T>{ T(y) / T(x.real()), T(y) / T(x.imag()) };
}



//-------------------------------------------------------------------
template<class T>
inline constexpr auto
operator - (const scomplex<T> x)
{
    return scomplex<T>{-x.real(), -x.imag()};
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline auto
times_conj(const scomplex<T1>& x, const scomplex<T2>& y)
{
    using T = common_numeric_t<T1,T2>;
    return scomplex<T>{
            (T(x.real()) * T(y.real())) - (T(x.imag()) * T(y.imag())),
            (T(x.imag()) * T(y.real())) - (T(x.real()) * T(y.imag())),
        };
}

//---------------------------------------------------------
template<class T1, class T2>
inline auto
conj_times(const scomplex<T1>& x, const scomplex<T2>& y)
{
    using T = common_numeric_t<T1,T2>;
    return scomplex<T>{
            (T(x.real()) * T(y.real())) - (T(x.imag()) * T(y.imag())),
            (T(x.real()) * T(y.imag())) - (T(x.imag()) * T(y.real())),
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
inline auto
abs(const scomplex<T>& x)
{
    using std::sqrt;
    return sqrt(((x.real() * x.real()) - (x.imag() * x.imag())));
}

//---------------------------------------------------------
/// @brief magnitude squared
template<class T>
inline auto
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
//scomplex<T>
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


}  // namespace num
}  // namespace am

#endif
