//
// Created by dell on 2019/9/18.
//

#ifndef SG_STL_SG_BASIC_H
#define SG_STL_SG_BASIC_H

#include <exception>
#include <string>
namespace sg {
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	struct sg_input_iterator_tag {
	};
	struct sg_output_iterator_tag {
	};
	struct sg_forward_iterator_tag :
			public sg_input_iterator_tag {
	};
	struct sg_bidirectional_iterator_tag :
			public sg_forward_iterator_tag {
	};
	struct sg_random_access_iterator_tag :
			public sg_bidirectional_iterator_tag {
	};
	
	template <typename T>
	class sg__container_base {
	
	};


class sg_out_of_memory_exception: public std::exception{

private:
//	std::string m_msg;
//	size_t m_address;
public:
//	sg_out_of_memory_exception(size_t address, const std::string & msg):m_address(address), m_msg(msg){}
	virtual const char* what () const noexcept override {
		return "OUT OF MEMORY";
	}
	

	};
	template< class T >
	struct is_numeral
			: std::integral_constant<
					bool,
					std::is_floating_point<T>::value ||
					(std::is_integral<T>::value &&
					!std::is_same<unsigned char, typename std::remove_cv<T>::type>::value &&
					!std::is_same<signed char, typename std::remove_cv<T>::type>::value &&
					!std::is_same<char, typename std::remove_cv<T>::type>::value
					)> {};
	
}


#endif //SG_STL_SG_BASIC_H
