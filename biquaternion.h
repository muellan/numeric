/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_BIQUATERNION_H_
#define AM_NUMERIC_BIQUATERNION_H_


#include <complex>

#include "quaternion.h"


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
biquaternion<common_numeric_t<T1,T2,T3,T4,T5,T6,T7,T8>>
make_biquaternion(
	const T1& aw, const T2& ax, const T3& ay, const T4& az,
	const T5& bw, const T6& bx, const T7& by, const T8& bz)
{
	using res_t = common_numeric_t<T1,T2,T3,T4,T5,T6,T7,T8>;

	return biquaternion<res_t>{
		std::complex<res_t>{aw,bw},
		std::complex<res_t>{ax,bx},
		std::complex<res_t>{ay,by},
		std::complex<res_t>{az,bz}	};
}

//---------------------------------------------------------
template<class T1, class T2>
inline constexpr
biquaternion<common_numeric_t<T1,T2>>
make_biquaternion(const quaternion<T1>& real, const quaternion<T2>& imag)
{
	using res_t = common_numeric_t<T1,T2>;

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


}  // namespace num

}  // namespace am


#endif
