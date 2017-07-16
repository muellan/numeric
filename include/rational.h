/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/

#ifndef AMLIB_NUMERIC_RATIONAL_H_
#define AMLIB_NUMERIC_RATIONAL_H_

#include <cmath>
#include <cfloat>

#include "constants.h"
#include "narrowing.h"
#include "equality.h"


namespace am {
namespace num {


/*****************************************************************************
 *
 * @brief rational number class
 *
 * @tparam IntT type of numerator and denominator
 *
 *
 * TODO include auto-normalizing mechanism (enabled by 2nd template parameter)
 *
 *****************************************************************************/
template<class IntT>
class rational
{
    static_assert(is_integral<IntT>::value,
        "rational<T>: T must be an integral number type");

public:
    //---------------------------------------------------------------
    using value_type      = IntT;
    using numeric_type    = value_type;


    //---------------------------------------------------------------
    // CONSTRUCTION / DESTRUCTION
    //---------------------------------------------------------------
    /// @brief
    constexpr
    rational() = default;

    //-----------------------------------------------------
    /// @brief
    constexpr
    rational(value_type wholes):
        n_{std::move(wholes)}, d_{value_type(1)}
    {}

    //-----------------------------------------------------
    /// @brief
    template<class T, class = typename
        std::enable_if<is_integral<T>::value>::type>
    explicit constexpr
    rational(T&& wholes):
        n_(std::forward<T>(wholes)), d_{value_type(1)}
    {
        AM_CHECK_NARROWING(value_type,T)
    }


    //-----------------------------------------------------
    /// @brief
    constexpr
    rational(value_type numerator, value_type denominator):
        n_(std::move(numerator)), d_(std::move(denominator))
    {}

    //-----------------------------------------------------
    /// @brief
    template<class T1, class T2, class = typename
        std::enable_if<is_integral<T1,T2>::value>::type>
    explicit constexpr
    rational(T1&& numerator, T2&& denominator):
        n_(std::forward<T1>(numerator)), d_(std::forward<T2>(denominator))
    {
        AM_CHECK_NARROWING2(value_type,T1,T2)
    }

    //-----------------------------------------------------
    constexpr
    rational(const rational&) = default;
    //-----------------------------------------------------
    constexpr
    rational(rational&&) = default;


    //---------------------------------------------------------------
    // ASSIGNMENT
    //---------------------------------------------------------------
    rational&
    operator = (const rational&) = default;

    //-----------------------------------------------------
    rational&
    operator = (rational&&) = default;

    //-----------------------------------------------------
    template<class T, class = typename
        std::enable_if<is_integral<T>::value>::type>
    rational&
    operator = (T&& n)
    {
        AM_CHECK_NARROWING(value_type,T)

        n_ = std::forward<T>(n);
        d_ = value_type(0);
        return *this;
    }


    //---------------------------------------------------------------
    // ACCESS
    //---------------------------------------------------------------
    constexpr const value_type&
    numer() const noexcept {
        return n_;
    }
    //-----------------------------------------------------
    constexpr const value_type&
    denom() const noexcept {
        return d_;
    }

    //---------------------------------------------------------------
    template<class T>
    explicit operator
    T() const {
        return (T(n_) / T(d_));
    }


    //---------------------------------------------------------------
    rational&
    invert() {
        using std::swap;
        swap(n_, d_);
        if(d_ < value_type(0)) {
            n_ = -n_;
            d_ = -d_;
        }
        return *this;
    }

    //---------------------------------------------------------
    rational&
    negate() noexcept {
        n_ = -n_;
        return *this;
    }


    //---------------------------------------------------------------
    // rational (op)= value_type
    //---------------------------------------------------------------
    template<class T, class = typename
        std::enable_if<is_integral<T>::value>::type>
    rational&
    operator += (const T& v)
    {
        AM_CHECK_NARROWING(value_type,T)

        n_ += (v * d_);
        return *this;
    }
    //-----------------------------------------------------
    template<class T, class = typename
        std::enable_if<is_integral<T>::value>::type>
    rational&
    operator -= (const T& v)
    {
        AM_CHECK_NARROWING(value_type,T)

        n_ -= (v * d_);
        return *this;
    }
    //-----------------------------------------------------
    template<class T, class = typename
        std::enable_if<is_integral<T>::value>::type>
    rational&
    operator *= (const T& v)
    {
        AM_CHECK_NARROWING(value_type,T)

        n_ *= v;
        return *this;
    }
    //-----------------------------------------------------
    template<class T, class = typename
        std::enable_if<is_integral<T>::value>::type>
    rational&
    operator /= (const T& v)
    {
        AM_CHECK_NARROWING(value_type,T)

        d_ *= v;
        return *this;
    }


    //---------------------------------------------------------------
    // increment / decrement
    //---------------------------------------------------------------
    rational&
    operator ++ () {
        n_ += d_;
        return *this;
    }
    //-----------------------------------------------------
    rational&
    operator -- () {
        n_ -= d_;
        return *this;
    }

    //-----------------------------------------------------
    rational
    operator ++ (int) {
        auto old = *this;
        ++*this;
        return old;
    }
    //-----------------------------------------------------
    rational
    operator -- (int) {
        auto old = *this;
        --*this;
        return old;
    }


    //---------------------------------------------------------------
    // rational (op)= rational with different value_type
    //---------------------------------------------------------------
    template<class T>
    rational&
    operator += (const rational<T>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

        n_ = n_ * o.denom() + o.numer() * d_;
        d_ *= o.denom();
        return *this;
    }
    //-----------------------------------------------------
    template<class T>
    rational&
    operator -= (const rational<T>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

        n_ = n_ * o.denom() - o.numer() * d_;
        d_ *= o.denom();
        return *this;
    }
    //-----------------------------------------------------
    template<class T>
    rational&
    operator *= (const rational<T>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

        n_ *= o.numer();
        d_ *= o.denom();
        return *this;
    }
    //-----------------------------------------------------
    template<class T>
    rational&
    operator /= (const rational<T>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

        n_ *= o.denom();
        d_ *= o.numer();
        return *this;
    }


    //---------------------------------------------------------------
    void
    normalize() {
        using std::abs;
        const auto x = gcd(abs(n_), d_);
        if(x > value_type(1)) {
            n_ /= x;
            d_ /= x;
        }
    }


private:
    //---------------------------------------------------------------
    static value_type
    gcd(value_type a, value_type b) {
        while(b > value_type(0)) {
            const auto m = value_type(a % b);
            a = b;
            b = m;
        }
        return a;
    }


    //---------------------------------------------------------------
    value_type n_;
    value_type d_;
};




/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class T1, class T2, class = typename
    std::enable_if<is_integral<T1>::value && is_integral<T2>::value>::type>
inline constexpr
rational<common_numeric_t<T1,T2>>
make_rational(T1&& a, T2&& b)
{
    return rational<common_numeric_t<T1,T2>>
        {std::forward<T1>(a), std::forward<T2>(b)};
}


//-------------------------------------------------------------------
template<class T, class = typename
    std::enable_if<is_integral<T>::value>::type>
inline constexpr
rational<decay_t<T>>
make_rational(T&& x)
{
    return rational<decay_t<T>>{std::forward<T>(x)};
}



//-------------------------------------------------------------------
// CONVERSION FROM FLOATING-POINT NUMBER
//-------------------------------------------------------------------
template<class IntT, class FpT, class = typename
    std::enable_if<is_floating_point<FpT>::value>::type>
rational<IntT>
make_rational(const FpT& f)
{
    using std::modf;
    using std::fmod;
    using std::floor;

    constexpr auto base = std::numeric_limits<FpT>::radix;

    auto rdigit = rational<IntT>{IntT(0)};

    auto digits = std::vector<rational<IntT> >{};
    digits.resize(base, IntT(0));
    for(auto& d : digits) {
        d = rdigit++;
    }

    auto intgr = FpT(0);

    const int sign = (f < 0);
    auto frac = FpT(sign ? modf(-f, &intgr) : modf(f, &intgr));

    auto rc = rational<IntT>{IntT(1)};
    auto ri = rational<IntT>{IntT(0)};
    while(floor(intgr) != FpT(0)) {
        ri += digits[static_cast<std::size_t>(fmod(intgr, base))];
        intgr /= base;
        ri /= rdigit;
        rc *= rdigit;
    }

    ri *= rc;
    rc = IntT(1);
    auto rf = rational<IntT>{IntT(0)};
    while(!approx_0(frac)) {
        rf *= rdigit;
        rc /= rdigit;
        frac *= base;
        frac = modf(frac, &intgr);
        rf += digits[static_cast<std::size_t>(intgr)];
    }
    rf *= rc;

    return (sign ? (-ri - rf) : (ri + rf));
}

//---------------------------------------------------------
template<class T, class = typename
    std::enable_if<is_floating_point<T>::value>::type>
inline rational<int>
make_rational(const T& f)
{
    return make_rational<int>(f);
}



//-------------------------------------------------------------------
// I/O
//-------------------------------------------------------------------
template<class Istream, class T>
inline Istream&
operator >> (Istream& is, rational<T>& x)
{
    is.clear();

    auto numer = T(0);
    auto denom = T(1);

    is >> numer;
    if(!is.good()) return is;

    auto c = is.peek();
    if(c == '/') {
        c = is.get();
        is >> denom;
        if(!is.good()) {
            is.clear(is.rdstate() | std::ios::badbit);
        }
    }
    x = rational<T>{numer,denom};
    return is;
}

//---------------------------------------------------------
template<class Ostream, class T>
inline Ostream&
operator << (Ostream& os, const rational<T>& x)
{
    return (os << x.numer() << " " << x.denom() );
}

//---------------------------------------------------------
template<class T, class Ostream>
inline Ostream&
print(Ostream& os, const rational<T>& d)
{
    return (os << d.numer() << "/" << d.denom());
}




/*****************************************************************************
 *
 * ACCESS
 *
 *****************************************************************************/
template<class T>
const T&
numer(const rational<T>& x) {
    return x.numer();
}


//-------------------------------------------------------------------
template<class T>
const T&
denom(const rational<T>& x) {
    return x.denom();
}


//-------------------------------------------------------------------
/// @brief whole parts
template<class T>
T
wholes(const rational<T>& x)
{
    using std::floor;
    return static_cast<T>(floor(static_cast<real_t>(x)));
}


//-------------------------------------------------------------------
/// @brief
template<class T>
inline rational<T>
normalized(rational<T> x)
{
    x.normalize();
    return x;
}


//-------------------------------------------------------------------
/// @brief
template<class T>
inline constexpr
rational<T>
reciprocal(const rational<T>& x)
{
    return (x.numer() < T(0))
        ? rational<T>{-x.denom(), -x.numer()}
        : rational<T>{x.denom(), x.numer()};
}




/*****************************************************************************
 *
 * COMPARISON
 *
 *****************************************************************************/
template<class T1, class T2>
inline bool
operator == (const rational<T1>& x, const rational<T2>& y)
{
    return ((x.numer() * y.denom()) == (y.numer() * x.denom()));
}

//---------------------------------------------------------
template<class T1, class T2>
inline bool
operator != (const rational<T1>& x, const rational<T2>& y)
{
    return ((x.numer() * y.denom()) != (y.numer() * x.denom()));
}


//-------------------------------------------------------------------
template<class T1, class T2, class T3 = common_numeric_t<T1,T2>>
inline constexpr bool
approx_equal(const rational<T1>& a, const rational<T2>& b,
    const T3& tol = tolerance<T3>::value())
{
    return approx_equal(a.numer() * b.denom(), b.numer() * a.denom(), tol);
}

//---------------------------------------------------------
template<class T>
inline constexpr bool
approx_1(const rational<T>& x, const T& tol = tolerance<T>::value())
{
    return approx_1(real_t(x.numer()) / real_t(x.denom()), tol);
}

//---------------------------------------------------------
template<class T>
inline constexpr bool
approx_0(const rational<T>& x, const T& tol = tolerance<T>::value())
{
    return approx_0(x.numer(), tol);
}



//-------------------------------------------------------------------
// COMPARISON
//-------------------------------------------------------------------
template<class T1, class T2>
inline bool
operator > (const rational<T1>& x, const rational<T2>& y)
{
    return ((x.numer() * y.denom()) > (y.numer() * x.denom()));
}

//---------------------------------------------------------
template<class T1, class T2>
inline bool
operator >= (const rational<T1>& x, const rational<T2>& y)
{
    return ((x.numer() * y.denom()) >= (y.numer() * x.denom()));
}

//---------------------------------------------------------
template<class T1, class T2>
inline bool
operator < (const rational<T1>& x, const rational<T2>& y)
{
    return ((x.numer() * y.denom()) < (y.numer() * x.denom()));
}

//---------------------------------------------------------
template<class T1, class T2>
inline bool
operator <= (const rational<T1>& x, const rational<T2>& y)
{
    return ((x.numer() * y.denom()) <= (y.numer() * x.denom()));
}



//-------------------------------------------------------------------
// COMPARISON WITH PLAIN NUMBERS
//-------------------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_integral<T2>::value>::type>
inline bool
operator > (const rational<T1>& x, const T2& r)
{
    return (x.numer() > (r * x.denom()));
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_integral<T2>::value>::type>
inline bool
operator > (const T2& r, const rational<T1>& x)
{
    return (x.numer() > (r * x.denom()));
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_integral<T2>::value>::type>
inline bool
operator >= (const rational<T1>& x, const T2& r)
{
    return (x.numer() >= (r * x.denom()));
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_integral<T2>::value>::type>
inline bool
operator >= (const T2& r, const rational<T1>& x)
{
    return (x.numer() >= (r * x.denom()));
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_integral<T2>::value>::type>
inline bool
operator < (const rational<T1>& x, const T2& r)
{
    return (x.numer() < (r * x.denom()));
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_integral<T2>::value>::type>
inline bool
operator < (const T2& r, const rational<T1>& x)
{
    return (x.numer() < (r * x.denom()));
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_integral<T2>::value>::type>
inline bool
operator <= (const rational<T1>& x, const T2& r)
{
    return (x.numer() <= (r * x.denom()));
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_integral<T2>::value>::type>
inline bool
operator <= (const T2& r, const rational<T1>& x)
{
    return (x.numer() <= (r * x.denom()));
}




/*****************************************************************************
 *
 * ARITHMETIC
 *
 *****************************************************************************/
template<class T1, class T2>
inline constexpr
rational<common_numeric_t<T1,T2>>
operator + (const rational<T1> x, const rational<T2>& y)
{
    return rational<common_numeric_t<T1,T2>>{
        (x.numer() * y.denom()) + (y.numer() * x.denom()),
         x.denom() * y.denom() };
}

//---------------------------------------------------------
template<class T>
inline constexpr
rational<T>
operator + (const rational<T> x, const T& y)
{
    return rational<T>{x.numer() + (y * x.denom()), x.denom()};
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
rational<common_numeric_t<T1,T2>>
operator - (const rational<T1> x, const rational<T2>& y)
{
    return rational<common_numeric_t<T1,T2>>{
        (x.numer() * y.denom()) - (y.numer() * x.denom()),
        x.denom() * y.denom() };
}

//---------------------------------------------------------
template<class T>
inline constexpr
rational<T>
operator - (const rational<T> x, const T& y)
{
    return rational<T>{x.numer() - (y * x.denom()), x.denom()};
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
rational<common_numeric_t<T1,T2>>
operator * (const rational<T1> x, const rational<T2>& y)
{
    return rational<common_numeric_t<T1,T2>>{
        x.numer() * y.numer(), x.denom() * y.denom() };
}

//---------------------------------------------------------
template<class T>
inline constexpr
rational<T>
operator * (const rational<T> x, const T& y)
{
    return rational<T>{x.numer() * y, x.denom()};
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
rational<common_numeric_t<T1,T2>>
operator / (const rational<T1> x, const rational<T2>& y)
{
    return rational<common_numeric_t<T1,T2>>{
        x.numer() * y.denom(), x.denom() * y.numer() };
}

//---------------------------------------------------------
template<class T>
inline constexpr
rational<T>
operator / (const rational<T> x, const T& y)
{
    return rational<T>{x.numer(), x.denom() * y};
}



//-------------------------------------------------------------------
template<class T>
inline
rational<T>
operator ^ (const rational<T> b, const T& e)
{
    return pow(b,e);
}



//-------------------------------------------------------------------
template<class T>
inline constexpr
rational<T>
operator - (const rational<T> x)
{
    return rational<T>{-x.numer(), x.denom()};
}




/*****************************************************************************
 *
 * FUNCTIONS
 *
 *****************************************************************************/
template<class T>
inline
rational<T>
real(const rational<T>& r) {
    return r;
}



//-------------------------------------------------------------------
template<class T>
inline
rational<T>
ceil(const rational<T>& x)
{
    using std::ceil;
    return rational<T>{ceil(static_cast<T>(x))};
}

//---------------------------------------------------------
template<class T>
inline
rational<T>
floor(const rational<T>& x)
{
    return rational<T>{wholes(x)};
}



//-------------------------------------------------------------------
/// @brief absolute value
template<class T>
inline rational<T>
abs(const rational<T>& x)
{
    using std::abs;
    return rational<T>{abs(x.numer()), abs(x.denom())};
}



//-------------------------------------------------------------------
template<class T>
inline
rational<T>
pow(const rational<T> b, const T& e)
{
    using std::pow;

    return ((e > T(0))
        ? rational<T>{pow(b.numer(),e), pow(b.denom(),e)}
        : rational<T>{pow(b.denom(),e), pow(b.numer(),e)} );
}



//-------------------------------------------------------------------
template<class T>
inline bool
isfinite(const rational<T>& x)
{
    using std::isfinite;
    return (isfinite(x.numer()) && isfinite(x.denom()));
}

//---------------------------------------------------------
template<class T>
inline bool
isinf(const rational<T>& x)
{
    using std::isinf;
    return (isinf(x.numer()) || isinf(x.denom()));
}

//---------------------------------------------------------
template<class T>
inline bool
isnan(const rational<T>& x)
{
    using std::isnan;
    return (isnan(x.numer()) || isnan(x.denom()));
}

//---------------------------------------------------------
template<class T>
inline bool
isnormal(const rational<T>& x)
{
    using std::isnormal;
    return (isnormal(x.numer()) && isnormal(x.denom()));
}




/*****************************************************************************
 *
 * TRAITS SPECIALIZATIONS
 *
 *****************************************************************************/
template<class T>
struct is_number<rational<T>> : std::true_type
{};


namespace detail {

//-------------------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, rational<To>, From> :
    public is_non_narrowing_helper<true,To,From>
{};

//---------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, rational<To>, rational<From> > :
    public is_non_narrowing_helper<true,To,From>
{};

//---------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, To, rational<From> > :
    public is_non_narrowing_helper<true,To,From>
{};


}  // namespace detail


}  // namespace num
}  // namespace am


#endif
