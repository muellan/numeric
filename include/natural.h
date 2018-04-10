/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/

#ifndef AMLIB_NUMERIC_NATURAL_H_
#define AMLIB_NUMERIC_NATURAL_H_


#include <cstdint>
#include <cmath>
#include <cassert>
#include <iostream>

#include "limits.h"
#include "conversion.h"


namespace am {
namespace num {


/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class> class natural;

template<class>
struct is_natural :
    std::false_type
{};

template<class T>
struct is_natural<natural<T>> :
    std::true_type
{};




/*************************************************************************//***
 *
 * @brief   represents an integral natural "safe unsigned int"
 * @details value is either in range [0,max] or infinity
 *
 *
 *****************************************************************************/
template<class IntT = int>
class natural
{
    static_assert(is_integral<IntT>::value && !is_unsigned<IntT>::value,
        "natural<T>: T has to be a signed integral number type");

public:
    //---------------------------------------------------------------
    using value_type = IntT;
    using numeric_type = value_type;


    //---------------------------------------------------------------
    constexpr
    natural(value_type v = zero_val()) noexcept :
        v_(v >= zero_val() ? v : zero_val())
    {}
    
    constexpr
    natural(const natural& source) noexcept :
        v_(source.v_)
    {}
    
    constexpr
    natural(natural&& source) noexcept :
        v_(std::move(source.v_))
    {}


    //---------------------------------------------------------------
    natural&
    operator = (const natural& source) noexcept {
        v_ = source.v_;
        return *this;
    }

    natural&
    operator = (natural&& source) noexcept {
        v_ = std::move(source.v_);
        return *this;
    }

    natural&
    operator = (const value_type v) noexcept
    {
        v_ = v < zero_val() ? zero_val() 
                            : v > max_val() ? infty_val() : v;
        return *this;
    }


    //---------------------------------------------------------------
    template<class T, class = std::enable_if_t<is_number<T>::value>>
    explicit
    operator T() const noexcept
    {
        return (isinf(*this) ? num::infinity<T> : T(v_));
    }

    value_type
    value() const noexcept
    {
        return (isinf(*this) ? num::infinity<value_type> : v_);
    }


    //---------------------------------------------------------------
    static constexpr natural
    zero() noexcept {
        return natural(zero_val());
    }
   
    static constexpr natural
    max() noexcept {
        return natural(max_val());
    }
    
    static constexpr natural
    infinity() noexcept {
        return natural{0,0};
    }


    //---------------------------------------------------------------
    inline friend
    constexpr bool
    isinf(const natural& q) noexcept {
        return (q.v_ < zero_val());
    }
  
    inline friend
    constexpr bool
    isfinite(const natural& q) noexcept {
        return (q.v_ > zero_val());
    }


    //---------------------------------------------------------------
    natural&
    operator += (const natural& c) noexcept
    {
        if(!isinf(*this)) {
            if(isinf(c)) {
                *this = infinity();
            } else {
                if((max_val() - v_) < c.v_) {
                    v_ = max_val();
                } else {
                    v_ += c.v_;
                }
            }
        }
        return *this;
    }

    natural&
    operator += (const value_type& v) noexcept
    {
        if(!isinf(*this)) {
            if(v > max_val()) {
                *this = infinity();
            } else {
                if((max_val() - v_) < v) {
                    v_ = max_val();
                } else {
                    v_ += v;
                }
            }
        }
        return *this;
    }


    //---------------------------------------------------------------
    natural&
    operator -= (const natural& c) noexcept
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

    natural&
    operator -= (const value_type& v) noexcept
    {
        if(!isinf(*this)) {
            if(v_ > v) {
                v_ -= v;
            } else {
                v_ = zero_val();
            }
        }
        return *this;
    }


    //---------------------------------------------------------------
    natural&
    operator *= (const natural& c) noexcept
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

    natural&
    operator *= (const value_type& v) noexcept
    {
        if(!isinf(*this)) {
            if(v > zero_val()) {
                if(v <= max_val()) {
                    if(v > (max_val() / v_)) {
                        v_ = max_val();
                    } else {
                        v_ *= v;
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
    natural&
    operator ++ () noexcept {
        if(v_ != infty_val() && v_ < max_val()) ++v_;
        return *this;
    }

    natural
    operator ++ (int) noexcept {
        auto old = *this;
        ++*this;
        return old;
    }

    natural&
    operator -- () noexcept {
        if(v_ != infty_val()) {
            if(v_ > zero_val()) --v_; else v_ = zero_val();
        }
        return *this;
    }

    natural
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
        return std::numeric_limits<value_type>::max();
    }


    //---------------------------------------------------------------
    /// @brief special ctor for infinity initialization
    constexpr
    natural(int,int) noexcept :
        v_(infty_val())
    {}

    //---------------------------------------------------------------
    value_type v_;

};






/*****************************************************************************
 *
 * ARITHMETIC
 *
 *****************************************************************************/
template<class T1, class T2>
inline constexpr
natural<common_numeric_t<T1,T2>>
operator + (const natural<T1>& a, const natural<T2>& b) noexcept
{
    using std::isinf;

    using cv_t = common_numeric_t<T1,T2>;
    using res_t = natural<cv_t>;

    return (isinf(a) || isinf(b))
        ? res_t::infinity()
        : ( (cv_t(res_t::max()) - cv_t(a)) < cv_t(b)
                ? res_t::max()
                : res_t{cv_t(a) + cv_t(b)});
}

//---------------------------------------------------------
template<class T1, class T2, class = std::enable_if_t<
    !is_natural<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr common_numeric_t<T1,T2>
operator + (const natural<T1>& a, const T2& b) noexcept
{
    using std::isinf;

    using cv_t = common_numeric_t<T1,T2>;

    return (isinf(a) || isinf(b))
        ? num::infinity<cv_t>
        : ( (max_value<cv_t> - cv_t(a)) < cv_t(b)
               ? max_value<cv_t>
               : cv_t(a) + cv_t(b));
}
//---------------------------------------------------------
template<class T1, class T2, class = std::enable_if_t<
    !is_natural<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator + (const T2& b, const natural<T1>& a) noexcept
{
    using std::isinf;

    using cv_t = common_numeric_t<T1,T2>;

    return (isinf(a) || isinf(b))
        ? num::infinity<cv_t>
        : ( (max_value<cv_t> - cv_t(b)) < cv_t(a)
               ? max_value<cv_t>
               : cv_t(b) + cv_t(a));
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
natural<common_numeric_t<T1,T2>>
operator - (const natural<T1>& a, const natural<T2>& b) noexcept
{
    using std::isinf;

    using cv_t = common_numeric_t<T1,T2>;
    using res_t = natural<cv_t>;

    return (isinf(a))
        ? (isinf(b)) ? res_t::zero() : res_t::infinity()
        : (isinf(b)) ? res_t::zero() :
            (cv_t(b) > cv_t(a)
               ? res_t::zero()
               : res_t{cv_t(a) - cv_t(b)});
}

//---------------------------------------------------------
template<class T1, class T2, class = std::enable_if_t<
    !is_natural<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr common_numeric_t<T1,T2>
operator - (const natural<T1>& a, const T2& b) noexcept
{
    using std::isinf;

    using cv_t = common_numeric_t<T1,T2>;

    return (isinf(a))
        ? (isinf(b)) ? cv_t(0) : num::infinity<cv_t>
        : (isinf(b)) ? cv_t(0) :
            (cv_t(b) > cv_t(a)
               ? cv_t(0)
               : cv_t(a) - cv_t(b));
}
//---------------------------------------------------------
template<class T1, class T2, class = std::enable_if_t<
    !is_natural<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator - (const T2& b, const natural<T1>& a) noexcept
{
    using std::isinf;

    using cv_t = common_numeric_t<T1,T2>;

    return (isinf(b))
        ? (isinf(a)) ? cv_t(0) : num::infinity<cv_t>
        : (isinf(a)) ? cv_t(0) :
            (cv_t(a) > cv_t(b)
               ? cv_t(0)
               : cv_t(b) - cv_t(a));
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
natural<common_numeric_t<T1,T2>>
operator * (const natural<T1>& a, const natural<T2>& b) noexcept
{
    using std::isinf;

    using cv_t = common_numeric_t<T1,T2>;
    using res_t = natural<cv_t>;

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
template<class T1, class T2, class = std::enable_if_t<
    !is_natural<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr common_numeric_t<T1,T2>
operator * (const natural<T1>& a, const T2& b) noexcept
{
    using std::isinf;

    using cv_t = common_numeric_t<T1,T2>;

    return (isinf(a))
        ? ( (isinf(b))
            ? num::infinity<cv_t>
            : ((b == T2(0)) ? cv_t(0) : num::infinity<cv_t>)
        )
        : ( (isinf(b))
            ? ((a == a.zero()) ? cv_t(0) : num::infinity<cv_t>)
            : ((a == a.zero() || b == T2(0))
                  ? cv_t(0)
                  : ((cv_t(a) > (max_value<cv_t> / cv_t(b)))
                      ? max_value<cv_t>
                      : cv_t(a) * cv_t(b) )
              )
        );
}
//---------------------------------------------------------
template<class T1, class T2, class = std::enable_if_t<
    !is_natural<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator * (const T2& b, const natural<T1>& a) noexcept
{
    using std::isinf;

    using cv_t = common_numeric_t<T1,T2>;

    return (isinf(a))
        ? ( (isinf(b))
            ? num::infinity<cv_t>
            : ((b == T2(0)) ? cv_t(0) : num::infinity<cv_t>)
        )
        : ( (isinf(b))
            ? ((a == a.zero()) ? cv_t(0) : num::infinity<cv_t>)
            : ((a == a.zero() || b == T2(0))
                  ? cv_t(0)
                  : ((cv_t(a) > (max_value<cv_t> / cv_t(b)))
                      ? max_value<cv_t>
                      : cv_t(a) * cv_t(b) )
              )
        );
}




/*****************************************************************************
 *
 * COMPARISONS
 *
 *****************************************************************************/
template<class T1, class T2>
inline constexpr bool
operator == (const natural<T1>& a, const natural<T2>& b) noexcept
{
    return isinf(a) ? isinf(b) : (isinf(b) ? false : (T1(a) == T2(b)) );
}

//---------------------------------------------------------
template<class T1, class T2, class = std::enable_if_t<
    !is_natural<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator == (const natural<T1>& a, const T2& b) noexcept
{
    using std::isinf;

    return isinf(a) ? isinf(b) : (isinf(b) ? false : (T1(a) == b) );
}
//---------------------------------------------------------
template<class T1, class T2, class = std::enable_if_t<
    !is_natural<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator == (const T2& b, const natural<T1>& a) noexcept
{
    using std::isinf;

    return isinf(a) ? isinf(b) : (isinf(b) ? false : (T1(a) == b) );
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
operator != (const natural<T1>& a, const natural<T2>& b) noexcept
{
    return !(a == b);
}

//---------------------------------------------------------
template<class T1, class T2, class = std::enable_if_t<
    !is_natural<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator != (const natural<T1>& a, const T2& b) noexcept
{
    return !(a == b);
}
//---------------------------------------------------------
template<class T1, class T2, class = std::enable_if_t<
    !is_natural<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator != (const T2& b, const natural<T1>& a) noexcept
{
    return !(a == b);
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
operator <= (const natural<T1>& a, const natural<T2>& b) noexcept
{
    return isinf(b) || (!isinf(a) && (T1(a) <= T2(b)));
}

//---------------------------------------------------------
template<class T1, class T2, class = std::enable_if_t<
    !is_natural<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator <= (const natural<T1>& a, const T2& b) noexcept
{
    using std::isinf;

    return isinf(b) || (!isinf(a) && (T1(a) <= b));
}
//---------------------------------------------------------
template<class T1, class T2, class = std::enable_if_t<
    !is_natural<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator <= (const T2& b, const natural<T1>& a) noexcept
{
    using std::isinf;

    return isinf(b) || (!isinf(a) && (b <= T1(a)));
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
operator <  (const natural<T1>& a, const natural<T2>& b) noexcept
{
    return !isinf(a) && (isinf(b) || (T1(a) < T2(b)));
}

//---------------------------------------------------------
template<class T1, class T2, class = std::enable_if_t<
    !is_natural<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator <  (const natural<T1>& a, const T2& b) noexcept
{
    using std::isinf;

    return !isinf(a) && (isinf(b) || (T1(a) < b));
}
//---------------------------------------------------------
template<class T1, class T2, class = std::enable_if_t<
    !is_natural<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator <  (const T2& b, const natural<T1>& a) noexcept
{
    using std::isinf;

    return !isinf(a) && (isinf(b) || (b < T1(a)) );
}


//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
operator >= (const natural<T1>& a, const natural<T2>& b) noexcept
{
    return isinf(a) || (!isinf(b) && (T1(a) >= T2(b)));
}

//---------------------------------------------------------
template<class T1, class T2, class = std::enable_if_t<
    !is_natural<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator >= (const natural<T1>& a, const T2& b) noexcept
{
    using std::isinf;

    return isinf(a) || (!isinf(b) && (T1(a) >= b));
}
//---------------------------------------------------------
template<class T1, class T2, class = std::enable_if_t<
    !is_natural<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator >= (const T2& b, const natural<T1>& a) noexcept
{
    using std::isinf;

    return isinf(a) || (!isinf(b) && (b >= T1(a)));
}


//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
operator >  (const natural<T1>& a, const natural<T2>& b) noexcept
{
    return !isinf(b) && (isinf(a) || (T1(a) > T2(b)) );
}

//---------------------------------------------------------
template<class T1, class T2, class = std::enable_if_t<
    !is_natural<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator >  (const natural<T1>& a, const T2& b) noexcept
{
    using std::isinf;

    return !isinf(b) && (isinf(a) || (T1(a) > b) );
}
//---------------------------------------------------------
template<class T1, class T2, class = std::enable_if_t<
    !is_natural<T2>::value && is_number<T2>::value && is_integral<T2>::value>>
inline constexpr bool
operator >  (const T2& b, const natural<T1>& a) noexcept
{
    using std::isinf;

    return !isinf(b) && (isinf(a) || (b > T1(a)) );
}




/*****************************************************************************
 *
 * I/O
 *
 *****************************************************************************/
template<class Ostream, class Int>
inline Ostream&
operator << (Ostream& os, const natural<Int>& c)
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
print(Ostream& os, const natural<Int>& c)
{
    if(isinf(c))
        os << "oo";
    else
        os << "#" << std::intmax_t(c);

    return os;
}





/*****************************************************************************
 *
 * TRAITS SPECIALIZATIONS
 *
 *****************************************************************************/
template<class T>
struct is_number<natural<T>> : std::true_type {};

template<class T>
struct is_number<natural<T>&> : std::true_type {};

template<class T>
struct is_number<natural<T>&&> : std::true_type {};

template<class T>
struct is_number<const natural<T>&> : std::true_type {};

template<class T>
struct is_number<const natural<T>> : std::true_type {};



//-------------------------------------------------------------------
template<class T>
struct is_floating_point<natural<T>> :
    std::integral_constant<bool, is_floating_point<T>::value>
{};



//-------------------------------------------------------------------
template<class T, class T2>
struct common_numeric_type<natural<T>,T2>
{
    using type = natural<common_numeric_t<T,T2>>;
};
//---------------------------------------------------------
template<class T, class T2>
struct common_numeric_type<T2,natural<T>>
{
    using type = natural<common_numeric_t<T,T2>>;
};
//---------------------------------------------------------
template<class T1, class T2>
struct common_numeric_type<natural<T1>,natural<T2>>
{
    using type = natural<common_numeric_t<T1,T2>>;
};




/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class IntT>
inline constexpr natural<signed_t<std::decay_t<IntT>>>
make_natural(IntT&& x)
{
    static_assert(is_integral<IntT>::value,
        "make_natural(x): x has to be an integral number");

//    assert(x <= numeric_limits<signed_t<std::decay_t<IntT>>>::max());

    return natural<signed_t<std::decay_t<IntT>>>{make_signed(std::forward<IntT>(x))};
}



namespace literals {

//-------------------------------------------------------------------
constexpr natural<long long int>
operator "" _n (unsigned long long int x)
{
    return (x > static_cast<unsigned long long int>(
            std::numeric_limits<long long int>::max()))
               ? natural<long long int>::infinity()
               : natural<long long int>(static_cast<long long int>(x));
}

} // namespace literals


}  // namespace num
}  // namespace am


namespace std {

/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class T>
class numeric_limits<am::num::natural<T>>
{
    using val_t = am::num::natural<T>;

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
    epsilon() noexcept { return std::numeric_limits<T>::epsilon(); }

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


} // namespace std

#endif
