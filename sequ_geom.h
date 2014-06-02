/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2014 André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_GEOEMETRIC_SEQUENCE_GENERATOR_H_
#define AM_NUMERIC_GEOEMETRIC_SEQUENCE_GENERATOR_H_

#include <cmath>
#include <iterator>
#include <cassert>

#include "traits.h"
#include "limits.h"
#include "equality.h"


namespace am {
namespace num {



/*****************************************************************************
 *
 * v(n) = scaleFactor * base^n,  for n with v(n) <= upperBound
 *
 *
 *
 *****************************************************************************/
template<class T>
class geometric_sequence
{
public:
    //---------------------------------------------------------------
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using size_type = std::size_t;
    //-----------------------------------------------------
    using value_type = T;
    using reference = const T&;
    using pointer = const T*;


    //---------------------------------------------------------------
    constexpr explicit
    geometric_sequence(
        value_type base = value_type(1),
        value_type upperBound = numeric_limits<value_type>::max())
    :
        cur_(1), base_{std::move(base)}, uBound_{std::move(upperBound)}
    {}
    //-----------------------------------------------------
    constexpr explicit
    geometric_sequence(
        value_type scaleFactor, value_type base, value_type upperBound)
    :
        cur_{std::move(scaleFactor)},
        base_{std::move(base)},
        uBound_{std::move(upperBound)}
    {}


    //---------------------------------------------------------------
    reference
    operator * () const {
        return cur_;
    }
    //-----------------------------------------------------
    pointer
    operator -> () const {
        return std::addressof(cur_);
    }
    //-----------------------------------------------------
    value_type
    operator [] (size_type offset) const {
       using std::pow;
       return cur_ * pow(base_,offset);
    }


    //---------------------------------------------------------------
    geometric_sequence&
    operator ++ () {
        cur_ *= base_;
        return *this;
    }
    //-----------------------------------------------------
    geometric_sequence&
    operator += (size_type offset) {
        using std::pow;
        cur_ *= pow(base_,offset);
        return *this;
    }


    //---------------------------------------------------------------
    reference
    base() const {
        return base_;
    }


    //---------------------------------------------------------------
    reference
    front() const {
        return cur_;
    }
    //-----------------------------------------------------
    value_type
    back() const {
        return (*this)[size()-1];
    }
    //-----------------------------------------------------
    size_type
    size() const {
        using std::log;

        return (1 + static_cast<size_type>(
            0.5 + (log(uBound_/cur_) / log(base_)) ) );
    }
    //-----------------------------------------------------
    bool
    empty() const {
        return
            (cur_ >= 0 && uBound_ >= 0)
            ? (base_ > 1) ? (cur_ > uBound_) : (cur_ < uBound_)
            : (base_ > 1) ? (cur_ < uBound_) : (cur_ > uBound_);
    }
    //-----------------------------------------------------
    explicit operator
    bool() const {
        return !empty();
    }


    //---------------------------------------------------------------
    const geometric_sequence&
    begin() const {
        return *this;
    }
    //-----------------------------------------------------
    geometric_sequence
    end() const {
        return geometric_sequence{(*this)[size()], base_, uBound_};
    }


    //---------------------------------------------------------------
    friend difference_type
    distance(const geometric_sequence& a, const geometric_sequence& b)
    {
        using std::log;

        return (1 +    static_cast<difference_type>(
            0.5 + (log(b.cur_/a.cur_) / log(a.base_)) ) );
    }


    //---------------------------------------------------------------
    bool
    operator == (const geometric_sequence& o) const {
        return
            approx_equal(cur_, o.cur_) &&
            approx_equal(base_, o.base_) &&
            approx_equal(uBound_, o.uBound_);
    }
    //-----------------------------------------------------
    bool
    operator != (const geometric_sequence& o) const {
        return !(*this == o);
    }


private:
    value_type cur_;
    value_type base_;
    value_type uBound_;
};








/*****************************************************************************
 *
 *
 * NON-MEMBER BEGIN/END
 *
 *
 *****************************************************************************/

//---------------------------------------------------------------
template<class T>
inline auto
begin(const geometric_sequence<T>& s) -> decltype(s.begin()) {
    return s.begin();
}
//-----------------------------------------------------
template<class T>
inline auto
cbegin(const geometric_sequence<T>& s) -> decltype(s.begin()) {
    return s.begin();
}

//-----------------------------------------------------
template<class T>
inline auto
end(const geometric_sequence<T>& s) -> decltype(s.end()) {
    return s.end();
}
//-----------------------------------------------------
template<class T>
inline auto
cend(const geometric_sequence<T>& s) -> decltype(s.end()) {
    return s.end();
}








/*****************************************************************************
 *
 *
 * FACTORRIES
 *
 *
 *****************************************************************************/

//---------------------------------------------------------
template<class Base, class UpperBound>
inline constexpr
geometric_sequence<common_numeric_t<
        typename std::decay<Base>::type,
        typename std::decay<UpperBound>::type>>
make_geometric_sequence(Base&& base, UpperBound&& upperBound)
{
//    assert( (base <  1 && cur >= upperBound) ||
//            (base >  1 && cur <= upperBound) );

    using num_t = common_numeric_t<
        typename std::decay<Base>::type,
        typename std::decay<UpperBound>::type>;

    return geometric_sequence<num_t>
        {std::forward<Base>(base), std::forward<UpperBound>(upperBound)};
}

//---------------------------------------------------------
template<class ScaleFac, class Base, class UpperBound>
inline constexpr
geometric_sequence<common_numeric_t<
        typename std::decay<ScaleFac>::type,
        typename std::decay<UpperBound>::type,
        typename std::decay<Base>::type>>
make_geometric_sequence(
    ScaleFac&& scaleFactor, Base&& base, UpperBound&& upperBound)
{
//    assert( (base <  1 && cur >= upperBound) ||
//            (base >  1 && cur <= upperBound) );

    using num_t = common_numeric_t<
        typename std::decay<ScaleFac>::type,
        typename std::decay<UpperBound>::type,
        typename std::decay<Base>::type>;

    return geometric_sequence<num_t> {
        std::forward<ScaleFac>(scaleFactor),
        std::forward<Base>(base),
        std::forward<UpperBound>(upperBound)};
}


}  // namespace num
}  // namespace am



#endif
