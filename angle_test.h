/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/


#ifndef AM_NUM_GEOMETRY_ANGLE_TEST_H_
#define AM_NUM_GEOMETRY_ANGLE_TEST_H_

#ifdef USE_TESTS

#include "angle.h"


namespace num {
namespace test {
namespace {


//-------------------------------------------------------------------
template<int a>
struct angle_tester {
	static constexpr int get() {return a; }
};


//-------------------------------------------------------------------
bool angle_init_correct()
{
	using std::abs;
	constexpr double eps = 1e-6;

	auto d0 = deg{15};
	deg  d1 {45.0f};
	auto d2 = 90_deg;	//via user-defined literal
	auto d3 = 1_pi_rad;	//via user-defined literal + implicit angle conversion
	auto d4 = radf{4.5};
	auto d5 = radd{pi};

	return (
		(abs(degrees_cast<double>(d0) - 15) < eps) &&
		(abs(degrees_cast<double>(d1) - 45) < eps) &&
		(abs(degrees_cast<double>(d2) - 90) < eps) &&
		(abs(radians_cast<double>(d3) - 1_pi) < eps) &&
		(abs(radians_cast<double>(d4) - 4.5) < eps) &&
		(abs(radians_cast<double>(d5) - pi) < eps) );
}


//-------------------------------------------------------------------
bool angle_conversion_correct()
{
	using std::abs;
	constexpr double eps = 1e-6;

	auto a = deg{90};
	auto b = rad{0.5_pi};

	return (
		(abs(radians_cast<double>(degi{180}) - 1.0_pi) < eps) &&
		(abs(degrees_cast<double>(rad{pi/4}) - 45) < eps) &&
		(abs(gons_cast<double>(rad{pi/4}) - 50) < eps) &&
		(abs(angle_cast<radians_turn<float>>(a) - 0.5_pi) < eps) &&
		(abs(angle_cast<degrees_turn<double>>(b) - 90) < eps) &&
		(abs(angle_cast<gons_turn<long double>>(b) - 100) < eps)
	);
}


//-------------------------------------------------------------------
bool angle_arithmetic_correct()
{
	using std::abs;
	constexpr double eps = 1e-6;

	auto a = degi{20};
	auto b = degi{30};
	auto c = degi{a+b};
	auto d = deg{deg{270} - rad{pi/2}};
	auto e = (90_deg + rad{0.5_pi} + 0.5_pi_rad);

	return (
		(degrees_cast<int>(c) == 50) &&
		(abs(degrees_cast<double>(d) - 180) < eps) &&
		(abs(degrees_cast<double>(e) - 270) < eps)	);
}


//-------------------------------------------------------------------
bool angle_modification_correct()
{
	//integral angles are incrementable
	degi di{65};
	++di;
	di++;

	//increment/decrement not possible with non-integral domain types
//	auto d = ++deg{40};

	return (degrees_cast<int>(di) == 67);
}


//-------------------------------------------------------------------
bool angle_static_eval_correct()
{
	//compile-time evaluation
	constexpr auto ca = degi{45};

	return (
			(angle_tester<degrees_cast<int>(ca)>::get() == 45) &&
			(angle_tester<degrees_cast<int>(degi{70})>::get() == 70)
		);
}


//-------------------------------------------------------------------
bool angle_functions_correct()
{
	using std::abs;
	constexpr double eps = 1e-6;

	return (
		(abs(cos(rad{0}) -  1) < eps) &&
		(abs(cos(rad{pi/2}) ) < eps) &&
		(abs(cos(deg{0}) -  1) < eps) &&
		(abs(cos(deg{90}) ) < eps) &&
		(abs(cos(deg{30}) - 0.8660254037844386) < eps) &&
		(abs(cos(rad{pi/3}) - 0.5) < eps) &&
		(abs(sin(deg{30}) - 0.5) < eps) &&
		(abs(sin(rad{pi/3}) - 0.8660254037844386) < eps) &&
		(abs(sin(rad{0}) ) < eps) &&
		(abs(sin(rad{pi/2}) - 1) < eps) &&
		(abs(sin(deg{0}) ) < eps) &&
		(abs(sin(deg{90}) - 1) < eps) &&
		(abs(degrees_cast<real_t>(deg{5}^2) - 25) < eps) );

}


//-------------------------------------------------------------------
bool angle_comparison_correct()
{
	//comparison: == and != only possible for integral angles
	return (
		(degi{30}  == degi{30}) &&
		(degi{30}  != degi{40}) &&
		(deg{180}  >  rad{pi/4}) &&
		(deg{30}   <  rad{pi}) &&
		(rad{pi}   >= deg{45}) &&
		(rad{2*pi} <= deg{720}) );
}


//-------------------------------------------------------------------
bool angle_correct()
{
	return (
			angle_init_correct() &&
			angle_conversion_correct() &&
			angle_arithmetic_correct() &&
			angle_modification_correct() &&
			angle_functions_correct() &&
			angle_comparison_correct() &&
			angle_static_eval_correct()
		);
}


}  // anonymous namespace
}  // namespace test
}  // namespace num


#endif

#endif
