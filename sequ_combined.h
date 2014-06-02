/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 *2008-2014  André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_COMBINED_SEQUENCE_GENERATOR_H_
#define AM_NUMERIC_COMBINED_SEQUENCE_GENERATOR_H_


#include <cstdint>
#include <type_traits>
#include <cmath>

#include "traits.h"


namespace am {
namespace num {



/*****************************************************************************
 *
 *
 *
 *
 *
 *****************************************************************************/
template<
    class Sequence1,
    class Sequence2 = Sequence1
>
class combined_sequence
{
public:
    //---------------------------------------------------------------
    using first_sequence_type = Sequence1;
    using second_sequence_type = Sequence2;
    //-----------------------------------------------------
    using iterator_category = std::input_iterator_tag;
    //-----------------------------------------------------
    using difference_type = common_numeric_t<
        typename first_sequence_type::difference_type,
        typename second_sequence_type::difference_type>;
    //-----------------------------------------------------
    using size_type = common_numeric_t<
        typename first_sequence_type::size_type,
        typename second_sequence_type::size_type>;
    //-----------------------------------------------------
    using value_type = common_numeric_t<
        typename first_sequence_type::value_type,
        typename second_sequence_type::value_type>;
    //-----------------------------------------------------
    using reference = const value_type&;
    using pointer = value_type*;


    //---------------------------------------------------------------
    constexpr explicit
    combined_sequence(
        first_sequence_type first = first_sequence_type(),
        second_sequence_type second = second_sequence_type())
    :
        fstSequ_{std::move(first)}, sndSequ_{std::move(second)}
    {}


    //---------------------------------------------------------------
    value_type
    operator * () const
    {
        return fstSequ_.empty() ? *sndSequ_ : *fstSequ_;
    }
    //-----------------------------------------------------
    pointer
    operator -> () const {
        return fstSequ_.emtpy() ?
            std::addressof(*sndSequ_) :
            std::addressof(*fstSequ_);
    }
    //-----------------------------------------------------
    value_type
    operator [] (size_type offset) const
    {
        const auto nfst = fstSequ_.size();
        if(offset >= nfst) {
            return sndSequ_[offset-nfst];
        }
        return fstSequ_[offset];
    }


    //---------------------------------------------------------------
    combined_sequence&
    operator ++ ()
    {
        if(!fstSequ_.empty()) {
            ++fstSequ_;
        }
        else if(!sndSequ_.empty()){
            ++sndSequ_;
        }
        return *this;
    }
    //-----------------------------------------------------
    combined_sequence&
    operator += (size_type offset)
    {
        if(!fstSequ_.empty()) {
            const auto nfst = fstSequ_.size();

            if(offset < nfst) {
                fstSequ_ += offset;
            }
            else {
                fstSequ_ += nfst;
                sndSequ_ += offset - nfst;
            }
        }
        else if(!sndSequ_.empty()){
            sndSequ_ += offset;
        }
        return *this;
    }


    //---------------------------------------------------------------
    value_type
    front() const {
        return fstSequ_.emtpy() ? sndSequ_.front() : fstSequ_.front();
    }
    //-----------------------------------------------------
    value_type
    back() const {
        return sndSequ_.back();
    }
    //-----------------------------------------------------
    size_type
    size() const {
        return fstSequ_.size() + sndSequ_.size();
    }
    //-----------------------------------------------------
    bool
    empty() const {
        return fstSequ_.empty() && sndSequ_.empty();
    }
    //-----------------------------------------------------
    explicit operator
    bool() const {
        return !empty();
    }


    //---------------------------------------------------------------
    const combined_sequence&
    begin() const {
        return *this;
    }
    //-----------------------------------------------------
    combined_sequence
    end() const {
        return combined_sequence{fstSequ_.end(), sndSequ_.end()};
    }


    //---------------------------------------------------------------
    bool
    operator == (const combined_sequence& o) const {
        return (fstSequ_ == o.fstSequ_) &&
               (sndSequ_ == o.sndSequ_);
    }
    //-----------------------------------------------------
    bool
    operator != (const combined_sequence& o) const {
        return !(*this == o);
    }


private:
    //---------------------------------------------------------------
    first_sequence_type fstSequ_;
    second_sequence_type sndSequ_;
};








/*****************************************************************************
 *
 *
 * NON-MEMBER BEGIN/END
 *
 *
 *****************************************************************************/

//---------------------------------------------------------------
template<class G>
inline auto
begin(const combined_sequence<G>& s) -> decltype(s.begin()) {
    return s.begin();
}
//-----------------------------------------------------
template<class G>
inline auto
cbegin(const combined_sequence<G>& s) -> decltype(s.begin()) {
    return s.begin();
}

//-----------------------------------------------------
template<class G>
inline auto
end(const combined_sequence<G>& s) -> decltype(s.end()) {
    return s.end();
}
//-----------------------------------------------------
template<class G>
inline auto
cend(const combined_sequence<G>& s) -> decltype(s.end()) {
    return s.end();
}








/*****************************************************************************
 *
 *
 * FACTORRIES
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class Sequ1, class Sequ2>
inline constexpr
combined_sequence<
    typename std::decay<Sequ1>::type,
    typename std::decay<Sequ2>::type>
make_combined_sequence(Sequ1&& fst, Sequ2&& snd)
{
    return combined_sequence<
        typename std::decay<Sequ1>::type,
        typename std::decay<Sequ2>::type>
        {std::forward<Sequ1>(fst),
         std::forward<Sequ2>(snd)};
}



} //namespace num

} //namespace am

#endif
