#ifndef AM_NUMERIC_CHOICE_TEST_H_
#define AM_NUMERIC_CHOICE_TEST_H_


#include "choice.h"



namespace num {
namespace test {

namespace {

//-------------------------------------------------------------------
bool choice_init_correct()
{
	using num::choice;
	auto c1 = choice<std::int8_t,8>{0};
	auto c2 = choice<std::int8_t,8>{1};
	auto c3 = choice<std::int8_t,8>{5};
	auto c4 = choice<std::int8_t,8>{8};
	auto c5 = choice<std::int8_t,8>{123};

	return (
			(int(c1) == 0) &&
			(int(c2) == 1) &&
			(int(c3) == 5) &&
			(int(c4) == 0) &&
			(int(c5) == 3)
		);
}


//-------------------------------------------------------------------
bool choice_arithmetic_correct()
{
	auto c1 = choice<std::int8_t,8>{0};

	auto c2 = choice<std::int8_t,11>{0};
	c2 += 123456;
	auto c3 = c2;
	c3 -= 1123;

	return (
		(int(c1 +    1) == 1) &&
		(int(c1 +    2) == 2) &&
		(int(c1 +    8) == 0) &&
		(int(c1 + 1233) == 1) &&
		(int(c1 -    5) == 3) &&
		(int(c1 - 2323) == 5)
		&&
		(int(c1 -    1) == 7) &&
		(int(c1 -    2) == 6) &&
		(int(c1 -    8) == 0) &&
		(int(c1 - 1233) == 7) &&
		(int(c1 +    5) == 5) &&
		(int(c1 + 2323) == 3)
		&&
		(int(c2) == 3) &&
		(int(c3) == 2)
	);
}



//-------------------------------------------------------------------
bool choice_correct()
{
	return (
		choice_init_correct() &&
		choice_arithmetic_correct()
	);
}


}  // namespace

}  // namespace test
}  // namespace num



#endif
