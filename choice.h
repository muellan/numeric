/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2016 André Müller
 *
 *****************************************************************************/

#ifndef AMLIB_NUMERIC_CHOICE_H_
#define AMLIB_NUMERIC_CHOICE_H_

#include <type_traits>
#include <limits>
#include <cstdint>


#include "limits.h"
#include "narrowing.h"


namespace am {
namespace num {


/*****************************************************************************
 *
 * @brief represents an element of IN / IN(numChoices)
 *
 * @tparam IntT       an integral type
 * @tparam numChoices number of choices
 *
 *
 *****************************************************************************/
template<class IntT, IntT numChoices>
class choice
{
    static_assert(is_integral<IntT>::value,
        "choice<T,n> : T has to be an integral number type");

    static_assert(numChoices > 0,
        "choice<T,n> : n must be > 0");

public:
    //---------------------------------------------------------------
    using value_type   = IntT;
    using numeric_type = value_type;


    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    constexpr explicit
    choice(const T& x):
        x_((x < 0) ? (numChoices + (x % numChoices)) : (x % numChoices))
    {
        static_assert(is_integral<T>::value,
            "choice::choice(N) : N has to be an integral number type");
    }

    //-----------------------------------------------------
    constexpr
    choice(const choice&) = default;

    //-----------------------------------------------------
    template<class OtherIntT, OtherIntT m>
    explicit constexpr
    choice(const choice<OtherIntT,m>& c):
        x_(c.x_ % numChoices)
    {}


    //---------------------------------------------------------------
    choice&
    operator = (const choice&) = default;

        //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    choice&
    operator = (const T& x)
    {
        static_assert(is_integral<T>::value,
            "choice::operator=(N) : N has to be an integral number type");

        x_ = value_type(
            (x < 0) ? (numChoices + (x % numChoices)) : (x % numChoices));

        return *this;
    }


    //---------------------------------------------------------------
    constexpr
    value_type
    value() const noexcept {
        return x_;
    }

    //-----------------------------------------------------
    template<class T>
    explicit
    operator T () const noexcept
    {
        return x_;
    }


    //---------------------------------------------------------------
    static constexpr value_type
    min() noexcept {
        return 0;
    }
    //-----------------------------------------------------
    static constexpr value_type
    max() noexcept {
        return (numChoices-1);
    }

    //-----------------------------------------------------
    static constexpr value_type
    choices() noexcept {
        return numChoices;
    }


    //---------------------------------------------------------------
    // +=
    //---------------------------------------------------------------
    choice&
    operator += (const choice& c)
    {
        x_ += c.x_;
        if(x_ > numChoices) x_ -= numChoices;
        return *this;
    }

    //-----------------------------------------------------
    template<class OtherIntT, OtherIntT m>
    choice&
    operator += (const choice<OtherIntT,m>& c)
    {
        return (*this += value_type(c));
    }

    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    choice&
    operator += (const T& x)
    {
        static_assert(is_integral<T>::value,
            "choice::operator+=(T) : T has to be an integral number type");

        auto v = (x_ + x) % numChoices;
        x_ = (v >= 0) ? value_type(v) : value_type(numChoices+v);
        return *this;
    }


    //---------------------------------------------------------------
    // -=
    //---------------------------------------------------------------
    choice&
    operator -= (const choice& c) {
        if(x_ > c.x_) {
            x_ -= c.x_;
        } else {
            x_ = numChoices - c.x_ + x_;
        }
        return *this;
    }

    //-----------------------------------------------------
    template<class OtherIntT, OtherIntT m>
    choice&
    operator -= (const choice<OtherIntT,m>& c)
    {
        return (*this -= value_type(c));
    }

    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    choice&
    operator -= (const T& x)
    {
        static_assert(is_integral<T>::value,
            "choice::operator-=(T) : T has to be an integral number type");

        auto v = (x_ - x) % numChoices;
        x_ = (v >= 0) ? value_type(v) : value_type(numChoices+v);
        return *this;
    }


    //---------------------------------------------------------------
    // * /
    //---------------------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    choice&
    operator *= (const T& x)
    {
        static_assert(is_integral<T>::value,
            "choice::operator*=(T) : T has to be an integral number type");

        auto v = (x_ * x) % numChoices;
        x_ = (v >= 0) ? value_type(v) : value_type(numChoices+v);
        return *this;
    }

    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    choice&
    operator /= (const T& x)
    {
        static_assert(is_integral<T>::value,
            "choice::operator/=(T) : T has to be an integral number type");

        auto v = (x_ / x) % numChoices;
        x_ = (v >= 0) ? value_type(v) : value_type(numChoices+v);
        return *this;
    }


    //---------------------------------------------------------------
    // + -
    //---------------------------------------------------------------
    choice
    operator + (const choice& c) const {
        auto nx = x_ + c.x_;
        if(nx > numChoices) nx -= numChoices;
        //use special non-mod ctor
        return choice{value_type(nx), 0};
    }

    //---------------------------------------------------------
    choice
    operator - (const choice& c) const {
        //use special non-mod ctor
        return choice{
            value_type((x_ > c.x_) ? (x_ - c.x_) : (numChoices - c.x_ + x_)), 0};
    }


    //---------------------------------------------------------------
    // INVERSION
    //---------------------------------------------------------------
    constexpr choice
    operator - () const {
        //use special non-mod ctor
        return choice{value_type(numChoices - x_), 0};
    }

    //-----------------------------------------------------
    choice&
    invert() {
        x_ = value_type(numChoices - x_);
        return *this;
    }


    //---------------------------------------------------------------
    // INCREMENT / DECREMENT
    //---------------------------------------------------------------
    choice&
    operator ++ () {
        if(x_ < (numChoices-1)) ++x_; else x_ = 0;
        return *this;
    }
    //-----------------------------------------------------
    choice
    operator ++ (int) {
        auto old = *this;
        ++*this;
        return old;
    }
    //-----------------------------------------------------
    choice&
    operator -- () {
        if(x_ > 0) --x_; else x_ = value_type(numChoices-1);
        return *this;
    }
    //-----------------------------------------------------
    choice
    operator -- (int) {
        auto old = *this;
        ++*this;
        return old;
    }


private:
    //---------------------------------------------------------------
    /// @brief special ctor without modulo operation
    constexpr explicit
    choice(const value_type& x, int):
        x_{x}
    {}

    value_type x_ = value_type(0);
};






/*****************************************************************************
 *
 * ARITHMETIC
 *
 *****************************************************************************/

#define AM_CHOICE_ARITHMETIC_OP(OPERATOR) \
\
template<class T, class IntT, IntT n, class = typename \
    std::enable_if<is_number<T>::value>::type \
> \
inline constexpr choice<IntT,n> \
operator OPERATOR (const T& x, const choice<IntT,n>& c) \
{ \
    static_assert(is_integral<T>::value, \
        "T has to be an integral number type"); \
\
    return choice<IntT,n>{x OPERATOR c.value()}; \
} \
\
\
template<class T, class IntT, IntT n, class = typename \
    std::enable_if<is_number<T>::value>::type \
> \
inline constexpr choice<IntT,n> \
operator OPERATOR (const choice<IntT,n>& c, const T& x) \
{ \
    static_assert(is_integral<T>::value, \
        "T has to be an integral number type"); \
\
    return choice<IntT,n>{c.value() OPERATOR x}; \
}


AM_CHOICE_ARITHMETIC_OP(+)
AM_CHOICE_ARITHMETIC_OP(-)
AM_CHOICE_ARITHMETIC_OP(*)


#undef AM_CHOICE_ARITHMETIC_OP




/*****************************************************************************
 *
 * COMPARISON
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class Int1, Int1 n, class Int2, Int2 m>
inline constexpr
bool
operator == (choice<Int1,n> a, choice<Int2,m> b) {
    return (a.value() == b.value());
}

//---------------------------------------------------------
template<class Int1, Int1 n, class Int2, Int2 m>
inline constexpr
bool
operator != (choice<Int1,n> a, choice<Int2,m> b) {
    return (a.value() != b.value());
}


//-------------------------------------------------------------------
template<class Int1, Int1 n, class Int2, Int2 m>
inline constexpr
bool
operator <  (choice<Int1,n> a, choice<Int2,m> b) {
    return (a.value() < b.value());
}

//---------------------------------------------------------
template<class Int1, Int1 n, class Int2, Int2 m>
inline constexpr
bool
operator <= (choice<Int1,n> a, choice<Int2,m> b) {
    return (a.value() <= b.value());
}


//-------------------------------------------------------------------
template<class Int1, Int1 n, class Int2, Int2 m>
inline constexpr
bool
operator >  (choice<Int1,n> a, choice<Int2,m> b) {
    return (a.value() > b.value());
}

//---------------------------------------------------------
template<class Int1, Int1 n, class Int2, Int2 m>
inline constexpr
bool
operator >= (choice<Int1,n> a, choice<Int2,m> b) {
    return (a.value() >= b.value());
}






/*****************************************************************************
 *
 * FACTORIES
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<std::uintmax_t n, class T>
inline constexpr choice<decay_t<T>,n>
make_choice(T&& x)
{
    static_assert(is_integral<decay_t<T>>::value,
            "make_choice(T) : T has to be an integral number type");

    return choice<decay_t<T>,n>{std::forward<T>(x)};
}






/*****************************************************************************
 *
 * I/O
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class Ostream, class Int, Int n>
inline constexpr Ostream&
operator << (Ostream& os, const choice<Int,n>& c)
{
    return (os << std::intmax_t(c));
}

//---------------------------------------------------------
template<class Ostream, class Int, Int n>
inline constexpr
Ostream&
print(Ostream& os, const choice<Int,n>& c)
{
    return (os << "[" << std::intmax_t(c) << "/" << std::intmax_t(n) << "]");
}




/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class Int, Int n>
inline constexpr choice<Int,n>
inverse(choice<Int,n> c)
{
    return choice<Int,n>{-c};
}






/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class T, T n>
class numeric_limits<choice<T,n>>
{
    using val_t = choice<T,n>;

public:
    static constexpr bool is_specialized = true;

    static constexpr val_t
    min() {return val_t::min(); }

    static constexpr val_t
    max() {return val_t::max(); }

    static constexpr val_t
    lowest() {return val_t::min(); }

    static constexpr int digits = numeric_limits<T>::digits;
    static constexpr int digits10 = numeric_limits<T>::digits10;
    static constexpr int max_digits10 = numeric_limits<T>::max_digits10;
    static constexpr bool is_signed = numeric_limits<T>::is_signed;
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

    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;
    static constexpr std::float_denorm_style has_denorm = std::denorm_absent;
    static constexpr bool has_denorm_loss = false;

    static constexpr val_t
    infinity() noexcept {return numeric_limits<T>::infinity(); }

    static constexpr val_t
    quiet_NaN() noexcept {return numeric_limits<T>::quiet_NaN(); }

    static constexpr val_t
    signaling_NaN() noexcept {return numeric_limits<T>::signaling_NaN(); }

    static constexpr val_t
    denorm_min() noexcept {return numeric_limits<T>::denorm_min(); }

    static constexpr bool is_iec559 = numeric_limits<T>::is_iec559;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = true;

    static constexpr bool traps = numeric_limits<T>::traps;
    static constexpr bool tinyness_before = numeric_limits<T>::tinyness_before;
    static constexpr std::float_round_style round_style = numeric_limits<T>::round_style;
};


}  // namespace num
}  // namespace am


#endif
