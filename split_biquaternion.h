#ifndef AM_NUMERIC_SPLIT_BIQUATERNION_H_
#define AM_NUMERIC_SPLIT_BIQUATERNION_H_


#include "quaternion.h"
#include "scomplex.h"


namespace num {


/*****************************************************************************
 *
 *
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T>
using split_biquaternion = quaternion<scomplex<T>>;


//-------------------------------------------------------------------
using split_biquatf = split_biquaternion<float>;
using split_biquatd = split_biquaternion<double>;
using split_biquat  = split_biquaternion<real_t>;






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
print(const split_biquaternion<T>& q, Ostream& os)
{
	os << "(";
	print(q[0], os); os << ",";
	print(q[1], os); os << ",";
	print(q[2], os); os << ",";
	print(q[3], os);
	os << ")";

	return os;
}



//-------------------------------------------------------------------
template<class T>
inline constexpr
split_biquaternion<T>
split_conj(const split_biquaternion<T>& q)
{
	return split_biquaternion<T> {
		scomplex<T>{q[0].real(), -q[0].imag()},
		scomplex<T>{q[1].real(), -q[1].imag()},
		scomplex<T>{q[2].real(), -q[2].imag()},
		scomplex<T>{q[3].real(), -q[3].imag()}
	};
}

//---------------------------------------------------------
template<class T>
inline constexpr
split_biquaternion<T>
full_conj(const split_biquaternion<T>& q)
{
	return split_biquaternion<T> {
		scomplex<T>{ q[0].real(),-q[0].imag()},
		scomplex<T>{-q[1].real(), q[1].imag()},
		scomplex<T>{-q[2].real(), q[2].imag()},
		scomplex<T>{-q[3].real(), q[3].imag()}
	};
}



//-------------------------------------------------------------------
// REAL PART
//-------------------------------------------------------------------
template<class T>
inline constexpr
quaternion<T>
real(const split_biquaternion<T>& dq)
{
	return quaternion<T>{dq[0].real(), dq[1].real(), dq[2].real(), dq[3].real()};
}



//-------------------------------------------------------------------
// DUAL PART
//-------------------------------------------------------------------
template<class T>
inline constexpr
quaternion<T>
imag(const split_biquaternion<T>& dq)
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
split_biquaternion<typename std::common_type<T1,T2,T3,T4,T5,T6,T7,T8>::type>
make_split_biquaternion(
	const T1& aw, const T2& ax, const T3& ay, const T4& az,
	const T5& bw, const T6& bx, const T7& by, const T8& bz)
{
	using res_t = typename std::common_type<T1,T2,T3,T4,T5,T6,T7,T8>::type;

	return split_biquaternion<res_t>{
		scomplex<res_t>{aw,bw},
		scomplex<res_t>{ax,bx},
		scomplex<res_t>{ay,by},
		scomplex<res_t>{az,bz}	};
}

//---------------------------------------------------------
template<class T1, class T2>
inline constexpr
split_biquaternion<typename std::common_type<T1,T2>::type>
make_split_biquaternion(const quaternion<T1>& real, const quaternion<T2>& imag)
{
	using res_t = typename std::common_type<T1,T2>::type;

	return split_biquaternion<res_t>{
		scomplex<res_t>{real[0], imag[0]},
		scomplex<res_t>{real[1], imag[1]},
		scomplex<res_t>{real[2], imag[2]},
		scomplex<res_t>{real[3], imag[3]}
	};
}

//---------------------------------------------------------
template<class T>
inline constexpr split_biquaternion<T>
make_split_biquaternion(const quaternion<T>& q)
{
	return split_biquaternion<T>{q};
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
real_product(const quaternion<T1>& p, const split_biquaternion<T2>& q)
{
	return quaternion<typename std::common_type<T1,T2>::type>{
		p[0]*q[0][0] - p[1]*q[1][0] - p[2]*q[2][0] - p[3]*q[3][0],
		p[0]*q[1][0] + p[1]*q[0][0] + p[2]*q[3][0] - p[3]*q[2][0],
		p[0]*q[2][0] - p[1]*q[3][0] + p[2]*q[0][0] + p[3]*q[1][0],
		p[0]*q[3][0] + p[1]*q[2][0] - p[2]*q[1][0] + p[3]*q[0][0]
	};
}

//---------------------------------------------------------
template<class T1, class T2, class T3, class =
	typename std::enable_if<is_non_narrowing<T3,T1,T2>::value,T1>::type>
void
real_product(
	const quaternion<T1>& p, const split_biquaternion<T2>& q,
	quaternion<T3>& out)
{
	out[0] = p[0]*q[0][0] - p[1]*q[1][0] - p[2]*q[2][0] - p[3]*q[3][0];
	out[1] = p[0]*q[1][0] + p[1]*q[0][0] + p[2]*q[3][0] - p[3]*q[2][0];
	out[2] = p[0]*q[2][0] - p[1]*q[3][0] + p[2]*q[0][0] + p[3]*q[1][0];
	out[3] = p[0]*q[3][0] + p[1]*q[2][0] - p[2]*q[1][0] + p[3]*q[0][0];
}

//---------------------------------------------------------
template<class T1, class T2>
split_biquaternion<typename std::common_type<T1,T2>::type>
real_product(const split_biquaternion<T1>& p, const split_biquaternion<T2>& q)
{
	return split_biquaternion<typename std::common_type<T1,T2>::type>{
		p[0][0]*q[0][0] - p[1][0]*q[1][0] - p[2][0]*q[2][0] - p[3][0]*q[3][0],
		p[0][0]*q[1][0] + p[1][0]*q[0][0] + p[2][0]*q[3][0] - p[3][0]*q[2][0],
		p[0][0]*q[2][0] - p[1][0]*q[3][0] + p[2][0]*q[0][0] + p[3][0]*q[1][0],
		p[0][0]*q[3][0] + p[1][0]*q[2][0] - p[2][0]*q[1][0] + p[3][0]*q[0][0]
	};
}



//-------------------------------------------------------------------
template<class T1, class T2>
split_biquaternion<typename std::common_type<T1,T2>::type>
imag_product(const split_biquaternion<T1>& p, const split_biquaternion<T2>& q)
{
	return split_biquaternion<typename std::common_type<T1,T2>::type>{
		p[0][1]*q[0][1] - p[1][1]*q[1][1] - p[2][1]*q[2][1] - p[3][1]*q[3][1],
		p[0][1]*q[1][1] + p[1][1]*q[0][1] + p[2][1]*q[3][1] - p[3][1]*q[2][1],
		p[0][1]*q[2][1] - p[1][1]*q[3][1] + p[2][1]*q[0][1] + p[3][1]*q[1][1],
		p[0][1]*q[3][1] + p[1][1]*q[2][1] - p[2][1]*q[1][1] + p[3][1]*q[0][1]
	};
}


}  // namespace num


#endif
