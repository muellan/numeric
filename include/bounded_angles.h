/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2016 André Müller
 *
 *****************************************************************************/


#ifndef AMLIB_NUMERIC_ANGLES_H_
#define AMLIB_NUMERIC_ANGLES_H_


#include "angle.h"
#include "bounded.h"


namespace am {
namespace num {



/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class Turn, class Interval = turn_interval<angle<Turn>>>
using clipped_angle = clipped<angle<Turn>, Interval>;

template<class T>
using clipped_degrees = clipped_angle<degrees_turn<T>>;

template<class T>
using clipped_radians = clipped_angle<radians_turn<T>>;

template<class T>
using clipped_gons = clipped_angle<gons_turn<T>>;






/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class Turn>
using inclination_angle = clipped<angle<Turn>, inclination_interval<angle<Turn>>>;

template<class T>
using inclination_degrees = inclination_angle<degrees_turn<T>>;

template<class T>
using inclination_radians = inclination_angle<radians_turn<T>>;

template<class T>
using inclination_gons = inclination_angle<gons_turn<T>>;






/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class Turn>
using fov_angle = clipped<angle<Turn>, half_turn_interval<angle<Turn>>>;

template<class T>
using fov_degrees = fov_angle<degrees_turn<T>>;

template<class T>
using fov_radians = fov_angle<radians_turn<T>>;

template<class T>
using fov_gons = fov_angle<gons_turn<T>>;






/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class Turn, class Interval = turn_interval<angle<Turn>>>
using wrapped_angle = wrapped<angle<Turn>, Interval>;

template<class T>
using wrapped_degrees = wrapped_angle<degrees_turn<T>>;

template<class T>
using wrapped_radians = wrapped_angle<radians_turn<T>>;

template<class T>
using wrapped_gons = wrapped_angle<gons_turn<T>>;



}  // namespace num
}  // namespace am

#endif
