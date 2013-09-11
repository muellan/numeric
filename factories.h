#ifndef AM_NUMERIC_FACTORIES_H_
#define AM_NUMERIC_FACTORIES_H_


#include "traits.h"




namespace num {


/*****************************************************************************
 *
 *
 *
 *
 *
 *****************************************************************************/

//-----------------------------------------------------
/// @brief calls multi-arg ctor of 'Target' with n times 'a'
template<class Target, class Arg>
inline constexpr Target
make_uniform(Arg&& a)
{
	return gen::make_uniform<Target,dimensions<Target>::value>(
		std::forward<Arg>(a));
}



//-----------------------------------------------------
/// @brief calls multi-arg ctor of 'Target' with n times value obtained by calling g()
template<class Target, class Generator>
inline Target
make_generate(Generator&& g)
{
	return gen::make_generate<Target,dimensions<Target>::value>(
		std::forward<Generator>(g));
}


}  // namespace num





#endif
