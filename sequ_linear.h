/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 *2008-2014  André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_LINEAR_SEQUENCE_GENERATOR_H_
#define AM_NUMERIC_LINEAR_SEQUENCE_GENERATOR_H_


#include <cassert>
#include <iterator>

#include "traits.h"
#include "equality.h"


namespace am {
namespace num {


/*****************************************************************************
 *
 *
 *
 *
 *****************************************************************************/
template<class T>
class ascending_sequence
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
    ascending_sequence(
        value_type first = value_type(0),
        value_type uBound = value_type(0))
    :
        cur_{std::move(first)}, uBound_{std::move(uBound)}
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
       return cur_ + offset;
    }


    //---------------------------------------------------------------
    ascending_sequence&
    operator ++ () {
        ++cur_;
        return *this;
    }
    //-----------------------------------------------------
    ascending_sequence&
    operator += (size_type offset) {
        cur_ += offset;
        return *this;
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

    //---------------------------------------------------------------
    size_type
    size() const {
        return (1 + static_cast<size_type>(0.5 + (uBound_ - cur_)));
    }
    //-----------------------------------------------------
    bool
    empty() const {
        return (cur_ > uBound_);
    }
    //-----------------------------------------------------
    explicit operator
    bool() const {
        return !empty();
    }


    //---------------------------------------------------------------
    const ascending_sequence&
    begin() const {
        return *this;
    }
    //-----------------------------------------------------
    ascending_sequence
    end() const {
        return ascending_sequence{(*this)[size()], uBound_};
    }


    //---------------------------------------------------------------
    friend difference_type
    distance(const ascending_sequence& a, const ascending_sequence& b) {
        return static_cast<difference_type>(b.cur_ - a.cur_);
    }


    //---------------------------------------------------------------
    bool
    operator == (const ascending_sequence& o) const {
        return approx_equal(cur_, o.cur_) && approx_equal(uBound_, o.uBound_);
    }
    //-----------------------------------------------------
    bool
    operator != (const ascending_sequence& o) const {
        return !(*this == o);
    }


private:
    value_type cur_;
    value_type uBound_;
};








/*****************************************************************************
 *
 *
 *
 *
 *****************************************************************************/
template<class T>
class descending_sequence
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
    descending_sequence(
        value_type first = value_type(0),
        value_type uBound = value_type(0))
    :
        cur_{std::move(first)}, lBound_{std::move(uBound)}
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
       return cur_ - offset;
    }


    //---------------------------------------------------------------
    descending_sequence&
    operator ++ () {
        --cur_;
        return *this;
    }
    //-----------------------------------------------------
    descending_sequence&
    operator += (size_type offset) {
        cur_ -= offset;
        return *this;
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

    //---------------------------------------------------------------
    size_type
    size() const {
        return (1 + static_cast<size_type>(0.5 + (cur_ - lBound_)));
    }
    //-----------------------------------------------------
    bool
    empty() const {
        return (cur_ < lBound_);
    }
    //-----------------------------------------------------
    explicit operator
    bool() const {
        return !empty();
    }


    //---------------------------------------------------------------
    const descending_sequence&
    begin() const {
        return *this;
    }
    //-----------------------------------------------------
    descending_sequence
    end() const {
        return descending_sequence{(*this)[size()], lBound_};
    }


    //---------------------------------------------------------------
    friend difference_type
    distance(const descending_sequence& a, const descending_sequence& b) {
        return static_cast<difference_type>(a.cur_ - b.cur_);
    }


    //---------------------------------------------------------------
    bool
    operator == (const descending_sequence& o) const {
        return approx_equal(cur_, o.cur_) && approx_equal(lBound_, o.lBound_);
    }
    //-----------------------------------------------------
    bool
    operator != (const descending_sequence& o) const {
        return !(*this == o);
    }


private:
    value_type cur_;
    value_type lBound_;
};








/*****************************************************************************
 *
 *
 *
 *
 *****************************************************************************/
template<class T>
class linear_sequence
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
    linear_sequence(
        value_type first = value_type(0),
        value_type stride = value_type(1),
        value_type uBound  = std::numeric_limits<value_type>::max())
    :
        cur_{std::move(first)}, stride_{std::move(stride)},
        uBound_{std::move(uBound)}
    {}


    //---------------------------------------------------------------
    value_type
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
       return cur_ + (stride_ * offset);
    }


    //---------------------------------------------------------------
    linear_sequence&
    operator ++ () {
        cur_ += stride_;
        return *this;
    }
    //-----------------------------------------------------
    linear_sequence&
    operator += (size_type offset) {
        cur_ += stride_ * offset;
        return *this;
    }


    //---------------------------------------------------------------
    reference
    stride() const {
        return stride_;
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
        return (1 + static_cast<size_type>(
            0.5 + ((uBound_ - cur_) / stride_)));
    }
    //-----------------------------------------------------
    bool
    empty() const {
        return ((stride_> 0) ? (cur_ > uBound_) : (cur_ < uBound_));
    }
    //-----------------------------------------------------
    explicit operator
    bool() const {
        return !empty();
    }


    //---------------------------------------------------------------
    const linear_sequence&
    begin() const {
        return *this;
    }
    //-----------------------------------------------------
    linear_sequence
    end() const {
        return linear_sequence{(*this)[size()], stride_, uBound_};
    }


    //---------------------------------------------------------------
    friend difference_type
    distance(const linear_sequence& a, const linear_sequence& b) {
        return static_cast<difference_type>((b.cur_ - a.cur_) / a.stride_);
    }


    //---------------------------------------------------------------
    bool
    operator == (const linear_sequence& o) const {
        return
            approx_equal(cur_, o.cur_) &&
            approx_equal(stride_, o.stride_) &&
            approx_equal(uBound_, o.uBound_);
    }
    //-----------------------------------------------------
    bool
    operator != (const linear_sequence& o) const {
        return !(*this == o);
    }


private:
    value_type cur_;
    value_type stride_;
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
begin(const ascending_sequence<T>& s) -> decltype(s.begin()) {
    return s.begin();
}
//-----------------------------------------------------
template<class T>
inline auto
cbegin(const ascending_sequence<T>& s) -> decltype(s.begin()) {
    return s.begin();
}

//-----------------------------------------------------
template<class T>
inline auto
end(const ascending_sequence<T>& s) -> decltype(s.end()) {
    return s.end();
}
//-----------------------------------------------------
template<class T>
inline auto
cend(const ascending_sequence<T>& s) -> decltype(s.end()) {
    return s.end();
}



//---------------------------------------------------------------
template<class T>
inline auto
begin(const descending_sequence<T>& s) -> decltype(s.begin()) {
    return s.begin();
}
//-----------------------------------------------------
template<class T>
inline auto
cbegin(const descending_sequence<T>& s) -> decltype(s.begin()) {
    return s.begin();
}

//-----------------------------------------------------
template<class T>
inline auto
end(const descending_sequence<T>& s) -> decltype(s.end()) {
    return s.end();
}
//-----------------------------------------------------
template<class T>
inline auto
cend(const descending_sequence<T>& s) -> decltype(s.end()) {
    return s.end();
}



//---------------------------------------------------------------
template<class T>
inline auto
begin(const linear_sequence<T>& s) -> decltype(s.begin()) {
    return s.begin();
}
//-----------------------------------------------------
template<class T>
inline auto
cbegin(const linear_sequence<T>& s) -> decltype(s.begin()) {
    return s.begin();
}

//-----------------------------------------------------
template<class T>
inline auto
end(const linear_sequence<T>& s) -> decltype(s.end()) {
    return s.end();
}
//-----------------------------------------------------
template<class T>
inline auto
cend(const linear_sequence<T>& s) -> decltype(s.end()) {
    return s.end();
}








/*****************************************************************************
 *
 *
 * FACTORIES
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class First>
inline constexpr
ascending_sequence<typename std::decay<First>::type>
make_ascending_sequence(First&& first)
{
    return ascending_sequence<typename std::decay<First>::type>
        {std::forward<First>(first)};
}

//---------------------------------------------------------
template<class First, class UpperBound>
inline constexpr
ascending_sequence<common_numeric_t<
        typename std::decay<First>::type,
        typename std::decay<UpperBound>::type>>
make_ascending_sequence(First&& first, UpperBound&& uBound)
{
//    assert(first <= uBound);

    using num_t = common_numeric_t<
        typename std::decay<First>::type,
        typename std::decay<UpperBound>::type>;

    return ascending_sequence<num_t>
        {std::forward<First>(first), std::forward<UpperBound>(uBound)};
}



//-------------------------------------------------------------------
template<class First>
inline constexpr
descending_sequence<typename std::decay<First>::type>
make_descending_sequence(First&& first)
{
//    assert(first >= uBound);

    return descending_sequence<typename std::decay<First>::type>
        {std::forward<First>(first)};
}

//---------------------------------------------------------
template<class First, class UpperBound>
inline constexpr
descending_sequence<common_numeric_t<
        typename std::decay<First>::type,
        typename std::decay<UpperBound>::type>>
make_descending_sequence(First&& first, UpperBound&& uBound)
{
//    assert(first >= uBound);

    using num_t = common_numeric_t<
        typename std::decay<First>::type,
        typename std::decay<UpperBound>::type>;

    return descending_sequence<num_t>
        {std::forward<First>(first), std::forward<UpperBound>(uBound)};
}



//-------------------------------------------------------------------
template<class First>
inline constexpr
linear_sequence<typename std::decay<First>::type>
make_linear_sequence(First&& first)
{
    return linear_sequence<typename std::decay<First>::type>
        {std::forward<First>(first)};
}

//---------------------------------------------------------
template<class First, class Stride>
inline constexpr
linear_sequence<common_numeric_t<
        typename std::decay<First>::type,
        typename std::decay<Stride>::type>>
make_linear_sequence(First&& first, Stride&& stride)
{
    using num_t = common_numeric_t<
        typename std::decay<First>::type,
        typename std::decay<Stride>::type>;

    return linear_sequence<num_t>
        {std::forward<First>(first), std::forward<Stride>(stride)};
}

//---------------------------------------------------------
template<class First, class Stride, class UpperBound>
inline constexpr
linear_sequence<common_numeric_t<
        typename std::decay<First>::type,
        typename std::decay<Stride>::type,
        typename std::decay<UpperBound>::type>>
make_linear_sequence(First&& first, Stride&& stride, UpperBound&& uBound)
{
//    assert((stride >  0 && first <= uBound) ||
//           (stride <  0 && first >= uBound) );

    using num_t = common_numeric_t<
        typename std::decay<First>::type,
        typename std::decay<Stride>::type,
        typename std::decay<UpperBound>::type>;

    return linear_sequence<num_t> {
        std::forward<First>(first),
        std::forward<Stride>(stride),
        std::forward<UpperBound>(uBound) };
}



}  // namespace num
}  // namespace am



#endif
