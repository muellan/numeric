/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
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
		reference base = value_type(1),
		reference upperBound = numeric_limits<value_type>::max())
	:
		cur_(1), base_{base}, uBound_{upperBound}
	{}
	//-----------------------------------------------------
	constexpr explicit
	geometric_sequence(
		reference scaleFactor, reference base, reference upperBound)
	:
		cur_{scaleFactor}, base_{base}, uBound_{upperBound}
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

		return (1 +	static_cast<difference_type>(
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
geometric_sequence<common_numeric_t<UpperBound,Base>>
make_geometric_sequence(const Base& base, const UpperBound& upperBound)
{
//	assert( (base <  1 && cur >= upperBound) ||
//			(base >  1 && cur <= upperBound) );

	return geometric_sequence<common_numeric_t<UpperBound,Base>>
		{base, upperBound};
}

//---------------------------------------------------------
template<class ScaleFac, class Base, class UpperBound>
inline constexpr
geometric_sequence<common_numeric_t<ScaleFac,UpperBound,Base>>
make_geometric_sequence(
	const ScaleFac& scaleFactor, const Base& base, const UpperBound& upperBound)
{
//	assert( (base <  1 && cur >= upperBound) ||
//			(base >  1 && cur <= upperBound) );

	return geometric_sequence<common_numeric_t<ScaleFac,UpperBound,Base>>
		{scaleFactor, base, upperBound};
}


}  // namespace num
}  // namespace am



#endif
