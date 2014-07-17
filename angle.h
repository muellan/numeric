/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2014 Andr� M�ller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_ANGLE_H_
#define AM_NUMERIC_ANGLE_H_

#include <cmath>
#include <utility>
#include <random>

#include "param.h"

#include "narrowing.h"
#include "traits.h"


namespace am {
namespace num {


/*****************************************************************************
 *
 *
 * ANGLE UNITS
 *
 *
 *****************************************************************************/


//-------------------------------------------------------------------
/// @brief degrees full turn
template<class T = real_t>
struct degrees_turn
{
    using type = T;
    static constexpr type value = 360;
};
//---------------------------------------------------------
/// @brief arc minutes full turn
template<class T = real_t>
struct arcmins_turn
{
    using type = T;
    static constexpr type value = 21600;
};
//---------------------------------------------------------
/// @brief arc seconds full turn
template<class T = real_t>
struct arcsecs_turn
{
    using type = T;
    static constexpr type value = 1296000;
};


//-------------------------------------------------------------------
/// @brief radians full turn
template<class T = real_t>
struct radians_turn
{
    using type = T;
    static constexpr type value = (2 * pi);
};


//-------------------------------------------------------------------
/// @brief grads/gons full turn
template<class T = real_t>
struct gons_turn
{
    using type = T;
    static constexpr type value = 400;
};
//---------------------------------------------------------
/// @brief grads/gons cs full turn
template<class T = real_t>
struct gon_cs_turn
{
    using type = T;
    static constexpr type value = 40000;
};
//---------------------------------------------------------
/// @brief grads/gons ccs full turn
template<class T = real_t>
struct gon_ccs_turn
{
    using type = T;
    static constexpr type value = 4000000;
};








/*****************************************************************************
 *
 *
 * TRAITS
 *
 *
 *****************************************************************************/
template<class> class angle;

//-------------------------------------------------------------------
template<class T, class... R>
struct is_angle :
    std::integral_constant<bool,
        is_angle<T>::value && is_angle<R...>::value>
{};

//---------------------------------------------------------
template<class T>
struct is_angle<T> :
    std::false_type
{};

//-----------------------------------------------------
template<class T>
struct is_angle<angle<T>> :
    std::true_type
{};







/*****************************************************************************
 *
 * @brief represents an angle
 *        supports automatic angle unit conversions
 *        supports constexpr compile-time evaluation
 *
 * @tparam Turn  policy class providing angle value of a full circle
 *               and the arithmetic type used for storing the angle value
 *
 *****************************************************************************/
template<class Turn>
class angle
{
    //---------------------------------------------------------
    template<class T>
    using conversion_t = typename std::common_type<
            typename T::type, typename Turn::type>::type;

    static_assert(is_number<typename Turn::type>::value &&
                  !is_angle<Turn>::value &&
                  !is_angle<typename Turn::type>::value,
                  "angle<T>: T must be a turn specifier and T::type must be a number");

    //befriend all angles
    template<class T> friend class angle;

public:
    //---------------------------------------------------------------
    // TYPES
    //---------------------------------------------------------------
    using turn_type    = Turn;
    using numeric_type = typename Turn::type;


    //---------------------------------------------------------------
    // CONSTANTS
    //---------------------------------------------------------------
    static constexpr numeric_type
    turn() noexcept {
        return (turn_type::value);
    }


    //---------------------------------------------------------------
    // CONSTRUCTION / DESTRUCTION
    //---------------------------------------------------------------
    ///@brief no default contructor, angle has to be initialized with a value
    angle() = delete;

    //-----------------------------------------------------
    /// @brief implicit conversion from simple values is not allowed
    explicit constexpr
    angle(numeric_type a):
        v_{std::move(a)}
    {}
    //-----------------------------------------------------
    /// @brief implicit conversion from simple values is not allowed
    template<class T, class = typename std::enable_if<
        is_number<decay_t<T>>::value && !is_angle<decay_t<T>>::value>::type>
    explicit constexpr
    angle(T&& a):
        v_(std::forward<T>(a))
    {
        AM_CHECK_NARROWING(numeric_type, T)
    }
    //-----------------------------------------------------
    /// @brief   implicit conversion of angles is allowed
    /// @details may cause numerical drift when performed repeatedly
    template<class T>
    constexpr
    angle(const angle<T>& a):
        v_{a.as<turn_type>()}
    {
        AM_CHECK_NARROWING(numeric_type, typename T::type)
    }

    //-----------------------------------------------------
    constexpr
    angle(const angle&) = default;

    //---------------------------------------------------------------
    template<class T>
    angle&
    operator = (const angle<T>& a)
    {
        AM_CHECK_NARROWING(numeric_type, typename T::type)

        v_ = a.as<turn_type>();
        return *this;
    }


    //---------------------------------------------------------------
    /// @brief maps the stored value to the range [0, turn]
    angle&
    wrap() {
        using std::fmod;
        if(v_ < numeric_type(0)) {
            v_ = -v_;
        }
        if(v_ > turn()) {
            v_ = fmod(v_, turn());
        }
        return *this;
    }


    //---------------------------------------------------------------
    // ASSIGNING ARITHMETIC OPERATIONS
    //---------------------------------------------------------------
    template<class T>
    angle&
    operator += (const angle<T>& a)
    {
        AM_CHECK_NARROWING(numeric_type, typename T::type)

        v_ += a.as<turn_type>();
        return *this;
    }
    //-----------------------------------------------------
    template<class T>
    angle&
    operator -= (const angle<T>& a)
    {
        AM_CHECK_NARROWING(numeric_type, typename T::type)

        v_ += a.as<turn_type>();
        return *this;
    }
    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<decay_t<T>>::value && !is_angle<decay_t<T>>::value>::type>
    angle&
    operator *= (const T& factor)
    {
        AM_CHECK_NARROWING(numeric_type, T)

        v_ *= factor;
        return *this;
    }
    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<decay_t<T>>::value && !is_angle<decay_t<T>>::value>::type>
    angle&
    operator /= (const T& factor)
    {
        AM_CHECK_NARROWING(numeric_type, T)

        v_ /= factor;
        return *this;
    }


    //---------------------------------------------------------------
    // ARITHMETIC OPERATIONS
    //---------------------------------------------------------------
    template<class T>
    constexpr angle
    operator + (const angle<T>& a) const
    {
        return angle{v_ + a.as<turn_type>()};
    }
    //-----------------------------------------------------
    template<class T>
    constexpr angle
    operator - (const angle<T>& a) const
    {
        return angle{v_ - a.as<turn_type>()};
    }
    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<decay_t<T>>::value && !is_angle<decay_t<T>>::value>::type>
    constexpr angle
    operator * (const T& factor) const
    {
        return angle{v_ * factor};
    }
    //-----------------------------------------------------
    template<class T, class = typename std::enable_if<
        is_number<decay_t<T>>::value && !is_angle<decay_t<T>>::value>::type>
    constexpr angle
    operator / (const T& factor) const
    {
        return angle{v_ / factor};
    }

    //-----------------------------------------------------
    /// @brief pre-multiplication
    template<class T, class = typename std::enable_if<
        is_number<decay_t<T>>::value && !is_angle<decay_t<T>>::value>::type>
    inline friend constexpr angle
    operator * (const T& factor, const angle& a)
    {
        return angle{factor * a.v_};
    }


    //---------------------------------------------------------------
    // inversion (works only on signed domains)
    //---------------------------------------------------------------
    template<class T = numeric_type, class = typename std::enable_if<
        !std::is_unsigned<T>::value && !is_angle<decay_t<T>>::value>::type>
    constexpr angle
    operator - () const {
        return angle{-v_};
    }


    //---------------------------------------------------------------
    // increment/decrement operations
    //---------------------------------------------------------------
    angle
    operator ++ () {
        ++v_;
        return *this;
    }
    //-----------------------------------------------------
    angle
    operator ++ (int) {
        auto old = angle{*this};
        operator++();
        return old;
    }
    //-----------------------------------------------------
    angle
    operator -- () {
        --v_;
        return *this;
    }
    //-----------------------------------------------------
    angle
    operator -- (int) {
        auto old = angle{*this};
        operator--();
        return old;
    }


    //---------------------------------------------------------------
    // COMPARISON
    //---------------------------------------------------------------
    template<class T>
    constexpr bool
    operator == (const angle<T>& other) {
        return (v_ == other.as<turn_type>());
    }
    //-----------------------------------------------------
    template<class T>
    constexpr bool
    operator != (const angle<T>& other) {
        return (v_ != other.as<turn_type>());
    }
    //-----------------------------------------------------
    template<class T>
    constexpr bool
    operator < (const angle<T>& other) {
        return (v_ < other.as<turn_type>());
    }
    //-----------------------------------------------------
    template<class T>
    constexpr bool
    operator > (const angle<T>& other) {
        return (v_ > other.as<turn_type>());
    }
    //-----------------------------------------------------
    template<class T>
    constexpr bool
    operator <= (const angle<T>& other) {
        return (v_ <= other.as<turn_type>());
    }
    //-----------------------------------------------------
    template<class T>
    constexpr bool
    operator >= (const angle<T>& other) {
        return (v_ >= other.as<turn_type>());
    }


    //---------------------------------------------------------------
    template<class OutTurn>
    inline friend constexpr conversion_t<OutTurn>
    angle_cast(const angle& a)    {
        return (a.as<OutTurn>());
    }

    //-----------------------------------------------------
    template<class T>
    inline friend constexpr T
    radians_cast(const angle& a) {
        return a.as<radians_turn<T>>();
    }

    //-----------------------------------------------------
    template<class T>
    inline friend constexpr T
    degrees_cast(const angle& a) {
        return a.as<degrees_turn<T>>();
    }
    //-----------------------------------------------------
    template<class T>
    inline friend constexpr T
    arcmins_cast(const angle& a) {
        return a.as<arcmins_turn<T>>();
    }
    //-----------------------------------------------------
    template<class T>
    inline friend constexpr T
    arcsecs_cast(const angle& a) {
        return a.as<arcsecs_turn<T>>();
    }

    //-----------------------------------------------------
    template<class T>
    inline friend constexpr T
    gons_cast(const angle& a) {
        return a.as<gons_turn<T>>();
    }
    //-----------------------------------------------------
    template<class T>
    inline friend constexpr T
    gon_cs_cast(const angle& a) {
        return a.as<gon_cs_turn<T>>();
    }
    //-----------------------------------------------------
    template<class T>
    inline friend constexpr T
    gon_ccs_cast(const angle& a) {
        return a.as<gon_ccs_turn<T>>();
    }


    //---------------------------------------------------------------
    inline friend angle
    fmod(const angle& numer, const angle& denom) {
        using std::fmod;
        return angle{fmod(numer.v_, denom.v_)};
    }
    //-----------------------------------------------------
    inline friend angle
    floor(const angle& a) {
        using std::floor;
        return angle{floor(a.v_)};
    }
    //-----------------------------------------------------
    inline friend angle
    ceil(const angle& a) {
        using std::ceil;
        return angle{ceil(a.v_)};
    }
    //-----------------------------------------------------
    inline friend angle
    trunc(const angle& a) {
        using std::trunc;
        return angle{trunc(a.v_)};
    }
    //-----------------------------------------------------
    inline friend angle
    remainder(const angle& numer, const angle& denom) {
        using std::remainder;
        return angle{remainder(numer.v_, denom.v_)};
    }

    //-----------------------------------------------------
    inline friend angle
    round(const angle& a) {
        using std::round;
        return angle{round(a.v_)};
    }
    //-----------------------------------------------------
    inline friend angle
    lround(const angle& a) {
        using std::lround;
        return angle{lround(a.v_)};
    }
    //-----------------------------------------------------
    inline friend angle
    llround(const angle& a) {
        using std::llround;
        return angle{llround(a.v_)};
    }
    //-----------------------------------------------------
    inline friend angle
    rint(const angle& a) {
        using std::rint;
        return angle{rint(a.v_)};
    }
    //-----------------------------------------------------
    inline friend angle
    lrint(const angle& a) {
        using std::lrint;
        return angle{lrint(a.v_)};
    }
    //-----------------------------------------------------
    inline friend angle
    llrint(const angle& a) {
        using std::llrint;
        return angle{llrint(a.v_)};
    }
    //-----------------------------------------------------
    inline friend angle
    nearbyint(const angle& a) {
        using std::nearbyint;
        return angle{nearbyint(a.v_)};
    }


private:
    //---------------------------------------------------------------
    template<class OutTurn, class = typename
        std::enable_if<std::is_same<turn_type,OutTurn>::value>::type>
    constexpr numeric_type
    as() const {
        return v_;
    }
    //-----------------------------------------------------
    template<class OutTurn, class = typename
        std::enable_if<!std::is_same<turn_type,OutTurn>::value>::type>
    constexpr conversion_t<OutTurn>
    as() const {
        using res_t = conversion_t<OutTurn>;
        return ( (res_t(OutTurn::value) / res_t(turn())) * v_);
    }

    //---------------------------------------------------------------
    numeric_type v_ = numeric_type(0);
};








/*****************************************************************************
 *
 *
 * CONVENIENCE DEFINITIONS
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class V>
using radians = angle<radians_turn<V>>;

//---------------------------------------------------------
template<class V>
using degrees = angle<degrees_turn<V>>;

template<class V>
using arcmins = angle<arcmins_turn<V>>;

template<class V>
using arcsecs = angle<arcsecs_turn<V>>;

//---------------------------------------------------------
template<class V>
using gons = angle<gons_turn<V>>;

template<class V>
using gon_cs = angle<gon_cs_turn<V>>;

template<class V>
using gon_ccs = angle<gon_ccs_turn<V>>;


//-------------------------------------------------------------------
using rad  = angle<radians_turn<real_t>>;
using radf = angle<radians_turn<float>>;
using radd = angle<radians_turn<double>>;

using deg  = angle<degrees_turn<real_t>>;
using degf = angle<degrees_turn<float>>;
using degd = angle<degrees_turn<double>>;
using degi = angle<degrees_turn<int>>;

using gon  = angle<gons_turn<real_t>>;
using gonf = angle<gons_turn<float>>;
using gond = angle<gons_turn<double>>;
using goni = angle<gons_turn<int>>;






/*****************************************************************************
 *
 *
 * LITERALS
 *
 *
 *****************************************************************************/

namespace literals {

//-------------------------------------------------------------------
constexpr degrees<real_t> operator"" _deg (long double x) {
    return degrees<real_t>{real_t(x)};
}
//---------------------------------------------------------
constexpr degrees<real_t> operator"" _deg (unsigned long long int x) {
    return degrees<real_t>{real_t(x)};
}


//-------------------------------------------------------------------
constexpr arcmins<real_t> operator"" _arcmin (long double x) {
    return arcmins<real_t>{real_t(x)};
}
//---------------------------------------------------------
constexpr arcmins<real_t> operator"" _arcmin (unsigned long long int x) {
    return arcmins<real_t>{real_t(x)};
}


//-------------------------------------------------------------------
constexpr arcsecs<real_t> operator"" _arcsec (long double x) {
    return arcsecs<real_t>{real_t(x)};
}
//---------------------------------------------------------
constexpr arcsecs<real_t> operator"" _arcsec (unsigned long long int x) {
    return arcsecs<real_t>{real_t(x)};
}


//-------------------------------------------------------------------
constexpr radians<real_t> operator"" _rad (long double x) {
    return radians<real_t>{real_t(x)};
}
//---------------------------------------------------------
constexpr radians<real_t> operator"" _rad (unsigned long long int x) {
    return radians<real_t>{real_t(x)};
}
//---------------------------------------------------------
constexpr radians<real_t> operator"" _pi_rad(long double x) {
    return radians<real_t>{real_t(x * pi)};
}
//---------------------------------------------------------
constexpr radians<real_t> operator"" _pi_rad(unsigned long long int x) {
    return radians<real_t>{real_t(x * pi)};
}


//-------------------------------------------------------------------
constexpr gons<real_t> operator"" _gon (long double x) {
    return gons<real_t>{real_t(x)};
}
//---------------------------------------------------------
constexpr gons<real_t> operator"" _gon (unsigned long long int x) {
    return gons<real_t>{real_t(x)};
}


//-------------------------------------------------------------------
constexpr gons<real_t> operator"" _goncs (long double x) {
    return gons<real_t>{real_t(x)};
}
//---------------------------------------------------------
constexpr gons<real_t> operator"" _goncs (unsigned long long int x) {
    return gons<real_t>{real_t(x)};
}


//-------------------------------------------------------------------
constexpr gon_cs<real_t> operator"" _gonccs (long double x) {
    return gon_cs<real_t>{real_t(x)};
}
//---------------------------------------------------------
constexpr gon_ccs<real_t> operator"" _gonccs (unsigned long long int x) {
    return gon_ccs<real_t>{real_t(x)};
}

}  // unnamed namespace






/*****************************************************************************
 *
 *
 * I/O
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class U1, class U2>
inline common_numeric_t<typename U1::type, typename U2::type>
operator / (const angle<U1>& a, const angle<U2>& b)
{
    using res_t = common_numeric_t<typename U1::type, typename U2::type>;

    return radians_cast<res_t>(a) / radians_cast<res_t>(b);
}
//---------------------------------------------------------
template<class U>
inline typename U::type
operator / (const angle<U>& a, const angle<U>& b)
{
    using res_t = typename U::type;

    return radians_cast<res_t>(a) / radians_cast<res_t>(b);
}



//-------------------------------------------------------------------
// I/O
//-------------------------------------------------------------------
template<class Ostream, class T>
inline Ostream&
operator << (Ostream& os, const angle<T>& r)
{
    return (os << angle_cast<T>(r));
}


//-------------------------------------------------------------------
// pretty printing
//-------------------------------------------------------------------
template<class T, class Ostream>
inline Ostream&
print(Ostream& os, const angle<radians_turn<T>>& a)
{
    return (os << a << "rad");
}

//---------------------------------------------------------
template<class T, class Ostream>
inline Ostream&
print(Ostream& os, const angle<degrees_turn<T>>& a)
{
    return (os << a << "�");
}

//---------------------------------------------------------
template<class T, class Ostream>
inline Ostream&
print(Ostream& os, const angle<arcmins_turn<T>>& a)
{
    return (os << a << "'");
}

//---------------------------------------------------------
template<class T, class Ostream>
inline Ostream&
print(Ostream& os, const angle<arcsecs_turn<T>>& a)
{
    return (os << a << "''");
}

//---------------------------------------------------------
template<class T, class Ostream>
inline Ostream&
print(Ostream& os, const angle<gons_turn<T>>& a)
{
    return (os << a << "gon");
}

//---------------------------------------------------------
template<class T, class Ostream>
inline Ostream&
print(Ostream& os, const angle<gon_cs_turn<T>>& a)
{
    return (os << a << "cs");
}

//---------------------------------------------------------
template<class T, class Ostream>
inline Ostream&
print(Ostream& os, const angle<gon_ccs_turn<T>>& a)
{
    return (os << a << "ccs");
}






/*****************************************************************************
 *
 *
 * TRAITS SPECIALIZATIONS
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T>
struct is_number<angle<T>> : std::true_type {};

template<class T>
struct is_number<angle<T>&> : std::true_type {};

template<class T>
struct is_number<angle<T>&&> : std::true_type {};

template<class T>
struct is_number<const angle<T>&> : std::true_type {};

template<class T>
struct is_number<const angle<T>> : std::true_type {};



//-------------------------------------------------------------------
template<class T>
struct is_floating_point<angle<T>> :
    std::integral_constant<bool,
        is_floating_point<typename angle<T>::numeric_type>::value>
{};



//-------------------------------------------------------------------
template<class T>
struct common_numeric_type<angle<T>,angle<T>>
{
    using type = angle<T>;
};



namespace detail {

//-------------------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, angle<To>, From> :
    public is_non_narrowing_helper<true,typename angle<To>::numeric_type,From>
{};

//---------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, angle<To>, angle<From> > :
    public is_non_narrowing_helper<true,
        typename angle<To>::numeric_type,
        typename angle<From>::numeric_type>
{};

//---------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, To, angle<From> > :
    public is_non_narrowing_helper<true,To,typename angle<From>::numeric_type>
{};


}  // namespace detail






/*****************************************************************************
 *
 *
 *
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
/// @brief maps the input angle to the turn range
template<class T>
inline constexpr angle<T>
mod_turn(angle<T> a)
{
    return angle<T>{a.wrap()};
}


//-------------------------------------------------------------------
/// @brief returns turn - wrapped(angle)
template<class T>
inline angle<T>
turn_remainder(angle<T> a)
{
    auto r = angle<T>{a.turn() - angle_cast<T>(a)};
    r.wrap();
    return r;
}







/*****************************************************************************
 *
 *
 * FACTORIES
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T>
inline constexpr radians<T>
make_radians(const T& t)
{
    return radians<T>{t};
}
//---------------------------------------------------------
template<class U>
inline constexpr radians<numeric_t<angle<U>>>
make_radians(const angle<U>& a)
{
    return radians<numeric_t<angle<U>>>{a};
}

//---------------------------------------------------------
template<class T>
inline constexpr degrees<T>
make_degrees(const T& t)
{
    return degrees<T>{t};
}
//---------------------------------------------------------
template<class U>
inline constexpr degrees<numeric_t<angle<U>>>
make_degrees(const angle<U>& a)
{
    return degrees<numeric_t<angle<U>>>{a};
}

//---------------------------------------------------------
template<class T>
inline constexpr gons<T>
make_gons(const T& t)
{
    return gons<T>{t};
}
//---------------------------------------------------------
template<class U>
inline constexpr gons<numeric_t<angle<U>>>
make_gons(const angle<U>& a)
{
    return gons<numeric_t<angle<U>>>{a};
}






/*****************************************************************************
 *
 *
 * TRIGONOMETRY
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
// TRIGONOMETRIC FUNCTIONS
//-------------------------------------------------------------------
template<class T>
inline typename std::common_type<typename T::type,real_t>::type
sin(const angle<T>& r)
{
    using std::sin;
    using res_t = typename std::common_type<typename T::type,real_t>::type;

    return sin(radians_cast<res_t>(r));
}

//-----------------------------------------------------
template<class T>
inline typename std::common_type<typename T::type,real_t>::type
cos(const angle<T>& r)
{
    using std::cos;
    using res_t = typename std::common_type<typename T::type,real_t>::type;

    return cos(radians_cast<res_t>(r));
}

//-----------------------------------------------------
template<class T>
inline typename std::common_type<typename T::type,real_t>::type
tan(const angle<T>& r)
{
    using std::tan;
    using res_t = typename std::common_type<typename T::type,real_t>::type;

    return tan(radians_cast<res_t>(r));
}



//-------------------------------------------------------------------
// HYPERBOLIC FUNCTIONS
//-------------------------------------------------------------------
template<class T>
inline typename std::common_type<typename T::type,real_t>::type
sinh(const angle<T>& r)
{
    using std::sinh;
    using res_t = typename std::common_type<typename T::type,real_t>::type;

    return sinh(radians_cast<res_t>(r));
}

//-----------------------------------------------------
template<class T>
inline typename std::common_type<typename T::type,real_t>::type
cosh(const angle<T>& r)
{
    using std::cosh;
    using res_t = typename std::common_type<typename T::type,real_t>::type;

    return cosh(radians_cast<res_t>(r));
}

//-----------------------------------------------------
template<class T>
inline typename std::common_type<typename T::type,real_t>::type
tanh(const angle<T>& r)
{
    using std::tanh;
    using res_t = typename std::common_type<typename T::type,real_t>::type;

    return tanh(radians_cast<res_t>(r));
}



//-------------------------------------------------------------------
// INVERSE TRIGONOMETRIC FUNCTIONS
//-------------------------------------------------------------------
template<class T>
inline radians<floating_point_t<T>>
rad_asin(T v)
{
    using std::asin;
    using res_t = floating_point_t<T>;

    return radians<res_t>{res_t(asin(v))};
}

//-----------------------------------------------------
template<class T>
inline radians<floating_point_t<T>>
rad_acos(T v)
{
    using std::acos;
    using res_t = floating_point_t<T>;

    return radians<res_t>{res_t(acos(v))};
}

//-----------------------------------------------------
template<class T>
inline radians<floating_point_t<T>>
rad_atan(T v)
{
    using std::atan;
    using res_t = floating_point_t<T>;

    return radians<res_t>{res_t(atan(v))};
}

//-----------------------------------------------------
template<class T>
inline radians<floating_point_t<T>>
rad_atan2(T x, T y)
{
    using std::atan2;
    using res_t = floating_point_t<T>;

    return radians<res_t>{res_t(atan2(x,y))};
}



//-------------------------------------------------------------------
// INVERSE HYPERBOLIC FUNCTIONS
//-------------------------------------------------------------------
template<class T>
inline radians<floating_point_t<T>>
rad_asinh(T v)
{
    using std::asinh;
    using res_t = floating_point_t<T>;

    return radians<res_t>{res_t(asinh(v))};
}

//-----------------------------------------------------
template<class T>
inline radians<floating_point_t<T>>
rad_acosh(T v)
{
    using std::acosh;
    using res_t = floating_point_t<T>;

    return radians<res_t>{res_t(acosh(v))};
}

//-----------------------------------------------------
template<class T>
inline radians<floating_point_t<T>>
rad_atanh(T v)
{
    using std::atanh;
    using res_t = floating_point_t<T>;

    return radians<res_t>{res_t(atanh(v))};
}








/*****************************************************************************
 *
 * @brief distribution adapter producing angles from random numbers
 *
 *
 *
 *****************************************************************************/
template<class Turn, class ValueDistr>
class angle_distribution
{
    AM_CHECK_NARROWING(typename Turn::type,typename ValueDistr::result_type)

public:
    using param_type   = gen::param_t<ValueDistr>;
    using numeric_type = typename ValueDistr::result_type;
    using result_type  = angle<Turn>;


    //---------------------------------------------------------------
    explicit
    angle_distribution(const param_type& par = param_type{}):
        distr_{par}
    {}


    //---------------------------------------------------------------
    void
    reset() {
        distr_.reset();
    }

    //-----------------------------------------------------
    auto
    param() const -> decltype(std::declval<const ValueDistr&>().param()) {
        return distr_.param();
    }
    //-----------------------------------------------------
    void
    param(const param_type& par) {
        distr_.param(par);
    }

    //-----------------------------------------------------
    result_type
    min() const {
        return result_type{distr_.min()};
    }
    //-----------------------------------------------------
    result_type
    max() const {
        return result_type{distr_.max()};
    }

    //---------------------------------------------------------------
    template<class URNG>
    result_type
    operator() (URNG& urng) {
        return result_type{distr_(urng)};
    }
    //-----------------------------------------------------
    template<class URNG>
    result_type
    operator() (URNG& urng, const param_type& par) {
        return result_type{distr_(urng,par)};
    }


    //---------------------------------------------------------------
    // I/O
    //---------------------------------------------------------------
    friend std::istream&
    operator >> (std::istream& is, angle_distribution& o) {
        return (is >> o.distr_);
    }
    friend std::ostream&
    operator << (std::ostream& os, const angle_distribution& o) {
        return (os << o.distr_);
    }

private:
    ValueDistr distr_;
};






/*****************************************************************************
 *
 * @brief produces uniformly distributed angles
 *
 *
 *****************************************************************************/
template<class Turn>
class uniform_angle_distribution :
    public angle_distribution<Turn,
        std::uniform_real_distribution<typename Turn::type>>
{

    using base_t_ = angle_distribution<Turn,
        std::uniform_real_distribution<typename Turn::type>>;

public:
    //---------------------------------------------------------------
    using numeric_type = typename Turn::type;
    using param_type = gen::param_t<base_t_>;

    //---------------------------------------------------------------
    explicit
    uniform_angle_distribution(
        const numeric_type& min = numeric_type(0),
        const numeric_type& max = numeric_type(Turn::value))
    :
        base_t_{param_type{min,max}}
    {}
    //-----------------------------------------------------
    explicit
    uniform_angle_distribution(const param_type& par):
        base_t_{par}
    {}
};



//-------------------------------------------------------------------
template<class T = real_t>
using uniform_radian_distribution = uniform_angle_distribution<radians_turn<T>>;

//---------------------------------------------------------
template<class T = real_t>
using uniform_degree_distribution = uniform_angle_distribution<degrees_turn<T>>;

//---------------------------------------------------------
template<class T = real_t>
using uniform_gon_distribution = uniform_angle_distribution<gons_turn<T>>;






/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class Angle>
struct turn_interval
{
    using value_type = Angle;

    static constexpr value_type
    min() noexcept {
        return value_type(0);
    }

    static constexpr value_type
    max() noexcept {
        return value_type{Angle::turn()};
    }
};

//-------------------------------------------------------------------
template<class T>
inline constexpr T
min(const turn_interval<T>&) noexcept { return turn_interval<T>::min(); }

template<class T>
inline constexpr T
max(const turn_interval<T>&) noexcept { return turn_interval<T>::max(); }






/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class Angle>
struct half_turn_interval
{
    using value_type = Angle;

    static constexpr value_type
    min() noexcept {
        return value_type(0);
    }

    static constexpr value_type
    max() noexcept {
        return value_type{Angle::turn()/2};
    }
};

//-------------------------------------------------------------------
template<class T>
inline constexpr T
min(const half_turn_interval<T>&) noexcept {
    return half_turn_interval<T>::min();
}

template<class T>
inline constexpr T
max(const half_turn_interval<T>&) noexcept {
    return half_turn_interval<T>::max();
}






/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class Angle>
struct quarter_turn_interval
{
    using value_type = Angle;

    static constexpr value_type
    min() noexcept {
        return value_type(0);
    }

    static constexpr value_type
    max() noexcept {
        return value_type{Angle::turn()/2};
    }
};

//-------------------------------------------------------------------
template<class T>
inline constexpr T
min(const quarter_turn_interval<T>&) noexcept {
    return quarter_turn_interval<T>::min();
}

template<class T>
inline constexpr T
max(const quarter_turn_interval<T>&) noexcept {
    return quarter_turn_interval<T>::max();
}






/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class Angle>
struct centered_turn_interval
{
    using value_type = Angle;

    static constexpr value_type
    min() noexcept {
        return value_type{-Angle::turn()/2};
    }

    static constexpr value_type
    max() noexcept {
        return value_type{Angle::turn()/2};
    }
};

//-------------------------------------------------------------------
template<class T>
inline constexpr T
min(const centered_turn_interval<T>&) noexcept {
    return centered_turn_interval<T>::min();
}

template<class T>
inline constexpr T
max(const centered_turn_interval<T>&) noexcept {
    return centered_turn_interval<T>::max();
}






/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class Angle>
struct inclination_interval
{
    using value_type = Angle;

    static constexpr value_type
    min() noexcept {
        return value_type{-Angle::turn()/4};
    }

    static constexpr value_type
    max() noexcept {
        return value_type{Angle::turn()/4};
    }
};

//-------------------------------------------------------------------
template<class T>
inline constexpr T
min(const inclination_interval<T>&) noexcept {
    return inclination_interval<T>::min();
}

template<class T>
inline constexpr T
max(const inclination_interval<T>&) noexcept {
    return inclination_interval<T>::max();
}



}  // namespace num
}  // namespace am


#endif
