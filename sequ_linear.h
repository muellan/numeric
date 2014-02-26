/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
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
class increment_sequence
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
	increment_sequence(
		reference first = value_type(0),
		reference uBound = value_type(0))
	:
		cur_{first}, uBound_{uBound}
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
	increment_sequence&
	operator ++ () {
		++cur_;
		return *this;
	}
	//-----------------------------------------------------
	increment_sequence&
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
	const increment_sequence&
	begin() const {
		return *this;
	}
	//-----------------------------------------------------
	increment_sequence
	end() const {
		return increment_sequence{(*this)[size()], uBound_};
	}


	//---------------------------------------------------------------
	friend difference_type
	distance(const increment_sequence& a, const increment_sequence& b) {
		return static_cast<difference_type>(b.cur_ - a.cur_);
	}


	//---------------------------------------------------------------
	bool
	operator == (const increment_sequence& o) const {
		return approx_equal(cur_, o.cur_) && approx_equal(uBound_, o.uBound_);
	}
	//-----------------------------------------------------
	bool
	operator != (const increment_sequence& o) const {
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
class decrement_sequence
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
	decrement_sequence(
		reference first = value_type(0),
		reference uBound = value_type(0))
	:
		cur_{first}, uBound_{uBound}
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
	decrement_sequence&
	operator ++ () {
		--cur_;
		return *this;
	}
	//-----------------------------------------------------
	decrement_sequence&
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
		return (1 + static_cast<size_type>(0.5 + (cur_ - uBound_)));
	}
	//-----------------------------------------------------
	bool
	empty() const {
		return (cur_ < uBound_);
	}
	//-----------------------------------------------------
	explicit operator
	bool() const {
		return !empty();
	}


	//---------------------------------------------------------------
	const decrement_sequence&
	begin() const {
		return *this;
	}
	//-----------------------------------------------------
	decrement_sequence
	end() const {
		return decrement_sequence{(*this)[size()], uBound_};
	}


	//---------------------------------------------------------------
	friend difference_type
	distance(const decrement_sequence& a, const decrement_sequence& b) {
		return static_cast<difference_type>(a.cur_ - b.cur_);
	}


	//---------------------------------------------------------------
	bool
	operator == (const decrement_sequence& o) const {
		return approx_equal(cur_, o.cur_) && approx_equal(uBound_, o.uBound_);
	}
	//-----------------------------------------------------
	bool
	operator != (const decrement_sequence& o) const {
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
		reference first = value_type(0),
		reference stride = value_type(1),
		reference uBound  = std::numeric_limits<value_type>::max())
	:
		cur_{first}, stride_{stride}, uBound_{uBound}
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
begin(const increment_sequence<T>& s) -> decltype(s.begin()) {
	return s.begin();
}
//-----------------------------------------------------
template<class T>
inline auto
cbegin(const increment_sequence<T>& s) -> decltype(s.begin()) {
	return s.begin();
}

//-----------------------------------------------------
template<class T>
inline auto
end(const increment_sequence<T>& s) -> decltype(s.end()) {
	return s.end();
}
//-----------------------------------------------------
template<class T>
inline auto
cend(const increment_sequence<T>& s) -> decltype(s.end()) {
	return s.end();
}



//---------------------------------------------------------------
template<class T>
inline auto
begin(const decrement_sequence<T>& s) -> decltype(s.begin()) {
	return s.begin();
}
//-----------------------------------------------------
template<class T>
inline auto
cbegin(const decrement_sequence<T>& s) -> decltype(s.begin()) {
	return s.begin();
}

//-----------------------------------------------------
template<class T>
inline auto
end(const decrement_sequence<T>& s) -> decltype(s.end()) {
	return s.end();
}
//-----------------------------------------------------
template<class T>
inline auto
cend(const decrement_sequence<T>& s) -> decltype(s.end()) {
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
increment_sequence<First>
make_ascending_sequence(const First& first)
{
	return increment_sequence<First>{first};
}

//---------------------------------------------------------
template<class First, class UpperBound>
inline constexpr
increment_sequence<common_numeric_t<First,UpperBound>>
make_ascending_sequence(const First& first, const UpperBound& uBound)
{
//	assert(first <= uBound);

	return increment_sequence<common_numeric_t<First,UpperBound>>
		{first, uBound};
}



//-------------------------------------------------------------------
template<class First>
inline constexpr
decrement_sequence<First>
make_descending_sequence(const First& first)
{
//	assert(first >= uBound);

	return decrement_sequence<First>{first};
}

//---------------------------------------------------------
template<class First, class UpperBound>
inline constexpr
decrement_sequence<common_numeric_t<First,UpperBound>>
make_descending_sequence(const First& first, const UpperBound& uBound)
{
//	assert(first >= uBound);

	return decrement_sequence<common_numeric_t<First,UpperBound>>
		{first, uBound};
}



//-------------------------------------------------------------------
template<class First>
inline constexpr
linear_sequence<First>
make_linear_sequence(const First& first)
{
	return linear_sequence<First>{first};
}

//---------------------------------------------------------
template<class First, class Stride>
inline constexpr
linear_sequence<common_numeric_t<First,Stride>>
make_linear_sequence(const First& first, const Stride& stride)
{
	return linear_sequence<common_numeric_t<First,Stride>>{first, stride};
}

//---------------------------------------------------------
template<class First, class Stride, class UpperBound>
inline constexpr
linear_sequence<common_numeric_t<First,Stride,UpperBound>>
make_linear_sequence(
	const First& first, const Stride& stride, const UpperBound& uBound)
{
//	assert((stride >  0 && first <= uBound) ||
//		   (stride <  0 && first >= uBound) );

	return linear_sequence<common_numeric_t<First,Stride,UpperBound>>
		{first, stride, uBound};
}



}  // namespace num
}  // namespace am



#endif
