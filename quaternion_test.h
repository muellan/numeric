#ifndef AM_NUMERIC_QUATERNION_TEST_H_
#define AM_NUMERIC_QUATERNION_TEST_H_

#ifdef AM_USE_TESTS

#include "quaternion.h"



namespace num {
namespace test {
namespace {


//-------------------------------------------------------------------
bool quaternion_assignment_correct()
{
	using std::abs;

	constexpr auto eps = epsilon<float>::value;

	auto q = quaternion<double>{0,0,0,0};

	q.assign(1,2,3,4);

	bool s1 = (abs(q[0] - 1) < eps) &&
		      (abs(q[1] - 2) < eps) &&
		      (abs(q[2] - 3) < eps) &&
		      (abs(q[3] - 4) < eps);

	return (s1);
}


//-------------------------------------------------------------------
bool quaternion_correct()
{
	return ( quaternion_assignment_correct() );
}


}  // anonymous namespace
}  // namespace test
}  // namespace num



#endif

#endif
