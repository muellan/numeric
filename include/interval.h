/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_RANGE_H_
#define AM_NUMERIC_RANGE_H_

#include <type_traits>
#include <utility>
#include <vector>

#include "limits.h"
#include "traits.h"


namespace am {
namespace num {


/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class T1, class T2>
inline typename std::common_type<T1,T2>::type
min(const std::pair<T1,T2>& p) noexcept(noexcept(p.first < p.second))
{
    return (p.first < p.second) ? p.first : p.second;
}


//-------------------------------------------------------------------
template<class T1, class T2>
inline typename std::common_type<T1,T2>::type
max(const std::pair<T1,T2>& p) noexcept(noexcept(p.first > p.second))
{
    return (p.first > p.second) ? p.first : p.second;
}




/*****************************************************************************
 *
 * INTERVAL ARITHMETIC
 *
 *****************************************************************************/
template<class T>
inline void
interval_add(const T& al, const T& ar, const T& bl, const T& br, T& l, T& r)
{
    l = al + bl;
    r = ar + br;
}

//---------------------------------------------------------
template<class T>
inline void
interval_sub(const T& al, const T& ar, const T& bl, const T& br, T& l, T& r)
{
    l = al - br;
    r = ar - bl;
}

//---------------------------------------------------------
template<class T>
inline void
interval_mul(const T& al, const T& ar, const T& bl, const T& br, T& l, T& r)
{
    using std::min;
    using std::max;

    auto lmin = T(0);
    auto lmax = T(0);
    {
        const auto lxl = al * bl;
        const auto rxl = ar * bl;
        if(lxl < rxl) {
            lmin = lxl;
            lmax = rxl;
        } else {
            lmin = rxl;
            lmax = lxl;
        }
    }
    auto rmin = T(0);
    auto rmax = T(0);
    {
        const auto lxr = al * br;
        const auto rxr = ar * br;
        if(lxr < rxr) {
            rmin = lxr;
            rmax = rxr;
        } else {
            rmin = rxr;
            rmax = lxr;
        }
    }

    l = min(lmin, rmin);
    r = max(lmax, rmax);
}

//---------------------------------------------------------
template<class T>
inline void
interval_div(const T& al, const T& ar, const T& bl, const T& br, T& l, T& r)
{
    using std::min;
    using std::max;

    if((bl <= T(0)) && (br >= T(0))) {
        l = T(0);
        r = T(0);
    }
    else {
        auto lmin = T(0);
        auto lmax = T(0);
        {
            const auto lxl = al / bl;
            const auto rxl = ar / bl;
            if(lxl < rxl) {
                lmin = lxl;
                lmax = rxl;
            } else {
                lmin = rxl;
                lmax = lxl;
            }
        }
        auto rmin = T(0);
        auto rmax = T(0);
        {
            const auto lxr = al / br;
            const auto rxr = ar / br;
            if(lxr < rxr) {
                rmin = lxr;
                rmax = rxr;
            } else {
                rmin = rxr;
                rmax = lxr;
            }
        }

        l = min(lmin, rmin);
        r = max(lmax, rmax);
    }
}




/*************************************************************************//***
 *
 * @brief static interval
 *
 *****************************************************************************/
template<class T, long long int left, long long int right>
class static_interval
{
public:
    using value_type = T;
    using numeric_type = T;

    static constexpr value_type min() noexcept {return left; }
    static constexpr value_type max() noexcept {return right; }
};

//-------------------------------------------------------------------
template<class T, long long int left, long long int right>
inline constexpr T
min(const static_interval<T,left,right>&) noexcept { return left; }

template<class T, long long int left, long long int right>
inline constexpr T
max(const static_interval<T,left,right>&) noexcept { return right; }




/*************************************************************************//***
 *
 * @brief empty interval
 *
 *****************************************************************************/
template<class T>
using empty_interval = static_interval<T,0,0>;



/*************************************************************************//***
 *
 * @brief interval [0,1]
 *
 *****************************************************************************/
template<class T>
using unit_interval = static_interval<T,0,1>;



/*************************************************************************//***
 *
 * @brief interval [-1,-1]
 *
 *****************************************************************************/
template<class T>
using symmetric_unit_interval = static_interval<T,-1,1>;



/*************************************************************************//***
 *
 * @brief interval [0, 2^n-1]
 *
 *****************************************************************************/
template<int n, class T>
struct pow2_interval
{
    using value_type = T;
    using numeric_type = T;

    static constexpr T min() noexcept {
        return T(0);
    }
    static constexpr T max() noexcept {
        return T(pow2_interval<n-1,T>::max() * T(2));
    }
};

//---------------------------------------------------------
template<class T>
struct pow2_interval<0,T>
{
    using value_type = T;
    using numeric_type = T;

    static constexpr T min() noexcept {return T(0); }
    static constexpr T max() noexcept {return T(1); }
};

//-------------------------------------------------------------------
template<int n, class T>
inline constexpr T
min(const pow2_interval<n,T>&) noexcept { return pow2_interval<n,T>::min(); }

template<int n, class T>
inline constexpr T
max(const pow2_interval<n,T>&) noexcept { return pow2_interval<n,T>::max(); }






/*************************************************************************//***
 *
 * @brief 1-dimensional interval represented by two numbers
 *
 *****************************************************************************/
template<class ValueType>
class interval
{
    static_assert(is_number<ValueType>::value,
        "interval<T> : T must be a number type");

    //befriend all intervals
    template<class T> friend class interval;


public:
    //---------------------------------------------------------------
    using value_type = ValueType;
    using numeric_type = value_type;


    //---------------------------------------------------------------
    constexpr
    interval():
        l_(std::numeric_limits<value_type>::lowest()),
        r_(std::numeric_limits<value_type>::max())
    {}

    explicit constexpr
    interval(const value_type& value) 
        noexcept( noexcept(value_type{value}) ) 
    :
        l_{value},
        r_{value}
    {}

    constexpr
    interval(value_type left, value_type right) 
        noexcept( noexcept(value_type{std::move(left)} ) )
    :
        l_{(left < right) ? std::move(left ) : std::move(right)},
        r_{(left < right) ? std::move(right) : std::move(left )}
    {}

    explicit constexpr
    interval(const std::pair<value_type,value_type>& p)
        noexcept( noexcept(value_type{p.first}) )
    :
        l_((p.first < p.second) ? p.first  : p.second),
        r_((p.first < p.second) ? p.second : p.first )
    {}

    explicit constexpr
    interval(std::pair<value_type,value_type>&& p)
        noexcept( noexcept(value_type{std::move(p.first)} ) )
    :
        l_((p.first < p.second) ? std::move(p.first)  : std::move(p.second)),
        r_((p.first < p.second) ? std::move(p.second) : std::move(p.first) )
    {}

    constexpr
    interval(const interval&) = default;
    
    template<class T>
    constexpr
    interval(const empty_interval<T>&) noexcept :
        l_{empty_interval<numeric_type>::min()},
        r_{empty_interval<numeric_type>::max()}
    {}

    template<class T>
    constexpr
    interval(const unit_interval<T>&) noexcept :
        l_{unit_interval<numeric_type>::min()},
        r_{unit_interval<numeric_type>::max()}
    {}

    template<class T>
    constexpr
    interval(const symmetric_unit_interval<T>&) noexcept :
        l_{symmetric_unit_interval<numeric_type>::min()},
        r_{symmetric_unit_interval<numeric_type>::max()}
    {}


    //-----------------------------------------------------------------
    interval&
    operator = (const interval&) = default;

    interval&
    operator = (interval&&) = default;

    interval&
    operator = (const std::pair<value_type,value_type>& source)
    {
        assign(source.first, source.second);
        return *this;
    }

    template<class T1, class T2>
    interval&
    operator = (std::pair<value_type,value_type>&& source)
    {
        assign(std::move(source.first), std::move(source.second));
        return *this;
    }

    template<class T>
    interval&
    operator = (const empty_interval<T>&)
    {
        clear();
        return *this;
    }


    //-----------------------------------------------------------------
    void
    assign(const value_type& left, const value_type& right) 
    {
        if(left < right) {
            l_ = left;
            r_ = right;
        } else {
            r_ = left;
            l_ = right;
        }
    }

    void
    assign(const value_type& left, value_type&& right) 
    {
        if(left < right) {
            l_ = left;
            r_ = std::move(right);
        } else {
            r_ = left;
            l_ = std::move(right);
        }
    }

    void
    assign(value_type&& left, const value_type& right) 
    {
        if(left < right) {
            l_ = std::move(left);
            r_ = right;
        } else {
            r_ = std::move(left);
            l_ = right;
        }
    }

    void
    assign(value_type&& left, value_type&& right) 
    {
        if(left < right) {
            l_ = std::move(left);
            r_ = std::move(right);
        } else {
            r_ = std::move(left);
            l_ = std::move(right);
        }
    }


    //---------------------------------------------------------------
    constexpr const value_type&
    min() const noexcept
    {
        return l_;
    }

    //-----------------------------------------------------
    constexpr const value_type&
    max() const noexcept
    {
        return r_;
    }
    //-----------------------------------------------------
    void
    clear()
    {
        l_ = value_type(0);
        r_ = value_type(0);
    }


    //---------------------------------------------------------------
    void
    center(const value_type& c)
    {
        operator+=(c - center());
    }
    
    floating_point_t<value_type>
    center() const
    {
        using res_t = floating_point_t<value_type>;
        return ((res_t(r_) + res_t(l_)) / res_t(2));
    }


    //---------------------------------------------------------------
    void
    half_width(const value_type& w)
    {
        expand(w - half_width());
    }
    
    floating_point_t<value_type>
    half_width() const
    {
        using res_t = floating_point_t<value_type>;
        return ((res_t(r_) - res_t(l_)) / res_t(2));
    }


    //---------------------------------------------------------------
    value_type
    width() const
    {
        return value_type{r_ - l_};
    }
    
    void
    width(const value_type& w)
    {
        expand((w - width()) / value_type(2));
    }


    //---------------------------------------------------------------
    void
    expand_include(const interval& i,
                   const value_type& offset = value_type(0))
    {
        if(i.l_ < l_) { l_ = i.l_ - offset; }
        if(i.r_ > r_) { r_ = i.r_ + offset; }
    }
   
    void
    expand_include(const value_type& bound,
                   const value_type& offset = value_type(0))
    {
        if(bound < l_) { l_ = bound - offset; }
        if(bound > r_) { r_ = bound + offset; }
    }
    
    void
    shrink_exclude(const value_type& bound,
                   const value_type& offset = tolerance<value_type>)
    {
        if(empty()) {
            l_ = bound + offset;
            r_ = l_;
        }
        else if(contains(bound,offset)) {

            using std::abs;
            const auto dl = abs(bound - l_);
            const auto dr = abs(r_ - bound);

            if(dl < dr) { 
                l_ = bound + offset; 
            } else { 
                r_ = bound - offset; 
            }
        }
    }


    //---------------------------------------------------------------
    void
    expand (const value_type& amount)
    {
        l_ -= amount;
        r_ += amount;
    }


    //---------------------------------------------------------------
    interval&
    operator += (const value_type& amount)
    {
        l_ += amount;
        r_ += amount;
        return *this;
    }
    
    interval&
    operator -= (const value_type& amount)
    {
        l_ -= amount;
        r_ -= amount;
        return *this;
    }
    
    interval&
    operator *= (const value_type& factor)
    {
        width(width() * factor);
        return *this;
    }
   
    interval&
    operator /= (const value_type& factor)
    {
        return operator *=(value_type(1)/factor);
    }


    //---------------------------------------------------------------
    interval&
    operator += (const interval& o)
    {
        l_ += o.l_;
        r_ += o.r_;
        return *this;
    }

    interval&
    operator -= (const interval& o)
    {
        l_ -= o.r_;
        r_ -= o.l_;
        return *this;
    }

    interval&
    operator *= (const interval& o)
    {
        interval_mul(l_, r_, o.l_, o.r_, l_, r_);
        return *this;
    }

    interval&
    operator /= (const interval& o)
    {
        interval_div(l_, r_, o.l_, o.r_, l_, r_);
        return *this;
    }

    //-----------------------------------------------------------------
    constexpr bool
    empty(const value_type& tol = tolerance<value_type>) const
    {
        using std::abs;
        //we have to use abs even if l_ should always be smaller than r_
        //because the result of (r_ - l_) could be < 0
        return (abs(r_ - l_) <= tol);
    }


    //---------------------------------------------------------------
    constexpr bool
    contains(const value_type& p) const
    {
        return ((p >= l_) && (p <= r_));
    }
    //-----------------------------------------------------
    constexpr bool
    contains(const value_type& p, const value_type& tol) const
    {
        return ( ((p + tol) >= l_) && ((p - tol) <= r_) );
    }

    //-----------------------------------------------------
    constexpr bool
    contains(const interval& o) const
    {
        return ((l_ <= o.l_) && (r_ >= o.r_));
    }
    //-----------------------------------------------------
    constexpr bool
    contains(const interval& o, const value_type& tol) const
    {
        return ( ((l_ - tol) <= o.l_) && ((r_ + tol) >= o.r_));
    }

    //-----------------------------------------------------
    constexpr bool
    intersects(const interval& o) const
    {
        return (l_ < o.l_) ? (r_ >= o.l_) : (l_ <= o.r_);
    }
    //-----------------------------------------------------
    constexpr bool
    intersects(const interval& o, const value_type& tol) const
    {
        return ((l_ - tol) < o.l_)
                ? ((r_ + tol) >= o.l_)
                : ((l_ - tol) <= o.r_);
    }


private:
    //---------------------------------------------------------------
    value_type l_, r_;
};




/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class T>
inline constexpr
interval<T>
make_interval(const T& a)
{
    return interval<T>{a, a};
}

template<class T1, class T2>
inline constexpr
interval<common_numeric_t<T1,T2>>
make_interval(T1&& a, T2&& b)
{
    return interval<common_numeric_t<T1,T2>>
        {std::forward<T1>(a), std::forward<T2>(b)};
}


//---------------------------------------------------------
template<class T1, class T2, class = std::enable_if_t<is_number<T1,T2>::value>>
inline constexpr interval<common_numeric_t<T1,T2>>
make_interval(const std::pair<T1,T2>& p)
{
    return interval<common_numeric_t<T1,T2>>{p};
}

template<class T1, class T2, class = std::enable_if_t<is_number<T1,T2>::value>>
inline constexpr interval<common_numeric_t<T1,T2>>
make_interval(std::pair<T1,T2>&& p)
{
    return interval<common_numeric_t<T1,T2>>{std::move(p)};
}


//-------------------------------------------------------------------
template<class W, class C>
auto
make_interval_half_width_center(const W& hwidth, const C& center)
{
    return make_interval(center - hwidth, center + hwidth);
}

template<class W, class C, class = std::enable_if_t<is_floating_point<W>::value>>
auto
make_interval_width_center(const W& width, C&& center)
{
    return make_interval_half_width_center(width / W(2), std::forward<C>(center));
}



/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class Ostream, class T>
inline Ostream&
operator << (Ostream& os, const interval<T>& r)
{
    return (os << r.min() << ' ' << r.max());
}

//-----------------------------------------------------
template<class Ostream, class T>
inline Ostream&
print(Ostream& os, const interval<T>& r)
{
    return (os << "[" << r.min() << "," << r.max() << "]");
}



/*****************************************************************************
 *
 * BOUNDS
 *
 *****************************************************************************/
template<class T>
inline constexpr auto
min(const interval<T>& i) noexcept 
{
    return i.min();
}
//---------------------------------------------------------
template<class T>
inline constexpr auto
max(const interval<T>& i) noexcept
{
    return i.max();
}



/*****************************************************************************
 *
 * DERIVED POINTS
 *
 *****************************************************************************/
template<class T>
inline auto
centroid(const interval<T>& i)
{
    return i.center();
}



/*****************************************************************************
 *
 * INTERVAL ARITHMETIC
 *
 *****************************************************************************/
template<class T1, class T2>
interval<common_numeric_t<T1,T2>>
operator + (const interval<T1>& a, const interval<T2>& b)
{
    auto res = interval<common_numeric_t<T1,T2>>{a};
    res += b;
    return res;
}

template<class T1, class T2, class = std::enable_if_t<is_number<T2>::value>>
interval<common_numeric_t<T1,T2>>
operator + (interval<T1> a, const T2& b)
{
    auto res = interval<common_numeric_t<T1,T2>>{a};
    res += b;
    return res;
}

template<class T1, class T2, class = std::enable_if_t<is_number<T1>::value>>
interval<common_numeric_t<T1,T2>>
operator + (const T1& b, interval<T2> a)
{
    auto res = interval<common_numeric_t<T1,T2>>{a};
    res += b;
    return res;
}


//-------------------------------------------------------------------
template<class T1, class T2>
interval<common_numeric_t<T1,T2>>
operator - (const interval<T1>& a, const interval<T2>& b)
{
    auto res = interval<common_numeric_t<T1,T2>>{a};
    res -= b;
    return res;
}

template<class T1, class T2, class = std::enable_if_t<is_number<T2>::value>>
interval<common_numeric_t<T1,T2>>
operator - (interval<T1> a, const T2& b)
{
    auto res = interval<common_numeric_t<T1,T2>>{a};
    res -= b;
    return res;
}

template<class T1, class T2, class = std::enable_if_t<is_number<T1>::value>>
interval<common_numeric_t<T1,T2>>
operator - (const T1& b, interval<T2> a)
{
    auto res = interval<common_numeric_t<T1,T2>>{a};
    res -= b;
    return res;
}


//-------------------------------------------------------------------
template<class T1, class T2>
interval<common_numeric_t<T1,T2>>
operator * (const interval<T1>& a, const interval<T2>& b)
{
    auto res = interval<common_numeric_t<T1,T2>>{a};
    res *= b;
    return res;
}

template<class T1, class T2, class = std::enable_if_t<is_number<T2>::value>>
interval<common_numeric_t<T1,T2>>
operator * (interval<T1> a, const T2& b)
{
    auto res = interval<common_numeric_t<T1,T2>>{a};
    res *= b;
    return res;
}

template<class T1, class T2, class = std::enable_if_t<is_number<T1>::value>>
interval<common_numeric_t<T1,T2>>
operator * (const T1& b, interval<T2> a)
{
    auto res = interval<common_numeric_t<T1,T2>>{a};
    res *= b;
    return res;
}


//-------------------------------------------------------------------
template<class T1, class T2>
interval<common_numeric_t<T1,T2>>
operator / (const interval<T1>& a, const interval<T2>& b)
{
    auto res = interval<common_numeric_t<T1,T2>>{a};
    res /= b;
    return res;
}

template<class T1, class T2, class = std::enable_if_t<is_number<T2>::value>>
interval<common_numeric_t<T1,T2>>
operator / (interval<T1> a, const T2& b)
{
    auto res = interval<common_numeric_t<T1,T2>>{a};
    res /= b;
    return res;
}

template<class T1, class T2, class = std::enable_if_t<is_number<T1>::value>>
interval<common_numeric_t<T1,T2>>
operator / (const T1& b, interval<T2> a)
{
    auto res = interval<common_numeric_t<T1,T2>>{a};
    res /= b;
    return res;
}



/*****************************************************************************
 *
 * INTERVAL CONTAINMENT/INTERSECTION TESTS
 *
 *****************************************************************************/
template<class T>
inline constexpr bool
intersects(const interval<T>& a, const interval<T>& b)
{
    return a.intersects(b);
}

template<class T>
inline constexpr bool
intersects(const interval<T>& a, const interval<T>& b, const T& tolerance)
{
    return a.intersects(b, tolerance);
}


//-------------------------------------------------------------------
template<class T>
inline constexpr bool
disjoint(const interval<T>& a, const interval<T>& b)
{
    return !a.intersects(b);
}

template<class T>
inline constexpr bool
disjoint(const interval<T>& a, const interval<T>& b, const T& tolerance)
{
    return !a.intersects(b, tolerance);
}


//-------------------------------------------------------------------
template<class T>
inline constexpr bool
contains(const interval<T>& a, const interval<T>& b)
{
    return a.contains(b);
}

template<class T>
inline constexpr bool
contains(const interval<T>& a, const interval<T>& b, const T& tolerance)
{
    return a.contains(b, tolerance);
}


//-------------------------------------------------------------------
template<class T>
inline constexpr bool
contains(const interval<T>& i, const T& v)
{
    return i.contains(v);
}

template<class T>
inline constexpr bool
contains(const interval<T>& i, const T& v, const T& tolerance)
{
    return i.contains(v, tolerance);
}




/*****************************************************************************
 *
 * PENETRATION DEPTH
 *
 *****************************************************************************/
template<class T1, class T2, class =
    typename std::enable_if<is_number<T2>::value>::type>
inline common_numeric_t<T1,T2>
penetration_depth(const interval<T1>& i, const T2& v)
{
    using std::min;
    using res_t = common_numeric_t<T1,T2>;

    if(v <= i.min() || v >= i.max()) return res_t(0);
    return min(res_t(v - i.min()), res_t(i.max() - v));
}

//---------------------------------------------------------
template<class T1, class T2>
inline common_numeric_t<T1,T2>
penetration_depth(const interval<T1>& a, const interval<T2>& b)
{
    using res_t = common_numeric_t<T1,T2>;

    if(b.min() > a.min()) {
        const res_t p = a.max() - b.min();
        return (p > res_t(0)) ? p : res_t(0);
    } else {
        const res_t p = b.max() - a.min();
        return (p > res_t(0)) ? p : res_t(0);
    }
}


//-------------------------------------------------------------------
template<class T1, class T2, class = typename
    std::enable_if<is_number<T2>::value>::type>
inline common_numeric_t<T1,T2>
distance(const interval<T1>& i, const T2& v)
{
    using std::min;
    using res_t = common_numeric_t<T1,T2>;

    if(v >= i.min() || v <= i.max()) return res_t(0);
    return min(res_t(i.min() - v), res_t(v - i.max()));
}
//---------------------------------------------------------
template<class T1, class T2>
inline common_numeric_t<T1,T2>
distance(const interval<T1>& a, const interval<T2>& b)
{
    using res_t = common_numeric_t<T1,T2>;

    if(b.min() > a.min()) {
        const res_t p = b.min() - a.max();
        return (p > res_t(0)) ? p : res_t(0);
    } else {
        const res_t p = a.min() - b.max();
        return (p > res_t(0)) ? p : res_t(0);
    }
}




/*****************************************************************************
 *
 * JOIN / INTERSECTION
 *
 *****************************************************************************/
template<class T1, class T2>
inline interval<common_numeric_t<T1,T2>>
intersection(interval<T1> a, const interval<T2>& b)
{
    using std::min;
    using std::max;
    using num_t = common_numeric_t<T1,T2>;

    //no intersection -> empty interval
    if(a.min() > b.max() || a.max() < b.min())
        return interval<num_t>{num_t(0), num_t(0)};

    //-> intersection interval
    return interval<num_t>{max(a.min(), b.min()), min(a.max(), b.max())};
}




/*****************************************************************************
 *
 * COMPARISON
 *
 *****************************************************************************/
template<class T>
inline bool
operator == (const interval<T>& a, const interval<T>& b) {
    return (a.min() == b.min()) && (a.max() == b.max());
}

//---------------------------------------------------------
template<class T>
inline bool
operator != (const interval<T>& a, const interval<T>& b) {
    return (a.min() != b.min()) || (a.max() != b.max());
}


//-------------------------------------------------------------------
template<class T1, class T2, class T3 = common_numeric_t<T1,T2>>
inline constexpr bool
approx_equal(const interval<T1>& a, const interval<T2>& b,
             const T3& tol = tolerance<T3>)
{
    return (approx_equal(a.min(), b.min(), tol) &&
            approx_equal(a.max(), b.max(), tol) );
}


//-------------------------------------------------------------------
template<class T1, class T2>
inline bool
narrower(const interval<T1>& a, const interval<T2>& b)
{
    return (a.width() < b.width());
}

//---------------------------------------------------------
template<class T1, class T2>
inline bool
wider(const interval<T1>& a, const interval<T2>& b)
{
    return (a.width() > b.width());
}




/*************************************************************************//***
 *
 * @brief
 *
 *****************************************************************************/
template<class Arg>
class value_in_range_predicate
{
public:

    using argument_type = Arg;
    using interval_type = interval<Arg>;

    value_in_range_predicate() = default;

    explicit
    value_in_range_predicate(interval_type ival):
        interval_{std::move(ival)}
    {}

//    explicit
    value_in_range_predicate(Arg min, Arg max):
        interval_{std::move(min), std::move(max)}
    {}

    bool
    operator () (const argument_type& value) const {
        return interval_.contains(value);
    }

private:
    interval_type interval_;
};




/*************************************************************************//***
 *
 * @brief
 *
 * @param  current : list of intervals
 * @param  toAdd   : new interval to be merged with current intervals
 *
 * @return false, if current intervals remain unchanged
 *
 *****************************************************************************/
template<class T>
bool
consolidate_intervals(std::vector<interval<T>>& ivals, const interval<T>& toAdd)
{

    if(ivals.empty()) {
        ivals.push_back(toAdd);
        return true;
    }

    bool newMin = min(toAdd) < min(ivals.front());
    bool newMax = max(toAdd) > max(ivals.back());

    //does new interval define new minimum?
    if(newMin) {
        //does new interval contain all ivals intervals?
        if(newMax) {
            ivals.clear();
            ivals.push_back(toAdd);
            return true;
        } else {
            //remove old intervals as long as their maximum is smaller
            auto i = begin(ivals);
            const auto e = end(ivals);
            while((i != e) && (max(toAdd) > max(*i)) ) {
                ++i;
            }
            ivals.erase(begin(ivals), i);
            ivals.insert(begin(ivals), toAdd);
            return true;
        }
    }
    //does new interval define new maximum?
    else if(newMax) {
        //remove old elements as long as their minimum is greater
        while((!ivals.empty()) && (min(toAdd) < min(ivals.back())) ) {
            ivals.pop_back();
        }
        ivals.push_back(toAdd);
        return true;
    }

    return false;
}


}  // namespace num
} //namespace am


#endif
