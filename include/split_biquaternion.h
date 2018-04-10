/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/

#ifndef AMLIB_NUMERIC_SPLIT_BIQUATERNION_H_
#define AMLIB_NUMERIC_SPLIT_BIQUATERNION_H_


#include "quaternion.h"
#include "scomplex.h"


namespace am {
namespace num {


/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class T>
using split_biquaternion = quaternion<scomplex<T>>;

using split_biquatf = split_biquaternion<float>;
using split_biquatd = split_biquaternion<double>;
using split_biquat  = split_biquaternion<real_t>;




/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class T, class Ostream>
inline Ostream&
print(Ostream& os, const split_biquaternion<T>& q)
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
inline constexpr auto
split_conj(const split_biquaternion<T>& q)
{
    return split_biquaternion<T> {
            scomplex<T>{q.real().real(),   -q.real().imag()  },
            scomplex<T>{q.imag_i().real(), -q.imag_i().imag()},
            scomplex<T>{q.imag_j().real(), -q.imag_j().imag()},
            scomplex<T>{q.imag_k().real(), -q.imag_k().imag()}
        };
}

//---------------------------------------------------------
template<class T>
inline constexpr auto
full_conj(const split_biquaternion<T>& q)
{
    return split_biquaternion<T> {
            scomplex<T>{ q.real().real(),  -q.real().imag()  },
            scomplex<T>{-q.imag_i().real(), q.imag_i().imag()},
            scomplex<T>{-q.imag_j().real(), q.imag_j().imag()},
            scomplex<T>{-q.imag_k().real(), q.imag_k().imag()}
        };
}



//-------------------------------------------------------------------
template<class T>
inline constexpr auto
real(const split_biquaternion<T>& dq)
{
    return quaternion<T>{dq.real().real(),
                         dq.imag_i().real(),
                         dq.imag_j().real(),
                         dq.imag_k().real()};
}



//-------------------------------------------------------------------
template<class T>
inline constexpr auto
imag(const split_biquaternion<T>& dq)
{
    return quaternion<T>{dq.real().imag(),
                         dq.imag_i().imag(),
                         dq.imag_j().imag(),
                         dq.imag_k().imag() };
}




/*****************************************************************************
 *
 * CREATION
 *
 *****************************************************************************/
template<
    class T1, class T2, class T3, class T4,
    class T5, class T6, class T7, class T8
>
inline constexpr auto
make_split_biquaternion(
    const T1& aw, const T2& ax, const T3& ay, const T4& az,
    const T5& bw, const T6& bx, const T7& by, const T8& bz)
{
    using res_t = common_numeric_t<T1,T2,T3,T4,T5,T6,T7,T8>;

    return split_biquaternion<res_t>{
            scomplex<res_t>{aw, bw},
            scomplex<res_t>{ax, bx},
            scomplex<res_t>{ay, by},
            scomplex<res_t>{az, bz}
        };
}


//---------------------------------------------------------
template<class T1, class T2>
inline constexpr auto
make_split_biquaternion(const quaternion<T1>& real, const quaternion<T2>& imag)
{
    using res_t = common_numeric_t<T1,T2>;

    return split_biquaternion<res_t>{
            scomplex<res_t>{real.real(),   imag.real()},
            scomplex<res_t>{real.imag_i(), imag.imag_i()},
            scomplex<res_t>{real.imag_j(), imag.imag_j()},
            scomplex<res_t>{real.imag_k(), imag.imag_k()} 
        };
}

//---------------------------------------------------------
template<class T>
inline constexpr auto
make_split_biquaternion(const quaternion<T>& q)
{
    return split_biquaternion<T>{q};
}


}  // namespace num
}  // namespace am

#endif
