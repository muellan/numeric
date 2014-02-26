/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_QUATERNION_H_
#define AM_NUMERIC_QUATERNION_H_

#include <cmath>
#include <random>
#include <cstdint>
#include <cassert>

#include "constants.h"
#include "limits.h"
#include "narrowing.h"


namespace am {

namespace num {


namespace low {

/*****************************************************************************
 *
 *
 *
 * LOW LEVEL QUATERNION {w, x, y, z} OPERATIONS
 *
 *
 *
 *****************************************************************************/


//-------------------------------------------------------------------
// random unit quaternion
//-------------------------------------------------------------------
template<class URNG, class Quat>
void
random_unit_quat(URNG& urng, Quat& q)
{
	using std::cos;
	using std::sin;
	using std::sqrt;
	using std::numeric_limits;

	using q_t = typename std::decay<decltype(q[0])>::type;
	using lim_t = numeric_limits<q_t>;

	auto d_0_1 = std::uniform_real_distribution<q_t>{q_t(0), q_t(1)};
	auto d_0_2pi = std::uniform_real_distribution<q_t>{q_t(0), q_t(2*pi)};

	const auto u0 = d_0_1(urng);
	const auto u1 = d_0_2pi(urng);
	const auto u2 = d_0_2pi(urng);
	const auto uA = sqrt(q_t(1) - u0);
	const auto uB = sqrt(u0);

	q[0] = uA * sin(u1);
	q[1] = uA * cos(u1);
	q[2] = uB * sin(u2);
	q[3] = uB * cos(u2);
}

//---------------------------------------------------------
template<class Quat, class URNG>
inline Quat
random_unit_quat(URNG& urng)
{
	Quat q;
	random_unit_quat(urng,q);
	return q;
}



//-------------------------------------------------------------------
// normalization
//-------------------------------------------------------------------
template<class Quat>
inline bool
quat_is_normalized(const Quat& q)
{
	return approx_1(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
}

//-----------------------------------------------------
template<class Quat>
inline void
normalize_quat(Quat& q)
{
	using std::sqrt;

	using q_t = typename std::decay<decltype(q[0])>::type;

	auto norm = q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3];
	if(!approx_1(norm)) {
		norm = q_t(1) / sqrt(norm);
		q[0] *= norm;
		q[1] *= norm;
		q[2] *= norm;
		q[3] *= norm;
	}
}


//-------------------------------------------------------------------
// conjugation
//-------------------------------------------------------------------
template<class Quat>
inline void
conjugate_quat(Quat& q)
{
	q[1] = -q[1];
	q[2] = -q[2];
	q[3] = -q[3];
}



//-------------------------------------------------------------------
// inversion
//-------------------------------------------------------------------
template<class Quat>
inline void
invert_quat(Quat& q)
{
	conjugate_quat(q);
	normalize_quat(q);
}



//-------------------------------------------------------------------
// multiplication
//-------------------------------------------------------------------
template<class Quat1, class Quat2, class QuatR>
inline void
multiply_quat_quat(const Quat1& p, const Quat2& q, QuatR& r)
{
	r[0] = p[0]*q[0] - p[1]*q[1] - p[2]*q[2] - p[3]*q[3];
	r[1] = p[0]*q[1] + p[1]*q[0] + p[2]*q[3] - p[3]*q[2];
	r[2] = p[0]*q[2] - p[1]*q[3] + p[2]*q[0] + p[3]*q[1];
	r[3] = p[0]*q[3] + p[1]*q[2] - p[2]*q[1] + p[3]*q[0];
}
//---------------------------------------------------------
template<class QuatR, class Quat1, class Quat2>
inline constexpr QuatR
multiply_quat_quat(const Quat1& p, const Quat2& q)
{
	return QuatR{
		p[0]*q[0] - p[1]*q[1] - p[2]*q[2] - p[3]*q[3],
		p[0]*q[1] + p[1]*q[0] + p[2]*q[3] - p[3]*q[2],
		p[0]*q[2] - p[1]*q[3] + p[2]*q[0] + p[3]*q[1],
		p[0]*q[3] + p[1]*q[2] - p[2]*q[1] + p[3]*q[0] };
}


//---------------------------------------------------------
template<class Quat1, class Quat2, class QuatR>
inline void
multiply_quat_conj_quat(const Quat1& p, const Quat2& q, QuatR& r)
{
	r[0] =  p[0]*q[0] + p[1]*q[1] + p[2]*q[2] + p[3]*q[3];
	r[1] = -p[0]*q[1] + p[1]*q[0] - p[2]*q[3] + p[3]*q[2];
	r[2] = -p[0]*q[2] + p[1]*q[3] + p[2]*q[0] - p[3]*q[1];
	r[3] = -p[0]*q[3] - p[1]*q[2] + p[2]*q[1] + p[3]*q[0];
}
//---------------------------------------------------------
template<class QuatR, class Quat1, class Quat2>
inline constexpr QuatR
multiply_quat_conj_quat(const Quat1& p, const Quat2& q)
{
	return QuatR{
		 p[0]*q[0] + p[1]*q[1] + p[2]*q[2] + p[3]*q[3],
		-p[0]*q[1] + p[1]*q[0] - p[2]*q[3] + p[3]*q[2],
		-p[0]*q[2] + p[1]*q[3] + p[2]*q[0] - p[3]*q[1],
		-p[0]*q[3] - p[1]*q[2] + p[2]*q[1] + p[3]*q[0] };
}


//---------------------------------------------------------
template<class Quat1, class Quat2, class QuatR>
inline void
multiply_conj_quat_quat(const Quat1& p, const Quat2& q, QuatR& r)
{
	r[0] = p[0]*q[0] + p[1]*q[1] + p[2]*q[2] + p[3]*q[3];
	r[1] = p[0]*q[1] - p[1]*q[0] - p[2]*q[3] + p[3]*q[2];
	r[2] = p[0]*q[2] + p[1]*q[3] - p[2]*q[0] - p[3]*q[1];
	r[3] = p[0]*q[3] - p[1]*q[2] + p[2]*q[1] - p[3]*q[0];
}
//---------------------------------------------------------
template<class QuatR, class Quat1, class Quat2>
inline constexpr QuatR
multiply_conj_quat_quat(const Quat1& p, const Quat2& q)
{
	return QuatR{
		p[0]*q[0] + p[1]*q[1] + p[2]*q[2] + p[3]*q[3],
		p[0]*q[1] - p[1]*q[0] - p[2]*q[3] + p[3]*q[2],
		p[0]*q[2] + p[1]*q[3] - p[2]*q[0] - p[3]*q[1],
		p[0]*q[3] - p[1]*q[2] + p[2]*q[1] - p[3]*q[0] };
}


//-------------------------------------------------------------------
// linear interpolation
//-------------------------------------------------------------------
template<class Quat1, class Quat2, class QuatO, class T>
void
quat_lerp(const Quat1& qFrom, const Quat2& qTo, T t, QuatO& out)
{
	assert((t >= 0) && (t <= 1));

	const auto t1 = 1-t;
	out[0] = qFrom[0] * t1 + qTo[0] * t;
	out[1] = qFrom[1] * t1 + qTo[1] * t;
	out[2] = qFrom[2] * t1 + qTo[2] * t;
	out[3] = qFrom[3] * t1 + qTo[3] * t;
	normalize_quat(out);
}



//-------------------------------------------------------------------
// spherical linear interpolation
//-------------------------------------------------------------------
template<class Quat0, class Quat1, class QuatO, class T>
void
quat_slerp(const Quat0& q0, const Quat1& q1, T t, QuatO& out)
{
	assert((t >= 0) && (t <= 1));

	using std::sin;
	using std::acos;

	using q_t = typename std::decay<decltype(out[0])>::type;

	auto cosPhi = q0[0]*q1[0] + q0[1]*q1[1] + q0[2]*q1[2] + q0[3]*q1[3];

	if(cosPhi < 0) {
		q_t from, to;
		if((1 + cosPhi) > tolerance<q_t>::value) {
			auto phi = acos(-cosPhi);
			auto sinPhi = sin(phi);
			from = sin((1 - t) * phi) / sinPhi;
			to = sin(t * phi) / sinPhi;
		}
		else {
			from = 1 - t;
			to = t;
		}
		out[0] = q0[0] * from - q1[0] * to;
		out[1] = q0[1] * from - q1[1] * to;
		out[2] = q0[2] * from - q1[2] * to;
		out[3] = q0[3] * from - q1[3] * to;
	}
	else {
		q_t from, to;
		if((1 - cosPhi) > tolerance<q_t>::value) {
			auto phi = acos(cosPhi);
			auto sinPhi = sin(phi);
			from = sin((1 - t) * phi) / sinPhi;
			to = sin(t * phi) / sinPhi;
		}
		else {
			from = 1 - t;
			to = t;
		}
		out[0] = q0[0] * from + q1[0] * to;
		out[1] = q0[1] * from + q1[1] * to;
		out[2] = q0[2] * from + q1[2] * to;
		out[3] = q0[3] * from + q1[3] * to;
	}

}



//-------------------------------------------------------------------
// spherical cubic interpolation
//-------------------------------------------------------------------
template<class Q0, class Q1, class Q2, class Q3, class T, class Qo>
void
quat_squad(
	const Q0& q0, const Q1& q1, const Q2& q2, const Q3& q3,
	T t,
	Qo& out)
{
	assert((t >= 0) && (t <= 1));

	return quat_slerp(quat_slerp(q0,q3,t), quat_slerp(q1,q2,t), 2*t*(1-t), out);
}



//-------------------------------------------------------------------
// log function application
//-------------------------------------------------------------------
template<class Quat1, class Quat2>
void
quat_log(const Quat1& qIn, Quat2& qOut)
{
	using std::sin;
	using std::acos;

	const auto phi = acos(qIn[0]);
	const auto sinPhi = sin(phi);

	qOut[0] = 0;
	if(sinPhi > 0) {
		qOut[1] = phi * qIn[0]/sinPhi;
		qOut[2] = phi * qIn[1]/sinPhi;
		qOut[3] = phi * qIn[2]/sinPhi;
	} else {
		qOut[1] = 0;
		qOut[2] = 0;
		qOut[3] = 0;
	}
}



//-------------------------------------------------------------------
/// @brief e^quat = exp(v*phi) = [cos(phi),vsin(phi)]
//-------------------------------------------------------------------
template<class Quat1, class Quat2>
void
quat_exp(const Quat1& qIn, Quat2& qOut)
{
	using std::sin;
	using std::cos;

	const auto phi = qIn[1]*qIn[1] + qIn[2]*qIn[2] + qIn[3]*qIn[3];
	const auto sinPhi = sin(phi);
	const auto cosPhi = cos(phi);

	qOut[0] = cosPhi;
	if(phi > 0) {
		qOut[1] = sinPhi * qIn[1] / phi;
		qOut[2] = sinPhi * qIn[2] / phi;
		qOut[3] = sinPhi * qIn[3] / phi;
	} else {
		qOut[1] = 0;
		qOut[2] = 0;
		qOut[3] = 0;
	}
}


} //namespace low








/*****************************************************************************
 *
 *
 *
 * 	Q U A T E R N I O N   C L A S S
 *
 *
 *
 *****************************************************************************/
template<class NumberT>
class quaternion
{
	static_assert(
		is_floating_point<NumberT>::value,
		"quaternion<T>: T must be a floating-point number");

public:
	//---------------------------------------------------------------
	// TYPES
	//---------------------------------------------------------------
	using numeric_type = NumberT;
	using value_type   = numeric_type;
	using dimension_type    = dims_t;

	//-----------------------------------------------------
	using pointer         = numeric_type*;
	using const_pointer   = const numeric_type*;
	//-----------------------------------------------------
	using reference       = numeric_type&;
	using const_reference = const numeric_type&;


	//---------------------------------------------------------------
	// CONSTRUCTION / DESTRUCTION
	//---------------------------------------------------------------

	//-----------------------------------------------------
	/// @brief
	//-----------------------------------------------------
	constexpr
	quaternion():
		v_{numeric_type(1),numeric_type(0),numeric_type(0),numeric_type(0)}
	{}

	//-----------------------------------------------------
	explicit constexpr
	quaternion(
		const_reference w,
		const_reference x, const_reference y, const_reference z):
		v_{w,x,y,z}
	{}

	//-----------------------------------------------------
	template<class W, class X, class Y, class Z, class = typename
		std::enable_if<
			is_number<W,X,Y,Z>::value>::type>
	explicit constexpr
	quaternion(const W& w, const X& x, const Y& y, const Z& z):
		v_{numeric_type(w), numeric_type(x), numeric_type(y), numeric_type(z)}
	{
		AM_CHECK_NARROWING4(numeric_type,W,X,Y,Z)
	}

	//-----------------------------------------------------
	constexpr
	quaternion(const quaternion&) = default;

	//-----------------------------------------------------
	/// @brief from other quaternion (different numeric_type), non-narrowing
	template<class T>
	explicit constexpr
	quaternion(const quaternion<T>& q):
		v_{
			numeric_type(q[0]),
			numeric_type(q[1]),
			numeric_type(q[2]),
			numeric_type(q[3]) }
	{
		AM_CHECK_NARROWING(numeric_type,T)
	}


	//---------------------------------------------------------------
	// ASSIGNMENT
	//---------------------------------------------------------------
	quaternion&
	operator = (const quaternion&) = default;

	//-----------------------------------------------------
	template<class T>
	quaternion&
	operator = (const quaternion<T>& q)
	{
		AM_CHECK_NARROWING(numeric_type,T)

		v_[0] = q[0];
		v_[1] = q[1];
		v_[2] = q[2];
		v_[3] = q[3];
		return *this;
	}

	//-----------------------------------------------------
	quaternion&
	assign(const_reference w,
		   const_reference x, const_reference y, const_reference z)
	{
		v_[0] = w;
		v_[1] = x;
		v_[2] = y;
		v_[3] = z;
		return *this;
	}
	//-----------------------------------------------------
	template<class W, class X, class Y, class Z, class = typename
		std::enable_if<is_number<W,X,Y,Z>::value>::type>
	quaternion&
	assign(const W& w, const X& x, const Y& y, const Z& z)
	{
		AM_CHECK_NARROWING4(numeric_type,W,X,Y,Z)

		v_[0] = numeric_type(w);
		v_[1] = numeric_type(x);
		v_[2] = numeric_type(y);
		v_[3] = numeric_type(z);
		return *this;
	}


	//---------------------------------------------------------------
	// ELEMENT ACCESS
	//---------------------------------------------------------------
	reference
	operator [] (dimension_type index) {
		return v_[index];
	}
	constexpr const_reference
	operator [] (dimension_type index) const noexcept {
		return v_[index];
	}

	//-----------------------------------------------------
	constexpr const_reference
	real() const noexcept {
		return v_[0];
	}
	//-----------------------------------------------------
	constexpr const_reference
	imag_i() const noexcept {
		return v_[1];
	}
	//-----------------------------------------------------
	constexpr const_reference
	imag_j() const noexcept {
		return v_[2];
	}
	//-----------------------------------------------------
	constexpr const_reference
	imag_k() const noexcept {
		return v_[3];
	}

	//---------------------------------------------------------------
	static constexpr dimension_type
	dimensions() noexcept {
		return 4;
	}


	//---------------------------------------------------------------
	// SPECIAL SETTERS
	//---------------------------------------------------------------
	quaternion&
	identity()
	{
		v_[0] = numeric_type(1);
		v_[1] = numeric_type(0);
		v_[2] = numeric_type(0);
		v_[3] = numeric_type(0);
		return *this;
	}

	//-----------------------------------------------------
	quaternion&
	conjugate() {
		low::conjugate_quat(v_);
		return *this;
	}
	//-----------------------------------------------------
	quaternion&
	invert() {
		low::invert_quat(v_);
		return *this;
	}
	//-----------------------------------------------------
	quaternion&
	normalize() {
		low::normalize_quat(v_);
		return *this;
	}


	//---------------------------------------------------------------
	// quaternion (op)= numeric_type
	//---------------------------------------------------------------
	template<class T, class = typename std::enable_if<
		is_number<T>::value>::type>
	quaternion&
	operator += (const T& v)
	{
		AM_CHECK_NARROWING(numeric_type,T)

		v_[0] += v;
		v_[1] += v;
		v_[2] += v;
		v_[3] += v;
		return *this;
	}
	//-----------------------------------------------------
	template<class T, class = typename std::enable_if<
		is_number<T>::value>::type>
	quaternion&
	operator -= (const T& v)
	{
		AM_CHECK_NARROWING(numeric_type,T)

		v_[0] -= v;
		v_[1] -= v;
		v_[2] -= v;
		v_[3] -= v;
		return *this;
	}
	//-----------------------------------------------------
	template<class T, class = typename std::enable_if<
		is_number<T>::value>::type>
	quaternion&
	operator *= (const T& v)
	{
		AM_CHECK_NARROWING(numeric_type,T)

		v_[0] *= v;
		v_[1] *= v;
		v_[2] *= v;
		v_[3] *= v;
		return *this;
	}
	//-----------------------------------------------------
	template<class T, class = typename std::enable_if<
		is_number<T>::value>::type>
	quaternion&
	operator /= (const T& v)
	{
		AM_CHECK_NARROWING(numeric_type,T)

		v_[0] /= v;
		v_[1] /= v;
		v_[2] /= v;
		v_[3] /= v;
		return *this;
	}


	//---------------------------------------------------------------
	// quaternion (op)= quaternion
	//---------------------------------------------------------------
	template<class T>
	quaternion&
	operator *= (const quaternion<T>& q)
	{
		AM_CHECK_NARROWING(numeric_type,T)

		auto old = *this;
		low::multiply_quat_quat(old, q, *this);
		return *this;
	}

	//---------------------------------------------------------
	template<class T>
	quaternion&
	times_conj(const quaternion<T>& q)
	{
		AM_CHECK_NARROWING(numeric_type,T)

		auto old = *this;
		low::multiply_quat_conj_quat(old, q, *this);
		return *this;
	}
	//---------------------------------------------------------
	template<class T>
	quaternion&
	conj_times(const quaternion<T>& q)
	{
		AM_CHECK_NARROWING(numeric_type,T)

		auto old = *this;
		low::multiply_conj_quat_quat(old, q, *this);
		return *this;
	}

	//-----------------------------------------------------
	template<class T>
	quaternion&
	operator += (const quaternion<T>& q)
	{
		AM_CHECK_NARROWING(numeric_type,T)

		v_[0] += q[0];
		v_[1] += q[1];
		v_[2] += q[2];
		v_[3] += q[3];
		return *this;
	}
	//-----------------------------------------------------
	template<class T>
	quaternion&
	operator -= (const quaternion<T>& q)
	{
		AM_CHECK_NARROWING(numeric_type,T)

		v_[0] -= q[0];
		v_[1] -= q[1];
		v_[2] -= q[2];
		v_[3] -= q[3];
		return *this;
	}

private:

	//---------------------------------------------------------------
	numeric_type v_[4];
};








/*****************************************************************************
 *
 *
 *
 * CONVENIENCE DEFINITIONS
 *
 *
 *
 *****************************************************************************/

//---------------------------------------------------------
using quatf = quaternion<float>;
using quatd = quaternion<double>;
using quat  = quaternion<real_t>;








/*****************************************************************************
 *
 *
 *
 * EXTERNAL ACCESSORS
 *
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<dims_t index, class T>
inline constexpr auto
get(const quaternion<T>& q) noexcept -> decltype(q[index])
{
	static_assert(index < 4, "get<i>(quaternion<T>) : (m >= 4) out of bounds!");

	return q[index];
}
//-----------------------------------------------------
template<dims_t index, class T>
inline auto
get(quaternion<T>& q) -> decltype(q[index])
{
	static_assert(index < 4, "get<i>(quaternion<T>) : (m >= 4) out of bounds!");

	return q[index];
}



//-------------------------------------------------------------------
template<class T>
inline constexpr auto
real(const quaternion<T>& q) noexcept -> decltype(q.real())
{
	return q.real();
}



//-------------------------------------------------------------------
template<class T>
inline constexpr auto
imag_i(const quaternion<T>& q) noexcept -> decltype(q.imag_i())
{
	return q.imag_i();
}

//---------------------------------------------------------
template<class T>
inline constexpr auto
imag_j(const quaternion<T>& q) noexcept -> decltype(q.imag_j())
{
	return q.imag_j();
}

//---------------------------------------------------------
template<class T>
inline constexpr auto
imag_k(const quaternion<T>& q) noexcept -> decltype(q.imag_k())
{
	return q.imag_k();
}






/*****************************************************************************
 *
 *
 * EXTERNAL ITERATOR-LIKE ACCESS
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
// FORWARD
//-------------------------------------------------------------------
template<class T>
inline constexpr T*
begin(quaternion<T>& q) noexcept
{
	return std::addressof(q[0]);
}
//-----------------------------------------------------
template<class T>
inline constexpr const T*
begin(const quaternion<T>& q) noexcept
{
	return std::addressof(q[0]);
}
//-----------------------------------------------------
template<class T>
inline constexpr const T*
cbegin(const quaternion<T>& q) noexcept
{
	return std::addressof(q[0]);
}

//-----------------------------------------------------
template<class T>
inline constexpr T*
end(quaternion<T>& q) noexcept
{
	return std::addressof(q[4]);
}
//-----------------------------------------------------
template<class T>
inline constexpr const T*
end(const quaternion<T>& q) noexcept
{
	return std::addressof(q[4]);
}
//-----------------------------------------------------
template<class T>
inline constexpr const T*
cend(const quaternion<T>& q) noexcept
{
	return std::addressof(q[4]);
}


//-------------------------------------------------------------------
// REVERSE
//-------------------------------------------------------------------
template<class T>
inline constexpr std::reverse_iterator<T*>
rbegin(quaternion<T>& q)
{
	return std::addressof(q[4]);
}
//-----------------------------------------------------
template<class T>
inline constexpr std::reverse_iterator<const T*>
rbegin(const quaternion<T>& q)
{
	return std::addressof(q[4]);
}
//-----------------------------------------------------
template<class T>
inline constexpr std::reverse_iterator<const T*>
crbegin(const quaternion<T>& q)
{
	return std::addressof(q[4]);
}

//-----------------------------------------------------
template<class T>
inline constexpr std::reverse_iterator<T*>
rend(quaternion<T>& q)
{
	return std::addressof(q[0]);
}
//-----------------------------------------------------
template<class T>
inline constexpr std::reverse_iterator<const T*>
rend(const quaternion<T>& q)
{
	return std::addressof(q[0]);
}
//-----------------------------------------------------
template<class T>
inline constexpr std::reverse_iterator<const T*>
crend(const quaternion<T>& q)
{
	return std::addressof(q[0]);
}






/*****************************************************************************
 *
 *
 * I/O
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class Ostream, class T>
inline Ostream&
operator << (Ostream& os, const quaternion<T>& q)
{
	return (os << q[0] <<" "<< q[1] <<" "<< q[2] <<" "<< q[3]);
}

//-------------------------------------------------------------------
template<class T, class Ostream>
inline Ostream&
print(Ostream& os, const quaternion<T>& q)
{
	return (os << "(" << q[0] <<","<< q[1] <<","<< q[2] <<","<< q[3] << ")");
}






/*****************************************************************************
 *
 *
 * ARITHMETIC
 *
 *
 *****************************************************************************/


//-------------------------------------------------------------------
// QUATERNION <-> SCALAR
//-------------------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<is_number<T1>::value>::type
>
constexpr quaternion<common_numeric_t<T1,T2>>
operator * (const T1& s, const quaternion<T2>& q)
{
	return quaternion<common_numeric_t<T1,T2>>
		{s*q.v_[0], s*q.v_[1], s*q.v_[2], s*q.v_[3]};
}



//-------------------------------------------------------------------
// QUATERNION <-> QUATERNION
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr quaternion<common_numeric_t<T1,T2>>
operator * (const quaternion<T1>& p, const quaternion<T2>& q)
{
	return low::multiply_quat_quat<
		quaternion<common_numeric_t<T1,T2>>>(p,q);
}
//---------------------------------------------------------
template<class T1, class T2>
inline constexpr
quaternion<common_numeric_t<T1,T2>>
operator + (const quaternion<T1>& a, const quaternion<T2>& b)
{
	return quaternion<common_numeric_t<T1,T2>>{
		a[0]+b[0], a[1]+b[1], a[2]+b[2], a[3]+b[3]};
}
//---------------------------------------------------------
template<class T1, class T2>
inline constexpr
quaternion<common_numeric_t<T1,T2>>
operator - (const quaternion<T1>& a, const quaternion<T2>& b)
{
	return quaternion<common_numeric_t<T1,T2>>{
		a[0]-b[0], a[1]-b[1], a[2]-b[2], a[3]-b[3]};
}

//---------------------------------------------------------
template<class T1, class T2>
inline quaternion<common_numeric_t<T1,T2>>
times_inverse(const quaternion<T1>& p, quaternion<T2> q)
{
	q.invert();
	return (p * q);
}
//---------------------------------------------------------
template<class T1, class T2>
inline quaternion<common_numeric_t<T1,T2>>
inverse_times(quaternion<T1> p, const quaternion<T2>& q)
{
	p.invert();
	return (p * q);
}

//---------------------------------------------------------
template<class T1, class T2>
inline constexpr quaternion<common_numeric_t<T1,T2>>
times_conj(const quaternion<T1>& p, const quaternion<T2>& q)
{
	return low::multiply_quat_conj_quat<
		quaternion<common_numeric_t<T1,T2>>>(p,q);
}

//---------------------------------------------------------
template<class T1, class T2>
inline constexpr quaternion<common_numeric_t<T1,T2>>
conj_times(const quaternion<T1>& p, const quaternion<T2>& q)
{
	return low::multiply_conj_quat_quat<
		quaternion<common_numeric_t<T1,T2>>>(p,q);
}






/*****************************************************************************
 *
 *
 * MODIFY
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
// INVERT
//-------------------------------------------------------------------
template<class T>
inline quaternion<T>
inverse(quaternion<T> q)
{
	q.invert();
	return q;
}


//-------------------------------------------------------------------
// CONJUGATE
//-------------------------------------------------------------------
template<class T>
inline constexpr quaternion<T>
conj(const quaternion<T>& q)
{
	return  quaternion<T>{q[0], -q[1], -q[2], -q[3]};
}


//-------------------------------------------------------------------
// NORMALIZE
//-------------------------------------------------------------------
template<class T>
inline quaternion<T>
normalized(quaternion<T> q)
{
	q.normalize();
	return q;
}






/*****************************************************************************
 *
 *
 * LENGTH /NORM
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T>
inline T
norm2(const quaternion<T>& q)
{
	return (q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
}

//---------------------------------------------------------
template<class T>
inline floating_point_t<T>
norm(const quaternion<T>& q)
{
	using std::sqrt;
	using real_t = floating_point_t<T> ;
	return sqrt(real_t(norm2(q)));
}

//---------------------------------------------------------
template<class T>
inline constexpr bool
is_normalized(const quaternion<T>& q)
{
	return approx_1(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
}






/*****************************************************************************
 *
 *
 * QUATERNION - QUATERNION OPERATIONS
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
// DISTANCE
//-------------------------------------------------------------------
template<class T1, class T2>
inline common_numeric_t<T1,T2>
norm2(const quaternion<T1>& a, const quaternion<T1>& b)
{
	return ( (a[0] - b[0]) * (a[0] - b[0]) +
		     (a[1] - b[1]) * (a[1] - b[1]) +
		     (a[2] - b[2]) * (a[2] - b[2]) +
		     (a[3] - b[3]) * (a[3] - b[3]) );
}
//---------------------------------------------------------
template<class T1, class T2>
inline floating_point_t<T1,T2>
norm(const quaternion<T1>& a, const quaternion<T1>& b)
{
	using real_t = floating_point_t<T1,T2> ;
	return sqrt(real_t(norm2(a, b)));
}




//-------------------------------------------------------------------
// dot
//-------------------------------------------------------------------
template<class T1, class T2>
inline common_numeric_t<T1,T2>
dot(const quaternion<T1>& a, const quaternion<T2>& b)
{
	return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3]);
}



//-------------------------------------------------------------------
// REAL PRODUCT
//-------------------------------------------------------------------
template<class T1, class T2>
quaternion<common_numeric_t<T1,T2>>
real_product(const quaternion<T1>& p, const quaternion<T2>& q)
{
	return quaternion<common_numeric_t<T1,T2>>{
		(real(p[0])*real(q[0])) - (real(p[1])*real(q[1])) - (real(p[2])*real(q[2])) - (real(p[3])*real(q[3])),
		(real(p[0])*real(q[1])) + (real(p[1])*real(q[0])) + (real(p[2])*real(q[3])) - (real(p[3])*real(q[2])),
		(real(p[0])*real(q[2])) - (real(p[1])*real(q[3])) + (real(p[2])*real(q[0])) + (real(p[3])*real(q[1])),
		(real(p[0])*real(q[3])) + (real(p[1])*real(q[2])) - (real(p[2])*real(q[1])) + (real(p[3])*real(q[0]))
	};
}

//---------------------------------------------------------
template<class T1, class T2, class T3>
void
real_product(
	const quaternion<T1>& p, const quaternion<T2>& q, quaternion<T3>& out)
{
	using num::real;

	AM_CHECK_NARROWING2(T3,T2,T1)

	out[0] = (real(p[0])*real(q[0])) - (real(p[1])*real(q[1])) - (real(p[2])*real(q[2])) - (real(p[3])*real(q[3]));
	out[1] = (real(p[0])*real(q[1])) + (real(p[1])*real(q[0])) + (real(p[2])*real(q[3])) - (real(p[3])*real(q[2]));
	out[2] = (real(p[0])*real(q[2])) - (real(p[1])*real(q[3])) + (real(p[2])*real(q[0])) + (real(p[3])*real(q[1]));
	out[3] = (real(p[0])*real(q[3])) + (real(p[1])*real(q[2])) - (real(p[2])*real(q[1])) + (real(p[3])*real(q[0]));
}



//-------------------------------------------------------------------
// IMAG PRODUCT
//-------------------------------------------------------------------
template<class T1, class T2>
quaternion<common_numeric_t<T1,T2>>
imag_product(const quaternion<T1>& p, const quaternion<T2>& q)
{
	return quaternion<common_numeric_t<T1,T2>>{
		(imag(p[0])*imag(q[0])) - (imag(p[1])*imag(q[1])) - (imag(p[2])*imag(q[2])) - (imag(p[3])*imag(q[3])),
		(imag(p[0])*imag(q[1])) + (imag(p[1])*imag(q[0])) + (imag(p[2])*imag(q[3])) - (imag(p[3])*imag(q[2])),
		(imag(p[0])*imag(q[2])) - (imag(p[1])*imag(q[3])) + (imag(p[2])*imag(q[0])) + (imag(p[3])*imag(q[1])),
		(imag(p[0])*imag(q[3])) + (imag(p[1])*imag(q[2])) - (imag(p[2])*imag(q[1])) + (imag(p[3])*imag(q[0]))
	};
}

//---------------------------------------------------------
template<class T1, class T2, class T3>
void
imag_product(
	const quaternion<T1>& p, const quaternion<T2>& q, quaternion<T3>& out)
{
	AM_CHECK_NARROWING2(T3,T2,T1)

	out[0] = (imag(p[0])*imag(q[0])) - (imag(p[1])*imag(q[1])) - (imag(p[2])*imag(q[2])) - (imag(p[3])*imag(q[3]));
	out[1] = (imag(p[0])*imag(q[1])) + (imag(p[1])*imag(q[0])) + (imag(p[2])*imag(q[3])) - (imag(p[3])*imag(q[2]));
	out[2] = (imag(p[0])*imag(q[2])) - (imag(p[1])*imag(q[3])) + (imag(p[2])*imag(q[0])) + (imag(p[3])*imag(q[1]));
	out[3] = (imag(p[0])*imag(q[3])) + (imag(p[1])*imag(q[2])) - (imag(p[2])*imag(q[1])) + (imag(p[3])*imag(q[0]));
}






/*****************************************************************************
 *
 *
 * GENERATION
 *
 *
 *****************************************************************************/


//-------------------------------------------------------------------
template<class T1, class T2, class T3, class T4, class = typename
	std::enable_if<is_number<T1,T2,T3,T4>::value>::type>
inline constexpr quaternion<common_numeric_t<T1,T2,T3,T4>>
make_quaternion(const T1& w, const T2& x, const T3& y, const T4& z)
{
	return quaternion<common_numeric_t<T1,T2,T3,T4>>{w,x,y,z};
}



//-------------------------------------------------------------------
// RANDOM UNIFORM
//-------------------------------------------------------------------
template<class T, class URNG>
inline quaternion<T>
random_unit_quaternion(URNG& urng)
{
	static_assert(is_number<T>::value,
		"random_unit_quaternion<T> : T has to be a number type");

	return low::random_unit_quat<quaternion<T>>(urng);
}



//-------------------------------------------------------------------
// spherical linear interpolation
//-------------------------------------------------------------------
template<class T1, class T2, class T3,
	class = typename std::enable_if<is_number<T3>::value>::type
>
inline quaternion<common_numeric_t<T1,T2,T3>>
lerp(
	const quaternion<T1>& qFrom, const quaternion<T2>& qTo, T3 t)
{
	quaternion<common_numeric_t<T1,T2,T3>> out;
	low::quat_lerp(qFrom, qTo, t, out);
	return out;
}



//-------------------------------------------------------------------
// spherical linear interpolation
//-------------------------------------------------------------------
template<class T1, class T2, class T3,
	class = typename std::enable_if<is_number<T3>::value>::type
>
inline quaternion<common_numeric_t<T1,T2,T3>>
slerp(
	const quaternion<T1>& qFrom, const quaternion<T2>& qTo, T3 t)
{
	quaternion<common_numeric_t<T1,T2,T3>> out;
	low::quat_slerp(qFrom, qTo, t, out);
	return out;
}



//-------------------------------------------------------------------
// spherical cubic interpolation
//-------------------------------------------------------------------
template<class T0, class T1, class T2, class T3, class T4, class =
	typename std::enable_if<is_number<T4>::value>::type
>
inline quaternion<common_numeric_t<T0,T1,T2,T3,T4>>
squad(
	const quaternion<T0>& q0, const quaternion<T1>& q1,
	const quaternion<T2>& q2, const quaternion<T3>& q3, T4 t)
{
	quaternion<common_numeric_t<T0,T1,T2,T3,T4>> out;
	low::quat_squad(q0, q1, q2, q3, t, out);
	return out;
}



//-------------------------------------------------------------------
// log function application
//-------------------------------------------------------------------
template<class T>
inline quaternion<T>
log(const quaternion<T>& q)
{
	quaternion<T> qOut;
	low::quat_log(q, qOut);
	return qOut;
}


//-------------------------------------------------------------------
/// @brief e^quat = exp(v*phi) = [cos(phi),vsin(phi)]
//-------------------------------------------------------------------
template<class T>
inline quaternion<T>
exp(const quaternion<T>& q)
{
	quaternion<T> qOut;
	low::quat_exp(q, qOut);
	return qOut;
}


//-------------------------------------------------------------------
/// @brief e^quat = exp(v*phi) = [cos(phi),vsin(phi)]
//-------------------------------------------------------------------
template<class T1, class T2>
inline quaternion<common_numeric_t<T1,T2>>
pow(const quaternion<T1>& q, const T2& exponent)
{
	return exp(log(q) * exponent);
}



}  // namespace num

}  // namespace am


#endif
