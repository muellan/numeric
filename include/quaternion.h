/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/

#ifndef AMLIB_NUMERIC_QUATERNION_H_
#define AMLIB_NUMERIC_QUATERNION_H_

#include <cmath>
#include <random>
#include <cstdint>
#include <cassert>

#include "constants.h"
#include "limits.h"
#include "narrowing.h"
#include "conversion.h"


namespace am {
namespace num {



/*************************************************************************//***
 *
 *  @brief  quaternion
 *
 *
 *****************************************************************************/
template<class NumberT>
class quaternion
{
    static_assert(
        is_floating_point<NumberT>::value,
        "quaternion<T>: T must be a floating-point number type");

    //befriend all quaternions
    template<class T> friend class quaternion;


public:
    //---------------------------------------------------------------
    using numeric_type    = NumberT;
    using value_type      = numeric_type;


    //---------------------------------------------------------------
    constexpr
    quaternion():
        w_(numeric_type(1)),
        x_(numeric_type(0)),
        y_(numeric_type(0)),
        z_(numeric_type(0))
    {}

    //-----------------------------------------------------
    template<class W, class X, class Y, class Z, class = typename
        std::enable_if<is_number<W,X,Y,Z>::value>::type>
    explicit constexpr
    quaternion(W&& w, X&& x, Y&& y, Z&& z):
        w_(std::forward<W>(w)),
        x_(std::forward<X>(x)),
        y_(std::forward<Y>(y)),
        z_(std::forward<Z>(z))
    {
        AM_CHECK_NARROWING4(numeric_type,W,X,Y,Z)
    }

    //-----------------------------------------------------
    constexpr
    quaternion(const quaternion&) = default;
    //-----------------------------------------------------
    constexpr
    quaternion(quaternion&&) = default;

    //-----------------------------------------------------
    /// @brief from other quaternion (different numeric_type), non-narrowing
    template<class T>
    explicit constexpr
    quaternion(const quaternion<T>& q):
        w_(q.real()),
        x_(q.imag_i()),
        y_(q.imag_j()),
        z_(q.imag_k())
    {
        AM_CHECK_NARROWING(numeric_type,T)
    }


    //---------------------------------------------------------------
    // ASSIGNMENT
    //---------------------------------------------------------------
    quaternion&
    operator = (const quaternion&) = default;

    //-----------------------------------------------------
    quaternion&
    operator = (quaternion&&) = default;

    //-----------------------------------------------------
    template<class T>
    quaternion&
    operator = (const quaternion<T>& q)
    {
        AM_CHECK_NARROWING(numeric_type,T)

        w_ = q.real();
        x_ = q.imag_i();
        y_ = q.imag_j();
        z_ = q.imag_k();
        return *this;
    }
    //-----------------------------------------------------
    template<class T>
    quaternion&
    operator = (quaternion<T>&& q)
    {
        AM_CHECK_NARROWING(numeric_type,T)

        w_ = std::move(q.real());
        x_ = std::move(q.imag_i());
        y_ = std::move(q.imag_j());
        z_ = std::move(q.imag_k());
        return *this;
    }


    //---------------------------------------------------------------
    constexpr const value_type&
    real() const noexcept {
        return w_;
    }
    //-----------------------------------------------------
    constexpr const value_type&
    imag_i() const noexcept {
        return x_;
    }
    //-----------------------------------------------------
    constexpr const value_type&
    imag_j() const noexcept {
        return y_;
    }
    //-----------------------------------------------------
    constexpr const value_type&
    imag_k() const noexcept {
        return z_;
    }


    //---------------------------------------------------------------
    void
    real(const value_type& w) {
        w_ = w;
    }
    //-----------------------------------------------------
    void
    imag_i(const value_type& x) {
        x_ = x;
    }
    //-----------------------------------------------------
    void
    imag_j(const value_type& y) {
        y_ = y;
    }
    //-----------------------------------------------------
    void
    imag_k(const value_type& z) {
        z_ = z;
    }


    //---------------------------------------------------------------
    static constexpr int
    dimensions() noexcept {
        return 4;
    }


    //---------------------------------------------------------------
    // SPECIAL SETTERS
    //---------------------------------------------------------------
    quaternion&
    identity()
    {
        w_ = numeric_type(1);
        x_ = numeric_type(0);
        y_ = numeric_type(0);
        z_ = numeric_type(0);
        return *this;
    }

    //-----------------------------------------------------
    quaternion&
    conjugate() {
        x_ = -x_;
        y_ = -y_;
        z_ = -z_;
        return *this;
    }
    //-----------------------------------------------------
    quaternion&
    invert() {
        conjugate();
        normalize();
        return *this;
    }
    //-----------------------------------------------------
    quaternion&
    normalize() {
        using std::sqrt;

        auto norm = w_*w_ + x_*x_ + y_*y_ + z_*z_;
        if(!approx_1(norm)) {
            norm = numeric_type(1) / sqrt(norm);
            w_ *= norm;
            x_ *= norm;
            y_ *= norm;
            z_ *= norm;
        }
        return *this;
    }


    //---------------------------------------------------------------
    // quaternion (op)= numeric_type
    //---------------------------------------------------------------
    template<class T, class = typename
        std::enable_if<is_number<T>::value>::type>
    quaternion&
    operator += (const T& v)
    {
        AM_CHECK_NARROWING(numeric_type,T)

        w_ += v;
        x_ += v;
        y_ += v;
        z_ += v;
        return *this;
    }
    //-----------------------------------------------------
    template<class T, class = typename
        std::enable_if<is_number<T>::value>::type>
    quaternion&
    operator -= (const T& v)
    {
        AM_CHECK_NARROWING(numeric_type,T)

        w_ -= v;
        x_ -= v;
        y_ -= v;
        z_ -= v;
        return *this;
    }
    //-----------------------------------------------------
    template<class T, class = typename
        std::enable_if<is_number<T>::value>::type>
    quaternion&
    operator *= (const T& v)
    {
        AM_CHECK_NARROWING(numeric_type,T)

        w_ *= v;
        x_ *= v;
        y_ *= v;
        z_ *= v;
        return *this;
    }
    //-----------------------------------------------------
    template<class T, class = typename
        std::enable_if<is_number<T>::value>::type>
    quaternion&
    operator /= (const T& v)
    {
        AM_CHECK_NARROWING(numeric_type,T)

        w_ /= v;
        x_ /= v;
        y_ /= v;
        z_ /= v;
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

        const auto p = *this;

        w_ = p.real()*q.real()   - p.imag_i()*q.imag_i() - p.imag_j()*q.imag_j() - p.imag_k()*q.imag_k();
        x_ = p.real()*q.imag_i() + p.imag_i()*q.real()   + p.imag_j()*q.imag_k() - p.imag_k()*q.imag_j();
        y_ = p.real()*q.imag_j() - p.imag_i()*q.imag_k() + p.imag_j()*q.real()   + p.imag_k()*q.imag_i();
        z_ = p.real()*q.imag_k() + p.imag_i()*q.imag_j() - p.imag_j()*q.imag_i() + p.imag_k()*q.real();

        return *this;
    }

    //---------------------------------------------------------
    template<class T>
    quaternion&
    times_conj(const quaternion<T>& q)
    {
        AM_CHECK_NARROWING(numeric_type,T)

        const auto p = *this;

        w_ =  p.real()*q.real()   + p.imag_i()*q.imag_i() + p.imag_j()*q.imag_j() + p.imag_k()*q.imag_k();
        x_ = -p.real()*q.imag_i() + p.imag_i()*q.real()   - p.imag_j()*q.imag_k() + p.imag_k()*q.imag_j();
        y_ = -p.real()*q.imag_j() + p.imag_i()*q.imag_k() + p.imag_j()*q.real()   - p.imag_k()*q.imag_i();
        z_ = -p.real()*q.imag_k() - p.imag_i()*q.imag_j() + p.imag_j()*q.imag_i() + p.imag_k()*q.real();

        return *this;
    }
    //---------------------------------------------------------
    template<class T>
    quaternion&
    conj_times(const quaternion<T>& q)
    {
        AM_CHECK_NARROWING(numeric_type,T)

        const auto p = *this;

        w_ = p.real()*q.real()   + p.imag_i()*q.imag_i() + p.imag_j()*q.imag_j() + p.imag_k()*q.imag_k();
        x_ = p.real()*q.imag_i() - p.imag_i()*q.real()   - p.imag_j()*q.imag_k() + p.imag_k()*q.imag_j();
        y_ = p.real()*q.imag_j() + p.imag_i()*q.imag_k() - p.imag_j()*q.real()   - p.imag_k()*q.imag_i();
        z_ = p.real()*q.imag_k() - p.imag_i()*q.imag_j() + p.imag_j()*q.imag_i() - p.imag_k()*q.real();

        return *this;
    }

    //-----------------------------------------------------
    template<class T>
    quaternion&
    operator += (const quaternion<T>& q)
    {
        AM_CHECK_NARROWING(numeric_type,T)

        w_ += q.real();
        x_ += q.imag_i();
        y_ += q.imag_j();
        z_ += q.imag_k();
        return *this;
    }
    //-----------------------------------------------------
    template<class T>
    quaternion&
    operator -= (const quaternion<T>& q)
    {
        AM_CHECK_NARROWING(numeric_type,T)

        w_ -= q.real();
        x_ -= q.imag_i();
        y_ -= q.imag_j();
        z_ -= q.imag_k();
        return *this;
    }


private:
    numeric_type w_;
    numeric_type x_;
    numeric_type y_;
    numeric_type z_;
};




/*****************************************************************************
 *
 * CONVENIENCE DEFINITIONS
 *
 *****************************************************************************/
using quatf  = quaternion<float>;
using quatd  = quaternion<double>;
using quatld = quaternion<long double>;
using quat   = quaternion<real_t>;




/*****************************************************************************
 *
 * EXTERNAL ACCESSORS
 *
 *****************************************************************************/
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
 * I/O
 *
 *****************************************************************************/
template<class Ostream, class T>
inline Ostream&
operator << (Ostream& os, const quaternion<T>& q)
{
    return (os << q.real()   <<" "
               << q.imag_i() <<" "
               << q.imag_j() <<" "
               << q.imag_k());
}

//-------------------------------------------------------------------
template<class T, class Ostream>
inline Ostream&
print(Ostream& os, const quaternion<T>& q)
{
    return (os << "("
               << q.real()   << ","
               << q.imag_i() << ","
               << q.imag_j() << ","
               << q.imag_k() << ")");
}




/*****************************************************************************
 *
 * ARITHMETIC
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
        {s*q.real(), s*q.imag_i(), s*q.imag_j(), s*q.imag_k()};
}



//-------------------------------------------------------------------
// QUATERNION <-> QUATERNION
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr quaternion<common_numeric_t<T1,T2>>
operator * (const quaternion<T1>& p, const quaternion<T2>& q)
{
    return quaternion<common_numeric_t<T1,T2>> {
        p.real()*q.real()   - p.imag_i()*q.imag_i() - p.imag_j()*q.imag_j() - p.imag_k()*q.imag_k(),
        p.real()*q.imag_i() + p.imag_i()*q.real()   + p.imag_j()*q.imag_k() - p.imag_k()*q.imag_j(),
        p.real()*q.imag_j() - p.imag_i()*q.imag_k() + p.imag_j()*q.real()   + p.imag_k()*q.imag_i(),
        p.real()*q.imag_k() + p.imag_i()*q.imag_j() - p.imag_j()*q.imag_i() + p.imag_k()*q.real() };
}

//---------------------------------------------------------
template<class T1, class T2>
inline constexpr
quaternion<common_numeric_t<T1,T2>>
operator + (const quaternion<T1>& a, const quaternion<T2>& b)
{
    return quaternion<common_numeric_t<T1,T2>>{
        a.real()   + b.real(),
        a.imag_i() + b.imag_i(),
        a.imag_j() + b.imag_j(),
        a.imag_k() + b.imag_k() };
}

//---------------------------------------------------------
template<class T1, class T2>
inline constexpr
quaternion<common_numeric_t<T1,T2>>
operator - (const quaternion<T1>& a, const quaternion<T2>& b)
{
    return quaternion<common_numeric_t<T1,T2>>{
        a.real()   - b.real(),
        a.imag_i() - b.imag_i(),
        a.imag_j() - b.imag_j(),
        a.imag_k() - b.imag_k() };
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
    return quaternion<common_numeric_t<T1,T2>> {
         p.real()*q.real()   + p.imag_i()*q.imag_i() + p.imag_j()*q.imag_j() + p.imag_k()*q.imag_k(),
        -p.real()*q.imag_i() + p.imag_i()*q.real()   - p.imag_j()*q.imag_k() + p.imag_k()*q.imag_j(),
        -p.real()*q.imag_j() + p.imag_i()*q.imag_k() + p.imag_j()*q.real()   - p.imag_k()*q.imag_i(),
        -p.real()*q.imag_k() - p.imag_i()*q.imag_j() + p.imag_j()*q.imag_i() + p.imag_k()*q.real() };
}

//---------------------------------------------------------
template<class T1, class T2>
inline constexpr quaternion<common_numeric_t<T1,T2>>
conj_times(const quaternion<T1>& p, const quaternion<T2>& q)
{
    return quaternion<common_numeric_t<T1,T2>>{
        p.real()*q.real()   + p.imag_i()*q.imag_i() + p.imag_j()*q.imag_j() + p.imag_k()*q.imag_k(),
        p.real()*q.imag_i() - p.imag_i()*q.real()   - p.imag_j()*q.imag_k() + p.imag_k()*q.imag_j(),
        p.real()*q.imag_j() + p.imag_i()*q.imag_k() - p.imag_j()*q.real()   - p.imag_k()*q.imag_i(),
        p.real()*q.imag_k() - p.imag_i()*q.imag_j() + p.imag_j()*q.imag_i() - p.imag_k()*q.real() };
}




/*****************************************************************************
 *
 * MODIFY
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
    return quaternion<T>{q.real(), -q.imag_i(), -q.imag_j(), -q.imag_k()};
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
 * LENGTH /NORM
 *
 *****************************************************************************/
template<class T>
inline constexpr T
norm2(const quaternion<T>& q)
{
    return (q.real()   * q.real()   +
            q.imag_i() * q.imag_i() +
            q.imag_j() * q.imag_j() +
            q.imag_k() * q.imag_k());
}

//---------------------------------------------------------
template<class T>
inline auto
norm(const quaternion<T>& q)
    -> decltype(sqrt(make_real(norm2(q))))
{
    using std::sqrt;
    return sqrt(make_real(norm2(q)));
}

//---------------------------------------------------------
template<class T>
inline constexpr bool
is_normalized(const quaternion<T>& q)
{
    return approx_1(norm2(q));
}




/*****************************************************************************
 *
 * QUATERNION - QUATERNION OPERATIONS
 *
 *****************************************************************************/

//-------------------------------------------------------------------
// DISTANCE
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr common_numeric_t<T1,T2>
norm2(const quaternion<T1>& a, const quaternion<T1>& b)
{
    return ( (a.real()   - b.real()  ) * (a.real()   - b.real()  ) +
             (a.imag_i() - b.imag_i()) * (a.imag_i() - b.imag_i()) +
             (a.imag_j() - b.imag_j()) * (a.imag_j() - b.imag_j()) +
             (a.imag_k() - b.imag_k()) * (a.imag_k() - b.imag_k()) );
}
//---------------------------------------------------------
template<class T1, class T2>
inline auto
norm(const quaternion<T1>& a, const quaternion<T1>& b)
    -> decltype(sqrt(make_real(norm2(a, b))))
{
    using std::sqrt;

    return sqrt(make_real(norm2(a, b)));
}




//-------------------------------------------------------------------
// dot
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr common_numeric_t<T1,T2>
dot(const quaternion<T1>& a, const quaternion<T2>& b)
{
    return (a.real()   * b.real()   +
            a.imag_i() * b.imag_i() +
            a.imag_j() * b.imag_j() +
            a.imag_k() * b.imag_k());
}



//-------------------------------------------------------------------
// REAL PRODUCT
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
quaternion<common_numeric_t<T1,T2>>
real_product(const quaternion<T1>& p, const quaternion<T2>& q)
{
    return quaternion<common_numeric_t<T1,T2>>{
        (real(p.real())*real(q.real()))   - (real(p.imag_i())*real(q.imag_i())) - (real(p.imag_j())*real(q.imag_j())) - (real(p.imag_k())*real(q.imag_k())),
        (real(p.real())*real(q.imag_i())) + (real(p.imag_i())*real(q.real()))   + (real(p.imag_j())*real(q.imag_k())) - (real(p.imag_k())*real(q.imag_j())),
        (real(p.real())*real(q.imag_j())) - (real(p.imag_i())*real(q.imag_k())) + (real(p.imag_j())*real(q.real()))   + (real(p.imag_k())*real(q.imag_i())),
        (real(p.real())*real(q.imag_k())) + (real(p.imag_i())*real(q.imag_j())) - (real(p.imag_j())*real(q.imag_i())) + (real(p.imag_k())*real(q.real()))
    };
}

//---------------------------------------------------------
template<class T1, class T2, class T3>
inline void
real_product(
    const quaternion<T1>& p, const quaternion<T2>& q, quaternion<T3>& r)
{
    r = quaternion<T3>{
        (real(p.real())*real(q.real()))   - (real(p.imag_i())*real(q.imag_i())) - (real(p.imag_j())*real(q.imag_j())) - (real(p.imag_k())*real(q.imag_k())),
        (real(p.real())*real(q.imag_i())) + (real(p.imag_i())*real(q.real()))   + (real(p.imag_j())*real(q.imag_k())) - (real(p.imag_k())*real(q.imag_j())),
        (real(p.real())*real(q.imag_j())) - (real(p.imag_i())*real(q.imag_k())) + (real(p.imag_j())*real(q.real()))   + (real(p.imag_k())*real(q.imag_i())),
        (real(p.real())*real(q.imag_k())) + (real(p.imag_i())*real(q.imag_j())) - (real(p.imag_j())*real(q.imag_i())) + (real(p.imag_k())*real(q.real()))
    };
}



//-------------------------------------------------------------------
// IMAG PRODUCT
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
quaternion<common_numeric_t<T1,T2>>
imag_product(const quaternion<T1>& p, const quaternion<T2>& q)
{
    return quaternion<common_numeric_t<T1,T2>>{
        (imag(p.real())*imag(q.real()))   - (imag(p.imag_i())*imag(q.imag_i())) - (imag(p.imag_j())*imag(q.imag_j())) - (imag(p.imag_k())*imag(q.imag_k())),
        (imag(p.real())*imag(q.imag_i())) + (imag(p.imag_i())*imag(q.real()))   + (imag(p.imag_j())*imag(q.imag_k())) - (imag(p.imag_k())*imag(q.imag_j())),
        (imag(p.real())*imag(q.imag_j())) - (imag(p.imag_i())*imag(q.imag_k())) + (imag(p.imag_j())*imag(q.real()))   + (imag(p.imag_k())*imag(q.imag_i())),
        (imag(p.real())*imag(q.imag_k())) + (imag(p.imag_i())*imag(q.imag_j())) - (imag(p.imag_j())*imag(q.imag_i())) + (imag(p.imag_k())*imag(q.real()))
    };
}

//---------------------------------------------------------
template<class T1, class T2, class T3>
inline void
imag_product(
    const quaternion<T1>& p, const quaternion<T2>& q, quaternion<T3>& r)
{
    r = quaternion<T3>{
        (imag(p.real())*imag(q.real()))   - (imag(p.imag_i())*imag(q.imag_i())) - (imag(p.imag_j())*imag(q.imag_j())) - (imag(p.imag_k())*imag(q.imag_k())),
        (imag(p.real())*imag(q.imag_i())) + (imag(p.imag_i())*imag(q.real()))   + (imag(p.imag_j())*imag(q.imag_k())) - (imag(p.imag_k())*imag(q.imag_j())),
        (imag(p.real())*imag(q.imag_j())) - (imag(p.imag_i())*imag(q.imag_k())) + (imag(p.imag_j())*imag(q.real()))   + (imag(p.imag_k())*imag(q.imag_i())),
        (imag(p.real())*imag(q.imag_k())) + (imag(p.imag_i())*imag(q.imag_j())) - (imag(p.imag_j())*imag(q.imag_i())) + (imag(p.imag_k())*imag(q.real()))
    };
}




/*****************************************************************************
 *
 * GENERATION
 *
 *****************************************************************************/
template<class T1, class T2, class T3, class T4, class = typename
    std::enable_if<is_number<T1,T2,T3,T4>::value>::type>
inline constexpr quaternion<common_numeric_t<T1,T2,T3,T4>>
make_quaternion(T1&& real, T2&& imagi, T3&& imagj, T4&& imagk)
{
    return quaternion<common_numeric_t<T1,T2,T3,T4>>{
            std::forward<T1>(real),
            std::forward<T2>(imagi),
            std::forward<T3>(imagj),
            std::forward<T4>(imagk) };
}



//-------------------------------------------------------------------
// RANDOM UNIFORM
//-------------------------------------------------------------------
template<class T, class URBG>
inline quaternion<T>
random_unit_quaternion(URBG& urbg)
{
    static_assert(
        is_floating_point<T>::value,
        "random_unit_quaternion<T>(urbg): T must be a floating-point number type");

    using std::cos;
    using std::sin;
    using std::sqrt;

    auto d_0_1 = std::uniform_real_distribution<T>{T(0), T(1)};
    auto d_0_2pi = std::uniform_real_distribution<T>{T(0), T(2*pi)};

    const auto u0 = d_0_1(urbg);
    const auto u1 = d_0_2pi(urbg);
    const auto u2 = d_0_2pi(urbg);
    const auto uA = sqrt(T(1) - u0);
    const auto uB = sqrt(u0);

    return quaternion<T>{uA * sin(u1), uA * cos(u1), uB * sin(u2), uB * cos(u2) };
}

//---------------------------------------------------------
template<class URBG>
inline quaternion<real_t>
random_unit_quat(URBG& urbg) {
    return random_unit_quaternion<quaternion<real_t>>(urbg);
}

template<class URBG>
inline quaternion<float>
random_unit_quatf(URBG& urbg) {
    return random_unit_quaternion<quaternion<float>>(urbg);
}

template<class URBG>
inline quaternion<double>
random_unit_quatd(URBG& urbg) {
    return random_unit_quaternion<quaternion<double>>(urbg);
}

template<class URBG>
inline quaternion<long double>
random_unit_quatld(URBG& urbg) {
    return random_unit_quaternion<quaternion<long double>>(urbg);
}



//-------------------------------------------------------------------
// linear interpolation
//-------------------------------------------------------------------
template<class T1, class T2, class T3,
    class = typename std::enable_if<is_number<T3>::value>::type
>
inline quaternion<common_numeric_t<T1,T2,T3>>
lerp(const quaternion<T1>& qFrom, const quaternion<T2>& qTo, T3 t)
{
    assert((t >= 0) && (t <= 1));

    const auto t1 = 1-t;
    auto out = quaternion<common_numeric_t<T1,T2,T3>>{
        qFrom.real()   * t1 + qTo.real()   * t,
        qFrom.imag_i() * t1 + qTo.imag_i() * t,
        qFrom.imag_j() * t1 + qTo.imag_j() * t,
        qFrom.imag_k() * t1 + qTo.imag_k() * t};

    out.normalize();

    return out;
}



//-------------------------------------------------------------------
// spherical linear interpolation
//-------------------------------------------------------------------
template<class T1, class T2, class T3,
    class = typename std::enable_if<is_number<T3>::value>::type
>
inline quaternion<common_numeric_t<T1,T2,T3>>
slerp(const quaternion<T1>& qFrom, const quaternion<T2>& qTo, T3 t)
{
    using q_t = common_numeric_t<T1,T2,T3>;

    assert((t >= T3(0)) && (t <= T3(1)));

    using std::sin;
    using std::acos;

    auto cosPhi = qFrom.real()*qTo.real() +
                  qFrom.imag_i()*qTo.imag_i() +
                  qFrom.imag_j()*qTo.imag_j() +
                  qFrom.imag_k()*qTo.imag_k();

    if(cosPhi < q_t(0)) {
        q_t from, to;
        if((q_t(1) + cosPhi) > tolerance<q_t>::value()) {
            auto phi = acos(-cosPhi);
            auto sinPhi = sin(phi);
            from = sin((q_t(1) - t) * phi) / sinPhi;
            to = sin(t * phi) / sinPhi;
        }
        else {
            from = q_t(1) - t;
            to = t;
        }
        return quaternion<q_t>{
            qFrom.real()   * from - qTo.real()   * to,
            qFrom.imag_i() * from - qTo.imag_i() * to,
            qFrom.imag_j() * from - qTo.imag_j() * to,
            qFrom.imag_k() * from - qTo.imag_k() * to };
    }

    q_t from, to;
    if((q_t(1) - cosPhi) > tolerance<q_t>::value()) {
        auto phi = acos(cosPhi);
        auto sinPhi = sin(phi);
        from = sin((q_t(1) - t) * phi) / sinPhi;
        to = sin(t * phi) / sinPhi;
    }
    else {
        from = q_t(1) - t;
        to = t;
    }
    return quaternion<q_t>{
        qFrom.real()   * from + qTo.real()   * to,
        qFrom.imag_i() * from + qTo.imag_i() * to,
        qFrom.imag_j() * from + qTo.imag_j() * to,
        qFrom.imag_k() * from + qTo.imag_k() * to };
}



//-------------------------------------------------------------------
// spherical cubic interpolation
//-------------------------------------------------------------------
template<class T0, class T1, class T2, class T3, class T4, class =
    typename std::enable_if<is_number<T4>::value>::type
>
inline auto
squad(
    const quaternion<T0>& q0, const quaternion<T1>& q1,
    const quaternion<T2>& q2, const quaternion<T3>& q3, T4 t)
    -> decltype(
        quat_slerp(quat_slerp(q0,q3,t), quat_slerp(q1,q2,t), T4(2)*t*(T4(1)-t)))
{
    assert((t >= T4(0)) && (t <= T4(1)));

    return quat_slerp(
                quat_slerp(q0,q3,t),
                quat_slerp(q1,q2,t), T4(2)*t*(T4(1)-t));
}



//-------------------------------------------------------------------
// log function application
//-------------------------------------------------------------------
template<class T>
inline quaternion<T>
log(const quaternion<T>& q)
{
    using std::sin;
    using std::acos;

    const auto phi = acos(q.real());
    const auto sinPhi = sin(phi);

    if(sinPhi > T(0)) {
        return quaternion<T>{
            T(0),
            phi * q.imag_i() / sinPhi,
            phi * q.imag_j() / sinPhi,
            phi * q.imag_k() / sinPhi};
    }

    return quaternion<T>{T(0), T(0), T(0), T(0)};
}


//-------------------------------------------------------------------
/// @brief e^quat = exp(v*phi) = [cos(phi),vsin(phi)]
//-------------------------------------------------------------------
template<class T>
inline quaternion<T>
exp(const quaternion<T>& q)
{
    using std::sin;
    using std::cos;

    const auto phi = q.imag_i()*q.imag_i() +
                     q.imag_j()*q.imag_j() +
                     q.imag_k()*q.imag_k();

    const auto sinPhi = sin(phi);
    const auto cosPhi = cos(phi);

    if(phi > T(0)) {
        return quaternion<T>{
            cosPhi,
            sinPhi * q.imag_i() / phi,
            sinPhi * q.imag_j() / phi,
            sinPhi * q.imag_k() / phi};
    }

    return quaternion<T>{cosPhi, T(0), T(0), T(0)};
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
