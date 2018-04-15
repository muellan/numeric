/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_NATURAL_INTERVAL_H_
#define AM_NUMERIC_NATURAL_INTERVAL_H_


#include "natural.h"


namespace am {
namespace num {


/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class IntT = int>
class natural_interval
{
public:
    static_assert(is_integral<IntT>::value && !is_unsigned<IntT>::value,
    "natural_interval<T>: T has to be a signed integral number type");

public:
    //---------------------------------------------------------------
    using value_type = natural<IntT>;
    using numeric_type = value_type;


    //---------------------------------------------------------------
    constexpr
    natural_interval() noexcept :
        min_(zero()), max_(infinity())
    {}
    //-----------------------------------------------------
    //no copy & move because of g++ 4.7.2 internal error on
    //cosntexpr evaluation
    explicit constexpr
    natural_interval(const value_type& max) noexcept :
        min_(zero()), max_(max)
    {}
    //-----------------------------------------------------
    //non-explicit
    constexpr
    natural_interval(value_type min, value_type max) noexcept :
        min_((min < max) ? std::move(min) : std::move(max)),
        max_((min < max) ? std::move(max) : std::move(min))
    {}


    //---------------------------------------------------------------
    constexpr const value_type&
    min() const noexcept {
        return min_;
    }
    constexpr const value_type&
    max() const noexcept {
        return max_;
    }


    //---------------------------------------------------------------
    static constexpr value_type
    zero() noexcept {
        return value_type::zero();
    }
    //-----------------------------------------------------
    static constexpr value_type
    infinity() noexcept {
        return value_type::infinity();
    }


    //---------------------------------------------------------------
    template<class T, class = typename std::enable_if<is_number<T>::value>::type>
    constexpr bool
    contains(const T& p) const noexcept {
        return ((p >= min_) && (p <= max_));
    }
    //-----------------------------------------------------
    constexpr bool
    contains(const natural_interval& r) const noexcept {
        return ((min_ <= r.min_) && (max_ >= r.max_));
    }


private:
    value_type min_;
    value_type max_;
};






/*****************************************************************************
 *
 *
 *
 *
 *****************************************************************************/
template<class Ostream, class T>
inline Ostream&
operator << (Ostream& os, const natural_interval<T>& r)
{
    return (os << r.min() << ' ' << r.max());
}

//-----------------------------------------------------
template<class Ostream, class T>
inline Ostream&
print(Ostream& os, const natural_interval<T>& r)
{
    return (os << "[" << r.min() << "," << r.max() << "]");
}



//-------------------------------------------------------------------
template<class T>
inline constexpr auto
min(const natural_interval<T>& i) noexcept -> decltype(i.min())
{
    return i.min();
}
//---------------------------------------------------------
template<class T>
inline constexpr auto
max(const natural_interval<T>& i) noexcept -> decltype(i.max())
{
    return i.max();
}



}  // namespace num
}  // namespace am


#endif
