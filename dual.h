/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2014 André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_DUAL_H_
#define AM_NUMERIC_DUAL_H_

#include <cmath>
#include <cfloat>

#include "constants.h"
#include "traits.h"
#include "limits.h"
#include "narrowing.h"
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







/*****************************************************************************
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
    // TYPES
    //---------------------------------------------------------------
    using value_type      = NumberType;
    using numeric_type    = value_type;
    using const_reference = const value_type&;


    //---------------------------------------------------------------
    // CONSTRUCTION / DESTRUCTION
    //---------------------------------------------------------------

    //-----------------------------------------------------
    /// @brief default constructor
    constexpr
    dual() = default;

    //-----------------------------------------------------
    /// @brief
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    explicit constexpr
    dual(T&& a):
        r_(std::forward<T>(a)), i_(value_type(0))
    {
        AM_CHECK_NARROWING(value_type,T)
    }

    //-----------------------------------------------------
    /// @brief
    constexpr
    dual(value_type real, value_type imag):
        r_(std::move(real)), i_(std::move(imag))
    {}

    //-----------------------------------------------------
    /// @brief
    template<class T1, class T2, class = typename std::enable_if<
        is_number<T1,T2>::value>::type>
    explicit constexpr
    dual(T1&& real, T2&& imag):
        r_(std::forward<T1>(real)), i_(std::forward<T2>(imag))
    {
        AM_CHECK_NARROWING2(value_type,T1,T2)
    }

    //-----------------------------------------------------
    constexpr
    dual(const dual&) = default;

    //-----------------------------------------------------
    constexpr
    dual(dual&&) = default;

    //-----------------------------------------------------
    template<class T>
    explicit constexpr
    dual(const dual<T>& src):
        r_(src.real()), i_(src.imag())
    {
        AM_CHECK_NARROWING(value_type,T)
    }
    //-----------------------------------------------------
    template<class T>
    explicit constexpr
    dual(dual<T>&& src):
        r_(std::move(src.real())), i_(std::move(src.imag()))
    {
        AM_CHECK_NARROWING(value_type,T)
    }


    //---------------------------------------------------------------
    // ASSIGNMENT
    //---------------------------------------------------------------
    dual&
    operator = (const dual&) = default;
    //-----------------------------------------------------
    dual&
    operator = (dual&&) = default;

    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    dual&
    operator = (const dual<T>& d)
    {
        AM_CHECK_NARROWING(value_type,T)

        r_ = d.r_;
        i_ = d.i_;
        return *this;
    }
    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    dual&
    operator = (dual<T>&& d)
    {
        AM_CHECK_NARROWING(value_type,T)

        r_ = std::move(d.r_);
        i_ = std::move(d.i_);
        return *this;
    }

    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    dual&
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
    constexpr const_reference
    real() const noexcept {
        return r_;
    }

    //-----------------------------------------------------
    constexpr const_reference
    imag() const noexcept {
        return i_;
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
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    dual&
    operator += (const T& v)
    {
        AM_CHECK_NARROWING(value_type,T)

        r_ += v;
        return *this;
    }
    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    dual&
    operator -= (const T& v) {
        r_ -= v;
        return *this;
    }
    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    dual&
    operator *= (const T& v)
    {
        AM_CHECK_NARROWING(value_type,T)

        r_ *= v;
        i_ *= v;
        return *this;
    }
    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    dual&
    operator /= (const T& v)
    {
        AM_CHECK_NARROWING(value_type,T)

        r_ /= v;
        i_ /= v;
        return *this;
    }
    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    dual&
    operator ^= (const T& e)
    {
        AM_CHECK_NARROWING(value_type,T)

        using std::pow;

        auto a_ya_1 = pow(r_ , e-1);

        r_ *= a_ya_1;
        i_ *= e * a_ya_1;

        return *this;
    }


    //---------------------------------------------------------------
    // increment / decrement
    //---------------------------------------------------------------
    dual&
    operator ++ ()
    {
        ++r_;
        return *this;
    }
    //-----------------------------------------------------
    dual&
    operator -- ()
    {
        --r_;
        return *this;
    }

    //-----------------------------------------------------
    dual
    operator ++ (int)
    {
        auto old = *this;
        ++*this;
        return old;
    }
    //-----------------------------------------------------
    dual
    operator -- (int)
    {
        auto old = *this;
        --*this;
        return old;
    }


    //---------------------------------------------------------------
    // dual (op)= dual with different value_type
    //---------------------------------------------------------------
    template<class T>
    dual&
    operator += (const dual<T>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

        r_ += o.real();
        i_ += o.imag();
        return *this;
    }
    //-----------------------------------------------------
    template<class T>
    dual&
    operator -= (const dual<T>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

        r_ -= o.real();
        i_ -= o.imag();
        return *this;
    }
    //-----------------------------------------------------
    template<class T>
    dual&
    operator *= (const dual<T>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

        i_ = (r_ * o.imag()) + (o.real() * i_);
        r_ *= o.real();
        return *this;
    }
    //-----------------------------------------------------
    template<class T>
    dual&
    operator /= (const dual<T>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

        i_ = ((i_ * o.real()) - (r_ * o.imag())) / (o.real() * o.real());
        r_ /= o.real();
        return *this;
    }
    //-----------------------------------------------------
    template<class T>
    dual&
    operator ^= (const dual<T>& y)
    {
        AM_CHECK_NARROWING(value_type,T)

        using std::pow;
        using std::log;

        auto a_ya_1 = pow(r_ , y.real()-1);

        r_ *= a_ya_1 * (1 + (y.imag() * log(r_)));
        i_ *= y.real() * a_ya_1;

        return *this;
    }

    //-----------------------------------------------------
    template<class T>
    dual&
    times_conj(const dual<T>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

        i_ = (r_ * (-o.imag())) + (o.real() * i_);
        r_ *= o.real();
        return *this;
    }
    //-----------------------------------------------------
    template<class T>
    dual&
    conj_times(const dual<T>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

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
 *
 *
 *****************************************************************************/


//---------------------------------------------------------------
template<class T1, class T2>
inline
dual<common_numeric_t<T1,T2>>
make_dual(T1&& a, T2&& b)
{
    return dual<common_numeric_t<T1,T2>>
            {std::forward<T1>(a), std::forward<T2>(b)};
}

//---------------------------------------------------------------
template<class T, class = typename
    std::enable_if<is_number<T>::value,T>::type>
inline
dual<decay_t<T>>
make_dual(T&& x)
{
    return dual<decay_t<T>>{std::forward<T>(x), T(0)};
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
 *
 * ACCESS
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T>
inline constexpr auto
real(const dual<T>& d) noexcept -> decltype(d.real())
{
    return d.real();
}



//-------------------------------------------------------------------
template<class T>
inline constexpr auto
imag(const dual<T>& d) noexcept -> decltype(d.imag())
{
    return d.imag();
}



//-------------------------------------------------------------------
template<class T>
inline constexpr
dual<T>
conj(const dual<T>& x)
{
    return dual<T>{x.real(), -x.imag()};
}






/*****************************************************************************
 *
 *
 * COMPARISON
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
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
    const T3& tol = tolerance<T3>::value())
{
    return (approx_equal(a.real(), b.real(), tol) &&
            approx_equal(a.imag(), b.imag(), tol) );
}

//---------------------------------------------------------
template<class T>
inline constexpr bool
approx_1(const dual<T>& x, const T& tol = tolerance<T>::value())
{
    return (
        approx_1(x.real(), tol) &&
        approx_0(x.imag(), tol) );
}

//---------------------------------------------------------
template<class T>
inline constexpr bool
approx_0(const dual<T>& x, const T& tol = tolerance<T>::value())
{
    return (
        approx_0(x.real(), tol) &&
        approx_0(x.imag(), tol) );
}



//-------------------------------------------------------------------
// COMPARISON WITH INTEGERS OR REAL NUMBERS
//-------------------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_number<T2>::value>::type>
inline bool
operator > (const dual<T1>& x, const T2& r)
{
    return (x.real() > r);
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_number<T2>::value>::type>
inline bool
operator > (const T2& r, const dual<T1>& x)
{
    return (r > x.real());
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_number<T2>::value>::type>
inline bool
operator >= (const dual<T1>& x, const T2& r)
{
    return (x.real() >= r);
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_number<T2>::value>::type>
inline bool
operator >= (const T2& r, const dual<T1>& x)
{
    return (r >= x.real());
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_number<T2>::value>::type>
inline bool
operator < (const dual<T1>& x, const T2& r)
{
    return (x.real() < r);
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_number<T2>::value>::type>
inline bool
operator < (const T2& r, const dual<T1>& x)
{
    return (r < x.real());
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_number<T2>::value>::type>
inline bool
operator <= (const dual<T1>& x, const T2& r)
{
    return (x.real() <= r);
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_number<T2>::value>::type>
inline bool
operator <= (const T2& r, const dual<T1>& x)
{
    return (r <= x.real());
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
template<class T1, class T2>
inline constexpr
dual<common_numeric_t<T1,T2>>
operator + (const dual<T1> x, const dual<T2>& y)
{
    return dual<common_numeric_t<T1,T2>>{
        x.real() + y.real(), x.imag() + y.imag()};
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<std::is_arithmetic<T2>::value>::type>
inline constexpr
dual<common_numeric_t<T1,T2>>
operator + (const dual<T1> x, const T2& y)
{
    return dual<common_numeric_t<T1,T2>>
        {x.real() + y, x.imag() + y};
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<std::is_arithmetic<T2>::value>::type>
inline constexpr
dual<common_numeric_t<T1,T2>>
operator + (const T2& y, const dual<T1> x)
{
    return dual<common_numeric_t<T1,T2>>
        {y + x.real(), y + x.imag()};
}

//---------------------------------------------------------
template<class T>
inline constexpr
dual<T>
operator + (const dual<T> x, const T& y)
{
    return dual<T>{x.real() + y, x.imag() + y};
}
//---------------------------------------------------------
template<class T>
inline constexpr
dual<T>
operator + (const T& y, const dual<T> x)
{
    return dual<T>{y + x.real(), y + x.imag()};
}



//-------------------------------------------------------------------
// SUBTRACTION
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
dual<common_numeric_t<T1,T2>>
operator - (const dual<T1> x, const dual<T2>& y)
{
    return dual<common_numeric_t<T1,T2>>{
        x.real() - y.real(), x.imag() - y.imag()};
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<std::is_arithmetic<T2>::value>::type>
inline constexpr
dual<common_numeric_t<T1,T2>>
operator - (const dual<T1> x, const T2& y)
{
    return dual<common_numeric_t<T1,T2>>
        {x.real() - y, x.imag() - y};
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<std::is_arithmetic<T2>::value>::type>
inline constexpr
dual<common_numeric_t<T1,T2>>
operator - (const T2& y, const dual<T1> x)
{
    return dual<common_numeric_t<T1,T2>>
        {y - x.real(), y - x.imag()};
}

//---------------------------------------------------------
template<class T>
inline constexpr
dual<T>
operator - (const dual<T> x, const T& y)
{
    return dual<T>{x.real() - y, x.imag() - y};
}
//---------------------------------------------------------
template<class T>
inline constexpr
dual<T>
operator - (const T& y, const dual<T> x)
{
    return dual<T>{y - x.real(), y - x.imag()};
}



//-------------------------------------------------------------------
// MULTIPLIPCATION
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
dual<common_numeric_t<T1,T2>>
operator * (const dual<T1> x, const dual<T2>& y)
{
    return dual<common_numeric_t<T1,T2>>{
        x.real() * y.real(),
        (x.real() * y.imag()) + (y.real() * x.imag()) };
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<std::is_arithmetic<T2>::value>::type>
inline constexpr
dual<common_numeric_t<T1,T2>>
operator * (const dual<T1> x, const T2& y)
{
    return dual<common_numeric_t<T1,T2>>
        {x.real() * y, x.imag() * y};
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<std::is_arithmetic<T2>::value>::type>
inline constexpr
dual<common_numeric_t<T1,T2>>
operator * (const T2& y, const dual<T1> x)
{
    return dual<common_numeric_t<T1,T2>>
        {y * x.real(), y * x.imag()};
}

//---------------------------------------------------------
template<class T>
inline constexpr
dual<T>
operator * (const dual<T> x, const T& y)
{
    return dual<T>{x.real() * y, x.imag() * y};
}
//---------------------------------------------------------
template<class T>
inline constexpr
dual<T>
operator * (const T& y, const dual<T> x)
{
    return dual<T>{y * x.real(), y * x.imag()};
}



//-------------------------------------------------------------------
// DIVISION
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
dual<common_numeric_t<T1,T2>>
operator / (const dual<T1> x, const dual<T2>& y)
{
    return dual<common_numeric_t<T1,T2>>{
        x.real() / y.real(),
        ((x.imag() * y.real()) - (x.real() * y.imag())) / (y.real() * y.real())
    };
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<std::is_arithmetic<T2>::value>::type>
inline constexpr
dual<common_numeric_t<T1,T2>>
operator / (const dual<T1> x, const T2& y)
{
    return dual<common_numeric_t<T1,T2>>
        {x.real() / y, x.imag() / y};
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<std::is_arithmetic<T2>::value>::type>
inline constexpr
dual<common_numeric_t<T1,T2>>
operator / (const T2& y, const dual<T1> x)
{
    return dual<common_numeric_t<T1,T2>>
        {y / x.real(), y / x.imag()};
}

//---------------------------------------------------------
template<class T>
inline constexpr
dual<T>
operator / (const dual<T> x, const T& y)
{
    return dual<T>{x.real() / y, x.imag() / y};
}
//---------------------------------------------------------
template<class T>
inline constexpr
dual<T>
operator / (const T& y, const dual<T> x)
{
    return dual<T>{y / x.real(), y / x.imag()};
}



//-------------------------------------------------------------------
// EXPONENTIATION
//-------------------------------------------------------------------
template<class T1, class T2>
inline
dual<common_numeric_t<T1,T2>>
operator ^ (const dual<T1> b, const dual<T2>& e)
{
    return pow(b,e);
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<std::is_arithmetic<T2>::value>::type>
inline
dual<common_numeric_t<T1,T2>>
operator ^ (const dual<T1> b, const T2& e)
{
    return pow(b,e);
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<std::is_arithmetic<T2>::value>::type>
inline
dual<common_numeric_t<T1,T2>>
operator ^ (const T2& b, const dual<T1> e)
{
    return pow(make_dual(b),e);
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
operator ^ (const dual<T> b, const T& e)
{
    return pow(b,e);
}
//---------------------------------------------------------
template<class T>
inline
dual<T>
operator ^ (const T& b, const dual<T> e)
{
    return pow(make_dual(b),e);
}



//-------------------------------------------------------------------
// INVERSION
//-------------------------------------------------------------------
template<class T>
inline constexpr
dual<T>
operator - (const dual<T> x)
{
    return dual<T>{-x.real(), -x.imag()};
}



//-------------------------------------------------------------------
// SPECIAL MULTIPLICATIONS
//-------------------------------------------------------------------
template<class T1, class T2>
inline dual<common_numeric_t<T1,T2>>
times_conj(const dual<T1>& x, const dual<T2>& y)
{
    return dual<common_numeric_t<T1,T2>>{
        x.real() * y.real(),
        (x.real() * (-y.imag())) + (y.real() * x.imag()) };
}

//---------------------------------------------------------
template<class T1, class T2>
inline dual<common_numeric_t<T1,T2>>
conj_times(const dual<T1>& x, const dual<T2>& y)
{
    return dual<common_numeric_t<T1,T2>>{
        x.real() * y.real(),
        (x.real() * y.imag()) + (y.real() * (-x.imag())) };
}






/*****************************************************************************
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
inline
dual<T>
ceil(const dual<T>& x)
{
    using std::ceil;
    return dual<T>{ceil(x.real()), ceil(x.imag())};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
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
inline T
abs(const dual<T>& x)
{
    using std::abs;
    return abs(x.real());
}

//---------------------------------------------------------
/// @brief magnitude squared
template<class T>
inline dual<T>
abs2(const dual<T>& x)
{
    return dual<T>{(x.real() * x.real()), 0};
}



//-------------------------------------------------------------------
// ROOTS
//-------------------------------------------------------------------
template<class T>
inline
dual<T>
sqrt(const dual<T>& x)
{
    using std::sqrt;
    auto sqrt_r = sqrt(x.real());

    return dual<T>{sqrt_r, x.imag() / (T(2) * sqrt_r)};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
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
inline
dual<common_numeric_t<T1,T2>>
pow(const dual<T1> b, const dual<T2>& e)
{
    using std::pow;
    using std::log;

    auto ba_ea_1 = pow(b.real(), e.real()-1);

    return dual<common_numeric_t<T1,T2>>{
        b.real() * ba_ea_1 * (1 + (e.imag() * log(b.real()))),
        b.imag() * e.real() * ba_ea_1 };
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
pow(const dual<T> b, const T& e)
{
    using std::pow;

    auto ba_e_1 = pow(b.real(), e - 1);

    return dual<T>{b.real() * ba_e_1, b.imag() * e * ba_e_1};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
exp(const dual<T>& x)
{
    using std::exp;
    auto expr = exp(x.real());
    return dual<T>{expr, x.imag() * expr};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
exp2(const dual<T>& x)
{
    using std::exp2;
    auto exp2r = exp2(x.real());

    return dual<T>{exp2r,
        x.imag() * exp2r * T(0.69314718055994530941723212145817656807550013436026)};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
expm1(const dual<T>& x)
{
    using std::expm1;
    using std::exp;
    return dual<T>{expm1(x.real()), x.imag() * exp(x.real())};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
ldexp(const dual<T>& a, const dual<int>&b)
{
    using std::ldexp;
    return dual<T>{ldexp(a.real(), b.real()), ldexp(a.imag(), b.imag())};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
frexp(const dual<T>& a, dual<int>* b)
{
    using std::frexp;
    return dual<T>{
        frexp(a.real(), std::addressof(b->real())),
        frexp(a.imag(), std::addressof(b->imag()))};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
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
inline
dual<T>
log(const dual<T>& x)
{
    using std::log;
    return dual<T>{log(x.real()), x.imag() / x.real()};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
log10(const dual<T>& x)
{
    using std::log10;
    return dual<T>{
        log10(x.real()), x.imag() /
            (x.real() * T(2.3025850929940456840179914546843642076011014886288))};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
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
inline
dual<T>
logb(const dual<T>& x)
{
    using std::logb;
    using std::log;
    return dual<T>{logb(x.real()), x.imag() / (x.real() * log(FLT_RADIX))};
}

//---------------------------------------------------------
/// @brief logarithm + 1
template<class T>
inline
dual<T>
log1p(const dual<T>& x)
{
    using std::log1p;
    return dual<T>{log1p(x.real()), x.imag() / x.real()};
}

//---------------------------------------------------------
/// @brief logarithm to any base
template<class T>
inline
dual<T>
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
inline
dual<T>
sin(const dual<T>& x)
{
    using std::sin;
    using std::cos;
    return dual<T>{sin(x.real()), x.imag() * cos(x.real())};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
cos(const dual<T>& x)
{
    using std::sin;
    using std::cos;
    return dual<T>{cos(x.real()), -x.imag() * sin(x.real())};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
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
inline
dual<T>
asin(const dual<T>& x)
{
    using std::asin;
    using std::sqrt;
    return dual<T>{asin(x.real()), x.imag() / sqrt(1 - (x.real()*x.real()))};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
acos(const dual<T>& x)
{
    using std::acos;
    using std::sqrt;
    return dual<T>{acos(x.real()), -x.imag() / sqrt(1 - (x.real()*x.real()))};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
atan(const dual<T>& x)
{
    using std::atan;
    return dual<T>{atan(x.real()), x.imag() / (1 + (x.real() * x.real()))};
}



//-------------------------------------------------------------------
// HYPERBOLIC
//-------------------------------------------------------------------
template<class T>
inline
dual<T>
sinh(const dual<T>& x)
{
    using std::sinh;
    using std::cosh;
    return dual<T>{sinh(x.real()), x.imag() * cosh(x.real())};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
cosh(const dual<T>& x)
{
    using std::sinh;
    using std::cosh;
    return dual<T>{cosh(x.real()), x.imag() * sinh(x.real())};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
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
inline
dual<T>
asinh(const dual<T>& x)
{
    using std::asinh;
    using std::sqrt;
    return dual<T>{asinh(x.real()), x.imag() / sqrt((x.real()*x.real()) + 1)};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
acosh(const dual<T>& x)
{
    using std::acosh;
    using std::sqrt;
    return dual<T>{acosh(x.real()), -x.imag() / sqrt((x.real()*x.real()) - 1)};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
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
inline dual<T>
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
inline dual<T>
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
//
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
 *
 * TRAITS SPECIALIZATIONS
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
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


namespace detail {

//-------------------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, dual<To>, From> :
    public is_non_narrowing_helper<true,To,From>
{};

//---------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, dual<To>, dual<From> > :
    public is_non_narrowing_helper<true,To,From>
{};

//---------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, To, dual<From> > :
    public is_non_narrowing_helper<true,To,From>
{};


}  // namespace detail



}  // namespace num
}  // namespace am


#endif
