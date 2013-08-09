#ifndef AM_NUMERIC_BIQUATERNION_H_
#define AM_NUMERIC_BIQUATERNION_H_


#include <complex>

#include "quaternion.h"


namespace num {


/*****************************************************************************
 *
 *
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T>
using biquaternion = quaternion<std::complex<T>>;


//-------------------------------------------------------------------
using biquatf = biquaternion<float>;
using biquatd = biquaternion<double>;
using biquat  = biquaternion<real_t>;






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
biquaternion<T>
bi_conj(const biquaternion<T>& q)
{
	return biquaternion<T> {
		std::complex<T>{q[0].real(), -q[0].imag()},
		std::complex<T>{q[1].real(), -q[1].imag()},
		std::complex<T>{q[2].real(), -q[2].imag()},
		std::complex<T>{q[3].real(), -q[3].imag()}
	};
}

//---------------------------------------------------------
template<class T>
inline constexpr
biquaternion<T>
full_conj(const biquaternion<T>& q)
{
	return biquaternion<T> {
		std::complex<T>{ q[0].real(),-q[0].imag()},
		std::complex<T>{-q[1].real(), q[1].imag()},
		std::complex<T>{-q[2].real(), q[2].imag()},
		std::complex<T>{-q[3].real(), q[3].imag()}
	};
}



//-------------------------------------------------------------------
// REAL PART
//-------------------------------------------------------------------
template<class T>
inline constexpr
quaternion<T>
real(const biquaternion<T>& dq)
{
	return quaternion<T>{dq[0].real(), dq[1].real(), dq[2].real(), dq[3].real()};
}



//-------------------------------------------------------------------
// DUAL PART
//-------------------------------------------------------------------
template<class T>
inline constexpr
quaternion<T>
imag(const biquaternion<T>& dq)
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
biquaternion<typename std::common_type<T1,T2,T3,T4,T5,T6,T7,T8>::type>
make_biquaternion(
	const T1& aw, const T2& ax, const T3& ay, const T4& az,
	const T5& bw, const T6& bx, const T7& by, const T8& bz)
{
	using res_t = typename std::common_type<T1,T2,T3,T4,T5,T6,T7,T8>::type;

	return biquaternion<res_t>{
		std::complex<res_t>{aw,bw},
		std::complex<res_t>{ax,bx},
		std::complex<res_t>{ay,by},
		std::complex<res_t>{az,bz}	};
}

//---------------------------------------------------------
template<class T1, class T2>
inline constexpr
biquaternion<typename std::common_type<T1,T2>::type>
make_biquaternion(const quaternion<T1>& real, const quaternion<T2>& imag)
{
	using res_t = typename std::common_type<T1,T2>::type;

	return biquaternion<res_t>{
		std::complex<res_t>{real[0], imag[0]},
		std::complex<res_t>{real[1], imag[1]},
		std::complex<res_t>{real[2], imag[2]},
		std::complex<res_t>{real[3], imag[3]}	};
}

//---------------------------------------------------------
template<class T>
inline constexpr biquaternion<T>
make_biquaternion(const quaternion<T>& q)
{
	return biquaternion<T>{q};
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
real_product(const quaternion<T1>& p, const biquaternion<T2>& q)
{
	return quaternion<typename std::common_type<T1,T2>::type>{
		p[0]*q[0].real() - p[1]*q[1].real() - p[2]*q[2].real() - p[3]*q[3].real(),
		p[0]*q[1].real() + p[1]*q[0].real() + p[2]*q[3].real() - p[3]*q[2].real(),
		p[0]*q[2].real() - p[1]*q[3].real() + p[2]*q[0].real() + p[3]*q[1].real(),
		p[0]*q[3].real() + p[1]*q[2].real() - p[2]*q[1].real() + p[3]*q[0].real()
	};
}

//---------------------------------------------------------
template<class T1, class T2, class T3, class =
	typename std::enable_if<is_non_narrowing<T3,T1,T2>::value,T1>::type>
void
real_product(
	const quaternion<T1>& p, const biquaternion<T2>& q,	quaternion<T3>& out)
{
	out[0] = p[0]*q[0].real() - p[1]*q[1].real() - p[2]*q[2].real() - p[3]*q[3].real();
	out[1] = p[0]*q[1].real() + p[1]*q[0].real() + p[2]*q[3].real() - p[3]*q[2].real();
	out[2] = p[0]*q[2].real() - p[1]*q[3].real() + p[2]*q[0].real() + p[3]*q[1].real();
	out[3] = p[0]*q[3].real() + p[1]*q[2].real() - p[2]*q[1].real() + p[3]*q[0].real();
}

//-------------------------------------------------------------------
template<class T1, class T2>
biquaternion<typename std::common_type<T1,T2>::type>
real_product(const biquaternion<T1>& q1, const biquaternion<T2>& q2)
{
	return biquaternion<typename std::common_type<T1,T2>::type> {
		q1[0].real()*q2[0].real() - q1[1].real()*q2[1].real() - q1[2].real()*q2[2].real() - q1[3].real()*q2[3].real(),
		q1[0].real()*q2[1].real() + q1[1].real()*q2[0].real() + q1[2].real()*q2[3].real() - q1[3].real()*q2[2].real(),
		q1[0].real()*q2[2].real() - q1[1].real()*q2[3].real() + q1[2].real()*q2[0].real() + q1[3].real()*q2[1].real(),
		q1[0].real()*q2[3].real() + q1[1].real()*q2[2].real() - q1[2].real()*q2[1].real() + q1[3].real()*q2[0].real()
	};
}



//-------------------------------------------------------------------
template<class T1, class T2>
biquaternion<typename std::common_type<T1,T2>::type>
imag_product(const biquaternion<T1>& q1, const biquaternion<T2>& q2)
{
	return biquaternion<typename std::common_type<T1,T2>::type> {
		q1[0].imag()*q2[0].imag() - q1[1].imag()*q2[1].imag() - q1[2].imag()*q2[2].imag() - q1[3].imag()*q2[3].imag(),
		q1[0].imag()*q2[1].imag() + q1[1].imag()*q2[0].imag() + q1[2].imag()*q2[3].imag() - q1[3].imag()*q2[2].imag(),
		q1[0].imag()*q2[2].imag() - q1[1].imag()*q2[3].imag() + q1[2].imag()*q2[0].imag() + q1[3].imag()*q2[1].imag(),
		q1[0].imag()*q2[3].imag() + q1[1].imag()*q2[2].imag() - q1[2].imag()*q2[1].imag() + q1[3].imag()*q2[0].imag()
	};
}


}  // namespace num


#endif
