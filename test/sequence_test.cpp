/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/


#include  "../include/sequ_linear.h"
#include  "../include/sequ_geometric.h"
#include  "../include/sequ_fibonacci.h"
#include  "../include/sequ_repeated.h"
#include  "../include/sequ_combined.h"
#include  "../include/equality.h"

#include <vector>
#include <iostream>

using namespace am;
using namespace am::num;


//-------------------------------------------------------------------
void linear_sequence_generation()
{
    {
        auto v = std::vector<int>{};
        for(auto x : linear_sequence<int>{0,1,0}) {
            v.push_back(x);
        }
        if(v.front() != 0 || v.back() != 0 || v.size() != 1) {
            throw std::logic_error("linear_sequence");
        }
    }

    {
        auto v = std::vector<int>{};
        for(auto x : linear_sequence<int>{0,2,20}) {
            v.push_back(x);
        }
        if(v.front() != 0 || v.back() != 20 || v.size() != 11) {
            throw std::logic_error("linear_sequence");
        }
    }

    {
        auto v = std::vector<int>{};
        for(auto x : linear_sequence<int>{20,-2,0}) {
            v.push_back(x);
        }
        if(v.front() != 20 || v.back() != 0 || v.size() != 11) {
            throw std::logic_error("linear_sequence");
        }
    }

    {
        auto g = linear_sequence<double>{0,2,20};

        if(g.size() != 11u) {
            throw std::logic_error("linear_sequence");
        }
    }


}



//-------------------------------------------------------------------
void geometric_sequence_generation()
{
    {
        auto v = std::vector<double>{};
        for(auto x : geometric_sequence<double>{1.0, 0.0, 1.0}) {
            v.push_back(x);
        }
        if(!approx_equal(v.front(),1) || !approx_equal(v.back(),1) ||
            v.size() != 1)
        {
            throw std::logic_error("geometric_sequence");
        }
    }

    {
        auto v = std::vector<double>{};
        for(auto x : geometric_sequence<double>{128.0, 0.5, 1.0}) {
            v.push_back(x);
        }
        if(!approx_equal(v.front(),128) || !approx_equal(v.back(),1) ||
            v.size() != 8)
        {
            throw std::logic_error("geometric_sequence");
        }

    }

    {
        auto g = geometric_sequence<double>{128.0, 0.5, 1.0};

        if(g.size() != 8u) {
            throw std::logic_error("geometric_sequence");
        }
    }

}



//-------------------------------------------------------------------
void fibonacci_sequence_generation()
{
    {
        auto v = std::vector<double>{};
        for(auto x : fibonacci_sequence<int>{15}) {
            v.push_back(x);
        }
        if( v.size() != 15 ||
            v[0] != 0 || v[1] != 1 || v[2] != 1 || v[3] != 2 ||
            v[4] != 3 || v[5] != 5 || v[6] != 8 || v[7] != 13 ||
            v[8] != 21 || v[9] != 34 || v[10] != 55 || v[11] != 89 ||
            v[12] != 144 || v[13] != 233 || v[14] != 377)
        {
            throw std::logic_error("fibonacci_sequence");
        }

    }

}



//-------------------------------------------------------------------
void repeated_sequence_generation()
{
    {
        auto v = std::vector<double>{};

        auto g = repeated_sequence<linear_sequence<int>> {
            make_linear_sequence(8, -1, 1)};

        for(auto x : g) {
            v.push_back(x);
        }
        if(!approx_equal(v.front(),8) || !approx_equal(v.back(),1) ||
            v.size() != 8)
        {
            throw std::logic_error("repeated_sequence");
        }
    }

    {
        auto v = std::vector<double>{};

        auto g = repeated_sequence<linear_sequence<int>> {
            make_linear_sequence(8, -1, 1), 2};

        for(auto x : g) {
            v.push_back(x);
        }
        if(!approx_equal(v.front(),8) || !approx_equal(v.back(),1) ||
            v.size() != 24)
        {
            throw std::logic_error("repeated_sequence");
        }
    }

    {
        auto v = std::vector<double>{};

        auto g = repeated_sequence<linear_sequence<int>> {
            make_linear_sequence(8, -1, 1),
            make_linear_sequence(5, -1, 1), 2};

        for(auto x : g) {
            v.push_back(x);
        }
        if(!approx_equal(v.front(),8) || !approx_equal(v.back(),1) ||
            v.size() != 18)
        {
            throw std::logic_error("repeated_sequence");
        }
    }

}



//-------------------------------------------------------------------
void combined_sequence_generation()
{
    {
        auto v = std::vector<double>{};

        auto g = combined_sequence<linear_sequence<int>,linear_sequence<double>> {
            make_linear_sequence(8, -1, 1),
            make_linear_sequence(1.0, 1.0, 8.0)};

        for(auto x : g) {
            v.push_back(x);
        }
        if(!approx_equal(v.front(),8) || !approx_equal(v.back(),8) ||
            v.size() != 16)
        {
            throw std::logic_error("combined_sequence");
        }
    }

    {
        auto v = std::vector<double>{};

        auto g = make_combined_sequence(
            make_linear_sequence(8, -1, 1),
            make_linear_sequence(1.0, 1.0, 8.0));

        for(auto x : g) {
            v.push_back(x);
        }
        if(!approx_equal(v.front(),8) || !approx_equal(v.back(),8) ||
            v.size() != 16)
        {
            throw std::logic_error("combined_sequence");
        }
    }


}



//-------------------------------------------------------------------
int main()
{
    try {
        linear_sequence_generation();
        geometric_sequence_generation();
        fibonacci_sequence_generation();
        repeated_sequence_generation();
        combined_sequence_generation();

//        std::cout << items(make_ascending_sequence(2,10)) << std::endl;
//        std::cout << items(make_descending_sequence(10,2)) << std::endl;
//        std::cout << items(make_linear_sequence(2,2,10)) << std::endl;
//        std::cout << items(make_linear_sequence(10,-2,2)) << std::endl;
//        std::cout << items(make_linear_sequence(2,3,10)) << std::endl;
//
//        std::cout << items(
//            repeated_sequence<linear_sequence<int>> {
//                make_linear_sequence(8, -1, 1)} ) << std::endl;
//
//        std::cout << items(
//            repeated_sequence<linear_sequence<int>> {
//                make_linear_sequence(8, -1, 1), 2} ) << std::endl;
//
//        std::cout << items(
//            repeated_sequence<linear_sequence<int>> {
//                make_linear_sequence(8, -1, 1),
//                make_linear_sequence(5, -1, 1), 2} ) << std::endl;
//
//        std::cout << items(num::fibonacci_sequence<int>(15)) << std::endl;
//        std::cout << fibonacci_sequence<int>(15).size() << std::endl;

    }
    catch(std::exception& e) {
        std::cerr << e.what();
        return 1;
    }
}


