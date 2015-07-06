#ifndef FORMAT_STRING_H
#define FORMAT_STRING_H

#include <sstream>
#include <exception>
#include <memory>
#include <cctype>
#include <iomanip>
#include <iostream>


// forvard declaration
// Here should be description
template <typename... Args>
std::string make_string (const std::string& str, Args&&... args);

// implements main functionality
class formater_base {
protected:
	struct value_format {
		size_t width;		// number
		size_t precision;	// .number
		char specifier;		// doxXeEfF
		char fill;			// any char
		char align;			// <>=
		char flag;			// +-#0
		
		value_format () 
			: width(0), precision(6), specifier('d'), fill(' '), align('<'), flag('-')
		{}
		
		void parse(std::istream& in);
		void set_to(std::ostream& os) const;
	};
		
	formater_base() 
		: mode(indexing_mode::none), next_index(0)
	{}
	virtual ~formater_base() {}

	// make string from field with index and specific format
	virtual std::string value_string(size_t index, const value_format& format) = 0;

	// Internal types
	struct value_base {
		virtual ~value_base(){};
		virtual void print_to(std::ostream& os) const = 0;
	};

	template <typename T>
	struct value_holder : public value_base {
		value_holder(const T& v) : value(v) {}
		void print_to(std::ostream& os) const override { os << value; }
		const T & value;
	};
	
	// Helper functions
	template <typename T>
	static value_holder<T>* make_value_holder(const T& t)
	{
		return new value_holder<T>(t);
	}

	static char read_char_flag (const std::string& chars, std::string& format_str, const char default_value = '\0') 
	{
		char out = default_value;
		if (chars.find(format_str.back()) != std::string::npos) {
			out = format_str.back();
			format_str.pop_back();
		}
		return out;
	}

	static size_t string_to_int (const std::string& str, size_t default_value = 0)
	{
		std::istringstream ist(str);
		size_t value = default_value;
		ist >> value;
		return value;
	}

	void detect_mode(std::istringstream& in)
	{
		char next_char = in.peek();
		if (next_char == '}' || next_char == ':') {
			if (mode == indexing_mode::manual) { 
				throw std::logic_error("Cannot switch from manual field specification mode to automatic field numbering"); 
			}
			mode = indexing_mode::automatic;
		} else if (std::isdigit(in.peek())) {
			if (mode == indexing_mode::automatic) { 
				throw std::logic_error("Cannot switch from automatic field numbering to manual field specification"); 
			}
			mode = indexing_mode::manual;
		} else 	{ 
			throw std::logic_error("Field number expected"); 
		}	
	}

	std::string parse(const std::string& str)
	{
		std::istringstream in(str);
		std::ostringstream out;
		char c;
		while (in.get(c))
		{
			if (c == '{') {
				if (in.peek() == '{') { 
					out << '{'; 
					in.ignore(1); 
					continue; 
				}

				detect_mode(in);
				
				size_t index;
				if (mode == indexing_mode::automatic) {
					index = next_index++;
				} else if (mode == indexing_mode::manual) {
					in >> index;
				} else {
					throw std::logic_error("Cannot detect field numbering mode"); 
				}

				value_format format;
				if (in.peek() == ':') {
					format.parse(in);
				}
				
				out << value_string(index, format);
				
				in.get(c);
				if (c != '}') { 
					throw std::logic_error("'}' expected"); 
				}
			} else if (c == '}') {
				if (in.peek() == '}') { 
					out << '}'; 
					in.ignore(1); 
					continue; 
				}
				throw std::logic_error("Single '}' in format string"); 
			} else {
				out << c;
			}
		}
		return out.str();
	}

private:
	enum class indexing_mode { none, automatic, manual };
	
	formater_base::indexing_mode mode;
	size_t next_index;
};

template <size_t N>
class formater : public formater_base {
private:
	// field values
	std::unique_ptr<value_base> values[N];

	// fill values from args
	template <typename... Args>
	formater(Args&&... args) 
		: values{std::unique_ptr<value_base>(make_value_holder(args))...,}
	{}
	
	// make string from field with index and specific format
	std::string value_string(size_t index, const formater_base::value_format& format) override
	{
		std::ostringstream buf;
		if (index >= N) {
			buf << index;
			throw std::logic_error(std::string("Index ") + buf.str() + " out of range"); 
		}
		
		format.set_to(buf);
		values[index]->print_to(buf);
		
		return buf.str();
	}

	// give access to private methods
	template <typename... Args>
	friend std::string make_string (const std::string& str, Args&&... args);
};


// implementation of value_format parser
void formater_base::value_format::parse(std::istream& in)
{
	if (in.peek() != ':') return;
	in.ignore(1);
	std::string format_str;
	
	// read all foramt string
	while (in.peek() != '}') {
		format_str += in.peek();
		in.ignore(1);
	}
	
	// read specifier
	specifier = formater_base::read_char_flag("doxXeEfF", format_str, specifier);
	
	// read precision and width
	if (format_str.back() == '.') {
		throw std::logic_error("Precision value expected after '.'");
	}
	
	std::string width_str;
	std::string precision_str;
	std::string buf_str;
	while (std::isdigit(format_str.back()) || format_str.back() == '.') {
		buf_str = format_str.back() + buf_str;
		format_str.pop_back();
	}
	
	std::string::size_type dot_pos = buf_str.find('.');
	
	if (dot_pos != std::string::npos) {
		width_str = buf_str.substr(0, dot_pos);
		precision_str = buf_str.substr(dot_pos + 1, buf_str.size() - dot_pos - 1);
	} else {
		width_str = buf_str; 
	}
	
	// set zeros as fill chars
	if (width_str.front() == '0' && width_str.size() > 1) {
		format_str += '0';
		width_str.erase(0, 1);
	}

	width = formater_base::string_to_int(width_str, width);	
	precision = formater_base::string_to_int(precision_str, precision);
	
	// read sign
	flag = formater_base::read_char_flag("+-#0", format_str, flag);
	
	// read align
	align = formater_base::read_char_flag("<>=", format_str, align);
	
	// get fill char
	if (format_str.size() == 1) {
		fill = format_str.back();
		format_str.pop_back();
	}

	if (!format_str.empty()) {
		throw std::logic_error("Wrong field format");
	}
}

void formater_base::value_format::set_to(std::ostream& os) const
{
	switch (specifier) {
		case 'd': os << std::dec; break;
		case 'o': os << std::oct; break;
		case 'x': os << std::hex; break;
		case 'X': os << std::hex << std::uppercase; break;
		case 'e': os << std::scientific; break;
		case 'E': os << std::scientific << std::uppercase; break;
		case 'f': os << std::fixed; break;
		case 'F': os << std::fixed << std::uppercase; break;
		default: throw std::logic_error (std::string("Unknown specifier '") + specifier + "'");
	}
	
	os.precision(precision);
	os.width(width);
	
	switch (flag) {
		case '-' : os << std::noshowpos; break;
		case '+' : os << std::showpos; break;
		case '#' : os << std::showbase << std::showpoint; break;
		case '0' : os << std::setfill('0'); break;
		default: throw std::logic_error (std::string("Unknown flag '") + flag + "'");
	}

	switch (align) {
		case '<': os << std::left; break;
		case '>': os << std::right; break;
		case '=': os << std::internal; break;
		default: throw std::logic_error (std::string("Unknown align flag '") + align + "'");
	}
	
	if (flag != '0') {
		os << std::setfill(fill);
	}
}

// implementation of interface functionality
template <typename... Args>
std::string make_string (const std::string& str, Args&&... args)
{
	formater<sizeof...(args)> f(args...);
	return f.parse(str);
}



#endif // FORMAT_STRING_H