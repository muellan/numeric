/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/

#ifdef AM_USE_TESTS

#include <stdexcept>

#include "quaternion.h"
#include "quaternion_test.h"


namespace am {
namespace num {
namespace test {


//-------------------------------------------------------------------
void quaternion_assignment_correctness()
{
	using std::abs;

	constexpr auto eps = 0.001;

	auto q = quaternion<double>{0,0,0,0};

	q.assign(1,2,3,4);

	if(!( (abs(q[0] - 1) < eps) &&
		  (abs(q[1] - 2) < eps) &&
		  (abs(q[2] - 3) < eps) &&
		  (abs(q[3] - 4) < eps)) )
	{
		throw std::logic_error("am::num::quaternion assignment");
	}
}


//-------------------------------------------------------------------
void quaternion_correctness()
{
	quaternion_assignment_correctness();
}


}  // namespace test
}  // namespace num
}  // namespace am


#endif
