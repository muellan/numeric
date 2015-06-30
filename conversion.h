/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2015 André Müller
 *
 *****************************************************************************/

#ifndef AMLIB_NUMERIC_CONVERSION_H_
#define AMLIB_NUMERIC_CONVERSION_H_

#include <cstdlib>
#include <string>

#include "traits.h"
#include "limits.h"


namespace am {
namespace num {


/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class T, class = typename std::enable_if<
    is_number<decay_t<T>>::value &&
    !is_floating_point<decay_t<T>>::value>::type>
inline constexpr floating_point_t<decay_t<T>>
make_real(T&& x)
{
    return floating_point_t<decay_t<T>>(std::forward<T>(x));
}

//-------------------------------------------------------------------
template<class T, class = typename std::enable_if<
    is_number<decay_t<T>>::value &&
    is_floating_point<decay_t<T>>::value>::type>
inline constexpr auto
make_real(T&& x) -> decltype(x)
{
    return std::forward<T>(x);
}




/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class T, class = typename std::enable_if<
    is_number<decay_t<T>>::value &&
    !is_unsigned<decay_t<T>>::value>::type>
inline constexpr unsigned_t<decay_t<T>>
make_unsigned(T&& x)
{
    return unsigned_t<decay_t<T>>(std::forward<T>(x));
}

//-------------------------------------------------------------------
template<class T, class = typename std::enable_if<
    is_number<decay_t<T>>::value &&
    is_unsigned<decay_t<T>>::value>::type>
inline constexpr auto
make_unsigned(T&& x) -> decltype(x)
{
    return std::forward<T>(x);
}




/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class T, class = typename std::enable_if<
    is_number<decay_t<T>>::value &&
    is_unsigned<decay_t<T>>::value>::type>
inline constexpr signed_t<decay_t<T>>
make_signed(T&& x)
{
    return signed_t<decay_t<T>>(std::forward<T>(x));
}

//-------------------------------------------------------------------
template<class T, class = typename std::enable_if<
    is_number<decay_t<T>>::value &&
    !is_unsigned<decay_t<T>>::value>::type>
inline constexpr auto
make_signed(T&& x) -> decltype(x)
{
    return std::forward<T>(x);
}








/*****************************************************************************
 *
 *
 *
 * RUNTIME VALUE CONVERSION
 *
 * default behavior: delegate conversion to functions defined below
 *
 *
 *
 *****************************************************************************/

namespace detail {

//-------------------------------------------------------------------
struct convert {
    template<class T>
    static T to(const char* s) noexcept;
};


//---------------------------------------------------------
template<> inline
char
convert::to<char>(const char* s) noexcept
{
    const auto i = std::atoi(s);

    if(i > std::numeric_limits<char>::max())
        return std::numeric_limits<char>::max();

    return static_cast<char>(i);
}
//---------------------------------------------------------
template<> inline
short
convert::to<short>(const char* s) noexcept
{
    const auto i = std::atoi(s);

    if(i > std::numeric_limits<short>::max())
        return std::numeric_limits<short>::max();

    return static_cast<short>(i);
}
//---------------------------------------------------------
template<> inline
int
convert::to<int>(const char* s) noexcept  {
    return std::atoi(s);
}
//---------------------------------------------------------
template<> inline
long
convert::to<long>(const char* s) noexcept  {
    return std::atol(s);
}
//---------------------------------------------------------
template<> inline
long long
convert::to<long long>(const char* s) noexcept  {
    return std::atol(s);
}


//---------------------------------------------------------
template<> inline
unsigned char
convert::to<unsigned char>(const char* s) noexcept
{
    const auto i = std::atoi(s);

    if(i <= 0) return static_cast<unsigned char>(0);

    if(i > std::numeric_limits<unsigned char>::max())
        return std::numeric_limits<unsigned char>::max();

    return static_cast<unsigned char>(i);
}
//---------------------------------------------------------
template<> inline
unsigned short
convert::to<unsigned short>(const char* s) noexcept
{
    const auto i = std::atoi(s);

    if(i <= 0) return static_cast<unsigned short>(0);

    if(i > std::numeric_limits<unsigned short>::max())
        return std::numeric_limits<unsigned short>::max();

    return static_cast<unsigned short>(i);
}
//---------------------------------------------------------
template<> inline
unsigned int
convert::to<unsigned int>(const char* s) noexcept
{
    const auto i = std::atoi(s);

    if(i <= 0) return static_cast<unsigned int>(0);
    return static_cast<unsigned int>(i);
}
//---------------------------------------------------------
template<> inline
unsigned long
convert::to<unsigned long>(const char* s) noexcept
{
    const auto i = std::atol(s);

    if(i <= 0) return static_cast<unsigned long>(0);
    return static_cast<unsigned long>(i);
}
//---------------------------------------------------------
template<> inline
unsigned long long
convert::to<unsigned long long>(const char* s) noexcept
{
    const auto i = std::atoi(s);

    if(i <= 0) return static_cast<unsigned long long>(0);
    return static_cast<unsigned long long>(i);
}


//---------------------------------------------------------
template<> inline
float
convert::to<float>(const char* s) noexcept
{
    const auto f = std::atof(s);

    if(f > std::numeric_limits<float>::max())
        return std::numeric_limits<float>::max();

    if(f < std::numeric_limits<float>::lowest())
        return std::numeric_limits<float>::lowest();

    return static_cast<float>(f);
}

//---------------------------------------------------------
template<> inline
double
convert::to<double>(const char* s) noexcept {
    return std::atof(s);
}


//---------------------------------------------------------
template<> inline
long double
convert::to<long double>(const char* s) noexcept {
    return std::atof(s);
}

}  // namespace detail


//-------------------------------------------------------------------
template<class To, class = typename std::enable_if<is_number<To>::value>>
inline To
to(const std::string& str) noexcept
{
    return detail::convert::to<To>(str.c_str());
}

//-------------------------------------------------------------------
template<class To, class = typename std::enable_if<is_number<To>::value>>
inline To
to(const char* str) noexcept
{
    return detail::convert::to<To>(str);
}






/*****************************************************************************
 *
 *
 * to-string converter functions
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T>
std::string
to_string(T&& x)
{
    return std::to_string(std::forward<T>(x));
}

//-------------------------------------------------------------------
template<class T, class = typename std::enable_if<is_number<T>::value>::type>
std::string
to_string(T number, int precision)
{
    std::ostringstream ss;
    if(precision > 0) ss.precision(precision);
    ss << number;
    return ss.str();
}


//-------------------------------------------------------------------
template<class T, class = typename std::enable_if<is_number<T>::value>::type>
std::string
to_fixed_string(T number, int precision)
{
    std::ostringstream ss;
    if(precision > 0) ss.precision(precision);
    ss << std::fixed << number;
    return ss.str();
}


//---------------------------------------------------------
template<class T, class = typename std::enable_if<is_number<T>::value>::type>
std::string
to_fixed_string(T number, int precision, int intDigits,
                const std::string& prefixFiller = std::string(" "))
{
    bool negative = number < 0;
    if(negative) number = -number;

    std::ostringstream ss;
    if(precision > 0) ss.precision(precision);
    ss << std::fixed << number;

    std::string s = (negative ? "-" : "") + ss.str();
    int p = static_cast<int>(log10(number)) + (negative ? 1 : 0);

    if(intDigits > 0 && p < intDigits) {
        if(p > 0) {
            for(int i = 0; i < (intDigits-p-1); ++i)
                s = prefixFiller + s;
        } else {
            for(int i = 0; i < (intDigits-1); ++i)
                s = prefixFiller + s;
        }
    }
    return s;
}



}  // namespace num
}  // namespace am


#endif
