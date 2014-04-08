/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_TESTS_H_
#define AM_NUMERIC_TESTS_H_

#ifdef AM_USE_TESTS


#include "angle_test.h"
#include "choice_test.h"
//#include "rational_test.h"
//#include "dual_test.h"
//#include "scomplex_test.h"
#include "quaternion_test.h"
#include "sequence_test.h"



namespace am {
namespace num {
namespace test {


//-------------------------------------------------------------------
void vector_expression_performance();


}  // namespace test
}  // namespace num
}  // namespace am



namespace am {
namespace test {


//-------------------------------------------------------------------
void numeric_correctness();


}  // namespace test
}  // namespace am


#endif

#endif
