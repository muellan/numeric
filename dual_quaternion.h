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
    print(os, q.real()); os << ",";
    print(os, q.imag_i()); os << ",";
    print(os, q.imag_j()); os << ",";
    print(os, q.imag_k());
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
        dual<T>{q.real().real(),   -q.real().imag()},
        dual<T>{q.imag_i().real(), -q.imag_i().imag()},
        dual<T>{q.imag_j().real(), -q.imag_j().imag()},
        dual<T>{q.imag_k().real(), -q.imag_k().imag()}
    };
}

//---------------------------------------------------------
template<class T>
inline constexpr
dual_quaternion<T>
full_conj(const dual_quaternion<T>& q)
{
    return dual_quaternion<T> {
        dual<T>{ q.real().real(),  -q.real().imag()},
        dual<T>{-q.imag_i().real(), q.imag_i().imag()},
        dual<T>{-q.imag_j().real(), q.imag_j().imag()},
        dual<T>{-q.imag_k().real(), q.imag_k().imag()}
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
imag(const dual_quaternion<T>& dq)
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
dual_quaternion<common_numeric_t<T1,T2,T3,T4,T5,T6,T7,T8>>
make_dual_quaternion(
    T1&& aw, T2&& ax, T3&& ay, T4&& az,
    T5&& bw, T6&& bx, T7&& by, T8&& bz)
{
    using res_t = common_numeric_t<T1,T2,T3,T4,T5,T6,T7,T8>;

    return dual_quaternion<res_t>{
        dual<res_t>{std::forward<T1>(aw), std::forward<T5>(bw)},
        dual<res_t>{std::forward<T2>(ax), std::forward<T6>(bx)},
        dual<res_t>{std::forward<T3>(ay), std::forward<T7>(by)},
        dual<res_t>{std::forward<T4>(az), std::forward<T8>(bz)} };
}

//---------------------------------------------------------
template<class T1, class T2>
inline constexpr
dual_quaternion<common_numeric_t<T1,T2>>
make_dual(const quaternion<T1>& real, const quaternion<T2>& imag)
{
    using res_t = common_numeric_t<T1,T2>;

    return dual_quaternion<res_t>{
        dual<res_t>{real.real(),   imag.real()},
        dual<res_t>{real.imag_i(), imag.imag_i()},
        dual<res_t>{real.imag_j(), imag.imag_j()},
        dual<res_t>{real.imag_k(), imag.imag_k()}
    };
}

//---------------------------------------------------------
template<class T>
inline constexpr dual_quaternion<T>
make_dual(const quaternion<T>& q)
{
    return dual_quaternion<T>{q};
}


}  // namespace num
}  // namespace am


#endif
