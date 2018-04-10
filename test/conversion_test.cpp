/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/


#include  "../include/conversion.h"
#include  "../include/limits.h"

#include <stdexcept>
#include <iostream>


template<class T>
constexpr T eps = T(10) * am::num::tolerance<T>;


//-------------------------------------------------------------------
int main()
{
    using am::num::to;
    using am::num::tolerance;
    using std::abs;

    try {
        
        if( to<char>("123") != char(123) || 
            to<short>("1234") != short(1234) ||
            to<int>("123456") != 123456 ||
            to<long>("123456789") != 123456789L || 
            to<long long>("1234567890123456") != 1234567890123456LL
          )
        {
            throw std::runtime_error{"integer > 0"};
        }

        if( to<char>("-123") != char(-123) || 
            to<short>("-1234") != short(-1234) || 
            to<int>("-123456") != -123456 || 
            to<long>("-123456789") != -123456789L || 
            to<long long>("-1234567890123456") != -1234567890123456LL 
          )
        {
            throw std::runtime_error{"integer < 0"};
        }

        if( to<unsigned char>("123") != static_cast<unsigned char>(123) || 
            to<unsigned short>("1234") != static_cast<unsigned short>(1234) || 
            to<unsigned int>("123456") != static_cast<unsigned int>(123456) || 
            to<unsigned long>("123456789") != static_cast<unsigned long>(123456789) || 
            to<unsigned long long>("1234567890123456") != static_cast<unsigned long long>(1234567890123456)
          )
        {
            throw std::runtime_error{"unsigned integer > 0"};
        }

        if( to<unsigned char>("-123") != 0 || 
            to<unsigned short>("-1234") != 0 || 
            to<unsigned int>("-123456") != 0 || 
            to<unsigned long>("-123456789") != 0 || 
            to<unsigned long long>("-1234567890123456") != 0 
          )
        {
            throw std::runtime_error{"unsigned integer < 0"};
        }

        if( abs( to<float>("-0.3326") + 0.3326f) > eps<float> || 
            abs( to<float>("10.3326") - 10.3326f) > eps<float>|| 
            abs( to<float>("1032123215454543.33212321312132136") - 1032123215454543.33212321312132136f) > eps<float> 
          )
        {
            throw std::runtime_error{"float"};
        }

        if( abs( to<double>("-0.3326") + 0.3326) > eps<double> || 
            abs( to<double>("10.3326") - 10.3326) > eps<double>|| 
            abs( to<double>("1032123215454543.33212321312132136") - 1032123215454543.33212321312132136) > eps<double> 
          )
        {
            throw std::runtime_error{"double"};
        }

        if( abs( to<long double>("-0.3326") + 0.3326L) > eps<long double> || 
            abs( to<long double>("10.3326") - 10.3326L) > eps<long double>|| 
            abs( to<long double>("1032123215454543.33212321312132136") - 1032123215454543.33212321312132136L) > eps<long double> 
          )
        {
            throw std::runtime_error{"long double"};
        }

    }
    catch(std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
}

