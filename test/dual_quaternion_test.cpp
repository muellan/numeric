/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/


#include  "../include/dual_quaternion.h"
#include  "../include/limits.h"
#include  "../include/traits.h"

#include <stdexcept>
#include <iostream>
#include <complex>




//-------------------------------------------------------------------
template<class T>
void test()
{
    using namespace am;
    using namespace am::num;

    using std::abs;

    using quat_t = dual_quaternion<T>;

    const auto eps = T(1) / T(1000);

    quat_t q1;
    quat_t q2 { dual<T>(1,-1), dual<T>(2,1), dual<T>(3,2), dual<T>(4,6)};
    
    q2.normalize();

    auto n = norm(q2);
    if(abs(n.real() - T(1)) > eps) {
        throw std::runtime_error{"wrong norm after normalization"};
    }

    quat_t q3 { {1,-1}, {2,1}, {3,2}, {4,6}};
    q3.conjugate();
}



//-------------------------------------------------------------------
int main()
{
    using namespace am::num;

    try {
        test<float>();
        test<double>();
        test<long double>();
    }
    catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}

