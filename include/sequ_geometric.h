/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2016 André Müller
 *
 *****************************************************************************/

#ifndef AMLIB_NUMERIC_GEOEMETRIC_SEQUENCE_GENERATOR_H_
#define AMLIB_NUMERIC_GEOEMETRIC_SEQUENCE_GENERATOR_H_

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
 * v(n) = scaleFactor * ratio^n,  for n with v(n) <= upperBound
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
        value_type initial = value_type(0),
        value_type ratio = value_type(1),
        value_type upperBound = numeric_limits<value_type>::max())
    :
        cur_{std::move(initial)},
        ratio_{std::move(ratio)},
        uBound_{std::move(upperBound)}
    {}


    //---------------------------------------------------------------
    reference
    operator * () const noexcept {
        return cur_;
    }
    //-----------------------------------------------------
    pointer
    operator -> () const noexcept {
        return std::addressof(cur_);
    }
    //-----------------------------------------------------
    value_type
    operator [] (size_type offset) const {
       using std::pow;
       return cur_ * pow(ratio_,offset);
    }


    //---------------------------------------------------------------
    geometric_sequence&
    operator ++ () {
        cur_ *= ratio_;
        return *this;
    }
    //-----------------------------------------------------
    geometric_sequence&
    operator += (size_type offset) {
        using std::pow;
        cur_ *= pow(ratio_,offset);
        return *this;
    }
    //-----------------------------------------------------
    geometric_sequence
    operator + (size_type offset) const {
        auto res = *this;
        res += offset;
        return res;
    }


    //---------------------------------------------------------------
    reference
    ratio() const noexcept {
        return ratio_;
    }


    //---------------------------------------------------------------
    reference
    front() const noexcept {
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
            0.5 + (log(uBound_/cur_) / log(ratio_)) ) );
    }
    //-----------------------------------------------------
    bool
    empty() const noexcept {
        return
            (cur_ >= 0 && uBound_ >= 0)
            ? (ratio_ > 1) ? (cur_ > uBound_) : (cur_ < uBound_)
            : (ratio_ > 1) ? (cur_ < uBound_) : (cur_ > uBound_);
    }
    //-----------------------------------------------------
    explicit operator
    bool() const noexcept {
        return !empty();
    }


    //---------------------------------------------------------------
    const geometric_sequence&
    begin() const noexcept {
        return *this;
    }
    //-----------------------------------------------------
    geometric_sequence
    end() const {
        return geometric_sequence{(*this)[size()], ratio_, uBound_};
    }


    //---------------------------------------------------------------
    friend difference_type
    distance(const geometric_sequence& a, const geometric_sequence& b)
    {
        using std::log;

        return (1 +    static_cast<difference_type>(
            0.5 + (log(b.cur_/a.cur_) / log(a.ratio_)) ) );
    }


    //---------------------------------------------------------------
    bool
    operator == (const geometric_sequence& o) const noexcept {
        return
            approx_equal(cur_, o.cur_) &&
            approx_equal(ratio_, o.ratio_) &&
            approx_equal(uBound_, o.uBound_);
    }
    //-----------------------------------------------------
    bool
    operator != (const geometric_sequence& o) const noexcept {
        return !(*this == o);
    }


private:
    value_type cur_;
    value_type ratio_;
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

template<class Initial, class Ratio, class UpperBound>
inline constexpr
geometric_sequence<common_numeric_t<
        typename std::decay<Initial>::type,
        typename std::decay<UpperBound>::type,
        typename std::decay<Ratio>::type>>
make_geometric_sequence(
    Initial&& initial, Ratio&& ratio, UpperBound&& upperBound)
{
//    assert( (ratio <  1 && cur >= upperBound) ||
//            (ratio >  1 && cur <= upperBound) );

    using num_t = common_numeric_t<
        typename std::decay<Initial>::type,
        typename std::decay<UpperBound>::type,
        typename std::decay<Ratio>::type>;

    return geometric_sequence<num_t> {
        std::forward<Initial>(initial),
        std::forward<Ratio>(ratio),
        std::forward<UpperBound>(upperBound)};
}


}  // namespace num
}  // namespace am



#endif
