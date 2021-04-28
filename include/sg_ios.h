//
// Created by dell on 2019/11/16.
//

#ifndef SG_STL_SG_IOS_H
#define SG_STL_SG_IOS_H

#include "sg_algorithm.h"
#include <exception>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>
#include <iterator>
namespace sg {
	
	template <typename T>
	bool no_op(const T& value) { return true; }
	
	class sg_ios : public std::iostream, std::streambuf {
	public:
		sg_ios(): std::iostream(this) {}

		template <typename ForwardIter, typename T>
		bool getline(T hint, ForwardIter begin, size_t n) {
		
			std::string tmp;
			do {
				std::getline(std::cin, tmp);
			} while (tmp.empty());
			std::stringstream ss;
			ss.str(tmp);
			using SS_Iter = std::istream_iterator<T>;
			SS_Iter ss_iter_begin(ss);
			SS_Iter ss_iter_end;
			size_t i = 0;
			for (; i != n && ss_iter_begin != ss_iter_end; ++i, ++ss_iter_begin, ++begin) {
				*begin = *ss_iter_begin;
			}
			if (i == n && ss_iter_begin == ss_iter_end) {
				return true;
			}
			if (i == n && ss_iter_begin != ss_iter_end) {
				std::cerr << "The number of your input is more than " << n << ", try again." << std::endl;
				return false;
			}
			if (i != n && ss_iter_begin == ss_iter_end) {
				std::cerr << "The number of your input is less than " << n << ", try again." << std::endl;
				return false;
			}
			return true;
		}
		
		template <typename T, typename Func>
		sg_ios& session(const std::string& out, T& value,
		                Func check, const std::string& wrong_hint = "") {
			do {
				*this << out << std::flush;
				*this >> value;
				if (check(value)) {
					break;
				}
				std::cerr << "[ERROR]: "<< wrong_hint << std::endl;
			} while (true);
			return *this;
		}

		template <typename T>
		sg_ios& session(const std::string& out, T& value) {
			*this << out;
			*this >> value;
			return *this;
		}

		template <typename T>
		sg_ios& operator << (const T& value) {
			std::cout << value;
			return *this;
		}
		
		sg_ios& operator<<(std::ostream& (* func)(std::ostream&)) {
			std::cout << func;
			return *this;
		}
		
		template <typename T>
		sg_ios& operator>>(T& value) {
			input_d_(value, is_numeral<T>());
			return *this;
		}
		template <typename T>
		void input_d_(T& value, std::false_type) {
			std::cin >> value;
		}
		
		template <typename T>
		void input_d_(T& value, std::true_type) {
			do {
				std::string tmp;
				std::cin >> tmp;
				if (is_satisfied(tmp, value)) {
					std::stringstream ss;
					ss.str(tmp);
					ss >> value;
					return;
				}
				std::string error_message{"[INPUT_ERROR]: you need a(n) "};
				error_message += typeid(T).name();
				error_message += ",but you input ";
				error_message += tmp;
				error_message += ",you need to input it again.\n";
				std::cerr << error_message;
				
			} while (true);
		}
		
		using StrIter = std::string::const_iterator;
		
		static bool is_unsigned_integer(StrIter begin, StrIter end) {
			return sg_find_if(begin, end, [](char c) { return !isdigit(c); }) == end;
		}
		
		static bool is_integer(StrIter begin, StrIter end) {
			if (*begin == '-') {
				++begin;
			}
			return is_unsigned_integer(begin, end);
		}
		
		static bool is_number(StrIter begin, StrIter end) {
			auto dot_iter = sg_find(begin, end, '.');
			return is_integer(begin, dot_iter) &&
			       (dot_iter == end || is_unsigned_integer(++dot_iter, end));
		}
		
		template  <typename T>
		static bool is_satisfied(const std::string& str, const T& value) {
			return is_satisfied_d(str, value, std::is_floating_point<T>());
		}
		template <typename T>
		static bool is_satisfied_d(const std::string& str, const T& value, std::true_type){
			return is_number(str.begin(), str.end());
		}
		
		template <typename T>
		static bool is_satisfied_d(const std::string& str, const T& value, std::false_type){
			return is_satisfied_d_d(str, value, std::is_signed<T>());
		}
		
		template <typename T>
		static bool is_satisfied_d_d(const std::string& str, const T& value, std::true_type){
			return is_integer(str.begin(), str.end());
			
		}template <typename T>
		static bool is_satisfied_d_d(const std::string& str, const T& value, std::false_type){
			return is_unsigned_integer(str.begin(), str.end());
		}
		
		bool in_session = false;
	};
	
	static sg_ios sg_cios;
}
/*	template <typename ...Args>
	std::string fomart(std::string&& s, Args...args) {
		std::string s_{s};
		auto begin = s.find('{');
		auto end = s.find('}');
		std::copy(s.begin(), )
		
	}
*//*
	class sg_session: public sg_ios {
	public:
		sg_session (const std::string& error_msg): error_msg_(error_msg){};
		template <typename T>
		using Check = std::function<bool(const T &)>;
		using RawCheck = Check<std::string>;
.
		template <typename T>
		sg_session& operator<< (const T& hint) {
			hint_ += hint;
			sg_cios << hint;
			return *this;
		}

		template <typename T>
		struct checker value
		sg_session& set_raw_check (RawCheck raw_check) {
			raw_check_ = raw_check;
			use_raw_check_ = true;
			return *this;
		}
		sg_session& set_check (Check check) {
			check_ = check;
			use_check_ = true;
			return *this;
		}


		template <typename T>
		sg_session& operator >>(T& value) {
			if (!use_check_ && !use_raw_check_){
				sg_cios >> value;
				return *this;
			}

			do {
				std::string tmp;
				std::cin >> tmp;
				if (use_raw_check_ && raw_check(tmp)) {
					std::stringstream ss;
					ss.str(tmp);
					ss >> value;
					if (use_check_ &&  check(value)) {
						return *this;
					}
				}
				std::cout << error_msg_ << '\n' << hint_;
			} while (true);
		}
	private:
		std::string hint_{};
		std::string error_msg_;

	};
 */
// namespace s
#endif // SG_STL_SG_IOS_H
