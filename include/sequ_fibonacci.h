/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2016 André Müller
 *
 *****************************************************************************/

#ifndef AMLIB_NUMERIC_FIBONACCI_SEQUENCE_GENERATOR_H_
#define AMLIB_NUMERIC_FIBONACCI_SEQUENCE_GENERATOR_H_


#include <iterator>
#include <limits>
#include <cstdint>

#include "traits.h"
#include "limits.h"


namespace am {
namespace num {


/*****************************************************************************
 *
 *
 *
 *
 *****************************************************************************/
template<class T = std::uint_least64_t>
class fibonacci_sequence
{
    static_assert(num::is_integral<T>::value,
        "fibonacci_sequence<T>: T has to be an integral number type");

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
    fibonacci_sequence(
        size_type maxIteration = numeric_limits<size_type>::max())
    :
        cur_(0), prev_(1), n_(0), maxN_(maxIteration)
    {}


    //---------------------------------------------------------------
    value_type
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
    operator [] (size_type offset) const
    {
        auto c = cur_;
        auto p = prev_;

        for(; offset > 0 ; --offset) {
            const auto oldc = c;
            c += p;
            p = oldc;
        }

        return c;
    }


    //---------------------------------------------------------------
    fibonacci_sequence&
    operator ++ () {
        const auto oldPrev = prev_;
        prev_ = cur_;
        cur_ += oldPrev;
        ++n_;
        return *this;
    }
    //-----------------------------------------------------
    fibonacci_sequence&
    operator += (size_type offset)
    {
        for(; offset > 0 ; --offset) {
            ++(*this);
        }

        return *this;
    }
    //-----------------------------------------------------
    fibonacci_sequence
    operator + (size_type offset) const {
        auto res = *this;
        res += offset;
        return res;
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
    size() const noexcept {
        return maxN_;
    }
    //-----------------------------------------------------
    bool
    empty() const noexcept {
        return (n_ >= maxN_);
    }
    //-----------------------------------------------------
    explicit operator
    bool() const noexcept {
        return !empty();
    }


    //---------------------------------------------------------------
    const fibonacci_sequence&
    begin() const noexcept {
        return *this;
    }
    //-----------------------------------------------------
    fibonacci_sequence
    end() const {
        return fibonacci_sequence{nullptr, maxN_};
    }


    //---------------------------------------------------------------
    bool
    operator == (const fibonacci_sequence& o) const noexcept {
        return (n_ == o.n_) && (maxN_ == o.maxN_);
    }
    //-----------------------------------------------------
    bool
    operator != (const fibonacci_sequence& o) const noexcept {
        return !(*this == o);
    }


private:
    //---------------------------------------------------------------
    constexpr explicit
    fibonacci_sequence(std::nullptr_t, size_type n):
        cur_(), prev_(), n_(n), maxN_(n)
    {}


    //---------------------------------------------------------------
    value_type cur_;
    value_type prev_;
    size_type n_, maxN_;
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
begin(const fibonacci_sequence<T>& s) -> decltype(s.begin()) {
    return s.begin();
}
//-----------------------------------------------------
template<class T>
inline auto
cbegin(const fibonacci_sequence<T>& s) -> decltype(s.begin()) {
    return s.begin();
}

//-----------------------------------------------------
template<class T>
inline auto
end(const fibonacci_sequence<T>& s) -> decltype(s.end()) {
    return s.end();
}
//-----------------------------------------------------
template<class T>
inline auto
cend(const fibonacci_sequence<T>& s) -> decltype(s.end()) {
    return s.end();
}



}  // namespace num
}  // namespace am



#endif
