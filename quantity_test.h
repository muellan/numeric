/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2014 André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_QUANTITY_TEST_H_
#define AM_NUMERIC_QUANTITY_TEST_H_

#ifdef AM_USE_TESTS


namespace am {
namespace num {
namespace test {


void quantity_init_correctness();
void quantity_arithmetic_correctness();
void quantity_comparison_correctness();

void quantity_correctness();


}  // namespace test
}  // namespace num
}  // namespace am


#endif

#endif
