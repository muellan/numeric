/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2014 André Müller
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
        std::complex<T>{q.real().real(),   -q.real().imag()},
        std::complex<T>{q.imag_i().real(), -q.imag_i().imag()},
        std::complex<T>{q.imag_j().real(), -q.imag_j().imag()},
        std::complex<T>{q.imag_k().real(), -q.imag_k().imag()}
    };
}

//---------------------------------------------------------
template<class T>
inline constexpr
biquaternion<T>
full_conj(const biquaternion<T>& q)
{
    return biquaternion<T> {
        std::complex<T>{ q.real().real(),  -q.real().imag()},
        std::complex<T>{-q.imag_i().real(), q.imag_i().imag()},
        std::complex<T>{-q.imag_j().real(), q.imag_j().imag()},
        std::complex<T>{-q.imag_k().real(), q.imag_k().imag()}
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
    return quaternion<T>{dq.real().real(),
                         dq.imag_i().real(),
                         dq.imag_j().real(),
                         dq.imag_k().real()};
}



//-------------------------------------------------------------------
// DUAL PART
//-------------------------------------------------------------------
template<class T>
inline constexpr
quaternion<T>
imag(const biquaternion<T>& dq)
{
    return quaternion<T>{dq.real().imag(),
                         dq.imag_i().imag(),
                         dq.imag_j().imag(),
                         dq.imag_k().imag()};
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
    T1&& aw, T2&& ax, T3&& ay, T4&& az,
    T5&& bw, T6&& bx, T7&& by, T8&& bz)
{
    using res_t = common_numeric_t<T1,T2,T3,T4,T5,T6,T7,T8>;

    return biquaternion<res_t>{
        dual<res_t>{std::forward<T1>(aw), std::forward<T5>(bw)},
        dual<res_t>{std::forward<T2>(ax), std::forward<T6>(bx)},
        dual<res_t>{std::forward<T3>(ay), std::forward<T7>(by)},
        dual<res_t>{std::forward<T4>(az), std::forward<T8>(bz)} };
}


//---------------------------------------------------------
template<class T1, class T2>
inline constexpr
biquaternion<common_numeric_t<T1,T2>>
make_biquaternion(const quaternion<T1>& real, const quaternion<T2>& imag)
{
    using res_t = common_numeric_t<T1,T2>;

    return biquaternion<res_t>{
        std::complex<res_t>{real.real(),   imag.real()},
        std::complex<res_t>{real.imag_i(), imag.imag_i()},
        std::complex<res_t>{real.imag_j(), imag.imag_j()},
        std::complex<res_t>{real.imag_k(), imag.imag_k()}    };
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
