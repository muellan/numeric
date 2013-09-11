#ifndef AM_NUMERIC_CHOICE_H_
#define AM_NUMERIC_CHOICE_H_

#include <type_traits>
#include <limits>
#include <cstdint>


#include "concepts.h"




namespace num {


/*****************************************************************************
 *
 * @brief represents an element of IN / IN(numChoices)
 *
 * @tparam IntT       an integral type
 * @tparam numChoices number of choices
 *
 *
 *****************************************************************************/
template<class IntT, IntT numChoices>
class choice
{
	static_assert(is_integral_number<IntT>::value,
		"choice<T> : T has to be an integral number type");

public:
	//---------------------------------------------------------------
	using value_type = IntT;

	static constexpr value_type count = numChoices;


	//---------------------------------------------------------------
	constexpr explicit
	choice(const value_type& x = value_type(0)):
		x_{value_type(x % numChoices)}
	{}

	//-----------------------------------------------------
	template<class T>
	constexpr explicit
	choice(const T& x):
		x_{value_type(x % numChoices)}
	{
		static_assert(is_integral_number<T>::value,
			"choice::choice(N) : N has to be an integral number type");
	}

	//-----------------------------------------------------
	constexpr
	choice(const choice& c):
		x_{c.x_}
	{}


	//---------------------------------------------------------------
	choice&
	operator = (const choice&) = default;

	//-----------------------------------------------------
	choice&
	operator = (const value_type& x) {
		x_ = value_type(x % numChoices);
		return *this;
	}

	//-----------------------------------------------------
	template<class T>
	choice&
	operator = (const T& x)
	{
		static_assert(is_integral_number<T>::value,
			"choice::operator=(N) : N has to be an integral number type");

		auto v = x % numChoices;
		x_ = value_type((v >= 0) ? v : (numChoices-v));
		return *this;
	}


	//---------------------------------------------------------------
	constexpr
	value_type
	operator * () const {
		return x_;
	}

	//-----------------------------------------------------
	constexpr
	value_type
	value() const {
		return x_;
	}

	//-----------------------------------------------------
	template<class T>
	explicit
	operator T () const
	{
		static_assert(is_integral_number<T>::value,
			"T(choice) : T has to be an integral number type");

		return x_;
	}


	//---------------------------------------------------------------
	choice&
	operator += (choice c) {
		x_ += c.x_;
		if(x_ > numChoices) x_ -= numChoices;
		return *this;
	}
	//-----------------------------------------------------
	choice&
	operator += (const value_type& x) {
		x_ = value_type((x_ + x) % numChoices);
		return *this;
	}
	//-----------------------------------------------------
	template<class T>
	choice&
	operator += (const T& x)
	{
		static_assert(is_integral_number<T>::value,
			"choice::operator+=(T) : T has to be an integral number type");

		auto v = (x_ + x) % numChoices;
		x_ = value_type((v >= 0) ? v : (numChoices+v));
		return *this;
	}


	//---------------------------------------------------------------
	choice&
	operator -= (choice c) {
		if(x_ > c.x_)
			x_ -= c.x_;
		else
			x_ = numChoices - c.x_ + x_;
		return *this;
	}
	//-----------------------------------------------------
	choice&
	operator -= (const value_type& x) {
		auto v = (x_ - x) % numChoices;
		x_ = value_type((v >= 0) ? v : (numChoices+v));
		return *this;
	}
	//-----------------------------------------------------
	template<class T>
	choice&
	operator -= (const T& x)
	{
		static_assert(is_integral_number<T>::value,
			"choice::operator-=(T) : T has to be an integral number type");

		auto v = (x_ - x) % numChoices;
		x_ = value_type((v >= 0) ? v : (numChoices+v));
		return *this;
	}



	//---------------------------------------------------------------
	choice
	operator + (choice c) const {
		auto nx = x_ + c.x_;
		if(nx > numChoices) nx -= numChoices;
		//use special non-mod ctor
		return choice{value_type(nx), 0};
	}
	//-----------------------------------------------------
	choice
	operator + (const value_type& x) const {
		//use special non-mod ctor
		return choice{value_type((x_ + x) % numChoices), 0};
	}
	//-----------------------------------------------------
	template<class T>
	choice
	operator + (const T& x) const
	{
		static_assert(is_integral_number<T>::value,
			"choice::operator+(T) : T has to be an integral number type");

		auto v = (int(x_) + x) % numChoices;
		//use special non-mod ctor
		return choice{value_type((v >= 0) ? v : (numChoices+v)), 0};
	}


	//---------------------------------------------------------------
	choice
	operator - (choice c) const {
		//use special non-mod ctor
		return choice{
			value_type((x_ > c.x_) ? (x_ - c.x_) : (numChoices - c.x_ + x_)), 0};
	}
	//-----------------------------------------------------
	choice
	operator - (const value_type& x) const {
		auto v = (int(x_) - x) % numChoices;
		//use special non-mod ctor
		return choice{value_type((v >= 0) ? v : (numChoices+v)), 0};
	}
	//-----------------------------------------------------
	template<class T>
	choice
	operator - (const T& x) const
	{
		static_assert(is_integral_number<T>::value,
			"choice::operator-(T) : T has to be an integral number type");

		auto v = (int(x_) - x) % numChoices;
		//use special non-mod ctor
		return choice{value_type((v >= 0) ? v : (numChoices+v)), 0};
	}


	//---------------------------------------------------------------
	constexpr choice
	operator - () const {
		//use special non-mod ctor
		return choice{value_type(numChoices - x_), 0};
	}

	//-----------------------------------------------------
	choice&
	invert() {
		x_ = value_type(numChoices - x_);
		return *this;
	}


	//---------------------------------------------------------------
	choice&
	operator ++ () {
		if(x_ < (numChoices-1)) ++x_; else x_ = 0;
		return *this;
	}
	//-----------------------------------------------------
	choice
	operator ++ (int) {
		choice old{*this};
		++*this;
		return old;
	}
	//-----------------------------------------------------
	choice&
	operator -- () {
		if(x_ > 0) --x_; else x_ = value_type(numChoices-1);
		return *this;
	}
	//-----------------------------------------------------
	choice
	operator -- (int) {
		choice old{*this};
		++*this;
		return old;
	}


private:
	//---------------------------------------------------------------
	/// @brief special ctor without modulo operation
	constexpr explicit
	choice(const value_type& x, int):
		x_{x}
	{}

	value_type x_ = value_type(0);
};






/*****************************************************************************
 *
 * COMPARISON
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class Int1, Int1 n, class Int2, Int2 m>
inline constexpr
bool
operator == (choice<Int1,n> a, choice<Int2,m> b) {
	return (*a == *b);
}

//---------------------------------------------------------
template<class Int1, Int1 n, class Int2, Int2 m>
inline constexpr
bool
operator != (choice<Int1,n> a, choice<Int2,m> b) {
	return (*a != *b);
}


//-------------------------------------------------------------------
template<class Int1, Int1 n, class Int2, Int2 m>
inline constexpr
bool
operator <  (choice<Int1,n> a, choice<Int2,m> b) {
	return (*a < *b);
}

//---------------------------------------------------------
template<class Int1, Int1 n, class Int2, Int2 m>
inline constexpr
bool
operator <= (choice<Int1,n> a, choice<Int2,m> b) {
	return (*a <= *b);
}


//-------------------------------------------------------------------
template<class Int1, Int1 n, class Int2, Int2 m>
inline constexpr
bool
operator >  (choice<Int1,n> a, choice<Int2,m> b) {
	return (*a > *b);
}

//---------------------------------------------------------
template<class Int1, Int1 n, class Int2, Int2 m>
inline constexpr
bool
operator >= (choice<Int1,n> a, choice<Int2,m> b) {
	return (*a >= *b);
}






/*****************************************************************************
 *
 * FACTORIES
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<std::uintmax_t n, class T>
inline constexpr choice<typename std::make_unsigned<T>::type,n>
make_choice(const T& k)
{
	static_assert(is_integral_number<T>::value,
			"make_choice(T) : T has to be an integral number type");

	using res_t = typename std::make_unsigned<T>::type;

	return choice<res_t,n>{res_t(k)};
}






/*****************************************************************************
 *
 * I/O
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class Ostream, class Int, Int n>
inline constexpr Ostream&
operator << (Ostream& os, const choice<Int,n>& c)
{
	return (os << std::intmax_t(*c));
}

//---------------------------------------------------------
template<class Ostream, class Int, Int n>
inline constexpr
Ostream&
print(Ostream& os, const choice<Int,n>& c)
{
	return (os << "[" << std::intmax_t(*c) << "/" << std::intmax_t(n) << "]");
}




/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class Int, Int n>
inline constexpr choice<Int,n>
inverse(choice<Int,n> c)
{
	return choice<Int,n>{-c};
}


}  // namespace num




#endif
