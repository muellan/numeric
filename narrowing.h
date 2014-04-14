/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 *2008-2014  André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_NARROWING_H_
#define AM_NUMERIC_NARROWING_H_


#include "traits.h"



namespace am {

namespace num {


/*****************************************************************************
 *
 * @brief traits class for testing if a conversion is narrowing
 *
 * @details
 * non-narrowing if 'To' is the same as common type of 'F' through 'Fs'
 * and all types are either all signed or all unsigned
 *
 *
 *****************************************************************************/

namespace detail {


//-------------------------------------------------------------------
template<bool numbers, class To, class F, class... Fs>
struct is_non_narrowing_helper :
    public std::false_type
{};

//---------------------------------------------------------
template<class To, class F, class... Fs>
struct is_non_narrowing_helper<true,To,F,Fs...> :
    public std::integral_constant<bool,
        is_non_narrowing_helper<true,
            decay_t<To>,
            decay_t<F>>::value
        &&
        is_non_narrowing_helper<true,
            decay_t<To>,
            decay_t<Fs>...>::value>
{};

//---------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true,To,From> :
    public std::integral_constant<bool,
        std::is_same<
            decay_t<To>,
            typename std::common_type<
                decay_t<From>,
                decay_t<To>
            >::type
        >::value &&
        ( ( std::is_signed<decay_t<From>>::value &&
            std::is_signed<decay_t<To>>::value
          ) ||
          ( std::is_unsigned<decay_t<From>>::value &&
            std::is_unsigned<decay_t<To>>::value
          )
        )
    >
{};




//---------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, std::complex<To>, From> :
    public is_non_narrowing_helper<true,To,From>
{};

//---------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, std::complex<To>, std::complex<From> > :
    public is_non_narrowing_helper<true,To,From>
{};

//---------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, To, std::complex<From> > :
    public is_non_narrowing_helper<true,To,From>
{};


}  // namespace detail



//-------------------------------------------------------------------
template<class To, class From, class... Froms>
struct is_non_narrowing :
    public std::integral_constant<bool,
        detail::is_non_narrowing_helper<
            is_number<decay_t<To>, decay_t<From>, decay_t<Froms>...>::value,
            decay_t<To>, decay_t<From>, decay_t<Froms>...>::value
    >
{};



//-------------------------------------------------------------------
#ifndef AM_ALLOW_NARROWING

#define AM_CHECK_NARROWING(TARGET,SOURCE) \
    static_assert(num::is_non_narrowing< \
        TARGET, SOURCE >::value, \
        "narrowing conversion");


#define AM_CHECK_NARROWING2(TARGET,S1,S2) \
    static_assert(num::is_non_narrowing< \
        TARGET, S1 , S2 >::value, \
        "narrowing conversion");


#define AM_CHECK_NARROWING3(TARGET,S1,S2,S3) \
    static_assert(num::is_non_narrowing< \
        TARGET , S1 , S2 , S3 >::value, \
        "narrowing conversion");


#define AM_CHECK_NARROWING4(TARGET,S1,S2,S3,S4) \
    static_assert(num::is_non_narrowing< \
        TARGET , S1 , S2 , S3 , S4 >::value, \
        "narrowing conversion");


#define AM_CHECK_NARROWING5(TARGET,S1,S2,S3,S4,S5) \
    static_assert(num::is_non_narrowing< \
        TARGET , S1 , S2 , S3 , S4 , S5 >::value, \
        "narrowing conversion");


#else

#define AM_CHECK_NARROWING(TARGET,SOURCE)
#define AM_CHECK_NARROWING2(TARGET,S1,S2)
#define AM_CHECK_NARROWING3(TARGET,S1,S2,S3)
#define AM_CHECK_NARROWING4(TARGET,S1,S2,S3,S4)
#define AM_CHECK_NARROWING5(TARGET,S1,S2,S3,S4,S5)

#endif


}  // namespace num

}  // namespace am



#endif
