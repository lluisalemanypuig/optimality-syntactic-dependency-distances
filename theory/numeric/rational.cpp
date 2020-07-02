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
 
#include "rational.hpp"

// C++ includes
using namespace std;

/* AUXILIRAY FUNCTIONS */

void mpz_divide_mpq(mpq_t& num, const mpz_t& c) {
	mpz_t b;
	mpz_init(b);

	mpq_get_den(b, num);	// num = a/b

	mpz_mul(b, b, c);

	mpq_set_den(num, b);
	mpq_canonicalize(num);

	mpz_clear(b);
}

void mpq_divide_mpq(mpq_t& num, const mpq_t& den) {
	mpz_t a, b, c, d;
	mpz_inits(a, b, c, d, nullptr);

	mpq_get_num(a, num);	// num = a/b
	mpq_get_den(b, num);
	mpq_get_num(c, den);	// den = c/d
	mpq_get_den(d, den);

	mpz_mul(a, a, d);
	mpz_mul(b, b, c);

	mpq_set_num(num, a);
	mpq_set_den(num, b);
	mpq_canonicalize(num);

	mpz_clears(a, b, c, d, nullptr);
}

/* CLASS IMPLEMENTATION */

// PUBLIC

rational::rational() { }
rational::rational(int64_t n, uint64_t d) {
	init_si(n, d);
}

rational::rational(const std::string& s) {
	init_str(s);
}

rational::rational(const rational& r) {
	*this = r;
}

rational::~rational() {
	clear();
}

/* ALLOC AND DEALLOC */

void rational::init() {
	if (is_initialized()) {
		return;
	}
	mpq_init(m_val);
	m_initialized = true;
}

void rational::init_si(int64_t n, uint64_t d) {
	init();
	set_si(n, d);
}

void rational::init_ui(uint64_t n, uint64_t d) {
	init();
	set_ui(n, d);
}

void rational::init_str(const std::string& s) {
	init();
	set_str(s);
}

void rational::init_mpq(const mpq_t& mpq) {
	init();
	mpq_set(m_val, mpq);
}

void rational::clear() {
	if (is_initialized()) {
		mpq_clear(m_val);
		m_initialized = false;
	}
}

/* SET VALUE */

void rational::set_si(int64_t n, uint64_t d) {
	mpq_set_si(m_val, n, d);
	mpq_canonicalize(m_val);
}
void rational::set_ui(uint64_t n, uint64_t d) {
	mpq_set_ui(m_val, n, d);
	mpq_canonicalize(m_val);
}
void rational::set_str(const std::string& s) {
	mpq_set_str(m_val, s.c_str(), 10);
	mpq_canonicalize(m_val);
}
void rational::set_mpq(const mpq_t& mpq) {
	mpq_set(m_val, mpq);
}
void rational::copy(const rational& r) {
	mpq_set(m_val, r.m_val);
}

void rational::invert() {
	mpq_inv(m_val, m_val);
}

/* OPERATORS */

rational& rational::operator= (int64_t i) {
	if (is_initialized()) {
		set_si(i);
	}
	else {
		init_si(i);
	}
	return *this;
}
rational& rational::operator= (const rational& r) {
	if (not r.is_initialized()) {
		return *this;
	}

	init();
	copy(r);
	return *this;
}

bool rational::operator== (int64_t i) const				{ rational r(i); 	return mpq_equal(m_val, r.m_val) != 0; }
bool rational::operator== (const rational& r) const		{					return mpq_equal(m_val, r.m_val) != 0; }

bool rational::operator!= (int64_t i) const				{ return not (*this == i); }
bool rational::operator!= (const rational& r) const		{ return not (*this == r); }

bool rational::operator< (int64_t i) const				{ rational r(i); 	return mpq_cmp(m_val, r.m_val) < 0; }
bool rational::operator< (const rational& r) const		{					return mpq_cmp(m_val, r.m_val) < 0; }

bool rational::operator<= (int64_t i) const				{ rational r(i); 	return mpq_cmp(m_val, r.m_val) <= 0; }
bool rational::operator<= (const rational& r) const		{ 					return mpq_cmp(m_val, r.m_val) <= 0; }

bool rational::operator> (int64_t i) const				{ rational r(i); 	return mpq_cmp(m_val, r.m_val) > 0; }
bool rational::operator> (const rational& r) const		{ 					return mpq_cmp(m_val, r.m_val) > 0; }

bool rational::operator>= (int64_t i) const				{ rational r(i); 	return mpq_cmp(m_val, r.m_val) >= 0; }
bool rational::operator>= (const rational& r) const		{					return mpq_cmp(m_val, r.m_val) >= 0; }

rational rational::operator+ (int64_t i) const			{ rational r(*this); r += i; return r; }
rational rational::operator+ (const rational& r) const	{ rational k(*this); k += r; return k; }

rational& rational::operator+= (int64_t i)				{ rational r(i); 	mpq_add(m_val, m_val, r.m_val); return *this; }
rational& rational::operator+= (const rational& r)		{					mpq_add(m_val, m_val, r.m_val); return *this; }

rational rational::operator- () const 					{ rational r(*this); mpq_neg(r.m_val, r.m_val); return r; }
rational rational::operator- (int64_t i) const			{ rational r(*this); r -= i; return r; }
rational rational::operator- (const rational& r) const	{ rational k(*this); k -= r; return k; }

rational& rational::operator- ()						{					mpq_neg(m_val, m_val); return *this; }
rational& rational::operator-= (int64_t i)				{ rational r; r.init_si(i);
																			mpq_sub(m_val, m_val, r.m_val); return *this; }
rational& rational::operator-= (const rational& r)		{					mpq_sub(m_val, m_val, r.m_val); return *this; }

rational rational::operator* (int64_t i) const			{ rational r(*this); r *= i; return r; }
rational rational::operator* (const rational& r) const	{ rational k(*this); k *= r; return k; }

rational& rational::operator*= (int64_t i)				{ rational r(i);	mpq_mul(m_val, m_val, r.m_val); return *this; }
rational& rational::operator*= (const rational& r)		{					mpq_mul(m_val, m_val, r.m_val); return *this; }

rational rational::operator/ (int64_t i) const			{ rational r(*this); r /= i; return r; }
rational rational::operator/ (const rational& r) const	{ rational k(*this); k /= r; return k; }

rational& rational::operator/= (int64_t I) {
	mpz_t I_t;
	mpz_init(I_t);
	mpz_set_si(I_t, I);
	
	mpz_divide_mpq(m_val, I_t);
	
	mpz_clear(I_t);
	return *this;
}

rational& rational::operator/= (const rational& r) {
	mpq_divide_mpq(m_val, r.m_val);
	return *this;
}

/* GETTERS */

bool rational::is_initialized() const {
	return m_initialized;
}

int rational::get_sign() const {
	return mpq_sgn(m_val);
}

/* CONVERTERS */

string rational::to_string() const {
	string k;
	as_string(k);
	return k;
}

void rational::as_string(string& s) const {
	if (not is_initialized()) {
		s = "uninitialized";
		return;
	}

	char *buf = nullptr;
	buf = mpq_get_str(buf, 10, m_val);
	s = string(buf);
	free(buf);
}

double rational::to_double() const {
	return mpq_get_d(m_val);
}

void rational::as_double(double& d) const {
	d = mpq_get_d(m_val);
}
