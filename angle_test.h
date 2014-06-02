/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2014 André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_ANGLE_TEST_H_
#define AM_NUMERIC_ANGLE_TEST_H_

#ifdef AM_USE_TESTS


namespace am {
namespace num {
namespace test {


void angle_init_correctness();
void angle_conversion_correctness();
void angle_arithmetic_correctness();
void angle_modification_correctness();
void angle_static_eval_correctness();
void angle_functions_correctness();
void angle_comparison_correctness();

void angle_correctness();


}  // namespace test
}  // namespace num
}  // namespace am


#endif

#endif
