/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_CONVERSION_H_
#define AM_NUMERIC_CONVERSION_H_

#include <cstdlib>
#include <cstring>
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
    is_number<std::decay_t<T>>::value &&
    !is_floating_point<std::decay_t<T>>::value>::type>
inline constexpr floating_point_t<std::decay_t<T>>
make_real(T&& x)
{
    return floating_point_t<std::decay_t<T>>(std::forward<T>(x));
}

//-------------------------------------------------------------------
template<class T, class = typename std::enable_if<
    is_number<std::decay_t<T>>::value &&
    is_floating_point<std::decay_t<T>>::value>::type>
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
    is_number<std::decay_t<T>>::value &&
    !is_unsigned<std::decay_t<T>>::value>::type>
inline constexpr unsigned_t<std::decay_t<T>>
make_unsigned(T&& x)
{
    return unsigned_t<std::decay_t<T>>(std::forward<T>(x));
}

//-------------------------------------------------------------------
template<class T, class = typename std::enable_if<
    is_number<std::decay_t<T>>::value &&
    is_unsigned<std::decay_t<T>>::value>::type>
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
    is_number<std::decay_t<T>>::value &&
    is_unsigned<std::decay_t<T>>::value>::type>
inline constexpr signed_t<std::decay_t<T>>
make_signed(T&& x)
{
    return signed_t<std::decay_t<T>>(std::forward<T>(x));
}

//-------------------------------------------------------------------
template<class T, class = typename std::enable_if<
    is_number<std::decay_t<T>>::value &&
    !is_unsigned<std::decay_t<T>>::value>::type>
inline constexpr auto
make_signed(T&& x) -> decltype(x)
{
    return std::forward<T>(x);
}




/*****************************************************************************
 *
 * RUNTIME VALUE CONVERSION
 *
 * default behavior: delegate conversion to functions defined below
 *
 *****************************************************************************/

namespace detail {


/*************************************************************************//**
 * @brief forwards string to first non-whitespace char;
 *        std string -> unsigned conv yields max value, but we want 0;
 *        also checks for nullptr
 *****************************************************************************/
inline bool
fwd_to_unsigned_int(const char*& s)
{
    if(!s) return false;
    for(; std::isspace(*s); ++s);
    if(!s[0] || s[0] == '-') return false;
    if(s[0] == '-') return false;
    return true;
}


/*************************************************************************//**
 *
 * @brief value limits clamping
 *
 *****************************************************************************/
template<class T, class V, bool = (sizeof(V) > sizeof(T))>
struct limits_clamped {
    static T from(const V& v) {
        if(v > V(std::numeric_limits<T>::max())) {
            return std::numeric_limits<T>::max();
        }
        if(v < V(std::numeric_limits<T>::lowest())) {
            return std::numeric_limits<T>::lowest();
        }
        return T(v);
    }
};

template<class T, class V>
struct limits_clamped<T,V,false> {
    static T from(const V& v) { return T(v); }
};


/*************************************************************************//**
 *
 * @brief returns value of v as a T, clamped at T's maximum
 *
 *****************************************************************************/
template<class T, class V>
inline T clamped_on_limits(const V& v) {
    return limits_clamped<T,V>::from(v);
}



/*************************************************************************//**
 *
 * @brief type conversion helpers
 *
 *****************************************************************************/
template<class T>
struct convert;

template<class T>
struct convert {
    static inline T from(const char* s) {
        if(!s) return false;
        //a conversion from const char* to / must exist
        return static_cast<T>(s);
    }
};

template<>
struct convert<bool> {
    static inline bool from(const char* s) {
        if(!s) return false;
        return static_cast<bool>(s);
    }
};

template<>
struct convert<unsigned char> {
    static inline unsigned char from(const char* s) {
        if(!fwd_to_unsigned_int(s)) return (0);
        return clamped_on_limits<unsigned char>(std::strtoull(s,nullptr,10));
    }
};

template<>
struct convert<unsigned short int> {
    static inline unsigned short int from(const char* s) {
        if(!fwd_to_unsigned_int(s)) return (0);
        return clamped_on_limits<unsigned short int>(std::strtoull(s,nullptr,10));
    }
};

template<>
struct convert<unsigned int> {
    static inline unsigned int from(const char* s) {
        if(!fwd_to_unsigned_int(s)) return (0);
        return clamped_on_limits<unsigned int>(std::strtoull(s,nullptr,10));
    }
};

template<>
struct convert<unsigned long int> {
    static inline unsigned long int from(const char* s) {
        if(!fwd_to_unsigned_int(s)) return (0);
        return clamped_on_limits<unsigned long int>(std::strtoull(s,nullptr,10));
    }
};

template<>
struct convert<unsigned long long int> {
    static inline unsigned long long int from(const char* s) {
        if(!fwd_to_unsigned_int(s)) return (0);
        return clamped_on_limits<unsigned long long int>(std::strtoull(s,nullptr,10));
    }
};

template<>
struct convert<char> {
    static inline char from(const char* s) {
        //parse as single character?
        const auto n = std::strlen(s);
        if(n == 1) return s[0];
        //parse as integer
        return clamped_on_limits<char>(std::strtoll(s,nullptr,10));
    }
};

template<>
struct convert<short int> {
    static inline short int from(const char* s) {
        return clamped_on_limits<short int>(std::strtoll(s,nullptr,10));
    }
};

template<>
struct convert<int> {
    static inline int from(const char* s) {
        return clamped_on_limits<int>(std::strtoll(s,nullptr,10));
    }
};

template<>
struct convert<long int> {
    static inline long int from(const char* s) {
        return clamped_on_limits<long int>(std::strtoll(s,nullptr,10));
    }
};

template<>
struct convert<long long int> {
    static inline long long int from(const char* s) {
        return (std::strtoll(s,nullptr,10));
    }
};

template<>
struct convert<float> {
    static inline float from(const char* s) {
        return (std::strtof(s,nullptr));
    }
};

template<>
struct convert<double> {
    static inline double from(const char* s) {
        return (std::strtod(s,nullptr));
    }
};

template<>
struct convert<long double> {
    static inline long double from(const char* s) {
        return (std::strtold(s,nullptr));
    }
};


}  // namespace detail


//-------------------------------------------------------------------
template<class Target, class = std::enable_if_t<is_number<Target>::value>>
inline Target
to(const std::string& str) noexcept
{
    return detail::convert<Target>::from(str.c_str());
}

//-------------------------------------------------------------------
template<class Target, class = std::enable_if_t<is_number<Target>::value>>
inline Target
to(const char* str) noexcept
{
    return detail::convert<Target>::from(str);
}




/*****************************************************************************
 *
 * to-string converter functions
 *
 *****************************************************************************/
template<class T>
std::string
to_string(T&& x)
{
    return std::to_string(std::forward<T>(x));
}

//-------------------------------------------------------------------
template<class T, class = std::enable_if_t<is_number<T>::value>>
std::string
to_string(T number, int precision)
{
    std::ostringstream ss;
    if(precision > 0) ss.precision(precision);
    ss << number;
    return ss.str();
}


//-------------------------------------------------------------------
template<class T, class = std::enable_if_t<is_number<T>::value>>
std::string
to_fixed_string(T number, int precision)
{
    std::ostringstream ss;
    if(precision > 0) ss.precision(precision);
    ss << std::fixed << number;
    return ss.str();
}


//---------------------------------------------------------
template<class T, class = std::enable_if_t<is_number<T>::value>>
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
