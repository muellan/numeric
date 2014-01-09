/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_RANGE_H_
#define AM_NUMERIC_RANGE_H_

#include <type_traits>
#include <utility>
#include <vector>

#include "traits.h"
#include "narrowing.h"


namespace am {

namespace num {


/*****************************************************************************
 *
 *
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T1, class T2>
inline typename std::common_type<T1,T2>::type
min(const std::pair<T1,T2>& p)
{
	return (p.first < p.second) ? p.first : p.second;
}


//-------------------------------------------------------------------
template<class T1, class T2>
inline typename std::common_type<T1,T2>::type
max(const std::pair<T1,T2>& p)
{
	return (p.first > p.second) ? p.first : p.second;
}






/*****************************************************************************
 *
 *
 * INTERVAL ARITHMETIC
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
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

	T lmin = 0;
	T lmax = 0;
	{
		T lxl = al * bl;
		T rxl = ar * bl;
		if(lxl < rxl) {
			lmin = lxl;
			lmax = rxl;
		} else {
			lmin = rxl;
			lmax = lxl;
		}
	}
	T rmin = 0;
	T rmax = 0;
	{
		T lxr = al * br;
		T rxr = ar * br;
		if(lxr < rxr) {
			rmin = lxr;
			rmax = rxr;
		} else {
			rmin = rxr;
			rmax = lxr;
		}
	}

	l = lmin, rmin;
	r = lmax, rmax;
}

//---------------------------------------------------------
template<class T>
inline void
interval_div(const T& al, const T& ar, const T& bl, const T& br, T& l, T& r)
{
	using std::min;
	using std::max;

	if((bl <= 0) && (br >= 0)) {
		l = 0;
		r = 0;
	}
	else {
		T lmin = 0;
		T lmax = 0;
		{
			T lxl = al / bl;
			T rxl = ar / bl;
			if(lxl < rxl) {
				lmin = lxl;
				lmax = rxl;
			} else {
				lmin = rxl;
				lmax = lxl;
			}
		}
		T rmin = 0;
		T rmax = 0;
		{
			T lxr = al / br;
			T rxr = ar / br;
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








/*****************************************************************************
 *
 * @brief N-dimensional interval represented by two numbers
 *
 *
 *
 *
 *****************************************************************************/
template<class ValueType>
class interval
{
	static_assert(is_number<ValueType>::value,
		"interval<T> : T must be a number type");

public:
	//---------------------------------------------------------------
	// TYPES
	//---------------------------------------------------------------
	using value_type = ValueType;


	//---------------------------------------------------------------
	// CONSTRUCTION / DESTRUCTION
	//---------------------------------------------------------------
	constexpr
	interval():
		l_{value_type(0)}, r_{value_type(0)}
	{}

	//-----------------------------------------------------
	interval(const value_type& left, const value_type& right):
		l_{left}, r_{right}
	{
		using std::swap;
		if(l_ > r_) swap(l_,r_);
	}

	//-----------------------------------------------------
	template<class T1, class T2, class = typename std::enable_if<
		is_number<T1,T2>::value>::type>
	explicit
	interval(const T1& left, const T2& right):
		l_(left), r_(right)
	{
		AM_CHECK_NARROWING2(value_type,T1,T2)

		using std::swap;
		if(l_ > r_) swap(l_,r_);
	}

	//-----------------------------------------------------
	template<class T1, class T2, class = typename std::enable_if<
		is_number<T1,T2>::value>::type>
	explicit
	interval(const std::pair<T1,T2>& p):
		l_(p.first), r_(p.second)
	{
		AM_CHECK_NARROWING2(value_type,T1,T2)

		using std::swap;
		if(l_ > r_) swap(l_,r_);
	}

	//-----------------------------------------------------
	constexpr
	interval(const interval& source) = default;

	//-----------------------------------------------------
	template<class T>
	constexpr
	interval(const interval<T>& source):
		l_(source.min()), r_(source.max())
	{
		AM_CHECK_NARROWING(value_type,T)
	}


	//-----------------------------------------------------------------
	// ASSIGNMENT
	//-----------------------------------------------------------------
	interval& operator = (const interval&) = default;

	//-----------------------------------------------------
	template<class T>
	interval& operator = (const interval<T>& source)
	{
		AM_CHECK_NARROWING(value_type,T)

		l_ = source.min();
		r_ = source.max();

		return *this;
	}

	//-----------------------------------------------------
	template<class T1, class T2>
	interval& operator = (const std::pair<T1,T2>& source)
	{
		assign(source.first, source.second);
		return *this;
	}

	//-----------------------------------------------------
	template<class T1, class T2, class = typename std::enable_if<
		is_number<T1,T2>::value>::type>
	void
	assign(const T1& left, const T2& right)
	{
		AM_CHECK_NARROWING2(value_type,T1,T2)

		if(left < right) {
			l_ = left;
			r_ = right;
		} else {
			r_ = left;
			l_ = right;
		}
	}


	//---------------------------------------------------------------
	// BOUNDS
	//---------------------------------------------------------------
	template<class T, class = typename std::enable_if<
		is_number<T>::value>::type>
	void
	min(const T& left)
	{
		AM_CHECK_NARROWING(value_type,T)

		l_ = left;
		if(r_ < l_) r_ = l_;
	}
	//-----------------------------------------------------
	constexpr const value_type&
	min() const noexcept
	{
		return l_;
	}

	//-----------------------------------------------------
	template<class T, class = typename std::enable_if<
		is_number<T>::value>::type>
	void
	max(const T& right)
	{
		AM_CHECK_NARROWING(value_type,T)

		r_ = right;
		if(r_ < l_) r_ = l_;
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
	// DERIVED VALUES
	//---------------------------------------------------------------
	template<class T, class = typename std::enable_if<
		is_number<T>::value>::type>
	void
	center(const T& c)
	{
		AM_CHECK_NARROWING(value_type,T)

		operator+=(c - center());
	}
	//-----------------------------------------------------
	value_type
	center() const
	{
		return value_type{(l_ + r_) / value_type(2)};
	}

	//-----------------------------------------------------
	template<class T, class = typename std::enable_if<
		is_number<T>::value>::type>
	void
	half_width(const T& w)
	{
		AM_CHECK_NARROWING(value_type,T)

		expand(w - half_width());
	}
	//-----------------------------------------------------
	value_type
	half_width() const
	{
		return value_type{(r_ - l_) / value_type(2)};
	}

	//-----------------------------------------------------
	value_type
	width() const
	{
		return value_type{r_ - l_};
	}
	//-----------------------------------------------------
	template<class T, class = typename std::enable_if<
		is_number<T>::value>::type>
	void
	width(const T& w)
	{
		AM_CHECK_NARROWING(value_type,T)

		expand((w - width()) / value_type(2));
	}

	//-----------------------------------------------------
	template<class T1, class T2, class = typename std::enable_if<
		is_number<T1,T2>::value>::type>
	void
	center_half_width(const T1& c, const T2& w)
	{
		AM_CHECK_NARROWING2(value_type,T1,T2)

		l_ = c - w;
		r_ = c + w;
	}
	//-----------------------------------------------------
	template<class T1, class T2, class = typename std::enable_if<
		is_number<T1,T2>::value>::type>
	void
	center_width(const T1& c, const T2& s)
	{
		AM_CHECK_NARROWING2(value_type,T1,T2)

		center_half_width(c, s/value_type(2));
	}


	//---------------------------------------------------------------
	// SHRINK / EXPAND
	//---------------------------------------------------------------
	template<class T, class = typename std::enable_if<
		is_number<T>::value>::type>
	void
	expand_include(const interval<T>& interval, const T& offset = 0)
	{
		AM_CHECK_NARROWING(value_type,T)

		if(interval.l_ < l_) l_ = interval.l_ - offset;
		if(interval.r_ > r_) r_ = interval.r_ + offset;
	}
	//-----------------------------------------------------
	template<class T, class = typename std::enable_if<
		is_number<T>::value>::type>
	void
	expand_include(const T& bound, const T& offset = 0)
	{
		AM_CHECK_NARROWING(value_type,T)

		if(bound < l_) l_ = bound - offset;
		if(bound > r_) r_ = bound + offset;
	}
	//-----------------------------------------------------
	template<class T, class = typename std::enable_if<
		is_number<T>::value>::type>
	void
	shrink_exclude(const T& bound, const T& offset = epsilon<T>::value)
	{
		AM_CHECK_NARROWING(value_type,T)

		if(bound > l_) l_ = bound + offset;
		if(bound < r_) r_ = bound - offset;
	}

	//-----------------------------------------------------
	template<class T, class = typename std::enable_if<
		is_number<T>::value>::type>
	void
	expand (const T& amount)
	{
		AM_CHECK_NARROWING(value_type,T)

		l_ -= amount;
		r_ += amount;
	}


	//---------------------------------------------------------------
	template<class T, class = typename std::enable_if<
		is_number<T>::value>::type>
	interval&
	operator += (const T& amount)
	{
		AM_CHECK_NARROWING(value_type,T)

		l_ += amount;
		r_ += amount;
		return *this;
	}
	//-----------------------------------------------------
	template<class T, class = typename std::enable_if<
		is_number<T>::value>::type>
	interval&
	operator -= (const T& amount)
	{
		AM_CHECK_NARROWING(value_type,T)

		l_ -= amount;
		r_ -= amount;
		return *this;
	}
	//-----------------------------------------------------
	template<class T, class = typename std::enable_if<
		is_number<T>::value>::type>
	interval&
	operator *= (const T& factor)
	{
		AM_CHECK_NARROWING(value_type,T)

		AM_CHECK_NARROWING(value_type,T)

		auto omf = (value_type(1) - factor);
		auto opf = (value_type(1) + factor);
		auto l = l_;

		l_ = omf * r_ + opf * l_;
		r_ = opf * r_ + omf * l;

		return *this;
	}
	//-----------------------------------------------------
	template<class T, class = typename std::enable_if<
		is_number<T>::value>::type>
	interval&
	operator /= (const T& factor)
	{
		AM_CHECK_NARROWING(value_type,T)

		return operator *=(value_type(1)/factor);
	}


	//---------------------------------------------------------------
	// INTERVAL ARITHMETIC
	//---------------------------------------------------------------
	template<class T>
	interval&
	operator += (const interval<T>& o)
	{
		AM_CHECK_NARROWING(value_type,T)

		l_ += o.l_;
		r_ += o.r_;
		return *this;
	}
	//-------------------------------------------
	template<class T>
	interval&
	operator -= (const interval<T>& o)
	{
		AM_CHECK_NARROWING(value_type,T)

		l_ -= o.r_;
		r_ -= o.l_;
		return *this;
	}
	//-------------------------------------------
	template<class T>
	interval&
	operator *= (const interval<T>& o)
	{
		AM_CHECK_NARROWING(value_type,T)

		interval_mul(l_, r_, o.l_, o.r_, l_, r_);
		return *this;
	}
	//-------------------------------------------
	template<class T>
	interval&
	operator /= (const interval<T>& o)
	{
		AM_CHECK_NARROWING(value_type,T)

		interval_div(l_, r_, o.l_, o.r_, l_, r_);
		return *this;
	}


	//-----------------------------------------------------------------
	constexpr bool
	empty(const value_type& tolerance = epsilon<value_type>::value) const
	{
		using std::abs;
		//we have to use abs even if l_ should always be smaller than r_
		//because the result of (r_ - l_) could be < 0
		return (abs(r_ - l_) <= tolerance);
	}


	//---------------------------------------------------------------
	// INTERACTION TESTS
	//---------------------------------------------------------------
	template<class T, class = typename std::enable_if<is_number<T>::value>::type>
	constexpr bool
	contains(const T& p) const
	{
		return ((p >= l_) && (p <= r_));
	}
	//-----------------------------------------------------
	template<class T>
	constexpr bool
	contains(const interval<T>& o) const
	{
		return ((l_ <= o.l_) && (r_ >= o.r_));
	}
	//-----------------------------------------------------
	template<class T>
	constexpr bool
	intersects(const interval<T>& o) const
	{
		return (l_ < o.l_) ? (r_ >= o.l_) : (l_ <= o.r_);
	}
	//-----------------------------------------------------
	template<class T>
	constexpr bool
	disjoint(const interval<T>& o) const
	{
		return !intersects(o);
	}


	//---------------------------------------------------------------
	// INTERACTION
	//---------------------------------------------------------------
	template<class T, class = typename std::enable_if<is_number<T>::value>::type>
	common_numeric_t<value_type,T>
	penetration_depth(const T& v) const
	{
		using std::min;
		using res_t = common_numeric_t<value_type,T>;

		if(v <= l_ || v >= r_) return res_t(0);
		return min(res_t(v) - res_t(l_), res_t(r_) - res_t(v));
	}
	//-----------------------------------------------------
	template<class T>
	common_numeric_t<value_type,T>
	penetration_depth(const interval<T>& o) const
	{
		using res_t = common_numeric_t<value_type,T>;

		if(o.l_ > l_) {
			auto p {r_ - o.l_};
			return res_t((p > 0) ? p : 0);
		} else {
			auto p { p = o.r_ - l_};
			return res_t((p > 0) ? p : 0);
		}
	}

	//-----------------------------------------------------
	template<class T, class = typename std::enable_if<is_number<T>::value>::type>
	common_numeric_t<value_type,T>
	distance(const T& v) const
	{
		using std::min;
		using res_t = common_numeric_t<value_type,T>;

		if(v >= l_ || v <= r_) return res_t(0);
		return min(res_t(l_) - res_t(v), res_t(v) - res_t(r_));
	}
	//-----------------------------------------------------
	template<class T>
	common_numeric_t<value_type,T>
	distance(const interval<T>& o) const
	{
		using res_t = common_numeric_t<value_type,T>;

		if(o.l_ > l_) {
			auto p {o.l_ - r_};
			return res_t((p > 0) ? p : 0);
		} else {
			auto p { p = l_ - o.r_};
			return res_t((p > 0) ? p : 0);
		}
	}


private:
	value_type l_, r_;
};








/*****************************************************************************
 *
 *
 *
 * empty interval
 *
 *
 *
 *****************************************************************************/
template<class ValueType>
class empty_interval
{
public:
	//---------------------------------------------------------------
	// TYPES
	//---------------------------------------------------------------
	using value_type = ValueType;


	//-----------------------------------------------------------------
	// GETTERS
	//-----------------------------------------------------------------
	static constexpr value_type min() noexcept        {return value_type(0); }
	static constexpr value_type max() noexcept        {return value_type(0); }
	//-----------------------------------------------------
	static constexpr value_type width() noexcept      {return value_type(0); }
	static constexpr value_type half_width() noexcept {return value_type(0); }
	static constexpr value_type center() noexcept     {return value_type(0); }
	//-----------------------------------------------------
	static constexpr bool empty() noexcept            {return true; }


	//---------------------------------------------------------------
	template<class Ostream>
	inline friend Ostream&
	operator << (Ostream& os, const empty_interval&) noexcept {return os; }
};








/*****************************************************************************
 *
 *
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr interval<common_numeric_t<T1,T2>>
make_interval(T1&& a, T2&& b)
{
	return interval<common_numeric_t<T1,T2>>
		{std::forward<T1>(a), std::forward<T2>(b)};
}



//---------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<is_number<T1,T2>::value>::type>
inline constexpr interval<common_numeric_t<T1,T2>>
make_interval(const std::pair<T1,T2>& p)
{
	return interval<common_numeric_t<T1,T2>>{p};
}






/*****************************************************************************
 *
 *
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
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
 *
 * BOUNDS
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T>
inline constexpr auto
min(const interval<T>& i) noexcept -> decltype(i.min())
{
	return i.min();
}
//---------------------------------------------------------
template<class T>
inline constexpr auto
max(const interval<T>& i) noexcept -> decltype(i.max())
{
	return i.max();
}






/*****************************************************************************
 *
 *
 * DERIVED POINTS
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T>
inline auto
centroid(const interval<T>& i) -> decltype(i.center())
{
	return i.center();
}







/*****************************************************************************
 *
 *
 * INTERVAL ARITHMETIC
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
// +
//-------------------------------------------------------------------
template<class T1, class T2>
interval<common_numeric_t<T1,T2>>
operator + (const interval<T1>& a, const interval<T2>& b)
{
	auto res = interval<common_numeric_t<T1,T2>>{a};
	res += b;
	return res;
}

//---------------------------------------------------------
template<class T1, class T2>
interval<common_numeric_t<T1,T2>>
operator + (interval<T1> a, const T2& b)
{
	auto res = interval<common_numeric_t<T1,T2>>{a};
	res += b;
	return res;
}

//---------------------------------------------------------
template<class T1, class T2>
interval<common_numeric_t<T1,T2>>
operator + (const T1& b, interval<T2> a)
{
	auto res = interval<common_numeric_t<T1,T2>>{a};
	res += b;
	return res;
}



//-------------------------------------------------------------------
// -
//-------------------------------------------------------------------
template<class T1, class T2>
interval<common_numeric_t<T1,T2>>
operator - (const interval<T1>& a, const interval<T2>& b)
{
	auto res = interval<common_numeric_t<T1,T2>>{a};
	res -= b;
	return res;
}

//---------------------------------------------------------
template<class T1, class T2>
interval<common_numeric_t<T1,T2>>
operator - (interval<T1> a, const T2& b)
{
	auto res = interval<common_numeric_t<T1,T2>>{a};
	res -= b;
	return res;
}

//---------------------------------------------------------
template<class T1, class T2>
interval<common_numeric_t<T1,T2>>
operator - (const T1& b, interval<T2> a)
{
	auto res = interval<common_numeric_t<T1,T2>>{a};
	res -= b;
	return res;
}



//-------------------------------------------------------------------
// *
//-------------------------------------------------------------------
template<class T1, class T2>
interval<common_numeric_t<T1,T2>>
operator * (const interval<T1>& a, const interval<T2>& b)
{
	auto res = interval<common_numeric_t<T1,T2>>{a};
	res *= b;
	return res;
}

//---------------------------------------------------------
template<class T1, class T2>
interval<common_numeric_t<T1,T2>>
operator * (interval<T1> a, const T2& b)
{
	auto res = interval<common_numeric_t<T1,T2>>{a};
	res *= b;
	return res;
}

//---------------------------------------------------------
template<class T1, class T2>
interval<common_numeric_t<T1,T2>>
operator * (const T1& b, interval<T2> a)
{
	auto res = interval<common_numeric_t<T1,T2>>{a};
	res *= b;
	return res;
}


//-------------------------------------------------------------------
// /
//-------------------------------------------------------------------
template<class T1, class T2>
interval<common_numeric_t<T1,T2>>
operator / (const interval<T1>& a, const interval<T2>& b)
{
	auto res = interval<common_numeric_t<T1,T2>>{a};
	res /= b;
	return res;
}

//---------------------------------------------------------
template<class T1, class T2>
interval<common_numeric_t<T1,T2>>
operator / (interval<T1> a, const T2& b)
{
	auto res = interval<common_numeric_t<T1,T2>>{a};
	res /= b;
	return res;
}

//---------------------------------------------------------
template<class T1, class T2>
interval<common_numeric_t<T1,T2>>
operator / (const T1& b, interval<T2> a)
{
	auto res = interval<common_numeric_t<T1,T2>>{a};
	res /= b;
	return res;
}






/*****************************************************************************
 *
 *
 * INTERVAL CONTAINMENT/INTERSECTION TESTS
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
intersects(const interval<T1>& a, const interval<T2>& b)
{
	return a.intersects(b);
}

//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
disjoint(const interval<T1>& a, const interval<T2>& b)
{
	return a.disjoint(b);
}

//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
contains(const interval<T1>& a, const interval<T2>& b)
{
	return a.contains(b);
}

//---------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
contains(const interval<T1>& i, const T2& v)
{
	return i.contains(v);
}






/*****************************************************************************
 *
 *
 * PENETRATION DEPTH
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T1, class T2>
inline auto
penetration_depth(const interval<T1>& i, const T2& v)
	-> decltype(i.penetration_depth(v))
{
	return i.penetration_depth(v);
}

//---------------------------------------------------------
template<class T1, class T2>
inline auto
penetration_depth(const interval<T1>& a, const interval<T2>& b)
	-> decltype(a.penetration_depth(b))
{
	return a.penetration_depth(b);
}


//-------------------------------------------------------------------
template<class T1, class T2>
inline auto
distance(const interval<T1>& i, const T2& v)
	-> decltype(i.distance(v))
{
	return i.distance(v);
}
//---------------------------------------------------------
template<class T1, class T2>
inline auto
distance(const interval<T1>& a, const interval<T2>& b)
	-> decltype(a.distance(b))
{
	return a.distance(b);
}







/*****************************************************************************
 *
 *
 * JOIN / INTERSECTION
 *
 *
 *****************************************************************************/

//-----------------------------------------------------
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
 *
 * COMPARISON
 *
 *
 *****************************************************************************/


//-------------------------------------------------------------------
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
	const T3& tolerance = epsilon<T3>::value)
{
	return (approx_equal(a.min(), b.min(), tolerance) &&
		    approx_equal(a.max(), b.max(), tolerance) );
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



//-------------------------------------------------------------------
struct interval_narrower
{
	using result_type = bool;

	template<class T1, class T2>
	result_type operator () (const interval<T1>& a, const interval<T2>& b) {
		return (a.width() < b.width());
	}
};



//-------------------------------------------------------------------
struct interval_wider
{
	using result_type = bool;

	template<class T1, class T2>
	result_type operator () (const interval<T1>& a, const interval<T2>& b) {
		return (a.width() > b.width());
	}
};



//-------------------------------------------------------------------
struct interval_min_smaller
{
	using result_type = bool;

	template<class T1, class T2>
	result_type operator () (const interval<T1>& a, const interval<T2>& b) {
		return (a.min() < b.min());
	}
};



//-------------------------------------------------------------------
struct interval_max_greater
{
	using result_type = bool;

	template<class T1, class T2>
	result_type operator () (const interval<T1>& a, const interval<T2>& b) {
		return (a.max() > b.max());
	}
};








/*****************************************************************************
 *
 *
 *
 * FUNCTIONS WORKING ON COLLECTIONS OF INTERVALS
 *
 *
 *
 *****************************************************************************/

/**
 * @brief
 * @param current list of intervals
 * @param toAdd   new interval to be merged with current intervals
 * @return false, if current intervals remain unchanged
 */
template<class T>
bool consolidate_intervals(
	std::vector<interval<T>>& current, const interval<T>& toAdd)
{

	if(current.empty()) {
		current.push_back(toAdd);
		return true;
	}

	bool newMin = min(toAdd) < min(current.front());
	bool newMax = max(toAdd) > max(current.back());

	//does new interval define new minimum?
	if(newMin) {
		//does new interval contain all current intervals?
		if(newMax) {
			current.clear();
			current.push_back(toAdd);
			return true;
		} else {
			//remove old intervals as long as their maximum is smaller
			auto i = begin(current);
			auto e = end(current);
			while((i != e) && (max(toAdd) > max(*i)) ) {
				++i;
			}
			current.erase(current.begin(), i);
			current.insert(current.begin(), toAdd);
			return true;
		}
	}
	//does new interval define new maximum?
	else if(newMax) {
		//remove old elements as long as their minimum is greater
		while((!current.empty()) && (min(toAdd) < min(current.back())) ) {
			current.pop_back();
		}
		current.push_back(toAdd);
		return true;
	}

	return false;
}


}  // namespace num

} //namespace am


#endif
