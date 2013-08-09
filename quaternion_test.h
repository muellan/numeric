/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/


#ifndef AM_NUMERIC_QUATERNION_TEST_H_
#define AM_NUMERIC_QUATERNION_TEST_H_

#ifdef USE_TESTS

#include "quaternion.h"


namespace num {
namespace test {
namespace {


//-------------------------------------------------------------------
bool quaternion_assignment_correct()
{
	using std::abs;

	constexpr auto eps = double(1e-8);
	auto q = quaternion<double>{0,0,0,0};

	q.real() = 1;
	q.imag()[0] = 2;
	q.imag()[1] = 3;
	q.imag()[2] = 4;

	bool s1 = (abs(q[0] - 1) < eps) &&
		      (abs(q[1] - 2) < eps) &&
		      (abs(q[2] - 3) < eps) &&
		      (abs(q[3] - 4) < eps);

	real(q)    = 4;
	imag(q)[0] = 5;
	imag(q)[1] = 6;
	imag(q)[2] = 7;

	bool s2 = (abs(q[0] - 4) < eps) &&
		      (abs(q[1] - 5) < eps) &&
		      (abs(q[2] - 6) < eps) &&
		      (abs(q[3] - 7) < eps);

	q.imag(0) = 1;
	q.imag(1) = 2;
	q.imag(2) = 3;

	bool s3 = (abs(q[0] - 4) < eps) &&
		      (abs(q[1] - 1) < eps) &&
		      (abs(q[2] - 2) < eps) &&
		      (abs(q[3] - 3) < eps);

	return (s1 && s2 && s3);
}


//-------------------------------------------------------------------
/// TODO more quaternion tests
bool quaternion_correct()
{
	return ( quaternion_assignment_correct() );
}


}  // anonymous namespace
}  // namespace test
}  // namespace num


#endif

#endif
