/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/


#include  "../include/quaternion.h"
#include  "../include/limits.h"

#include <stdexcept>
#include <iostream>




//-------------------------------------------------------------------
template<class T>
void test()
{
    using namespace am;
    using namespace am::num;

    using std::abs;
    using std::sqrt;

    const auto eps = T(1)/T(1000);

    quaternion<T> q0;
    if( abs(q0.real()   - 1) > eps ||
        abs(q0.imag_i()    ) > eps ||
        abs(q0.imag_j()    ) > eps ||
        abs(q0.imag_k()    ) > eps )
    {
        throw std::runtime_error{"wrong values after default construction"};
    }

    quaternion<T> q1{1,2,3,4};

    if( abs(q1.real()   - 1) > eps ||
        abs(q1.imag_i() - 2) > eps ||
        abs(q1.imag_j() - 3) > eps ||
        abs(q1.imag_k() - 4) > eps ) 
    {
        throw std::runtime_error{"wrong values after non-default construction"};
    }

    q1.conjugate();
    if( abs(q1.real()   - 1) > eps ||
        abs(q1.imag_i() + 2) > eps ||
        abs(q1.imag_j() + 3) > eps ||
        abs(q1.imag_k() + 4) > eps ) 
    {
        throw std::runtime_error{"wrong values after conjugation"};
    }

    if(abs(norm(q1) - std::sqrt(T(30))) > eps) throw std::runtime_error{"wrong norm"};

    norm2(q1);
    if(abs(norm2(q1) - 30) > eps) throw std::runtime_error{"wrong norm2"};

    q1.normalize();

    if(abs(norm(q1) - 1) > eps) throw std::runtime_error{"wrong norm after normalization"};
    if(abs(norm2(q1) - 1) > eps) throw std::runtime_error{"wrong norm2 after normalization"};

    const auto v = T(3);
    q1 += v;
    q1 -= v;
    q1 *= v;
    q1 /= v;

    if( abs(q1.real()   - T(0.182574)) > eps ||
        abs(q1.imag_i() + T(0.365148)) > eps ||
        abs(q1.imag_j() + T(0.547723)) > eps ||
        abs(q1.imag_k() + T(0.730297)) > eps ) 
    {
        throw std::runtime_error{"wrong values after mutating arithmetic quat-value ops"};
    }

    quaternion<T> q2{4,3,2,1};
    q2.invert();
    if(abs(norm(q2) - 1) > eps) throw std::runtime_error{"wrong norm after inversion"};
    if(abs(norm2(q2) - 1) > eps) throw std::runtime_error{"wrong norm2 after inversion"};

    quaternion<T> q3{4,3,2,1};

    q1 += q3;
    q1 -= q3;
    q1 *= q3;
    
    if( abs(q1.real()   - T(3.65148)) > eps ||
        abs(q1.imag_i()             ) > eps ||
        abs(q1.imag_j() + T(3.65148)) > eps ||
        abs(q1.imag_k() + T(1.82574)) > eps ) 
    {
        throw std::runtime_error{"wrong values after mutating arithmetic quat-quat ops"};
    }

    q1.times_conj(q3);
    
    if( abs(q1.real()   - T(5.47723)) > eps ||
        abs(q1.imag_i() + T(10.9545)) > eps ||
        abs(q1.imag_j() + T(16.4317)) > eps ||
        abs(q1.imag_k() + T(21.9089)) > eps ) 
    {
        throw std::runtime_error{"wrong values after mutating times_conj"};
    }
    
    q1.conj_times(q3);
   
    if( abs(q1.real()   + T(65.7267)) > eps ||
        abs(q1.imag_i() - T(32.8634)) > eps ||
        abs(q1.imag_j() - T(131.453)) > eps ||
        abs(q1.imag_k() - T(65.7267)) > eps ) 
    {
        throw std::runtime_error{"wrong values after mutating conj_times"};
    }
    

    auto q4 = conj_times(q1,q2);

    if( abs(q4.real()   + T(126)) > eps ||
        abs(q4.imag_i() - T(12)) > eps ||
        abs(q4.imag_j() + T(42)) > eps ||
        abs(q4.imag_k() + T(96)) > eps ) 
    {
        throw std::runtime_error{"wrong values after conj_times(q1,q2)"};
    }

    auto q5 = times_conj(q1,q2);

    if( abs(q5.real()   + T(126)) > eps ||
        abs(q5.imag_i() + T(12)) > eps ||
        abs(q5.imag_j() - T(102)) > eps ||
        abs(q5.imag_k() + T(24)) > eps ) 
    {
        throw std::runtime_error{"wrong values after conj_times(q1,q2)"};
    }
}



//-------------------------------------------------------------------
int main()
{
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
