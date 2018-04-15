/******************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_RATIONAL_H_
#define AM_NUMERIC_RATIONAL_H_

#include <cmath>
#include <cfloat>
#include <vector>

#include "constants.h"
#include "equality.h"


namespace am {
namespace num {


/*************************************************************************//***
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
    using value_type   = IntT;
    using numeric_type = value_type;


    //---------------------------------------------------------------
    constexpr
    rational() = default;

    constexpr
    rational(value_type wholes):
        n_{std::move(wholes)}, d_{value_type(1)}
    {}

    constexpr
    rational(value_type numerator, value_type denominator):
        n_(std::move(numerator)), d_(std::move(denominator))
    {}

    constexpr
    rational(const rational&) = default;

    constexpr
    rational(rational&&) = default;


    //---------------------------------------------------------------
    rational&
    operator = (const rational&) = default;

    rational&
    operator = (rational&&) = default;

    rational&
    operator = (const value_type& n)
    {
        n_ = n;
        d_ = value_type(0);
        return *this;
    }


    //---------------------------------------------------------------
    constexpr const value_type&
    numer() const noexcept {
        return n_;
    }
    
    constexpr const value_type&
    denom() const noexcept {
        return d_;
    }

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

    rational&
    negate() noexcept {
        n_ = -n_;
        return *this;
    }


    //---------------------------------------------------------------
    rational&
    operator += (const value_type& v)
    {
        n_ += (v * d_);
        return *this;
    }
    
    rational&
    operator -= (const value_type& v)
    {
        n_ -= (v * d_);
        return *this;
    }

    rational&
    operator *= (const value_type& v)
    {
        n_ *= v;
        return *this;
    }

    rational&
    operator /= (const value_type& v)
    {
        d_ *= v;
        return *this;
    }


    //---------------------------------------------------------------
    rational&
    operator ++ () {
        n_ += d_;
        return *this;
    }

    rational&
    operator -- () {
        n_ -= d_;
        return *this;
    }

    rational
    operator ++ (int) {
        auto old = *this;
        ++*this;
        return old;
    }

    rational
    operator -- (int) {
        auto old = *this;
        --*this;
        return old;
    }


    //---------------------------------------------------------------
    rational&
    operator += (const rational& o)
    {
        n_ = n_ * o.denom() + o.numer() * d_;
        d_ *= o.denom();
        return *this;
    }

    rational&
    operator -= (const rational& o)
    {
        n_ = n_ * o.denom() - o.numer() * d_;
        d_ *= o.denom();
        return *this;
    }

    rational&
    operator *= (const rational& o)
    {
        n_ *= o.numer();
        d_ *= o.denom();
        return *this;
    }
    
    rational&
    operator /= (const rational& o)
    {
        n_ *= o.denom();
        d_ *= o.numer();
        return *this;
    }


    //---------------------------------------------------------------
    void
    normalize() {
        using std::abs;
        const value_type x = gcd(value_type(abs(n_)), d_);
        if(x > value_type(1)) {
            n_ = value_type(n_ / x);
            d_ = value_type(d_ / x);
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
template<class T1, class T2, class = 
    std::enable_if_t<is_integral<T1>::value && is_integral<T2>::value>>
inline constexpr auto
make_rational(T1&& a, T2&& b)
{
    using T = common_numeric_t<T1,T2>;
    return rational<T>{T(std::forward<T1>(a)), T(std::forward<T2>(b))};
}


//-------------------------------------------------------------------
template<class T, class = std::enable_if_t<is_integral<T>::value>>
inline constexpr auto
make_rational(T&& x)
{
    return rational<std::decay_t<T>>{std::forward<T>(x)};
}



//-------------------------------------------------------------------
// CONVERSION FROM FLOATING-POINT NUMBER
//-------------------------------------------------------------------
template<class IntT, class FpT, class = 
    std::enable_if_t<is_floating_point<FpT>::value>>
auto
make_rational(const FpT& f)
{
    using std::modf;
    using std::fmod;
    using std::floor;

    constexpr auto base = std::numeric_limits<FpT>::radix;

    auto rdigit = rational<IntT>{IntT(0)};

    std::vector<rational<IntT> > digits;
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
template<class T, class = std::enable_if_t<is_floating_point<T>::value>>
inline auto
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
inline decltype(auto)
numer(const rational<T>& x) noexcept 
{
    return x.numer();
}

template<class T>
inline decltype(auto)
denom(const rational<T>& x) noexcept 
{
    return x.denom();
}

/// @brief whole parts
template<class T>
inline auto 
wholes(const rational<T>& x)
{
    using std::floor;
    return static_cast<T>(floor(static_cast<real_t>(x)));
}


//-------------------------------------------------------------------
/// @brief
template<class T>
inline auto
normalized(rational<T> x)
{
    x.normalize();
    return x;
}


//-------------------------------------------------------------------
/// @brief
template<class T>
inline constexpr auto
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

template<class T1, class T2>
inline bool
operator != (const rational<T1>& x, const rational<T2>& y)
{
    return ((x.numer() * y.denom()) != (y.numer() * x.denom()));
}


template<class T1, class T2, class T3 = common_numeric_t<T1,T2>>
inline constexpr bool
approx_equal(const rational<T1>& a, const rational<T2>& b,
    const T3& tol = tolerance<T3>)
{
    return approx_equal(a.numer() * b.denom(), b.numer() * a.denom(), tol);
}


template<class T>
inline constexpr bool
approx_1(const rational<T>& x, const T& tol = tolerance<T>)
{
    return approx_1(real_t(x.numer()) / real_t(x.denom()), tol);
}


template<class T>
inline constexpr bool
approx_0(const rational<T>& x, const T& tol = tolerance<T>)
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

template<class T1, class T2>
inline bool
operator >= (const rational<T1>& x, const rational<T2>& y)
{
    return ((x.numer() * y.denom()) >= (y.numer() * x.denom()));
}

template<class T1, class T2>
inline bool
operator < (const rational<T1>& x, const rational<T2>& y)
{
    return ((x.numer() * y.denom()) < (y.numer() * x.denom()));
}

template<class T1, class T2>
inline bool
operator <= (const rational<T1>& x, const rational<T2>& y)
{
    return ((x.numer() * y.denom()) <= (y.numer() * x.denom()));
}



//-------------------------------------------------------------------
// COMPARISON WITH PLAIN NUMBERS
//-------------------------------------------------------------------
template<class T1, class T2, class = std::enable_if_t<is_integral<T2>::value>>
inline bool
operator > (const rational<T1>& x, const T2& r)
{
    return (x.numer() > (r * x.denom()));
}

template<class T1, class T2, class = std::enable_if_t<is_integral<T2>::value>>
inline bool
operator > (const T2& r, const rational<T1>& x)
{
    return (x.numer() > (r * x.denom()));
}


template<class T1, class T2, class = std::enable_if_t<is_integral<T2>::value>>
inline bool
operator >= (const rational<T1>& x, const T2& r)
{
    return (x.numer() >= (r * x.denom()));
}

template<class T1, class T2, class = std::enable_if_t<is_integral<T2>::value>>
inline bool
operator >= (const T2& r, const rational<T1>& x)
{
    return (x.numer() >= (r * x.denom()));
}


template<class T1, class T2, class = std::enable_if_t<is_integral<T2>::value>>
inline bool
operator < (const rational<T1>& x, const T2& r)
{
    return (x.numer() < (r * x.denom()));
}

template<class T1, class T2, class = std::enable_if_t<is_integral<T2>::value>>
inline bool
operator < (const T2& r, const rational<T1>& x)
{
    return (x.numer() < (r * x.denom()));
}


template<class T1, class T2, class = std::enable_if_t<is_integral<T2>::value>>
inline bool
operator <= (const rational<T1>& x, const T2& r)
{
    return (x.numer() <= (r * x.denom()));
}

template<class T1, class T2, class = std::enable_if_t<is_integral<T2>::value>>
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
inline constexpr auto
operator + (const rational<T1> x, const rational<T2>& y)
{
    using T = common_numeric_t<T1,T2>;

    return rational<T>{
        T( (x.numer() * y.denom())  + (y.numer() * x.denom()) ),
        T( x.denom() * y.denom() ) };
}

template<class T>
inline constexpr auto
operator + (const rational<T> x, const T& y)
{
    return rational<T>{x.numer() + (y * x.denom()), x.denom()};
}


//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr auto
operator - (const rational<T1> x, const rational<T2>& y)
{
    using T = common_numeric_t<T1,T2>;

    return rational<T>{
         T( (x.numer() * y.denom()) - (y.numer() * x.denom()) ),
         T( x.denom() * y.denom() ) };
}

template<class T>
inline constexpr auto
operator - (const rational<T> x, const T& y)
{
    return rational<T>{ x.numer() - (y * x.denom()), x.denom() };
}


//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr auto
operator * (const rational<T1> x, const rational<T2>& y)
{
    using T = common_numeric_t<T1,T2>;

    return rational<T>{ T(x.numer() * y.numer()), 
                        T(x.denom() * y.denom()) };
}

template<class T>
inline constexpr auto
operator * (const rational<T> x, const T& y)
{
    return rational<T>{ x.numer() * y, x.denom() };
}


//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr auto
operator / (const rational<T1> x, const rational<T2>& y)
{
    using T = common_numeric_t<T1,T2>;

    return rational<T>{ T(x.numer() * y.denom()), 
                        T(x.denom() * y.numer()) };
}

template<class T>
inline constexpr auto
operator / (const rational<T> x, const T& y)
{
    return rational<T>{ x.numer(), x.denom() * y };
}


//-------------------------------------------------------------------
template<class T>
inline auto
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
    return rational<T>{ -x.numer(), x.denom() };
}




/*****************************************************************************
 *
 * FUNCTIONS
 *
 *****************************************************************************/
template<class T>
inline rational<T>&
real(rational<T>& r) noexcept {
    return r;
}

template<class T>
inline const rational<T>&
real(const rational<T>& r) noexcept {
    return r;
}

template<class T>
inline rational<T>
real(rational<T>&& r) noexcept {
    return r;
}


//-------------------------------------------------------------------
template<class T>
inline auto
ceil(const rational<T>& x)
{
    using std::ceil;
    return rational<T>{ceil(static_cast<T>(x))};
}

template<class T>
inline auto
floor(const rational<T>& x)
{
    return rational<T>{wholes(x)};
}



//-------------------------------------------------------------------
/// @brief absolute value
template<class T>
inline auto
abs(const rational<T>& x)
{
    using std::abs;
    return rational<T>{abs(x.numer()), abs(x.denom())};
}



//-------------------------------------------------------------------
template<class T>
inline auto
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


}  // namespace num
}  // namespace am


namespace std {


/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class T>
class numeric_limits<am::num::rational<T>>
{
    using val_t = am::num::rational<T>;

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
    epsilon() noexcept { return std::numeric_limits<T>::epsilon(); }

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

} // namespace std

#endif
