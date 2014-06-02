/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 *2008-2014  André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_REPEATED_SEQUENCE_GENERATOR_H_
#define AM_NUMERIC_REPEATED_SEQUENCE_GENERATOR_H_


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
template<class SequenceGenerator>
class repeated_sequence
{
public:
    //---------------------------------------------------------------
    using sequence_type = SequenceGenerator;
    //-----------------------------------------------------
    using iterator_category = std::input_iterator_tag;
    using difference_type = typename sequence_type::difference_type;
    using size_type = typename sequence_type::size_type;
    //-----------------------------------------------------
    using value_type = typename sequence_type::value_type;
    using reference = typename sequence_type::reference;
    using pointer = typename sequence_type::pointer;


    //---------------------------------------------------------------
    constexpr explicit
    repeated_sequence(
        sequence_type sequence = sequence_type(),
        size_type repetitions = 0)
    :
        reps_(0), maxReps_(repetitions),
        curSequ_{std::move(sequence)}, repSequ_{curSequ_}
    {}
    //-----------------------------------------------------
    constexpr explicit
    repeated_sequence(
        sequence_type first,
        sequence_type repeat,
        size_type repetitions = 0)
    :
        reps_(0), maxReps_(repetitions),
        curSequ_{std::move(first)}, repSequ_{std::move(repeat)}
    {}


    //---------------------------------------------------------------
    auto
    operator * () const -> decltype(*std::declval<sequence_type>())
    {
        return *curSequ_;
    }
    //-----------------------------------------------------
    pointer
    operator -> () const {
        return std::addressof(*curSequ_);
    }
    //-----------------------------------------------------
    value_type
    operator [] (size_type offset) const
    {
        const auto nfst = curSequ_.size();
        if(offset >= nfst) {
            return repSequ_[(offset-nfst) % repSequ_.size()];
        }
        return curSequ_[offset];
    }


    //---------------------------------------------------------------
    repeated_sequence&
    operator ++ ()
    {
        ++curSequ_;
        if(curSequ_.empty() && (reps_ < maxReps_)) {
            ++reps_;
            curSequ_ = repSequ_;
        }
        return *this;
    }
    //-----------------------------------------------------
    repeated_sequence&
    operator += (size_type offset)
    {
        if(offset >= curSequ_.size()) {
            offset -= curSequ_.size();
            reps_ += offset / repSequ_.size();
            if(reps_ <= maxReps_) {
                offset %= repSequ_.size();
                curSequ_ = repSequ_;
                curSequ_ += offset;
            } else {
                curSequ_ += curSequ_.size();
            }
        } else {
            curSequ_ += offset;
        }
        return *this;
    }


    //---------------------------------------------------------------
    const sequence_type&
    repeat_sequence() const {
        return repSequ_;
    }
    //-----------------------------------------------------
    size_type
    repetitions_required() const {
        return maxReps_;
    }
    //-----------------------------------------------------
    size_type
    repetitions_so_far() const {
        return reps_;
    }


    //---------------------------------------------------------------
    reference
    front() const {
        return curSequ_.front();
    }
    //-----------------------------------------------------
    value_type
    back() const {
        return repSequ_.back();
    }
    //-----------------------------------------------------
    size_type
    size() const {
        return curSequ_.size() + (maxReps_ * repSequ_.size());
    }
    //-----------------------------------------------------
    bool
    empty() const {
        return curSequ_.empty() && (reps_ >= maxReps_);
    }
    //-----------------------------------------------------
    explicit operator
    bool() const {
        return !empty();
    }


    //---------------------------------------------------------------
    const repeated_sequence&
    begin() const {
        return *this;
    }
    //-----------------------------------------------------
    repeated_sequence
    end() const {
        return repeated_sequence{curSequ_.end(), repSequ_, maxReps_, maxReps_};
    }


    //---------------------------------------------------------------
//    friend difference_type
//    distance(const repeated_sequence& a, const repeated_sequence& b)
//    {
//    }


    //---------------------------------------------------------------
    bool
    operator == (const repeated_sequence& o) const {
        return
            (reps_ == o.reps_) &&
            (curSequ_ == o.curSequ_) &&
            (repSequ_ == o.repSequ_) &&
            (maxReps_ == o.maxReps_);
    }
    //-----------------------------------------------------
    bool
    operator != (const repeated_sequence& o) const {
        return !(*this == o);
    }


private:
    //---------------------------------------------------------------
    constexpr explicit
    repeated_sequence(
        const sequence_type& first,
        const sequence_type& repeat,
        size_type curReps, size_type maxReps)
    :
        reps_(curReps), maxReps_(maxReps),
        curSequ_{first}, repSequ_{repeat}
    {}


    //---------------------------------------------------------------
    size_type reps_, maxReps_;
    sequence_type curSequ_;
    sequence_type repSequ_;
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
begin(const repeated_sequence<G>& s) -> decltype(s.begin()) {
    return s.begin();
}
//-----------------------------------------------------
template<class G>
inline auto
cbegin(const repeated_sequence<G>& s) -> decltype(s.begin()) {
    return s.begin();
}

//-----------------------------------------------------
template<class G>
inline auto
end(const repeated_sequence<G>& s) -> decltype(s.end()) {
    return s.end();
}
//-----------------------------------------------------
template<class G>
inline auto
cend(const repeated_sequence<G>& s) -> decltype(s.end()) {
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
template<class SequGen>
inline constexpr
repeated_sequence<SequGen>
make_repeated_sequence(SequGen&& seq, std::size_t repetitions)
{
    return repeated_sequence<SequGen>{std::forward<SequGen>(seq), repetitions};
}

//-----------------------------------------------------
template<class SequGen>
inline constexpr
repeated_sequence<SequGen>
make_repeated_sequence(
    SequGen firstSeq, SequGen repSeq, std::size_t repetitions)
{
    return repeated_sequence<SequGen>{
        std::move(firstSeq), std::move(repSeq), repetitions};
}



} //namespace num
} //namespace am

#endif
