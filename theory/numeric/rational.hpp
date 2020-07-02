/*********************************************************************
 *
 * Optimality Syntactic Dependency Distances
 * 
 * Copyright (C) 2020 Lluís Alemany-Puig
 *
 * This file is part of Optimality Syntactic Dependency Distances.
 *
 * Optimality Syntactic Dependency Distances is free software: you can redistribute
 * it and/or modify it under the terms of the GNU Affero General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Optimality Syntactic Dependency Distances is distributed in the hope that
 * it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Linear Arrangement Library.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact:
 * 
 *     Lluís Alemany Puig (lalemany@cs.upc.edu)
 *         LARCA (Laboratory for Relational Algorithmics, Complexity and Learning)
 *         CQL (Complexity and Quantitative Linguistics Lab)
 *         Jordi Girona St 1-3, Campus Nord UPC, 08034 Barcelona.    CATALONIA, SPAIN
 *         Webpage: https://cqllab.upc.edu/people/lalemany/
 *
 ********************************************************************/
 
#pragma once

// C includes
#include <gmp.h>

// C++ includes
#include <cstdint>
#include <string>
#include <ostream>

/**
 * @brief Exact rational number.
 *
 * This class wraps the @e mpq_t data structure from GMP
 * with operators for an easy usage.
 *
 * In order to be used, it must first be initialised, with
 * one of the following methods:
 * - @ref init
 * - @ref init_si
 * - @ref init_ui
 * - @ref init_str
 * - @ref init_integer
 *
 * or via the class constructors. The empty constructor, however,
 * does not initialise the internal memory.
 *
 * Although the internal memory is deallocated automatically
 * the user can do so manually by calling method @ref clear.
 */
class rational {
	public:
		/// Default constructor.
		rational();
		/// Constructor with numerator and denominator.
		rational(int64_t n, uint64_t d = 1);
		/// Constructor with string.
		rational(const std::string& s);
		/// Copy constructor.
		rational(const rational& r);
		/// Destructor.
		~rational();

		/* ALLOC AND DEALLOC */

		/// Initialises internal memory.
		void init();
		/// Initialises this rational as \f$n/d\f$.
		void init_si(int64_t n, uint64_t d = 1);
		/// Initialises this rational as \f$n/d\f$.
		void init_ui(uint64_t n, uint64_t d = 1);
		/// Initialises this rational with the contents in string @e s, in base 10.
		void init_str(const std::string& s);
		/// Initialises this rational with the value in @e mpq.
		void init_mpq(const mpq_t& mpq);

		/// Clear the memory of this rational.
		void clear();

		/* SET VALUE */

		/// Overwrites the value of this rational with \f$n/d\f$.
		void set_si(int64_t n, uint64_t d = 1);
		/// Overwrites the value of this rational with \f$n/d\f$.
		void set_ui(uint64_t n, uint64_t d = 1);
		/// Overwrites the value in the string @e s.
		void set_str(const std::string& s);
		/// Overwrites the value of this rational with the value in @e mpq.
		void set_mpq(const mpq_t& mpq);
		/// Overwrites the value of this rational with the value in @e r.
		void copy(const rational& r);

		/**
		 * @brief Changes numerator and denominator.
		 *
		 * If this rational value was \f$n/d\f$ then after calling this
		 * method it will be \f$d/n\f$.
		 */
		void invert();

		/* OPERATORS */
		
		/// Standard output operator for the @ref rational class.
		friend inline
		std::ostream& operator<< (std::ostream& os, const rational& r) {
			os << r.to_string();
			return os;
		}

		/// Assignation operator.
		rational& operator= (int64_t i);
		/// Assignation operator.
		rational& operator= (const rational& r);

		/// Equality operator.
		bool operator== (int64_t i) const;
		/// Equality operator.
		bool operator== (const rational& r) const;

		/// Difference operator.
		bool operator!= (int64_t i) const;
		/// Difference operator.
		bool operator!= (const rational& r) const;

		/// Less than operator.
		bool operator< (int64_t i) const;
		/// Less than operator.
		bool operator< (const rational& r) const;

		/// Less than or equal to operator.
		bool operator<= (int64_t i) const;
		/// Less than or equal to operator.
		bool operator<= (const rational& r) const;

		/// Greater than operator.
		bool operator> (int64_t i) const;
		/// Greater than operator.
		bool operator> (const rational& r) const;

		/// Greater than or equal to operator.
		bool operator>= (int64_t i) const;
		/// Greater than or equal to operator.
		bool operator>= (const rational& r) const;

		/// Addition operator. Returns a new object of type 'rational'.
		rational operator+ (int64_t i) const;
		/// Addition operator. Returns a new object of type 'rational'.
		rational operator+ (const rational& r) const;

		/// Addition operator. Modifies the current instance.
		rational& operator+= (int64_t i);
		/// Addition operator. Modifies the current instance.
		rational& operator+= (const rational& r);

		/// Substraction unary operator. Returns a new object of type 'rational'.
		rational operator- () const;
		/// Substraction operator. Returns a new object of type 'rational'.
		rational operator- (int64_t i) const;
		/// Substraction operator. Returns a new object of type 'rational'.
		rational operator- (const rational& r) const;

		/// Substraction unary operator. Modifies the current instance.
		rational& operator- ();
		/// Substraction operator. Modifies the current instance.
		rational& operator-= (int64_t i);
		/// Substraction operator. Modifies the current instance.
		rational& operator-= (const rational& r);

		/// Product operator. Returns a new object of type 'rational'.
		rational operator* (int64_t i) const;
		/// Product operator. Returns a new object of type 'rational'.
		rational operator* (const rational& r) const;

		/// Product operator. Modifies the current instance.
		rational& operator*= (int64_t i);
		/// Product operator. Modifies the current instance.
		rational& operator*= (const rational& r);

		/// Quotient operator. Returns a new object of type 'rational'.
		rational operator/ (int64_t i) const;
		/// Quotient operator. Returns a new object of type 'rational'.
		rational operator/ (const rational& r) const;

		/// Quotient operator. Modifies the current instance.
		rational& operator/= (int64_t i);
		/// Quotient operator. Modifies the current instance.
		rational& operator/= (const rational& r);

		/* GETTERS */

		/// Returns whether this object is initialised or not.
		bool is_initialized() const;
		/// Returns the sign of this rational.
		int32_t get_sign() const;

		/* CONVERTERS */

		/// Converts this integer to a string.
		std::string to_string() const;
		/// Converts this integer to a string.
		void as_string(std::string& s) const;

		/// Converts this rational to a double-precision floating-point value.
		double to_double() const;
		/// Converts this rational to a double-precision floating-point value.
		void as_double(double& d) const;

	private:
		/// Structure from GMP storing the integer's value.
		mpq_t m_val;
		/// Is this rational initialised?
		bool m_initialized = false;
};

/// Swaps the contents of two rational values.
inline void swap_rational(rational& a, rational& b) {
	rational copy = a;
	a = b;
	b = copy;
}

/// Make a rational value from two 64-bit unsigned integers
inline rational rational_from_ui(uint64_t n, uint64_t d = 1) {
	rational R;
	R.init_ui(n, d);
	return R;
}

