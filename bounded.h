#ifndef AM_NUMERIC_BOUNDED_H_
#define AM_NUMERIC_BOUNDED_H_


#include <cmath>
#include <cstdint>
#include <cfloat>
#include <iostream>
#include <sstream>

#include "interval.h"
#include "equality.h"


namespace am {
namespace num {



/*****************************************************************************
 *
 *
 * BOUNDING POLICIES
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
struct silent_clip
{
    template<class T>
    constexpr T
    operator () (T x, T min, T max) noexcept
    {
        return (x < min ? min : x > max ? max : x);
    }
};

//-------------------------------------------------------------------
struct clip_and_report
{
    template<class T>
    T
    operator () (T x, T min, T max) noexcept
    {
        if(x < min) {
            std::cerr << x << " below [" << min << ',' << max << "]\n";
            return min;
        }
        else if(x > max) {
            std::cerr << x << " above [" << min << ',' << max << "]\n";
            return max;
        }
        return x;
    }
};

//-------------------------------------------------------------------
struct throw_if_out_of_bounds
{
    template<class T>
    T
    operator () (T x, const T& min, const T& max)
    {
        if(x < min) {
            std::ostringstream os;
            os << x << " below [" << min << ',' << max << "]\n";
            throw std::out_of_range(os.str());
        }
        else if(x > max) {
            std::ostringstream os;
            os << x << " above [" << min << ',' << max << "]\n";
            throw std::out_of_range(os.str());
        }
        return x;
    }
};








/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class,class,class> class bounded;

template<class>
struct is_bounded :
    std::false_type
{};

template<class T, class B, class P>
struct is_bounded<bounded<T,B,P>> :
    std::true_type
{};






/*****************************************************************************
 *
 * @brief  represents a number that is restricted to an interval
 *
 * @tparam NumberType        value type
 * @tparam BoundingInterval  must model the num::Interval concept
 * @tparam BoundingPolicy    action to take if value has to be bounded
 *
 * @details
 * inherits from bounding interval to make use of emtpy base class optimization
 *
 *****************************************************************************/
template<
    class NumberType,
    class BoundingInterval,
    class BoundingPolicy = silent_clip
>
class bounded :
    private BoundingInterval,
    private BoundingPolicy
{
public:

    static_assert(is_number<NumberType>::value,
        "bounded<T,B,P>: T must be a number type");

    static_assert(!is_bounded<NumberType>::value,
        "bounded<T,B,P>: T must not be a bounded<> type itself");


    //---------------------------------------------------------------
    using value_type      = NumberType;
    using numeric_type    = value_type;
    using interval_type   = BoundingInterval;
    using bounding_policy = BoundingPolicy;


    //-------------------------------------------------------------------
    constexpr value_type
    min() const noexcept {
        using num::min;
        return value_type(min(*static_cast<const interval_type*>(this)));
    }
    //---------------------------------------------------------
    constexpr value_type
    max() const noexcept {
        using num::max;
        return value_type(max(*static_cast<const interval_type*>(this)));
    }


    //---------------------------------------------------------------
    /// @brief default constructor
    constexpr
    bounded() = default;

    //-----------------------------------------------------
    /// @brief
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    explicit constexpr
    bounded(T&& v,
            interval_type b = interval_type(),
            bounding_policy bp = bounding_policy())
    :
        interval_type(std::move(b)), bounding_policy(std::move(bp)),
        v_(get_bounded(value_type(std::forward<T>(v))))
    {
        AM_CHECK_NARROWING(value_type,T)
    }
    //-----------------------------------------------------
    /// @brief
    explicit constexpr
    bounded(interval_type b,
            bounding_policy bp = bounding_policy())
    :
        interval_type(std::move(b)), bounding_policy(std::move(bp)),
        v_(min())
    {}
    //-----------------------------------------------------
    /// @brief
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    explicit constexpr
    bounded(T&& v, bounding_policy bp) :
        interval_type(), bounding_policy(std::move(bp)),
        v_(get_bounded(value_type(std::forward<T>(v))))
    {
        AM_CHECK_NARROWING(value_type,T)
    }
    //-----------------------------------------------------
    /// @brief
    template<class T, class B, class P>
    explicit constexpr
    bounded(const bounded<T,B,P>& v,
            interval_type b,
            bounding_policy bp = bounding_policy())
     :
        interval_type(std::move(b)), bounding_policy(std::move(bp)),
        v_(get_bounded(value_type(v)))
    {
        AM_CHECK_NARROWING(value_type,T)
    }

    //-----------------------------------------------------
    constexpr
    bounded(const bounded&) = default;

    //-----------------------------------------------------
    constexpr
    bounded(bounded&&) = default;

    //-----------------------------------------------------
    template<class T, class B, class P>
    explicit constexpr
    bounded(const bounded<T,B,P>& src):
        interval_type(), bounding_policy(),
        v_(get_bounded(value_type(src)))
    {
        AM_CHECK_NARROWING(value_type,T)
    }
    //-----------------------------------------------------
    template<class T, class B, class P>
    explicit constexpr
    bounded(bounded<T,B,P>&& src):
        interval_type(), bounding_policy(),
        v_(get_bounded(value_type(std::move(src))))
    {
        AM_CHECK_NARROWING(value_type,T)
    }


    //---------------------------------------------------------------
    // ASSIGNMENT
    //---------------------------------------------------------------
    bounded&
    operator = (const bounded&) = default;
    //-----------------------------------------------------
    bounded&
    operator = (bounded&&) = default;

    //-----------------------------------------------------
    template<class T, class B, class P, class = typename std::enable_if<
        is_number<T>::value>::type>
    bounded&
    operator = (const bounded<T,B,P>& b)
    {
        AM_CHECK_NARROWING(value_type,T)

        v_ = get_bounded(value_type(b));

        return *this;
    }
    //-----------------------------------------------------
    template<class T, class B, class P, class = typename std::enable_if<
        is_number<T>::value>::type>
    bounded&
    operator = (bounded<T,B,P>&& b)
    {
        AM_CHECK_NARROWING(value_type,T)

        v_ = get_bounded(value_type(std::move(b)));

        return *this;
    }

    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    bounded&
    operator = (T&& v)
    {
        AM_CHECK_NARROWING(value_type,T)

        v_ = get_bounded(value_type(std::forward<T>(v)));

        return *this;
    }


    //---------------------------------------------------------------
    // ELEMENT ACCESS
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
    // bounded (op)= number
    //---------------------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    bounded&
    operator += (const T& v)
    {
        AM_CHECK_NARROWING(value_type,T)

        v_ = get_bounded(v_ + value_type(v));

        return *this;
    }
    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    bounded&
    operator -= (const T& v)
    {
        AM_CHECK_NARROWING(value_type,T)

        v_ = get_bounded(v_ - value_type(v));

        return *this;
    }
    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    bounded&
    operator *= (const T& v)
    {
        AM_CHECK_NARROWING(value_type,T)

        v_ = get_bounded(v_ * value_type(v));

        return *this;
    }
    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<T>::value>::type>
    bounded&
    operator /= (const T& v)
    {
        AM_CHECK_NARROWING(value_type,T)

        v_ = get_bounded(v_ /= value_type(v));

        return *this;
    }


    //---------------------------------------------------------------
    // increment / decrement
    //---------------------------------------------------------------
    bounded&
    operator ++ ()
    {
        v_ = get_bounded(v_ + 1);
        return *this;
    }
    //-----------------------------------------------------
    bounded&
    operator -- ()
    {
        v_ = get_bounded(v_ - 1);
        return *this;
    }

    //-----------------------------------------------------
    bounded
    operator ++ (int)
    {
        auto old = *this;
        ++*this;
        return old;
    }
    //-----------------------------------------------------
    bounded
    operator -- (int)
    {
        auto old = *this;
        --*this;
        return old;
    }


    //---------------------------------------------------------------
    // bounded (op)= bounded with different value_type
    //---------------------------------------------------------------
    template<class T, class B, class P>
    bounded&
    operator += (const bounded<T,B,P>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

        v_ = get_bounded(v_ + o.value());

        return *this;
    }
    //-----------------------------------------------------
    template<class T, class B, class P>
    bounded&
    operator -= (const bounded<T,B,P>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

        v_ = get_bounded(v_ - o.value());

        return *this;
    }
    //-----------------------------------------------------
    template<class T, class B, class P>
    bounded&
    operator *= (const bounded<T,B,P>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

        v_ = get_bounded(v_ * o.value());

        return *this;
    }
    //-----------------------------------------------------
    template<class T, class B, class P>
    bounded&
    operator /= (const bounded<T,B,P>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

        v_ = get_bounded(v_ / o.value());

        return *this;
    }
    //-----------------------------------------------------
    template<class T, class B, class P>
    bounded&
    operator %= (const bounded<T,B,P>& o)
    {
        AM_CHECK_NARROWING(value_type,T)

        v_ = get_bounded(v_ % o.value());

        return *this;
    }


private:
    value_type
    get_bounded(value_type v) {
        return bounding_policy::operator()(std::move(v), min(), max());
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
using unit_bounded = bounded<T,unit_interval<T>>;


//-------------------------------------------------------------------
template<class T>
using symunit_bounded = bounded<T,symmetric_unit_interval<T>>;


//-------------------------------------------------------------------
template<class T>
using interval_bounded = bounded<T,interval<T>>;






/*****************************************************************************
 *
 *
 * FACTORIES
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T, class = typename std::enable_if<
    is_number<decay_t<T>>::value &&
    !is_bounded<decay_t<T>>::value>::type>
inline constexpr
unit_bounded<decay_t<T>>
make_unit_bounded(T&& x)
{
    return unit_bounded<decay_t<T>>{std::forward<T>(x)};
}

//---------------------------------------------------------
template<class T, class B, class P>
inline constexpr
unit_bounded<T>
make_unit_bounded(bounded<T,B,P> x)
{
    return unit_bounded<T>{std::move(x)};
}



//-------------------------------------------------------------------
template<class T, class = typename std::enable_if<
    is_number<decay_t<T>>::value &&
    !is_bounded<decay_t<T>>::value>::type>
inline constexpr
symunit_bounded<decay_t<T>>
make_symunit_bounded(T&& x)
{
    return symunit_bounded<decay_t<T>>{std::forward<T>(x)};
}

//---------------------------------------------------------
template<class T, class B, class P>
inline constexpr
symunit_bounded<T>
make_symunit_bounded(bounded<T,B,P> x)
{
    return symunit_bounded<T>{std::move(x)};
}



//-------------------------------------------------------------------
template<class T, class = typename std::enable_if<
    is_number<decay_t<T>>::value &&
    !is_bounded<decay_t<T>>::value>::type>
inline constexpr
interval_bounded<decay_t<T>>
make_bounded(T&& x)
{
    return interval_bounded<decay_t<T>>{std::forward<T>(x)};
}

//---------------------------------------------------------
template<class T1, class T2, class = typename std::enable_if<
    is_number<decay_t<T1>>::value &&
    !is_bounded<decay_t<T1>>::value>::type>
inline constexpr
interval_bounded<common_numeric_t<decay_t<T1>,T2>>
make_bounded(T1&& x, interval<T2> bounds)
{
    return interval_bounded<common_numeric_t<decay_t<T1>,T2>>{
        std::forward<T1>(x), std::move(bounds)};
}



//-------------------------------------------------------------------
template<class T, class B, class P>
inline constexpr bounded<T,B,P>
make_bounded(const bounded<T,B,P>& x)
{
    return x;
}
//---------------------------------------------------------
template<class T, class B, class P>
inline constexpr bounded<T,B,P>&&
make_bounded(bounded<T,B,P>&& x)
{
    return std::move(x);
}

//---------------------------------------------------------
template<class T1, class B, class P, class T2>
inline constexpr
interval_bounded<common_numeric_t<T1,T2>>
make_bounded(const bounded<T1,B,P>& x, interval<T2> bounds)
{
    return interval_bounded<common_numeric_t<T1,T2>>{
        common_numeric_t<T1,T2>(x), std::move(bounds)};
}

//---------------------------------------------------------
template<class T1, class B, class P, class T2>
inline constexpr
unit_bounded<common_numeric_t<T1,T2>>
make_bounded(const bounded<T1,B,P>& x, const unit_interval<T2>&)
{
    return unit_bounded<common_numeric_t<T1,T2>>{
        common_numeric_t<T1,T2>(x)};
}

//---------------------------------------------------------
template<class T1, class B, class P, class T2>
inline constexpr
symunit_bounded<common_numeric_t<T1,T2>>
make_bounded(const bounded<T1,B,P>& x, const symmetric_unit_interval<T2>&)
{
    return symunit_bounded<common_numeric_t<T1,T2>>{
        common_numeric_t<T1,T2>(x)};
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
template<class Istream, class T, class B, class P>
inline Istream&
operator >> (Istream& is, bounded<T,B,P>& x)
{
    T v;
    is >> v;
    x = v;
    return is;
}

//---------------------------------------------------------
template<class Ostream, class T, class B, class P>
inline Ostream&
operator << (Ostream& os, const bounded<T,B,P>& x)
{
    return (os << x.value());
}

//---------------------------------------------------------
template<class Ostream, class T, class B, class P>
inline Ostream&
print(Ostream& os, const bounded<T,B,P>& x)
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
template<class T1, class B1, class P1, class T2, class B2, class P2>
inline bool
operator == (const bounded<T1,B1,P1>& a, const bounded<T2,B2,P2>& b)
{
    return (a.value() == b.value());
}

//---------------------------------------------------------
template<class T1, class B1, class P1, class T2, class B2, class P2>
inline bool
operator != (const bounded<T1,B1,P1>& a, const bounded<T2,B2,P2>& b)
{
    return (a.value() != b.value());
}


//-------------------------------------------------------------------
template<class T1, class B1, class P1, class T2, class B2, class P2,
    class T3 = common_numeric_t<T1,T2>>
inline constexpr bool
approx_equal(const bounded<T1,B1,P1>& a, const bounded<T2,B2,P2>& b,
    const T3& tol = tolerance<T3>::value())
{
    return approx_equal(a.value(), b.value(), tol);
}

//---------------------------------------------------------
template<class T, class B, class P>
inline constexpr bool
approx_1(const bounded<T,B,P>& x, const T& tol = tolerance<T>::value())
{
    return approx_1(x.value(), tol);
}

//---------------------------------------------------------
template<class T, class B, class P>
inline constexpr bool
approx_0(const bounded<T,B,P>& x, const T& tol = tolerance<T>::value())
{
    return approx_0(x.value(), tol);
}



//-------------------------------------------------------------------
// COMPARISON WITH INTEGERS OR REAL NUMBERS
//-------------------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value>::type>
inline bool
operator > (const bounded<T1,B,P>& x, const T2& r)
{
    return (x.value() > r);
}
//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value>::type>
inline bool
operator > (const T2& r, const bounded<T1,B,P>& x)
{
    return (r > x.value());
}

//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value>::type>
inline bool
operator >= (const bounded<T1,B,P>& x, const T2& r)
{
    return (x.value() >= r);
}
//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value>::type>
inline bool
operator >= (const T2& r, const bounded<T1,B,P>& x)
{
    return (r >= x.value());
}

//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value>::type>
inline bool
operator < (const bounded<T1,B,P>& x, const T2& r)
{
    return (x.value() < r);
}
//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value>::type>
inline bool
operator < (const T2& r, const bounded<T1,B,P>& x)
{
    return (r < x.value());
}

//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value>::type>
inline bool
operator <= (const bounded<T1,B,P>& x, const T2& r)
{
    return (x.value() <= r);
}
//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value>::type>
inline bool
operator <= (const T2& r, const bounded<T1,B,P>& x)
{
    return (r <= x.value());
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
template<class T1, class B1, class P1, class T2, class B2, class P2>
inline constexpr
common_numeric_t<bounded<T1,B1,P1>, bounded<T2,B2,P2>>
operator + (const bounded<T1,B1,P1> x, const bounded<T2,B2,P2>& y)
{
    return common_numeric_t<bounded<T1,B1,P1>, bounded<T2,B2,P2>>
               {x.value() + y.value()};
}

//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<std::is_arithmetic<T2>::value>::type>
inline constexpr
bounded<common_numeric_t<T1,T2>,B,P>
operator + (const bounded<T1,B,P> x, const T2& y)
{
    return bounded<common_numeric_t<T1,T2>,B,P>{x.value() + y};
}
//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<std::is_arithmetic<T2>::value>::type>
inline constexpr
bounded<common_numeric_t<T1,T2>,B,P>
operator + (const T2& y, const bounded<T1,B,P> x)
{
    return bounded<common_numeric_t<T1,T2>,B,P>{y + x.value()};
}

//---------------------------------------------------------
template<class T, class B, class P>
inline constexpr
bounded<T,B,P>
operator + (const bounded<T,B,P> x, const T& y)
{
    return bounded<T,B,P>{x.value() + y};
}
//---------------------------------------------------------
template<class T, class B, class P>
inline constexpr
bounded<T,B,P>
operator + (const T& y, const bounded<T,B,P> x)
{
    return bounded<T,B,P>{y + x.value()};
}



//-------------------------------------------------------------------
// SUBTRACTION
//-------------------------------------------------------------------
template<class T1, class B1, class P1, class T2, class B2, class P2>
inline constexpr
common_numeric_t<bounded<T1,B1,P1>, bounded<T2,B2,P2>>
operator - (const bounded<T1,B1,P1> x, const bounded<T2,B2,P2>& y)
{
    return common_numeric_t<bounded<T1,B1,P1>, bounded<T2,B2,P2>>
               {x.value() - y.value()};
}

//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<std::is_arithmetic<T2>::value>::type>
inline constexpr
bounded<common_numeric_t<T1,T2>,B,P>
operator - (const bounded<T1,B,P> x, const T2& y)
{
    return bounded<common_numeric_t<T1,T2>,B,P>{x.value() - y};
}
//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<std::is_arithmetic<T2>::value>::type>
inline constexpr
bounded<common_numeric_t<T1,T2>,B,P>
operator - (const T2& y, const bounded<T1,B,P> x)
{
    return bounded<common_numeric_t<T1,T2>,B,P>{y - x.value()};
}

//---------------------------------------------------------
template<class T, class B, class P>
inline constexpr
bounded<T,B,P>
operator - (const bounded<T,B,P> x, const T& y)
{
    return bounded<T,B,P>{x.value() - y};
}
//---------------------------------------------------------
template<class T, class B, class P>
inline constexpr
bounded<T,B,P>
operator - (const T& y, const bounded<T,B,P> x)
{
    return bounded<T,B,P>{y - x.value()};
}



//-------------------------------------------------------------------
// MULTIPLIPCATION
//-------------------------------------------------------------------
template<class T1, class B1, class P1, class T2, class B2, class P2>
inline constexpr
common_numeric_t<bounded<T1,B1,P1>, bounded<T2,B2,P2>>
operator * (const bounded<T1,B1,P1> x, const bounded<T2,B2,P2>& y)
{
    return common_numeric_t<bounded<T1,B1,P1>, bounded<T2,B2,P2>>
               {x.value() * y.value()};
}

//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<std::is_arithmetic<T2>::value>::type>
inline constexpr
bounded<common_numeric_t<T1,T2>,B,P>
operator * (const bounded<T1,B,P> x, const T2& y)
{
    return bounded<common_numeric_t<T1,T2>,B,P>{x.value() * y};
}
//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<std::is_arithmetic<T2>::value>::type>
inline constexpr
bounded<common_numeric_t<T1,T2>,B,P>
operator * (const T2& y, const bounded<T1,B,P> x)
{
    return bounded<common_numeric_t<T1,T2>,B,P>{y * x.value()};
}

//---------------------------------------------------------
template<class T, class B, class P>
inline constexpr
bounded<T,B,P>
operator * (const bounded<T,B,P> x, const T& y)
{
    return bounded<T,B,P>{x.value() * y};
}
//---------------------------------------------------------
template<class T, class B, class P>
inline constexpr
bounded<T,B,P>
operator * (const T& y, const bounded<T,B,P> x)
{
    return bounded<T,B,P>{y * x.value()};
}



//-------------------------------------------------------------------
// DIVISION
//-------------------------------------------------------------------
template<class T1, class B1, class P1, class T2, class B2, class P2>
inline constexpr
common_numeric_t<bounded<T1,B1,P1>, bounded<T2,B2,P2>>
operator / (const bounded<T1,B1,P1> x, const bounded<T2,B2,P2>& y)
{
    return common_numeric_t<bounded<T1,B1,P1>, bounded<T2,B2,P2>>
               {x.value() / y.value()};
}

//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<std::is_arithmetic<T2>::value>::type>
inline constexpr
bounded<common_numeric_t<T1,T2>,B,P>
operator / (const bounded<T1,B,P> x, const T2& y)
{
    return bounded<common_numeric_t<T1,T2>,B,P>{x.value() / y};
}
//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<std::is_arithmetic<T2>::value>::type>
inline constexpr
bounded<common_numeric_t<T1,T2>,B,P>
operator / (const T2& y, const bounded<T1,B,P> x)
{
    return bounded<common_numeric_t<T1,T2>,B,P>{y / x.value()};
}

//---------------------------------------------------------
template<class T, class B, class P>
inline constexpr
bounded<T,B,P>
operator / (const bounded<T,B,P> x, const T& y)
{
    return bounded<T,B,P>{x.value() / y};
}
//---------------------------------------------------------
template<class T, class B, class P>
inline constexpr
bounded<T,B,P>
operator / (const T& y, const bounded<T,B,P> x)
{
    return bounded<T,B,P>{y / x.value()};
}



//-------------------------------------------------------------------
// MODULO
//-------------------------------------------------------------------
template<class T1, class B1, class P1, class T2, class B2, class P2>
inline constexpr
common_numeric_t<bounded<T1,B1,P1>, bounded<T2,B2,P2>>
operator % (const bounded<T1,B1,P1> x, const bounded<T2,B2,P2>& y)
{
    return common_numeric_t<bounded<T1,B1,P1>, bounded<T2,B2,P2>>
               {x.value() % y.value()};
}

//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<std::is_arithmetic<T2>::value>::type>
inline constexpr
bounded<common_numeric_t<T1,T2>,B,P>
operator % (const bounded<T1,B,P> x, const T2& y)
{
    return bounded<common_numeric_t<T1,T2>,B,P>{x.value() % y};
}
//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<std::is_arithmetic<T2>::value>::type>
inline constexpr
bounded<common_numeric_t<T1,T2>,B,P>
operator % (const T2& y, const bounded<T1,B,P> x)
{
    return bounded<common_numeric_t<T1,T2>,B,P>{y % x.value()};
}

//---------------------------------------------------------
template<class T, class B, class P>
inline constexpr
bounded<T,B,P>
operator % (const bounded<T,B,P> x, const T& y)
{
    return bounded<T,B,P>{x.value() % y};
}
//---------------------------------------------------------
template<class T, class B, class P>
inline constexpr
bounded<T,B,P>
operator % (const T& y, const bounded<T,B,P> x)
{
    return bounded<T,B,P>{y % x.value()};
}



//-------------------------------------------------------------------
// INVERSION
//-------------------------------------------------------------------
template<class T, class B, class P>
inline constexpr
bounded<T,B,P>
operator - (const bounded<T,B,P> x)
{
    return bounded<T,B,P>{-x.value()};
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
template<class T, class B, class P>
inline bool
isfinite(const bounded<T,B,P>& x)
{
    using std::isfinite;
    return isfinite(x.value());
}

//---------------------------------------------------------
template<class T, class B, class P>
inline bool
isinf(const bounded<T,B,P>& x)
{
    using std::isinf;
    return isinf(x.value());
}

//---------------------------------------------------------
template<class T, class B, class P>
inline bool
isnan(const bounded<T,B,P>& x)
{
    using std::isnan;
    return isnan(x.value());
}

//---------------------------------------------------------
template<class T, class B, class P>
inline bool
isnormal(const bounded<T,B,P>& x)
{
    using std::isnormal;
    return isnormal(x.value());
}

//---------------------------------------------------------
template<class T, class B, class P>
inline bounded<T,B,P>
abs(const bounded<T,B,P>& x)
{
    using std::abs;
    return bounded<T,B,P>(abs(x.value()));
}








/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class T, class B, class P>
class numeric_limits<bounded<T,B,P>>
{
    using val_t = bounded<T,B,P>;

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
    infinity() noexcept {return numeric_limits<T>::infinity(); }

    static constexpr val_t
    quiet_NaN() noexcept {return numeric_limits<T>::quiet_NaN(); }

    static constexpr val_t
    signaling_NaN() noexcept {return numeric_limits<T>::signaling_NaN(); }

    static constexpr val_t
    denorm_min() noexcept {return numeric_limits<T>::denorm_min(); }

    static constexpr bool is_iec559 = numeric_limits<T>::is_iec559;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = numeric_limits<T>::is_modulo;

    static constexpr bool traps = numeric_limits<T>::traps;
    static constexpr bool tinyness_before = numeric_limits<T>::tinyness_before;
    static constexpr std::float_round_style round_style = numeric_limits<T>::round_style;
};






/*****************************************************************************
 *
 * DEFAULT COMMON BOUNDS
 *
 *****************************************************************************/

//-------------------------------------------------------------------
/// @brief default behaviour: widen bounds
//-------------------------------------------------------------------
template<class B1, class B2>
class common_bounds
{

    using res_t = common_numeric_t<
                      decay_t<decltype(min(std::declval<B1>()))>,
                      decay_t<decltype(min(std::declval<B2>()))>,
                      decay_t<decltype(max(std::declval<B1>()))>,
                      decay_t<decltype(max(std::declval<B2>()))> >;

    struct not_same_t :
        private B1,
        private B2
    {
        not_same_t() = default;

        template<class...Args>
        not_same_t(Args&&... args):
            B1(std::forward<Args>(args)...),
            B2(std::forward<Args>(args)...)
        {}

        constexpr res_t
        min() noexcept {
            return res_t(B1::min()) < res_t(B2::min())
                       ? res_t(B1::min())
                       : res_t(B2::min());
        }

        constexpr res_t
        max() noexcept {
            return res_t(B1::max()) > res_t(B2::max())
                       ? res_t(B1::max())
                       : res_t(B2::max());
        }

        inline friend constexpr res_t
        min(const not_same_t& x) noexcept {return x.min(); }

        inline friend constexpr res_t
        max(const not_same_t& x) noexcept {return x.max(); }
    };

public:
    using type = typename std::conditional<
                    std::is_same<B1,B2>::value, B1, not_same_t>::type;

};



//-------------------------------------------------------------------
template<class B1, class B2>
using common_bounds_t = typename common_bounds<B1,B2>::type;






/*****************************************************************************
 *
 * DEFAULT COMMON BOUNDING POLICY
 *
 *****************************************************************************/

//-------------------------------------------------------------------
/// @brief default behaviour: use the 'mightier' policy
//-------------------------------------------------------------------
template<class, class>
struct common_bounding_policy {};

//---------------------------------------------------------
template<class P>
struct common_bounding_policy<P,P> {
    using type = P;
};

//---------------------------------------------------------
template<>
struct common_bounding_policy<clip_and_report,silent_clip> {
    using type = clip_and_report;
};

//---------------------------------------------------------
template<>
struct common_bounding_policy<silent_clip,clip_and_report> {
    using type = clip_and_report;
};

//---------------------------------------------------------
template<>
struct common_bounding_policy<silent_clip,throw_if_out_of_bounds> {
    using type = throw_if_out_of_bounds;
};

//---------------------------------------------------------
template<>
struct common_bounding_policy<throw_if_out_of_bounds,silent_clip> {
    using type = throw_if_out_of_bounds;
};

//---------------------------------------------------------
template<>
struct common_bounding_policy<clip_and_report,throw_if_out_of_bounds> {
    using type = throw_if_out_of_bounds;
};

//---------------------------------------------------------
template<>
struct common_bounding_policy<throw_if_out_of_bounds,clip_and_report> {
    using type = throw_if_out_of_bounds;
};



//-------------------------------------------------------------------
template<class P1, class P2>
using common_bounding_policy_t = typename common_bounding_policy<P1,P2>::type;






/*****************************************************************************
 *
 *
 * TRAITS SPECIALIZATIONS
 *
 *
 *****************************************************************************/


//-------------------------------------------------------------------
template<class T, class B, class P>
struct is_number<bounded<T,B,P>> : std::true_type {};

template<class T, class B, class P>
struct is_number<bounded<T,B,P>&> : std::true_type {};

template<class T, class B, class P>
struct is_number<bounded<T,B,P>&&> : std::true_type {};

template<class T, class B, class P>
struct is_number<const bounded<T,B,P>&> : std::true_type {};

template<class T, class B, class P>
struct is_number<const bounded<T,B,P>> : std::true_type {};



//-------------------------------------------------------------------
template<class T, class B, class P>
struct is_floating_point<bounded<T,B,P>> :
    std::integral_constant<bool, is_floating_point<T>::value>
{};



//-------------------------------------------------------------------
template<class T1, class B, class P, class T2>
struct common_numeric_type<bounded<T1,B,P>,T2>
{
    using type = bounded<common_numeric_t<T1,T2>,B,P>;
};
//---------------------------------------------------------
template<class T1, class B, class P, class T2>
struct common_numeric_type<T2,bounded<T1,B,P>>
{
    using type = bounded<common_numeric_t<T1,T2>,B,P>;
};
//---------------------------------------------------------
/// @brief
template<class T1, class B1, class P1, class T2, class B2, class P2>
struct common_numeric_type<bounded<T1,B1,P1>,bounded<T2,B2,P2>>
{
    using type = bounded<
        common_numeric_t<T1,T2>,
        common_bounds_t<B1,B2>,
        common_bounding_policy_t<P1,P2>>;
};


namespace detail {

//-------------------------------------------------------------------
template<class To, class B, class P, class From>
struct is_non_narrowing_helper<true, bounded<To,B,P>, From> :
    public is_non_narrowing_helper<true,To,From>
{};

//---------------------------------------------------------
template<class To, class B1, class P1, class From, class B2, class P2>
struct is_non_narrowing_helper<true, bounded<To,B1,P1>, bounded<From,B2,P2> > :
    public is_non_narrowing_helper<true,To,From>
{};

//---------------------------------------------------------
template<class To, class From, class B, class P>
struct is_non_narrowing_helper<true, To, bounded<From,B,P> > :
    public is_non_narrowing_helper<true,To,From>
{};


}  // namespace detail



}  // namespace num
}  // namespace am


#endif
