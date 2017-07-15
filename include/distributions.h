/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/

#ifndef AMLIB_NUMERIC_DISTRIBUTIONS_H_
#define AMLIB_NUMERIC_DISTRIBUTIONS_H_


#include <random>
#include <functional>

#include "param.h"
#include "traits.h"


namespace am {
namespace num {

/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
namespace detail {

template<class NumT, bool isInt = is_integral<NumT>::value>
struct uniform_number_distribution_helper
{
    using type = std::uniform_real_distribution<NumT>;
};

template<class NumT>
struct uniform_number_distribution_helper<NumT,true>
{
    using type = std::uniform_int_distribution<NumT>;
};


}  // namespace detail



//-------------------------------------------------------------------
template<class NumT>
using uniform_number_distribution = typename
    detail::uniform_number_distribution_helper<NumT>::type;







/*****************************************************************************
 *
 *
 *
 *
 *****************************************************************************/
template<class Result, class ValueDistribution>
class distribution_adapter
{
    using component_type = numeric_t<Result>;

public:
    //---------------------------------------------------------------
    using result_type = Result;
    using param_type = typename ValueDistribution::param_type;


    //---------------------------------------------------------------
    distribution_adapter() = default;

    //-----------------------------------------------------
    explicit
    distribution_adapter(const param_type& p) :
        distr_{p}
    {}
    //-----------------------------------------------------
    template<class... Args>
    explicit
    distribution_adapter(Args&&... args) :
        distr_{std::forward<Args>(args)...}
    {}


    //---------------------------------------------------------------
    void
    reset() {
        distr_.reset();
    }


    //---------------------------------------------------------------
    void
    param(const param_type& par)    {
        distr_.param(par);
    }
    //-----------------------------------------------------
    auto
    param() const -> decltype(std::declval<const ValueDistribution&>().param()) {
        return distr_.param();
    }

    //-----------------------------------------------------
    auto
    min() const -> decltype(std::declval<const ValueDistribution&>().min()) {
        return distr_.min();
    }
    //-----------------------------------------------------
    auto
    max() const -> decltype(std::declval<const ValueDistribution&>().max()) {
        return distr_.max();
    }

    //---------------------------------------------------------------
    template<class URBG>
    result_type
    operator () (URBG& urbg)
    {
        return operator()(urbg, distr_.param());
    }
    //-----------------------------------------------------
    template<class URBG>
    result_type
    operator () (URBG& urbg, const param_type& par)
    {
        return result_type{distr_(urbg,par)};
    }


    //---------------------------------------------------------------
    template<class Ostream>
    friend Ostream&
    operator<<(Ostream& os, const distribution_adapter& d) {
        return (os << d.distr_);
    }
    //-----------------------------------------------------
    template<class Istream>
    friend Istream&
    operator>>(Istream& is, distribution_adapter& d) {
        return (is >> d.distr_);
    }


private:
    ValueDistribution distr_;
};



//-------------------------------------------------------------------
template<class Result>
using uniform_distribution_adapter =
    distribution_adapter<Result, uniform_number_distribution<numeric_t<Result>> >;


}  // namespace num
}  // namespace am

#endif // AMLIB_GEOMETRY_CREATION_POINTS_H_
