/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/

#ifndef AMLIB_NUMERIC_BOUNDED_H_
#define AMLIB_NUMERIC_BOUNDED_H_


#include <cmath>
#include <cstdint>
#include <cfloat>
#include <iostream>

#include "interval.h"
#include "equality.h"


namespace am {
namespace num {



/*****************************************************************************
 *
 * @brief silently cut off at interval bounds
 *        no check for narrowing!
 *
 *****************************************************************************/
struct silent_clip
{
    template<class In, class Tgt>
    constexpr
    Tgt operator () (In x, Tgt min, Tgt max) const noexcept
    {
        return (Tgt(x) < min ? std::move(min)
                             : (Tgt(x) > max ? std::move(max) : Tgt(x)));
    }
};




/*****************************************************************************
 *
 * @brief silently wrap around ("modulo")
 *        no check for narrowing!
 *
 *****************************************************************************/
struct silent_wrap
{
    //this relies on max beeing greater than min
    template<class In, class Tgt>
    Tgt operator () (In x, const Tgt& min, const Tgt& max) const {
        using std::fmod;

        auto res = Tgt(x);
        if(res < min || res > max) {
            return (fmod(res - min, max-min) + min);
        }

        return res;
    }
};




/*****************************************************************************
 *
 * @brief clip off and report each clipping on stderr
 *
 *****************************************************************************/
struct clip_and_report
{
    template<class In, class Tgt>
    Tgt operator () (In x, Tgt min, Tgt max) const noexcept
    {
        AM_CHECK_NARROWING(Tgt,In);

        auto res = Tgt(std::move(x));
        if(res < min) {
            std::cerr << res << " below [" << min << ',' << max << "]\n";
            return Tgt(std::move(min));
        }
        else if(res > max) {
            std::cerr << res << " above [" << min << ',' << max << "]\n";
            return Tgt(std::move(max));
        }
        return res;
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
    class BoundingPolicy
>
class bounded :
    private BoundingInterval,
    private BoundingPolicy
{
public:

    static_assert(is_number<NumberType>::value,
        "bounded<T,I,P>: T must be a number type");

    static_assert(!is_bounded<NumberType>::value,
        "bounded<T,I,P>: T must not be a bounded<> type itself");

    static_assert(is_interval<BoundingInterval>::value,
        "bounded<T,I,P>: I must be an interval (concept num::Interval)");


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
        !is_bounded<decay_t<T>>::value &&
        is_number<decay_t<T>>::value>::type>
    constexpr
    bounded(T&& v,
            interval_type ival = interval_type(),
            bounding_policy bp = bounding_policy())
    :
        interval_type(ival), bounding_policy(bp),
        v_(get_bounded(std::forward<T>(v), ival, bp))
    { }
    //-----------------------------------------------------
    /// @brief
    explicit constexpr
    bounded(interval_type ival,
            bounding_policy bp = bounding_policy())
    :
        interval_type(std::move(ival)), bounding_policy(std::move(bp)),
        v_(min(ival))
    {}
    //-----------------------------------------------------
    /// @brief
    template<class T, class B, class P>
    explicit constexpr
    bounded(const bounded<T,B,P>& v,
            interval_type ival,
            bounding_policy bp = bounding_policy())
     :
        interval_type(ival), bounding_policy(bp),
        v_(get_bounded(v, ival, bp))
    { }

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
        v_(get_bounded(src, interval_type(), bounding_policy()))
    { }
    //-----------------------------------------------------
    template<class T, class B, class P>
    explicit constexpr
    bounded(bounded<T,B,P>&& src):
        interval_type(), bounding_policy(),
        v_(get_bounded(std::move(src), interval_type(), bounding_policy()))
    { }


    //---------------------------------------------------------------
    // ASSIGNMENT
    //---------------------------------------------------------------
    bounded&
    operator = (const bounded&) = default;
    //-----------------------------------------------------
    bounded&
    operator = (bounded&& src)
    {
        interval_type::operator = (src);
        bounding_policy::operator = (src);
        v_ = std::move(src.v_);
        return *this;
    }


    //-----------------------------------------------------
    template<class T, class B, class P>
    bounded&
    operator = (const bounded<T,B,P>& b)
    {
        v_ = get_bounded(b);

        return *this;
    }
    //-----------------------------------------------------
    template<class T, class B, class P>
    bounded&
    operator = (bounded<T,B,P>&& b)
    {
        v_ = get_bounded(std::move(b));

        return *this;
    }

    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        !is_bounded<decay_t<T>>::value &&
        is_number<decay_t<T>>::value>::type>
    bounded&
    operator = (T&& v)
    {
        v_ = get_bounded(std::forward<T>(v));

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
    operator const value_type& () const noexcept
    {
        return v_;
    }
    //-----------------------------------------------------
//    constexpr
//    operator const value_type& () const & noexcept
//    {
//        return v_;
//    }
//    //-----------------------------------------------------
//    constexpr
//    operator value_type&& () && noexcept
//    {
//        return std::move(v_);
//    }


    //---------------------------------------------------------------
    // bounded (op)= number
    //---------------------------------------------------------------
    template<class T, class = typename std::enable_if<
        !is_bounded<T>::value && is_number<T>::value>::type>
    bounded&
    operator += (const T& v)
    {
        v_ = get_bounded(v_ + v);

        return *this;
    }
    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        !is_bounded<T>::value && is_number<T>::value>::type>
    bounded&
    operator -= (const T& v)
    {
        v_ = get_bounded(v_ - v);

        return *this;
    }
    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        !is_bounded<T>::value && is_number<T>::value>::type>
    bounded&
    operator *= (const T& v)
    {
        v_ = get_bounded(v_ * v);

        return *this;
    }
    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        !is_bounded<T>::value && is_number<T>::value>::type>
    bounded&
    operator /= (const T& v)
    {
        v_ = get_bounded(v_ /= v);

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
        v_ = get_bounded(v_ + o.value());

        return *this;
    }
    //-----------------------------------------------------
    template<class T, class B, class P>
    bounded&
    operator -= (const bounded<T,B,P>& o)
    {
        v_ = get_bounded(v_ - o.value());

        return *this;
    }
    //-----------------------------------------------------
    template<class T, class B, class P>
    bounded&
    operator *= (const bounded<T,B,P>& o)
    {
        v_ = get_bounded(v_ * o.value());

        return *this;
    }
    //-----------------------------------------------------
    template<class T, class B, class P>
    bounded&
    operator /= (const bounded<T,B,P>& o)
    {
        v_ = get_bounded(v_ / o.value());

        return *this;
    }
    //-----------------------------------------------------
    template<class T, class B, class P>
    bounded&
    operator %= (const bounded<T,B,P>& o)
    {
        v_ = get_bounded(v_ % o.value());

        return *this;
    }


private:
    //---------------------------------------------------------------
    template<class T>
    constexpr value_type
    get_bounded(T v) const noexcept {
        return bounding_policy::operator()(std::move(v), min(), max());
    }

    //-----------------------------------------------------
    /// @brief needed for constexpr construction
    template<class T>
    static constexpr value_type
    get_bounded(T v, const interval_type& i, const bounding_policy& p) noexcept
    {
        using num::min;
        using num::max;
        return p(std::move(v), min(i), max(i));
    }

    value_type v_;

};






/*****************************************************************************
 *
 * @brief numbers that are clipped on the interval bounds
 *
 *
 *
 *****************************************************************************/
template<class NumericT, class BoundingInterval>
using clipped = bounded<NumericT,BoundingInterval,silent_clip>;


//-------------------------------------------------------------------
template<class NumericT, long long int left, long long int right>
using static_clipped = clipped<NumericT,static_interval<NumericT,left,right>>;


//-------------------------------------------------------------------
template<class T>
using unit_clipped = clipped<T,unit_interval<T>>;

//-------------------------------------------------------------------
template<class T>
using symunit_clipped = clipped<T,symmetric_unit_interval<T>>;

//-------------------------------------------------------------------
template<class T>
using interval_clipped = clipped<T,interval<T>>;



//-------------------------------------------------------------------
template<class T, class = typename std::enable_if<
    is_number<decay_t<T>>::value &&
    !is_bounded<decay_t<T>>::value>::type>
inline constexpr
unit_clipped<decay_t<T>>
make_unit_clipped(T&& x)
{
    return unit_clipped<decay_t<T>>{std::forward<T>(x)};
}



//-------------------------------------------------------------------
template<class T, class = typename std::enable_if<
    is_number<decay_t<T>>::value &&
    !is_bounded<decay_t<T>>::value>::type>
inline constexpr
symunit_clipped<decay_t<T>>
make_symunit_clipped(T&& x)
{
    return symunit_clipped<decay_t<T>>{std::forward<T>(x)};
}



//-------------------------------------------------------------------
template<class T, class = typename std::enable_if<
    is_number<decay_t<T>>::value &&
    !is_bounded<decay_t<T>>::value>::type>
inline constexpr
interval_clipped<decay_t<T>>
make_clipped(T&& x)
{
    return interval_clipped<decay_t<T>>{std::forward<T>(x)};
}

//---------------------------------------------------------
template<class T1, class T2, class = typename std::enable_if<
    is_number<decay_t<T1>>::value &&
    !is_bounded<decay_t<T1>>::value>::type>
inline constexpr
interval_clipped<common_numeric_t<decay_t<T1>,T2>>
make_clipped(T1&& x, interval<T2> bounds)
{
    return interval_clipped<common_numeric_t<decay_t<T1>,T2>>{
        std::forward<T1>(x), std::move(bounds)};
}



//-------------------------------------------------------------------
template<class T, class B>
inline constexpr clipped<T,B>
make_clipped(const clipped<T,B>& x)
{
    return x;
}
//---------------------------------------------------------
template<class T, class B>
inline constexpr clipped<T,B>&&
make_clipped(clipped<T,B>&& x)
{
    return std::move(x);
}

//---------------------------------------------------------
template<class T1, class B, class T2>
inline constexpr
interval_clipped<common_numeric_t<T1,T2>>
make_clipped(const clipped<T1,B>& x, interval<T2> bounds)
{
    return interval_clipped<common_numeric_t<T1,T2>>{
        common_numeric_t<T1,T2>(x), std::move(bounds)};
}

//---------------------------------------------------------
template<class T1, class B, class T2>
inline constexpr
unit_clipped<common_numeric_t<T1,T2>>
make_clipped(const clipped<T1,B>& x, const unit_interval<T2>&)
{
    return unit_clipped<common_numeric_t<T1,T2>>{
        common_numeric_t<T1,T2>(x)};
}

//---------------------------------------------------------
template<class T1, class B, class T2>
inline constexpr
symunit_clipped<common_numeric_t<T1,T2>>
make_clipped(const clipped<T1,B>& x, const symmetric_unit_interval<T2>&)
{
    return symunit_clipped<common_numeric_t<T1,T2>>{
        common_numeric_t<T1,T2>(x)};
}








/*****************************************************************************
 *
 * @brief numbers that are wrapped around on the interval bounds
 *
 *
 *
 *****************************************************************************/
template<class NumericT, class BoundingInterval>
using wrapped = bounded<NumericT,BoundingInterval,silent_wrap>;


//-------------------------------------------------------------------
template<class NumericT, long long int left, long long int right>
using static_wrapped = wrapped<NumericT,static_interval<NumericT,left,right>>;


//-------------------------------------------------------------------
template<class T>
using unit_wrapped = wrapped<T,unit_interval<T>>;

//-------------------------------------------------------------------
template<class T>
using symunit_wrapped = wrapped<T,symmetric_unit_interval<T>>;

//-------------------------------------------------------------------
template<class T>
using interval_wrapped = wrapped<T,interval<T>>;



//-------------------------------------------------------------------
template<class T, class = typename std::enable_if<
    is_number<decay_t<T>>::value &&
    !is_bounded<decay_t<T>>::value>::type>
inline constexpr
unit_wrapped<decay_t<T>>
make_unit_wrapped(T&& x)
{
    return unit_wrapped<decay_t<T>>{std::forward<T>(x)};
}



//-------------------------------------------------------------------
template<class T, class = typename std::enable_if<
    is_number<decay_t<T>>::value &&
    !is_bounded<decay_t<T>>::value>::type>
inline constexpr
symunit_wrapped<decay_t<T>>
make_symunit_wrapped(T&& x)
{
    return symunit_wrapped<decay_t<T>>{std::forward<T>(x)};
}



//-------------------------------------------------------------------
template<class T, class = typename std::enable_if<
    is_number<decay_t<T>>::value &&
    !is_bounded<decay_t<T>>::value>::type>
inline constexpr
interval_wrapped<decay_t<T>>
make_wrapped(T&& x)
{
    return interval_wrapped<decay_t<T>>{std::forward<T>(x)};
}

//---------------------------------------------------------
template<class T1, class T2, class = typename std::enable_if<
    is_number<decay_t<T1>>::value &&
    !is_bounded<decay_t<T1>>::value>::type>
inline constexpr
interval_wrapped<common_numeric_t<decay_t<T1>,T2>>
make_wrapped(T1&& x, interval<T2> bounds)
{
    return interval_wrapped<common_numeric_t<decay_t<T1>,T2>>{
        std::forward<T1>(x), std::move(bounds)};
}



//-------------------------------------------------------------------
template<class T, class B>
inline constexpr wrapped<T,B>
make_wrapped(const wrapped<T,B>& x)
{
    return x;
}
//---------------------------------------------------------
template<class T, class B>
inline constexpr wrapped<T,B>&&
make_wrapped(wrapped<T,B>&& x)
{
    return std::move(x);
}

//---------------------------------------------------------
template<class T1, class B, class T2>
inline constexpr
interval_wrapped<common_numeric_t<T1,T2>>
make_wrapped(const wrapped<T1,B>& x, interval<T2> bounds)
{
    return interval_wrapped<common_numeric_t<T1,T2>>{
        common_numeric_t<T1,T2>(x), std::move(bounds)};
}

//---------------------------------------------------------
template<class T1, class B, class T2>
inline constexpr
unit_wrapped<common_numeric_t<T1,T2>>
make_wrapped(const wrapped<T1,B>& x, const unit_interval<T2>&)
{
    return unit_wrapped<common_numeric_t<T1,T2>>{
        common_numeric_t<T1,T2>(x)};
}

//---------------------------------------------------------
template<class T1, class B, class T2>
inline constexpr
symunit_wrapped<common_numeric_t<T1,T2>>
make_wrapped(const wrapped<T1,B>& x, const symmetric_unit_interval<T2>&)
{
    return symunit_wrapped<common_numeric_t<T1,T2>>{
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
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_bounded<T2>::value>::type>
inline bool
operator == (const bounded<T1,B,P>& x, const T2& r)
{
    return (x.value() == r);
}
//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_bounded<T2>::value>::type>
inline bool
operator == (const T2& r, const bounded<T1,B,P>& x)
{
    return (r == x.value());
}
//---------------------------------------------------------
template<class T1, class B1, class P1, class T2, class B2, class P2>
inline bool
operator == (const bounded<T1,B1,P1>& a, const bounded<T2,B2,P2>& b)
{
    return (a.value() == b.value());
}



//-------------------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_bounded<T2>::value>::type>
inline bool
operator != (const bounded<T1,B,P>& x, const T2& r)
{
    return (x.value() != r);
}
//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_bounded<T2>::value>::type>
inline bool
operator != (const T2& r, const bounded<T1,B,P>& x)
{
    return (r != x.value());
}
//---------------------------------------------------------
template<class T1, class B1, class P1, class T2, class B2, class P2>
inline bool
operator != (const bounded<T1,B1,P1>& a, const bounded<T2,B2,P2>& b)
{
    return (a.value() != b.value());
}



//-------------------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_bounded<T2>::value>::type>
inline bool
operator > (const bounded<T1,B,P>& x, const T2& r)
{
    return (x.value() > r);
}
//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_bounded<T2>::value>::type>
inline bool
operator > (const T2& r, const bounded<T1,B,P>& x)
{
    return (r > x.value());
}
//---------------------------------------------------------
template<class T1, class B1, class P1, class T2, class B2, class P2>
inline bool
operator > (const bounded<T1,B1,P1>& a, const bounded<T2,B2,P2>& b)
{
    return (a.value() > b.value());
}


//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_bounded<T2>::value>::type>
inline bool
operator >= (const bounded<T1,B,P>& x, const T2& r)
{
    return (x.value() >= r);
}
//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_bounded<T2>::value>::type>
inline bool
operator >= (const T2& r, const bounded<T1,B,P>& x)
{
    return (r >= x.value());
}
//---------------------------------------------------------
template<class T1, class B1, class P1, class T2, class B2, class P2>
inline bool
operator >= (const bounded<T1,B1,P1>& a, const bounded<T2,B2,P2>& b)
{
    return (a.value() >= b.value());
}


//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_bounded<T2>::value>::type>
inline bool
operator < (const bounded<T1,B,P>& x, const T2& r)
{
    return (x.value() < r);
}
//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_bounded<T2>::value>::type>
inline bool
operator < (const T2& r, const bounded<T1,B,P>& x)
{
    return (r < x.value());
}
//---------------------------------------------------------
template<class T1, class B1, class P1, class T2, class B2, class P2>
inline bool
operator < (const bounded<T1,B1,P1>& a, const bounded<T2,B2,P2>& b)
{
    return (a.value() < b.value());
}


//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_bounded<T2>::value>::type>
inline bool
operator <= (const bounded<T1,B,P>& x, const T2& r)
{
    return (x.value() <= r);
}
//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_bounded<T2>::value>::type>
inline bool
operator <= (const T2& r, const bounded<T1,B,P>& x)
{
    return (r <= x.value());
}
//---------------------------------------------------------
template<class T1, class B1, class P1, class T2, class B2, class P2>
inline bool
operator <= (const bounded<T1,B1,P1>& a, const bounded<T2,B2,P2>& b)
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
template<class T1, class B1, class P1, class T2, class B2, class P2>
inline constexpr
auto
operator + (const bounded<T1,B1,P1>& x, const bounded<T2,B2,P2>& y)
    -> decltype(x.value() + y.value())
{
    return (x.value() + y.value());
}

//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_bounded<T2>::value>::type>
inline constexpr
auto
operator + (const bounded<T1,B,P>& x, const T2& y)
    -> decltype(x.value() + y)
{
    return (x.value() + y);
}
//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_bounded<T2>::value>::type>
inline constexpr
auto
operator + (const T2& y, const bounded<T1,B,P>& x)
    -> decltype(y + x.value())
{
    return (y + x.value());
}



//-------------------------------------------------------------------
// SUBTRACTION
//-------------------------------------------------------------------
template<class T1, class B1, class P1, class T2, class B2, class P2>
inline constexpr
auto
operator - (const bounded<T1,B1,P1>& x, const bounded<T2,B2,P2>& y)
    -> decltype(x.value() - y.value())
{
    return (x.value() - y.value());
}

//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_bounded<T2>::value>::type>
inline constexpr
auto
operator - (const bounded<T1,B,P>& x, const T2& y)
    -> decltype(x.value() - y)
{
    return (x.value() - y);
}
//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_bounded<T2>::value>::type>
inline constexpr
auto
operator - (const T2& y, const bounded<T1,B,P>& x)
    -> decltype(y - x.value())
{
    return (y - x.value());
}



//-------------------------------------------------------------------
// MULTIPLIPCATION
//-------------------------------------------------------------------
template<class T1, class B1, class P1, class T2, class B2, class P2>
inline constexpr
auto
operator * (const bounded<T1,B1,P1>& x, const bounded<T2,B2,P2>& y)
    -> decltype(x.value() * y.value())
{
    return (x.value() * y.value());
}

//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_bounded<T2>::value>::type>
inline constexpr
auto
operator * (const bounded<T1,B,P>& x, const T2& y)
    -> decltype(x.value() * y)
{
    return (x.value() * y);
}
//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_bounded<T2>::value>::type>
inline constexpr
auto
operator * (const T2& y, const bounded<T1,B,P>& x)
    -> decltype(y * x.value())
{
    return (y * x.value());
}



//-------------------------------------------------------------------
// DIVISION
//-------------------------------------------------------------------
template<class T1, class B1, class P1, class T2, class B2, class P2>
inline constexpr
auto
operator / (const bounded<T1,B1,P1>& x, const bounded<T2,B2,P2>& y)
    -> decltype(x.value() / y.value())
{
    return (x.value() / y.value());
}

//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_bounded<T2>::value>::type>
inline constexpr
auto
operator / (const bounded<T1,B,P>& x, const T2& y)
    -> decltype(x.value() / y)
{
    return (x.value() / y);
}
//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_bounded<T2>::value>::type>
inline constexpr
auto
operator / (const T2& y, const bounded<T1,B,P>& x)
    -> decltype(y / x.value())
{
    return (y / x.value());
}



//-------------------------------------------------------------------
// MODULO
//-------------------------------------------------------------------
template<class T1, class B1, class P1, class T2, class B2, class P2>
inline constexpr
auto
operator % (const bounded<T1,B1,P1>& x, const bounded<T2,B2,P2>& y)
    -> decltype(x.value() % y.value())
{
    return (x.value() % y.value());
}

//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_bounded<T2>::value>::type>
inline constexpr
auto
operator % (const bounded<T1,B,P>& x, const T2& y)
    -> decltype(x.value() % y)
{
    return (x.value() % y);
}
//---------------------------------------------------------
template<class T1, class B, class P, class T2, class = typename
    std::enable_if<is_number<T2>::value && !is_bounded<T2>::value>::type>
inline constexpr
auto
operator % (const T2& y, const bounded<T1,B,P>& x)
    -> decltype(y % x.value())
{
    return (y % x.value());
}



//-------------------------------------------------------------------
// INVERSION
//-------------------------------------------------------------------
template<class T, class B, class P>
inline constexpr
bounded<T,B,P>
operator - (const bounded<T,B,P>& x)
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

//---------------------------------------------------------
template<class T, class B, class P>
inline constexpr auto
min(const bounded<T,B,P>& x) -> decltype(x.min())
{
    return x.min();
}
//---------------------------------------------------------
template<class T, class B, class P>
inline constexpr auto
max(const bounded<T,B,P>& x) -> decltype(x.max())
{
    return x.max();
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
    infinity() noexcept {return val_t(numeric_limits<T>::infinity()); }

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
    using type = common_numeric_t<T1,T2>;
};
//---------------------------------------------------------
template<class T1, class B, class P, class T2>
struct common_numeric_type<T2,bounded<T1,B,P>>
{
    using type = common_numeric_t<T1,T2>;
};
//---------------------------------------------------------
/// @brief
template<class T1, class B1, class P1, class T2, class B2, class P2>
struct common_numeric_type<bounded<T1,B1,P1>,bounded<T2,B2,P2>>
{
    using type = common_numeric_t<T1,T2>;
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