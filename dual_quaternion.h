/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/


#ifndef AM_NUM_NUMERIC_DUAL_QUATERNION_H_
#define AM_NUM_NUMERIC_DUAL_QUATERNION_H_


#include "quaternion.h"
#include "dual.h"


namespace num {


/*****************************************************************************
 *
 *
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T>
using dual_quaternion = quaternion<dual<T>>;






/*****************************************************************************
 *
 *
 *
 *
 *****************************************************************************/

namespace low {


//---------------------------------------------------------
template<class QuatIn, class DualQuat, class QuatOut>
void
multiply_quat_real_of_dual_quat(
	const QuatIn& p, const DualQuat& q, QuatOut& r)
{
	r[0] = p[0]*q[0][0] - p[1]*q[1][0] - p[2]*q[2][0] - p[3]*q[3][0];
	r[1] = p[0]*q[1][0] + p[1]*q[0][0] + p[2]*q[3][0] - p[3]*q[2][0];
	r[2] = p[0]*q[2][0] - p[1]*q[3][0] + p[2]*q[0][0] + p[3]*q[1][0];
	r[3] = p[0]*q[3][0] + p[1]*q[2][0] - p[2]*q[1][0] + p[3]*q[0][0];
}

//---------------------------------------------------------
template<int i, int j, class QuatIn, class DualQuat, class QuatOut>
void
multiply_dual_quat_parts(
	const QuatIn& p, const DualQuat& q, QuatOut& r)
{
	r[0] = p[0][i]*q[0][j] - p[1][i]*q[1][j] - p[2][i]*q[2][j] - p[3][i]*q[3][j];
	r[1] = p[0][i]*q[1][j] + p[1][i]*q[0][j] + p[2][i]*q[3][j] - p[3][i]*q[2][j];
	r[2] = p[0][i]*q[2][j] - p[1][i]*q[3][j] + p[2][i]*q[0][j] + p[3][i]*q[1][j];
	r[3] = p[0][i]*q[3][j] + p[1][i]*q[2][j] - p[2][i]*q[1][j] + p[3][i]*q[0][j];
}


}  // namespace low






/*****************************************************************************
 *
 *
 *
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T>
inline constexpr
dual_quaternion<T>
dual_conj(const dual_quaternion<T>& q)
{
	return dual_quaternion<T> {
		{q[0].real(), -q[0].imag()},
		{q[1].real(), -q[1].imag()},
		{q[2].real(), -q[2].imag()},
		{q[3].real(), -q[3].imag()}
	};
}

//---------------------------------------------------------
template<class T>
inline constexpr
dual_quaternion<T>
full_conj(const dual_quaternion<T>& q)
{
	return dual_quaternion<T> {
		{ q[0].real(),-q[0].imag()},
		{-q[1].real(), q[1].imag()},
		{-q[2].real(), q[2].imag()},
		{-q[3].real(), q[3].imag()}
	};
}



//-------------------------------------------------------------------
// REAL PART
//-------------------------------------------------------------------
template<class T>
inline constexpr
quaternion<T>
real(const dual_quaternion<T>& dq)
{
	return quaternion<T>{dq[0].real(), dq[1].real(), dq[2].real(), dq[3].real()};
}



//-------------------------------------------------------------------
// DUAL PART
//-------------------------------------------------------------------
template<class T>
inline constexpr
quaternion<T>
imag(const dual_quaternion<T>& dq)
{
	return quaternion<T>{dq[0].imag(), dq[1].imag(), dq[2].imag(), dq[3].imag()};
}






/*****************************************************************************
 *
 *
 * CREATION
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<
	class T1, class T2, class T3, class T4,
	class T5, class T6, class T7, class T8
>
inline constexpr
dual_quaternion<typename std::common_type<T1,T2,T3,T4,T5,T6,T7,T8>::type>
make_dual_quaternion(
	const T1& aw, const T2& ax, const T3& ay, const T4& az,
	const T5& bw, const T6& bx, const T7& by, const T8& bz)
{
	using res_t = typename std::common_type<T1,T2,T3,T4,T5,T6,T7,T8>::type;

	return dual_quaternion<res_t>{{aw,bw}, {ax,bx}, {ay,by}, {az,bz}};
}

//---------------------------------------------------------
template<class T1, class T2>
inline constexpr
dual_quaternion<typename std::common_type<T1,T2>::type>
make_dual(const quaternion<T1>& real, const quaternion<T2>& dual)
{
	return dual_quaternion<typename std::common_type<T1,T2>::type>{
		{real[0], dual[0]},
		{real[1], dual[1]},
		{real[2], dual[2]},
		{real[3], dual[3]}
	};
}

//---------------------------------------------------------
template<class T>
inline constexpr dual_quaternion<T>
make_dual(const quaternion<T>& q)
{
	return dual_quaternion<T>{q};
}






/*****************************************************************************
 *
 *
 * CREATION
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T1, class T2>
dual_quaternion<typename std::common_type<T1,T2>::type>
real_product(const dual_quaternion<T1>& q1, const dual_quaternion<T1>& q2)
{
	dual_quaternion<typename std::common_type<T1,T2>::type> out;
	low::multiply_dual_quat_parts<0,0>(q1, q2, out);
	return out;
}

//---------------------------------------------------------
template<class T1, class T2>
dual_quaternion<typename std::common_type<T1,T2>::type>
dual_product(const dual_quaternion<T1>& q1, const dual_quaternion<T1>& q2)
{
	dual_quaternion<typename std::common_type<T1,T2>::type> out;
	low::multiply_dual_quat_parts<1,1>(q1, q2, out);
	return out;
}


}  // namespace num


#endif
