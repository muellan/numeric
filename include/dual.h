/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_DUAL_H_
#define AM_NUMERIC_DUAL_H_

#include <cmath>
#include <cfloat>

#include "constants.h"
#include "traits.h"
#include "limits.h"
#include "equality.h"


namespace am {
namespace num {


/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class> class dual;

template<class>
struct is_dual :
    std::false_type
{};

template<class T>
struct is_dual<dual<T>> :
    std::true_type
{};




/*************************************************************************//***
 *
 * @brief
 * represents a dual number r + e * d where
 * r is the real part,
 * d is the dual part and
 * e is the dual unit with e*e = 0
 *
 *****************************************************************************/
template<class NumberType>
class dual
{
public:

    static_assert(is_number<NumberType>::value,
        "dual<T>: T must be a number type");

    static_assert(!is_dual<NumberType>::value,
        "dual<T>: T must not be a dual<> type itself");


    //---------------------------------------------------------------
    using value_type      = NumberType;
    using numeric_type    = value_type;


    //---------------------------------------------------------------
    /// @brief default constructor
    constexpr
    dual() = default;

    /// @brief
    explicit constexpr
    dual(const value_type& a):
        r_{a}, i_(0)
    {}

    /// @brief
    constexpr
    dual(const value_type& realPart, const value_type& imagPart):
        r_{realPart}, i_{imagPart}
    {}


    //---------------------------------------------------------------
    constexpr
    dual(const dual&) = default;

    constexpr
    dual(dual&&) = default;


    //---------------------------------------------------------------
    dual&
    operator = (const dual&) = default;
    
    dual&
    operator = (dual&&) = default;


    //-----------------------------------------------------
    dual&
    operator = (const value_type& realPart)
    {
        r_ = realPart;
        i_ = value_type(0);
        return *this;
    }


    //---------------------------------------------------------------
    constexpr const value_type&
    real() const noexcept {
        return r_;
    }

    constexpr const value_type&
    imag() const noexcept {
        return i_;
    }

    dual&
    real(const value_type& v) noexcept {
        r_ = v;
        return *this;
    }

    dual&
    imag(const value_type& v) noexcept {
        i_ = v;
        return *this;
    }


    //---------------------------------------------------------------
    dual&
    conjugate() {
        i_ = -i_;
        return *this;
    }

    //---------------------------------------------------------
    dual&
    negate() noexcept {
        r_ = -r_;
        i_ = -i_;
        return *this;
    }


    //---------------------------------------------------------------
    // dual (op)= number
    //---------------------------------------------------------------
    dual&
    operator += (const value_type& v) {
        r_ += v;
        return *this;
    }
    //-----------------------------------------------------
    dual&
    operator -= (const value_type& v) {
        r_ -= v;
        return *this;
    }
    //-----------------------------------------------------
    dual&
    operator *= (const value_type& v) {
        r_ *= v;
        i_ *= v;
        return *this;
    }
    //-----------------------------------------------------
    dual&
    operator /= (const value_type& v) {
        r_ /= v;
        i_ /= v;
        return *this;
    }
    //-----------------------------------------------------
    dual&
    operator ^= (const value_type& e) {
        using std::pow;

        auto a_ya_1 = pow(r_ , e-1);

        r_ *= a_ya_1;
        i_ *= e * a_ya_1;

        return *this;
    }


    //---------------------------------------------------------------
    dual&
    operator ++ () {
        ++r_;
        return *this;
    }
    //-----------------------------------------------------
    dual&
    operator -- () {
        --r_;
        return *this;
    }

    //-----------------------------------------------------
    dual
    operator ++ (int) {
        auto old = *this;
        ++*this;
        return old;
    }
    //-----------------------------------------------------
    dual
    operator -- (int) {
        auto old = *this;
        --*this;
        return old;
    }


    //---------------------------------------------------------------
    // dual (op)= dual with different value_type
    //---------------------------------------------------------------
    dual&
    operator += (const dual& o) {
        r_ += o.real();
        i_ += o.imag();
        return *this;
    }
    //-----------------------------------------------------
    dual&
    operator -= (const dual& o) {
        r_ -= o.real();
        i_ -= o.imag();
        return *this;
    }
    //-----------------------------------------------------
    dual&
    operator *= (const dual& o)
    {
        i_ = (r_ * o.imag()) + (o.real() * i_);
        r_ *= o.real();
        return *this;
    }
    //-----------------------------------------------------
    dual&
    operator /= (const dual& o)
    {
        i_ = ((i_ * o.real()) - (r_ * o.imag())) / (o.real() * o.real());
        r_ /= o.real();
        return *this;
    }
    //-----------------------------------------------------
    dual&
    operator ^= (const dual& y)
    {
        using std::pow;
        using std::log;

        auto a_ya_1 = pow(r_ , y.real()-1);

        r_ *= a_ya_1 * (1 + (y.imag() * log(r_)));
        i_ *= y.real() * a_ya_1;

        return *this;
    }

    //-----------------------------------------------------
    dual&
    times_conj(const dual& o)
    {
        i_ = (r_ * (-o.imag())) + (o.real() * i_);
        r_ *= o.real();
        return *this;
    }
    //-----------------------------------------------------
    dual&
    conj_times(const dual& o) {
        i_ = (r_ * o.imag()) + (o.real() * (-i_));
        r_ *= o.real();
        return *this;
    }


private:

    //---------------------------------------------------------------
    value_type r_;
    value_type i_;

};




/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class T1, class T2, class = std::enable_if_t<
    !is_dual<T1>::value &&
    is_number<T1>::value &&
    !is_dual<T2>::value &&
    is_number<T2>::value>>
inline auto
make_dual(const T1& a, const T2& b)
{
    using T = common_numeric_t<T1,T2>;
    return dual<T>{T(a), T(b)};
}

//---------------------------------------------------------------
template<class T, class = std::enable_if_t<
    !is_dual<T>::value &&
    is_number<std::decay_t<T>>::value,T>>
inline auto
make_dual(const T& x)
{
    return dual<T>{x, T(0)};
}



//-------------------------------------------------------------------
// I/O
//-------------------------------------------------------------------
template<class Istream, class T>
inline Istream&
operator >> (Istream& is, dual<T>& x)
{
    return (is >> x.real() >> x.imag());
}

//---------------------------------------------------------
template<class Ostream, class T>
inline Ostream&
operator << (Ostream& os, const dual<T>& x)
{
    return (os << x.real() << " " << x.imag() );
}

//---------------------------------------------------------
template<class T, class Ostream>
inline Ostream&
print(Ostream& os, const dual<T>& d)
{
    return (os << "(" << d.real() << "," << d.imag() << ")" );
}




/*****************************************************************************
 *
 * ACCESS
 *
 *****************************************************************************/
template<class T>
inline constexpr decltype(auto)
real(const dual<T>& d) noexcept
{
    return d.real();
}



//-------------------------------------------------------------------
template<class T>
inline constexpr decltype(auto)
imag(const dual<T>& d) noexcept
{
    return d.imag();
}



//-------------------------------------------------------------------
template<class T>
inline constexpr auto
conj(const dual<T>& x)
{
    return dual<T>{x.real(), -x.imag()};
}




/*****************************************************************************
 *
 * COMPARISON
 *
 *****************************************************************************/
template<class T1, class T2>
inline bool
operator == (const dual<T1>& a, const dual<T2>& b)
{
    return ((a.real() == b.real()) && (a.imag() == b.imag()));
}

//---------------------------------------------------------
template<class T1, class T2>
inline bool
operator != (const dual<T1>& a, const dual<T2>& b)
{
    return ((a.real() != b.real()) || (a.imag() != b.imag()));
}


//-------------------------------------------------------------------
template<class T1, class T2, class T3 = common_numeric_t<T1,T2>>
inline constexpr bool
approx_equal(const dual<T1>& a, const dual<T2>& b,
    const T3& tol = tolerance<T3>)
{
    return (approx_equal(a.real(), b.real(), tol) &&
            approx_equal(a.imag(), b.imag(), tol) );
}

//---------------------------------------------------------
template<class T>
inline constexpr bool
approx_1(const dual<T>& x, const T& tol = tolerance<T>)
{
    return (
        approx_1(x.real(), tol) &&
        approx_0(x.imag(), tol) );
}

//---------------------------------------------------------
template<class T>
inline constexpr bool
approx_0(const dual<T>& x, const T& tol = tolerance<T>)
{
    return (
        approx_0(x.real(), tol) &&
        approx_0(x.imag(), tol) );
}



//-------------------------------------------------------------------
// COMPARISON WITH INTEGERS OR REAL NUMBERS
//-------------------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_dual<T2>::value && is_number<T2>::value>::type>
inline bool
operator > (const dual<T1>& x, const T2& r)
{
    return (x.real() > r);
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_dual<T2>::value && is_number<T2>::value>::type>
inline bool
operator > (const T2& r, const dual<T1>& x)
{
    return (r > x.real());
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_dual<T2>::value && is_number<T2>::value>::type>
inline bool
operator >= (const dual<T1>& x, const T2& r)
{
    return (x.real() >= r);
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_dual<T2>::value && is_number<T2>::value>::type>
inline bool
operator >= (const T2& r, const dual<T1>& x)
{
    return (r >= x.real());
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_dual<T2>::value && is_number<T2>::value>::type>
inline bool
operator < (const dual<T1>& x, const T2& r)
{
    return (x.real() < r);
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_dual<T2>::value && is_number<T2>::value>::type>
inline bool
operator < (const T2& r, const dual<T1>& x)
{
    return (r < x.real());
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_dual<T2>::value && is_number<T2>::value>::type>
inline bool
operator <= (const dual<T1>& x, const T2& r)
{
    return (x.real() <= r);
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_dual<T2>::value && is_number<T2>::value>::type>
inline bool
operator <= (const T2& r, const dual<T1>& x)
{
    return (r <= x.real());
}




/*****************************************************************************
 *
 * ARITHMETIC
 *
 *****************************************************************************/

//-------------------------------------------------------------------
// ADDITION
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr auto
operator + (const dual<T1> x, const dual<T2>& y)
{
    using T = common_numeric_t<T1,T2>;
    return dual<T>{ T(x.real()) + T(y.real()), 
                    T(x.imag()) + T(y.imag()) };
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_dual<T2>::value && is_number<T2>::value>::type>
inline constexpr auto
operator + (const dual<T1> x, const T2& y)
{
    using T = common_numeric_t<T1,T2>;
    return dual<T>{ T(x.real()) + T(y), T(x.imag()) + T(y) };
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_dual<T2>::value && is_number<T2>::value>::type>
inline constexpr auto
operator + (const T2& y, const dual<T1> x)
{
    using T = common_numeric_t<T1,T2>;
    return dual<T>{ T(y) + T(x.real()), T(y) + T(x.imag()) };
}



//-------------------------------------------------------------------
// SUBTRACTION
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr auto
operator - (const dual<T1> x, const dual<T2>& y)
{
    using T = common_numeric_t<T1,T2>;
    return dual<T>{ T(x.real()) - T(y.real()), 
                    T(x.imag()) - T(y.imag()) };
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_dual<T2>::value && is_number<T2>::value>::type>
inline constexpr auto
operator - (const dual<T1> x, const T2& y)
{
    using T = common_numeric_t<T1,T2>;
    return dual<T>{ T(x.real()) - T(y), T(x.imag()) - T(y)};
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_dual<T2>::value && is_number<T2>::value>::type>
inline constexpr auto
operator - (const T2& y, const dual<T1> x)
{
    using T = common_numeric_t<T1,T2>;
    return dual<T>{T(y) - T(x.real()), T(y) - T(x.imag())};
}



//-------------------------------------------------------------------
// MULTIPLIPCATION
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr auto
operator * (const dual<T1> x, const dual<T2>& y)
{
    using T = common_numeric_t<T1,T2>;
    return dual<T>{
        T(x.real()) * T(y.real()),
        (T(x.real()) * T(y.imag())) + (T(y.real()) * T(x.imag())) };
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_dual<T2>::value && is_number<T2>::value>::type>
inline constexpr auto
operator * (const dual<T1> x, const T2& y)
{
    using T = common_numeric_t<T1,T2>;
    return dual<T>{T(x.real()) * T(y), T(x.imag()) * T(y)};
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_dual<T2>::value && is_number<T2>::value>::type>
inline constexpr auto
operator * (const T2& y, const dual<T1> x)
{
    using T = common_numeric_t<T1,T2>;
    return dual<T>{T(y) * T(x.real()), T(y) * T(x.imag())};
}



//-------------------------------------------------------------------
// DIVISION
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr auto
operator / (const dual<T1> x, const dual<T2>& y)
{
    using T = common_numeric_t<T1,T2>;
    return dual<T>{
        T(x.real()) / T(y.real()),
        ((T(x.imag()) * T(y.real())) - (T(x.real()) * T(y.imag()))) / 
            (T(y.real()) * T(y.real()))
    };
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_dual<T2>::value && is_number<T2>::value>::type>
inline constexpr auto
operator / (const dual<T1> x, const T2& y)
{
    using T = common_numeric_t<T1,T2>;
    return dual<T>{T(x.real()) / T(y), T(x.imag()) / T(y)};
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_dual<T2>::value && is_number<T2>::value>::type>
inline constexpr auto
operator / (const T2& y, const dual<T1> x)
{
    using T = common_numeric_t<T1,T2>;
    return dual<T>{T(y) / T(x.real()), T(y) / T(x.imag())};
}



//-------------------------------------------------------------------
// EXPONENTIATION
//-------------------------------------------------------------------
template<class T1, class T2>
inline auto
operator ^ (const dual<T1> b, const dual<T2>& e)
{
    return pow(b,e);
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_dual<T2>::value && is_number<T2>::value>::type>
inline auto
operator ^ (const dual<T1> b, const T2& e)
{
    return pow(b,e);
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<!is_dual<T2>::value && is_number<T2>::value>::type>
inline auto
operator ^ (const T2& b, const dual<T1> e)
{
    return pow(make_dual(b),e);
}



//-------------------------------------------------------------------
// INVERSION
//-------------------------------------------------------------------
template<class T>
inline constexpr auto
operator - (const dual<T> x)
{
    return dual<T>{-x.real(), -x.imag()};
}



//-------------------------------------------------------------------
// SPECIAL MULTIPLICATIONS
//-------------------------------------------------------------------
template<class T1, class T2>
inline auto
times_conj(const dual<T1>& x, const dual<T2>& y)
{
    using T = common_numeric_t<T1,T2>;
    return dual<T>{
        T(x.real()) * T(y.real()),
        (T(x.real()) * T(-y.imag())) + (T(y.real()) * T(x.imag())) };
}

//---------------------------------------------------------
template<class T1, class T2>
inline auto
conj_times(const dual<T1>& x, const dual<T2>& y)
{
    using T = common_numeric_t<T1,T2>;
    return dual<T>{
        T(x.real()) * T(y.real()),
        (T(x.real()) * T(y.imag())) + (T(y.real()) * T(-x.imag())) };
}




/*************************************************************************//***
 *
 *
 * FUNCTIONS
 *
 * @note f(r + e*d) = f(r) + d*f'(r) * e
 *       where e is the dual unit, r the real part and d the dual part
 *
 *
 *****************************************************************************/


//-------------------------------------------------------------------
template<class T>
inline auto
ceil(const dual<T>& x)
{
    using std::ceil;
    return dual<T>{ceil(x.real()), ceil(x.imag())};
}

//---------------------------------------------------------
template<class T>
inline auto
floor(const dual<T>& x)
{
    using std::floor;
    return dual<T>{floor(x.real()), floor(x.imag())};
}


//-------------------------------------------------------------------
// ABSOLUTE
//-------------------------------------------------------------------
/// @brief absolute value
template<class T>
inline auto
abs(const dual<T>& x)
{
    using std::abs;
    return abs(x.real());
}

//---------------------------------------------------------
/// @brief magnitude squared
template<class T>
inline auto
abs2(const dual<T>& x)
{
    return dual<T>{(x.real() * x.real()), 0};
}



//-------------------------------------------------------------------
// ROOTS
//-------------------------------------------------------------------
template<class T>
inline auto
sqrt(const dual<T>& x)
{
    using std::sqrt;
    auto sqrt_r = sqrt(x.real());

    return dual<T>{sqrt_r, x.imag() / (T(2) * sqrt_r)};
}

//---------------------------------------------------------
template<class T>
inline auto
cbrt(const dual<T>& x)
{
    using std::cbrt;
    auto cbrt_r = cbrt(x.real());

    return dual<T>{cbrt_r, x.imag() / (T(3) * cbrt_r*cbrt_r)};
}



//-------------------------------------------------------------------
// EXPONENTIATION
//-------------------------------------------------------------------
template<class T1, class T2>
inline auto
pow(const dual<T1> b, const dual<T2>& e)
{
    using std::pow;
    using std::log;

    using T = common_numeric_t<T1,T2>;

    const auto ba_ea_1 = T(pow(b.real(), e.real()-1));

    return dual<T>{
        T(b.real()) * ba_ea_1 * (T(1) + (T(e.imag()) * T(log(b.real()))) ),
        T(b.imag()) * T(e.real()) * ba_ea_1 };
}

//---------------------------------------------------------
template<class T>
inline auto
pow(const dual<T> b, const T& e)
{
    using std::pow;

    const auto ba_e_1 = pow(b.real(), e - 1);

    return dual<T>{b.real() * ba_e_1, b.imag() * e * ba_e_1};
}

//---------------------------------------------------------
template<class T>
inline auto
exp(const dual<T>& x)
{
    using std::exp;
    const auto expr = exp(x.real());
    return dual<T>{expr, x.imag() * expr};
}

//---------------------------------------------------------
template<class T>
inline auto
exp2(const dual<T>& x)
{
    using std::exp2;
    auto exp2r = exp2(x.real());

    return dual<T>{exp2r,
        x.imag() * exp2r * T(0.69314718055994530941723212145817656807550013436026)};
}

//---------------------------------------------------------
template<class T>
inline auto
expm1(const dual<T>& x)
{
    using std::expm1;
    using std::exp;
    return dual<T>{expm1(x.real()), x.imag() * exp(x.real())};
}

//---------------------------------------------------------
template<class T>
inline auto
ldexp(const dual<T>& a, const dual<int>&b)
{
    using std::ldexp;
    return dual<T>{ldexp(a.real(), b.real()), ldexp(a.imag(), b.imag())};
}

//---------------------------------------------------------
template<class T>
inline auto
frexp(const dual<T>& a, dual<int>* b)
{
    using std::frexp;
    return dual<T>{
        frexp(a.real(), std::addressof(b->real())),
        frexp(a.imag(), std::addressof(b->imag()))};
}

//---------------------------------------------------------
template<class T>
inline auto
modf(const dual<T>& a, dual<T>* b)
{
    using std::modf;
    return dual<T>{
        modf(a.real(), std::addressof(b->real())),
        modf(a.imag(), std::addressof(b->imag()))};
}



//-------------------------------------------------------------------
// LOGARITHMS
//-------------------------------------------------------------------
template<class T>
inline auto
log(const dual<T>& x)
{
    using std::log;
    return dual<T>{log(x.real()), x.imag() / x.real()};
}

//---------------------------------------------------------
template<class T>
inline auto
log10(const dual<T>& x)
{
    using std::log10;
    return dual<T>{
        log10(x.real()), x.imag() /
            (x.real() * T(2.3025850929940456840179914546843642076011014886288))};
}

//---------------------------------------------------------
template<class T>
inline auto
log2(const dual<T>& x)
{
    using std::log2;
    return dual<T>{
        log2(x.real()), x.imag() /
            (x.real() * T(0.69314718055994530941723212145817656807550013436026))};
}

//---------------------------------------------------------
/// @brief logarithm to floating-point basis (FLT_RADIX)
template<class T>
inline auto
logb(const dual<T>& x)
{
    using std::logb;
    using std::log;
    return dual<T>{logb(x.real()), x.imag() / (x.real() * log(FLT_RADIX))};
}

//---------------------------------------------------------
/// @brief logarithm + 1
template<class T>
inline auto
log1p(const dual<T>& x)
{
    using std::log1p;
    return dual<T>{log1p(x.real()), x.imag() / x.real()};
}

//---------------------------------------------------------
/// @brief logarithm to any base
template<class T>
inline auto
log_base(const T& base, const dual<T>& x)
{
    using std::log;
    auto logbase_inv = T(1)/log(base);
    return dual<T>{
        log(x.real()) * logbase_inv,
        (x.imag() / x.real()) * logbase_inv};
}



//-------------------------------------------------------------------
// TRIGONOMETRIC
//-------------------------------------------------------------------
template<class T>
inline auto
sin(const dual<T>& x)
{
    using std::sin;
    using std::cos;
    return dual<T>{sin(x.real()), x.imag() * cos(x.real())};
}

//---------------------------------------------------------
template<class T>
inline auto
cos(const dual<T>& x)
{
    using std::sin;
    using std::cos;
    return dual<T>{cos(x.real()), -x.imag() * sin(x.real())};
}

//---------------------------------------------------------
template<class T>
inline auto
tan(const dual<T>& x)
{
    using std::cos;
    using std::tan;
    auto cosr2 = cos(x.real());
    cosr2 *= cosr2;
    return dual<T>{tan(x.real()), x.imag() / cosr2};
}




//-------------------------------------------------------------------
// INVERSE TRIGONOMETRIC
//-------------------------------------------------------------------
template<class T>
inline auto
asin(const dual<T>& x)
{
    using std::asin;
    using std::sqrt;
    return dual<T>{asin(x.real()), x.imag() / sqrt(1 - (x.real()*x.real()))};
}

//---------------------------------------------------------
template<class T>
inline auto
acos(const dual<T>& x)
{
    using std::acos;
    using std::sqrt;
    return dual<T>{acos(x.real()), -x.imag() / sqrt(1 - (x.real()*x.real()))};
}

//---------------------------------------------------------
template<class T>
inline auto
atan(const dual<T>& x)
{
    using std::atan;
    return dual<T>{atan(x.real()), x.imag() / (1 + (x.real() * x.real()))};
}



//-------------------------------------------------------------------
// HYPERBOLIC
//-------------------------------------------------------------------
template<class T>
inline auto
sinh(const dual<T>& x)
{
    using std::sinh;
    using std::cosh;
    return dual<T>{sinh(x.real()), x.imag() * cosh(x.real())};
}

//---------------------------------------------------------
template<class T>
inline auto
cosh(const dual<T>& x)
{
    using std::sinh;
    using std::cosh;
    return dual<T>{cosh(x.real()), x.imag() * sinh(x.real())};
}

//---------------------------------------------------------
template<class T>
inline auto
tanh(const dual<T>& x)
{
    using std::cosh;
    using std::tanh;
    auto coshr2 = cosh(x.real());
    coshr2 *= coshr2;
    return dual<T>{tanh(x.real()), x.imag() / coshr2};
}




//-------------------------------------------------------------------
// INVERSE HYPERBOLIC
//-------------------------------------------------------------------
template<class T>
inline auto
asinh(const dual<T>& x)
{
    using std::asinh;
    using std::sqrt;
    return dual<T>{asinh(x.real()), x.imag() / sqrt((x.real()*x.real()) + 1)};
}

//---------------------------------------------------------
template<class T>
inline auto
acosh(const dual<T>& x)
{
    using std::acosh;
    using std::sqrt;
    return dual<T>{acosh(x.real()), -x.imag() / sqrt((x.real()*x.real()) - 1)};
}

//---------------------------------------------------------
template<class T>
inline auto
atanh(const dual<T>& x)
{
    using std::atanh;
    return dual<T>{atanh(x.real()), x.imag() / (1 - (x.real() * x.real()))};
}



//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
///@brief  error function
template<class T>
inline auto
erf(const dual<T>& x)
{
    using std::erf;
    using std::exp;
    return dual<T>{erf(x.real()), x.imag() * exp(-x.real()*x.real()) *
        T(1.1283791670955125738961589031215451716881012586580) };
}

//---------------------------------------------------------
///@brief complementary error function
template<class T>
inline auto
erfc(const dual<T>& x)
{
    using std::erfc;
    using std::exp;
    return dual<T>{erfc(x.real()), -x.imag() * exp(-x.real()*x.real()) *
        T(1.1283791670955125738961589031215451716881012586580) };
}

//---------------------------------------------------------
///@brief gamma function
//template<class T>
//inline dual<T>
//tgamma(const dual<T>& x)
//{
//    using std::tgamma;
//    auto tgamma_r = tgamma(x.real());
//    return dual<T>{tgamma_r, x.imag() * tgamma_r * digamma(x.real())};
//}

//---------------------------------------------------------
///@brief log-gamma function
//template<class T>
//inline dual<T>
//lgamma(const dual<T>& x)
//{
//    using std::lgamma;
//    return dual<T>{lgamma(x.real()), x.imag() * digamma(x.real())};
//}



//-------------------------------------------------------------------
template<class T>
inline bool
isfinite(const dual<T>& x)
{
    using std::isfinite;
    return (isfinite(x.real()) && isfinite(x.imag()));
}

//---------------------------------------------------------
template<class T>
inline bool
isinf(const dual<T>& x)
{
    using std::isinf;
    return (isinf(x.real()) || isinf(x.imag()));
}

//---------------------------------------------------------
template<class T>
inline bool
isnan(const dual<T>& x)
{
    using std::isnan;
    return (isnan(x.real()) || isnan(x.imag()));
}

//---------------------------------------------------------
template<class T>
inline bool
isnormal(const dual<T>& x)
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
struct is_number<dual<T>> : std::true_type {};

template<class T>
struct is_number<dual<T>&> : std::true_type {};

template<class T>
struct is_number<dual<T>&&> : std::true_type {};

template<class T>
struct is_number<const dual<T>&> : std::true_type {};

template<class T>
struct is_number<const dual<T>> : std::true_type {};



//-------------------------------------------------------------------
template<class T>
struct is_floating_point<dual<T>> :
    std::integral_constant<bool, is_floating_point<T>::value>
{};



//-------------------------------------------------------------------
template<class T, class T2>
struct common_numeric_type<dual<T>,T2>
{
    using type = dual<common_numeric_t<T,T2>>;
};
//---------------------------------------------------------
template<class T, class T2>
struct common_numeric_type<T2,dual<T>>
{
    using type = dual<common_numeric_t<T,T2>>;
};
//---------------------------------------------------------
template<class T1, class T2>
struct common_numeric_type<dual<T1>,dual<T2>>
{
    using type = dual<common_numeric_t<T1,T2>>;
};


}  // namespace num
}  // namespace am


#endif
