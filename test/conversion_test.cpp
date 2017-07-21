/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/


#include  "conversion.h"

#include <stdexcept>
#include <iostream>

using namespace am;


//-------------------------------------------------------------------
int main()
{
    return 0;

    std::cout.precision(32);

    std::cout << "integeral > 0:\n"
              << int(num::to<char>("123")) <<'\n'
              << num::to<short>("1234") <<'\n'
              << num::to<int>("123456") <<'\n'
              << num::to<long>("123456789") <<'\n'
              << num::to<long long>("1234567890123456") <<'\n'
              << "integeral < 0:\n"
              << int(num::to<char>("-123")) <<'\n'
              << num::to<short>("-1234") <<'\n'
              << num::to<int>("-123456") <<'\n'
              << num::to<long>("-123456789") <<'\n'
              << num::to<long long>("-1234567890123456") <<'\n'
              << "unsigned integeral > 0:\n"
              << int(num::to<unsigned char>("123")) <<'\n'
              << num::to<unsigned short>("1234") <<'\n'
              << num::to<unsigned int>("123456") <<'\n'
              << num::to<unsigned long>("123456789") <<'\n'
              << num::to<unsigned long long>("1234567890123456") <<'\n'
              << "unsigned integeral < 0:\n"
              << int(num::to<unsigned char>("-123")) <<'\n'
              << num::to<unsigned short>("-1234") <<'\n'
              << num::to<unsigned int>("-123456") <<'\n'
              << num::to<unsigned long>("-123456789") <<'\n'
              << num::to<unsigned long long>("-1234567890123456") <<'\n'
              << "float:\n"
              << num::to<float>("-0.3326") <<'\n'
              << num::to<float>("10.3326") <<'\n'
              << num::to<float>("1032123215454543.33212321312132136") <<'\n'
              << "double:\n"
              << num::to<double>("-0.3326") <<'\n'
              << num::to<double>("10.3326") <<'\n'
              << num::to<double>("1032123215454543.33212321312132136") <<'\n'
              << "long double:\n"
              << num::to<long double>("-0.3326") <<'\n'
              << num::to<long double>("10.3326") <<'\n'
              << num::to<long double>("1032123215454543.33212321312132136") <<'\n'
              << std::endl;
}

