/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/


#ifndef AM_NUM_NUMERIC_QUATERNION_H_
#define AM_NUM_NUMERIC_QUATERNION_H_

#include <cmath>
#include <random>
#include <cstdint>

#include "constants.h"


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
template<class RandomEngine, class Quat>
void
make_random_unit_quat(RandomEngine& rnd, Quat& q)
{
	using std::cos;
	using std::sin;
	using std::sqrt;
	using std::generate_canonical;
	using std::numeric_limits;

	using q_t = typename std::decay<decltype(q[0])>::type;
	using lim_t = numeric_limits<q_t>;

	const auto u0 = generate_canonical<q_t,lim_t::digits>(rnd);
	const auto u1 = generate_canonical<q_t,lim_t::digits>(rnd) * q_t(2*pi);
	const auto u2 = generate_canonical<q_t,lim_t::digits>(rnd) * q_t(2*pi);
	const auto uA = sqrt(q_t(1) - u0);
	const auto uB = sqrt(u0);

	q[0] = uA * sin(u1);
	q[1] = uA * cos(u1);
	q[2] = uB * sin(u2);
	q[3] = uB * cos(u2);
}



//-------------------------------------------------------------------
// normalization
//-------------------------------------------------------------------
template<class Quat>
inline bool
quat_is_normalized(const Quat& q)
{
	return is_numerically_1(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
}

//-----------------------------------------------------
template<class Quat>
inline void
normalize_quat(Quat& q)
{
	using std::sqrt;

	using q_t = typename std::decay<decltype(q[0])>::type;

	auto norm = q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3];
	if(!is_numerically_1(norm)) {
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
template<class Quat1, class Quat2, class QuatR>
inline void
multiply_conj_quat_quat(const Quat1& p, const Quat2& q, QuatR& r)
{
	r[0] = p[0]*q[0] + p[1]*q[1] + p[2]*q[2] + p[3]*q[3];
	r[1] = p[0]*q[1] - p[1]*q[0] - p[2]*q[3] + p[3]*q[2];
	r[2] = p[0]*q[2] + p[1]*q[3] - p[2]*q[0] - p[3]*q[1];
	r[3] = p[0]*q[3] - p[1]*q[2] + p[2]*q[1] - p[3]*q[0];
}


//-------------------------------------------------------------------
// linear interpolation
//-------------------------------------------------------------------
template<class Quat, class T>
void
quat_linear_interpolation(const Quat& qFrom, const Quat& qTo, T t, Quat& out)
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
template<class Quat, class T>
void
quat_slerp(const Quat& q0, const Quat& q1, T t, Quat& out)
{
	assert((t >= 0) && (t <= 1));

	using std::sin;
	using std::acos;

	using q_t = typename std::decay<decltype(out[0])>::type;

	auto cosPhi = q0[0]*q1[0] + q0[1]*q1[1] + q0[2]*q1[2] + q0[3]*q1[3];

	if(cosPhi < 0) {
		q_t from, to;
		if((1 + cosPhi) > epsilon<q_t>::value) {
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
		if((1 - cosPhi) > epsilon<q_t>::value) {
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
template<class Quat, class T>
void
quat_squad(
	const Quat& q0, const Quat& q1, const Quat& q2, const Quat& q3,
	T t,
	Quat& out)
{
	assert((t >= 0) && (t <= 1));

	return quat_slerp(quat_slerp(q0,q3,t), quat_slerp(q1,q2,t), 2*t*(1-t), out);
}



//-------------------------------------------------------------------
// log function application
//-------------------------------------------------------------------
template<class Quat>
void
quat_log(const Quat& qIn, Quat& qOut)
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
template<class Quat>
void
quat_exp(const Quat& qIn, Quat& qOut)
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
	static_assert(is_number<NumberT>::value, "quaternion<T>: T must be a number");

public:
	//---------------------------------------------------------------
	// TYPES
	//---------------------------------------------------------------
	using numeric_type    = NumberT;
	using value_type      = numeric_type;
	using dimension_type  = std::uint8_t;
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
		const numeric_type& w,
		const numeric_type& x, const numeric_type& y, const numeric_type& z):
		v_{w,x,y,z}
	{}

	//-----------------------------------------------------
	template<class W, class X, class Y, class Z, class =
		typename std::enable_if<
			is_number<W,X,Y,Z>::value &&
			is_non_narrowing<numeric_type,W,X,Y,Z>::value>::type>
	explicit constexpr
	quaternion(const W& w, const X& x, const Y& y, const Z& z):
		v_{numeric_type(w), numeric_type(x), numeric_type(y), numeric_type(z)}
	{}

	//-----------------------------------------------------
	constexpr
	quaternion(const quaternion&) = default;

	//-----------------------------------------------------
	/// @brief from other quaternion (different numeric_type), non-narrowing
	template<class V, class =
		typename std::enable_if<is_non_narrowing<numeric_type,V>::value>::type>
	explicit constexpr
	quaternion(const quaternion<V>& q):
		v_{q[0], q[1], q[2], q[3]}
	{}


	//---------------------------------------------------------------
	// ASSIGNMENT
	//---------------------------------------------------------------
	quaternion&
	operator = (const quaternion&) = default;

	//-----------------------------------------------------
	template<class T, class =
		typename std::enable_if<is_non_narrowing<numeric_type,T>::value,T>::type>
	quaternion&
	operator = (const quaternion<T>& q)
	{
		v_[0] = q[0];
		v_[1] = q[1];
		v_[2] = q[2];
		v_[3] = q[3];
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
	operator [] (dimension_type index) const {
		return v_[index];
	}

	//-----------------------------------------------------
	reference
	real() {
		return v_[0];
	}
	constexpr const_reference
	real() const {
		return v_[0];
	}

	//-----------------------------------------------------
	reference
	imag(dimension_type index) {
		return v_[index+1];
	}
	//-----------------------------------------------------
	constexpr const_reference
	imag(dimension_type index) const {
		return v_[index+1];
	}

	//-----------------------------------------------------
	pointer
	imag() {
		return std::addressof(v_[1]);
	}
	//-----------------------------------------------------
	constexpr const_pointer
	imag() const {
		return std::addressof(v_[1]);
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
	identity() {
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
	quaternion&
	operator += (const numeric_type& v) {
		v_[0] += v;
		v_[1] += v;
		v_[2] += v;
		v_[3] += v;
		return *this;
	}
	//-----------------------------------------------------
	quaternion&
	operator -= (const numeric_type& v) {
		v_[0] -= v;
		v_[1] -= v;
		v_[2] -= v;
		v_[3] -= v;
		return *this;
	}
	//-----------------------------------------------------
	quaternion&
	operator *= (const numeric_type& v) {
		v_[0] *= v;
		v_[1] *= v;
		v_[2] *= v;
		v_[3] *= v;
		return *this;
	}
	//-----------------------------------------------------
	quaternion&
	operator /= (const numeric_type& v) {
		v_[0] /= v;
		v_[1] /= v;
		v_[2] /= v;
		v_[3] /= v;
		return *this;
	}

	//---------------------------------------------------------------
	friend constexpr quaternion
	operator * (numeric_type s, const quaternion& q)
	{
		return quaternion{s*q.v_[0], s*q.v_[1], s*q.v_[2], s*q.v_[3]};
	}


	//---------------------------------------------------------------
	// quaternion (op)= quaternion
	//---------------------------------------------------------------
	template<class T, class = typename std::enable_if<
		is_non_narrowing<numeric_type,T>::value,T>::type>
	quaternion&
	operator *= (const quaternion<T>& q) {
		auto old = *this;
		low::multiply_quat_quat(old, q, *this);
		return *this;
	}

	//---------------------------------------------------------
	template<class T, class = typename std::enable_if<
		is_non_narrowing<numeric_type,T>::value,T>::type>
	quaternion&
	times_conj(const quaternion<T>& q) {
		auto old = *this;
		low::multiply_quat_conj_quat(old, q, *this);
		return *this;
	}
	//---------------------------------------------------------
	template<class T, class = typename std::enable_if<
		is_non_narrowing<numeric_type,T>::value,T>::type>
	quaternion&
	conj_times(const quaternion<T>& q) {
		auto old = *this;
		low::multiply_conj_quat_quat(old, q, *this);
		return *this;
	}

	//-----------------------------------------------------
	template<class T, class = typename std::enable_if<
		is_non_narrowing<numeric_type,T>::value,T>::type>
	quaternion&
	operator += (const quaternion<T>& q) {
		v_[0] += q[0];
		v_[1] += q[1];
		v_[2] += q[2];
		v_[3] += q[3];
		return *this;
	}
	//-----------------------------------------------------
	template<class T, class = typename std::enable_if<
		is_non_narrowing<numeric_type,T>::value,T>::type>
	quaternion&
	operator -= (const quaternion<T>& q) {
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
// I/O
//-------------------------------------------------------------------
template<class CharT, class Traits, class T>
inline std::basic_ostream<CharT,Traits>&
operator << (std::basic_ostream<CharT,Traits>& os, const quaternion<T>& q)
{
	return (os << q[0] <<" "<< q[1] <<" "<< q[2] <<" "<< q[3]);
}



//-------------------------------------------------------------------
template<std::uint8_t index, class T>
inline constexpr const
typename std::enable_if<(index < 4),T>::type&
get(const quaternion<T>& q)
{
	return q[index];
}
//-----------------------------------------------------
template<std::uint8_t index, class T>
inline
typename std::enable_if<(index < 4),T>::type&
get(quaternion<T>& q)
{
	return q[index];
}


//---------------------------------------------------------
template<class T>
inline constexpr const T&
w(const quaternion<T>& q) {
	return q.w();
}
//-----------------------------------------------------
template<class T>
inline T&
w(quaternion<T>& q) {
	return q.w();
}


//---------------------------------------------------------
template<class T>
inline constexpr const T&
x(const quaternion<T>& q) {
	return q.x();
}
//-----------------------------------------------------
template<class T>
inline T&
x(quaternion<T>& q) {
	return q.x();
}


//---------------------------------------------------------
template<class T>
inline constexpr const T&
y(const quaternion<T>& q) {
	return q.y();
}
//-----------------------------------------------------
template<class T>
inline T&
y(quaternion<T>& q) {
	return q.y();
}



//---------------------------------------------------------
template<class T>
inline constexpr const T&
z(const quaternion<T>& q) {
	return q.z();
}
//-----------------------------------------------------
template<class T>
inline T&
z(quaternion<T>& q) {
	return q.z();
}



//-----------------------------------------------------
template<class T>
inline auto
real(quaternion<T>& q) -> decltype(q.real())
{
	return q.real();
}
//-----------------------------------------------------
template<class T>
inline constexpr auto
real(const quaternion<T>& q) -> decltype(q.real())
{
	return q.real();
}


//-----------------------------------------------------
template<class T>
inline auto
imag(quaternion<T>& q) -> decltype(q.imag())
{
	return q.imag();
}
//-----------------------------------------------------
template<class T>
inline constexpr auto
imag(const quaternion<T>& q) -> decltype(q.imag())
{
	return q.imag();
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
template<class T, class CharT, class Traits>
inline std::basic_ostream<CharT,Traits>&
print(const quaternion<T>& q, std::basic_ostream<CharT,Traits>& os)
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
// QUATERNION <-> QUATERNION
//-------------------------------------------------------------------
template<class T1, class T2>
inline quaternion<typename std::common_type<T1,T2>::type>
operator * (const quaternion<T1>& a, const quaternion<T2>& b)
{
	quaternion<typename std::common_type<T1,T2>::type> r;
	low::multiply_quat_quat(a, b, r);
	return r;
}
//---------------------------------------------------------
template<class T1, class T2>
inline constexpr
quaternion<typename std::common_type<T1,T2>::type>
operator + (const quaternion<T1>& a, const quaternion<T2>& b)
{
	return quaternion<typename std::common_type<T1,T2>::type>{
		a[0]+b[0], a[1]+b[1], a[2]+b[2], a[3]+b[3]};
}
//---------------------------------------------------------
template<class T1, class T2>
inline constexpr
quaternion<typename std::common_type<T1,T2>::type>
operator - (const quaternion<T1>& a, const quaternion<T2>& b)
{
	return quaternion<typename std::common_type<T1,T2>::type>{
		a[0]-b[0], a[1]-b[1], a[2]-b[2], a[3]-b[3]};
}

//---------------------------------------------------------
template<class T1, class T2>
inline quaternion<typename std::common_type<T1,T2>::type>
times_inverse(const quaternion<T1>& a, quaternion<T2> b)
{
	b.invert();
	quaternion<typename std::common_type<T1,T2>::type> r;
	low::multiply_quat_quat(a, b, r);
	return r;
}
//---------------------------------------------------------
template<class T1, class T2>
inline quaternion<typename std::common_type<T1,T2>::type>
inverse_times(quaternion<T1> a, const quaternion<T2>& b)
{
	a.invert();
	quaternion<typename std::common_type<T1,T2>::type> r;
	low::multiply_quat_quat(a, b, r);
	return r;
}

//---------------------------------------------------------
template<class T1, class T2>
inline quaternion<typename std::common_type<T1,T2>::type>
times_conj(const quaternion<T1>& a, const quaternion<T2>& b)
{
	quaternion<typename std::common_type<T1,T2>::type> r;
	low::multiply_quat_conj_quat(a, b, r);
	return r;
}

//---------------------------------------------------------
template<class T1, class T2>
inline quaternion<typename std::common_type<T1,T2>::type>
conj_times(const quaternion<T1>& a, const quaternion<T2>& b)
{
	quaternion<typename std::common_type<T1,T2>::type> r;
	low::multiply_conj_quat_quat(a, b, r);
	return r;
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
inline quaternion<T>
conj(const quaternion<T>& q)
{
	return  quaternion<T>{q[0], -q[1], -q[2], -q[3]};
}


//-------------------------------------------------------------------
// NORMALIZE
//-------------------------------------------------------------------
template<class T>
inline void
normalize(quaternion<T>& q)
{
	q.normalize();
}

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
norm_squared(const quaternion<T>& q)
{
	return (q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
}

//---------------------------------------------------------
template<class T>
inline typename std::common_type<T,real_t>::type
norm(const quaternion<T>& q)
{
	using std::sqrt;
	using real_t = typename std::common_type<T,real_t>::type ;
	return sqrt(real_t(norm_squared(q)));
}

//---------------------------------------------------------
template<class T>
inline constexpr bool
is_normalized(const quaternion<T>& q)
{
	return is_numerically_1(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
}

//---------------------------------------------------------
template<class T>
inline T
abs_squared(const quaternion<T>& q)
{
	return norm_squared(q);
}

//---------------------------------------------------------
template<class T>
inline typename std::common_type<T,real_t>::type
abs(const quaternion<T>& q)
{
	return norm(q);
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
inline typename std::common_type<T1,T2>::type
distance_squared(const quaternion<T1>& a, const quaternion<T1>& b)
{
	return ( (a[0] - b[0]) * (a[0] - b[0]) +
		     (a[1] - b[1]) * (a[1] - b[1]) +
		     (a[2] - b[2]) * (a[2] - b[2]) +
		     (a[3] - b[3]) * (a[3] - b[3]) );
}
//---------------------------------------------------------
template<class T1, class T2>
inline typename std::common_type<T1,T2,real_t>::type
distance(const quaternion<T1>& a, const quaternion<T1>& b)
{
	using real_t = typename std::common_type<T1,T2,real_t>::type ;
	return sqrt(real_t(distance_squared(a, b)));
}




//-------------------------------------------------------------------
// dot
//-------------------------------------------------------------------
template<class T1, class T2>
inline typename std::common_type<T1,T2>::type
dot(const quaternion<T1>& a, const quaternion<T2>& b)
{
	return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3]);
}






/*****************************************************************************
 *
 *
 * GENERATION
 *
 *
 *****************************************************************************/


//-------------------------------------------------------------------
template<class T>
inline constexpr quaternion<T>
make_quaternion(const T& w, const T& x, const T& y, const T& z)
{
	return quaternion<T>{w,x,y,z};
}



//-------------------------------------------------------------------
// RANDOM UNIFORM
//-------------------------------------------------------------------
template<class T, class RandomEngine>
inline quaternion<T>
make_random_unit_quaternion(RandomEngine& random)
{
	quaternion<T> q;
	low::make_random_unit_quat(random, q);
	return q;
}

//---------------------------------------------------------
template<class T, class RandomEngine>
inline void
set_random_unit_quaternion(RandomEngine& random, quaternion<T>& q)
{
	low::make_random_unit_quat(random, q);
}



//-------------------------------------------------------------------
// spherical linear interpolation
//-------------------------------------------------------------------
template<class T>
inline void
linear_interpolation(const quaternion<T>& qFrom, const quaternion<T>& qTo, T t,
	quaternion<T>& out)
{
	low::quat_linear_interpolation(qFrom, qTo, t, out);
}
//-----------------------------------------------------
template<class T>
inline quaternion<T>
linear_interpolation(const quaternion<T>& qFrom, const quaternion<T>& qTo, T t)
{
	quaternion<T> out;
	low::quat_linear_interpolation(qFrom, qTo, t, out);
	return out;
}



//-------------------------------------------------------------------
// spherical linear interpolation
//-------------------------------------------------------------------
template<class T>
inline void
slerp(const quaternion<T>& qFrom, const quaternion<T>& qTo, T t,
	quaternion<T>& out)
{
	low::quat_slerp(qFrom, qTo, t, out);
}
//-----------------------------------------------------
template<class T>
inline quaternion<T>
slerp(const quaternion<T>& qFrom, const quaternion<T>& qTo, T t)
{
	quaternion<T> out;
	low::quat_slerp(qFrom, qTo, t, out);
	return out;
}



//-------------------------------------------------------------------
// spherical cubic interpolation
//-------------------------------------------------------------------
template<class T>
inline void
squad(
	const quaternion<T>& q0, const quaternion<T>& q1,
	const quaternion<T>& q2, const quaternion<T>& q3, T t,
	quaternion<T>& out)
{
	low::quat_squad(q0, q1, q2, q3, t, out);
}
//-----------------------------------------------------
template<class T>
inline quaternion<T>
squad(
	const quaternion<T>& q0, const quaternion<T>& q1,
	const quaternion<T>& q2, const quaternion<T>& q3, T t)
{
	quaternion<T> out;
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
template<class T>
inline quaternion<T>
pow(const quaternion<T>& q, const T& exponent)
{
	return exp(log(q) * exponent);
}



}  // namespace num


#endif
