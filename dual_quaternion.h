/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_DUAL_QUATERNION_H_
#define AM_NUMERIC_DUAL_QUATERNION_H_


#include "quaternion.h"
#include "dual.h"




namespace am {

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


//-------------------------------------------------------------------
using dual_quatf = dual_quaternion<float>;
using dual_quatd = dual_quaternion<double>;
using dual_quat  = dual_quaternion<real_t>;






/*****************************************************************************
 *
 *
 *
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
// I/O
//-------------------------------------------------------------------
template<class T, class Ostream>
inline Ostream&
print(Ostream& os, const dual_quaternion<T>& q)
{
	os << "(";
	print(os, q[0]); os << ",";
	print(os, q[1]); os << ",";
	print(os, q[2]); os << ",";
	print(os, q[3]);
	os << ")";

	return os;
}



//-------------------------------------------------------------------
template<class T>
inline constexpr
dual_quaternion<T>
dual_conj(const dual_quaternion<T>& q)
{
	return dual_quaternion<T> {
		dual<T>{q[0].real(), -q[0].imag()},
		dual<T>{q[1].real(), -q[1].imag()},
		dual<T>{q[2].real(), -q[2].imag()},
		dual<T>{q[3].real(), -q[3].imag()}
	};
}

//---------------------------------------------------------
template<class T>
inline constexpr
dual_quaternion<T>
full_conj(const dual_quaternion<T>& q)
{
	return dual_quaternion<T> {
		dual<T>{ q[0].real(),-q[0].imag()},
		dual<T>{-q[1].real(), q[1].imag()},
		dual<T>{-q[2].real(), q[2].imag()},
		dual<T>{-q[3].real(), q[3].imag()}
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

	return dual_quaternion<res_t>{
		dual<res_t>{aw,bw},
		dual<res_t>{ax,bx},
		dual<res_t>{ay,by},
		dual<res_t>{az,bz}	};
}

//---------------------------------------------------------
template<class T1, class T2>
inline constexpr
dual_quaternion<typename std::common_type<T1,T2>::type>
make_dual(const quaternion<T1>& real, const quaternion<T2>& imag)
{
	using res_t = typename std::common_type<T1,T2>::type;

	return dual_quaternion<res_t>{
		dual<res_t>{real[0], imag[0]},
		dual<res_t>{real[1], imag[1]},
		dual<res_t>{real[2], imag[2]},
		dual<res_t>{real[3], imag[3]}
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
 * SPECIAL ARITHMETIC
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T1, class T2>
quaternion<typename std::common_type<T1,T2>::type>
real_product(const quaternion<T1>& p, const dual_quaternion<T2>& q)
{
	return quaternion<typename std::common_type<T1,T2>::type>{
		p[0]*q[0][0] - p[1]*q[1][0] - p[2]*q[2][0] - p[3]*q[3][0],
		p[0]*q[1][0] + p[1]*q[0][0] + p[2]*q[3][0] - p[3]*q[2][0],
		p[0]*q[2][0] - p[1]*q[3][0] + p[2]*q[0][0] + p[3]*q[1][0],
		p[0]*q[3][0] + p[1]*q[2][0] - p[2]*q[1][0] + p[3]*q[0][0]
	};
}

//---------------------------------------------------------
template<class T1, class T2, class T3>
void
real_product(
	const quaternion<T1>& p, const dual_quaternion<T2>& q,
	quaternion<T3>& out)
{
	AM_CHECK_NARROWING2(T3,T2,T1)

	out[0] = p[0]*q[0][0] - p[1]*q[1][0] - p[2]*q[2][0] - p[3]*q[3][0];
	out[1] = p[0]*q[1][0] + p[1]*q[0][0] + p[2]*q[3][0] - p[3]*q[2][0];
	out[2] = p[0]*q[2][0] - p[1]*q[3][0] + p[2]*q[0][0] + p[3]*q[1][0];
	out[3] = p[0]*q[3][0] + p[1]*q[2][0] - p[2]*q[1][0] + p[3]*q[0][0];
}

//---------------------------------------------------------
template<class T1, class T2>
dual_quaternion<typename std::common_type<T1,T2>::type>
real_product(const dual_quaternion<T1>& p, const dual_quaternion<T2>& q)
{
	return dual_quaternion<typename std::common_type<T1,T2>::type>{
		p[0][0]*q[0][0] - p[1][0]*q[1][0] - p[2][0]*q[2][0] - p[3][0]*q[3][0],
		p[0][0]*q[1][0] + p[1][0]*q[0][0] + p[2][0]*q[3][0] - p[3][0]*q[2][0],
		p[0][0]*q[2][0] - p[1][0]*q[3][0] + p[2][0]*q[0][0] + p[3][0]*q[1][0],
		p[0][0]*q[3][0] + p[1][0]*q[2][0] - p[2][0]*q[1][0] + p[3][0]*q[0][0]
	};
}



//-------------------------------------------------------------------
template<class T1, class T2>
dual_quaternion<typename std::common_type<T1,T2>::type>
imag_product(const dual_quaternion<T1>& p, const dual_quaternion<T2>& q)
{
	return dual_quaternion<typename std::common_type<T1,T2>::type>{
		p[0][1]*q[0][1] - p[1][1]*q[1][1] - p[2][1]*q[2][1] - p[3][1]*q[3][1],
		p[0][1]*q[1][1] + p[1][1]*q[0][1] + p[2][1]*q[3][1] - p[3][1]*q[2][1],
		p[0][1]*q[2][1] - p[1][1]*q[3][1] + p[2][1]*q[0][1] + p[3][1]*q[1][1],
		p[0][1]*q[3][1] + p[1][1]*q[2][1] - p[2][1]*q[1][1] + p[3][1]*q[0][1]
	};
}


} //namespace num
} //namespace am




#endif
