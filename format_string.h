#ifndef FORMAT_STRING_H
#define FORMAT_STRING_H

#include <sstream>
#include <exception>
#include <memory>
#include <cctype>
#include <iomanip>


// forvard declaration
// Here should be description
template <typename... Args>
std::string make_string (const std::string& str, Args&&... args);

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


	
	// Helper function
	template <typename T>
	value_holder<T>* make_value_holder(const T& t)
	{
		return new value_holder<T>(t);
	}

	size_t get_index(std::istringstream& in)
	{
		size_t index = 0xFFFFFFFF;
		char next_char = in.peek();
		if (next_char == '}' || next_char == ':') {
			if (mode == indexing_mode::manual) { 
				throw std::logic_error("Cannot switch from manual field specification mode to automatic field numbering"); 
			}
			mode = indexing_mode::automatic;
			index = next_index++;
		} else if (std::isdigit(in.peek())) {
			if (mode == indexing_mode::automatic) { 
				throw std::logic_error("Cannot switch from automatic field numbering to manual field specification"); 
			}
			mode = indexing_mode::manual;
			in >> index;
		} else 	{ 
			throw std::logic_error("Field number expected"); 
		}	

		return index;
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
				size_t index = get_index(in); 
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
class formater : private formater_base {
	// format data
	std::unique_ptr<value_base> values[N];

	// constructor
	template <typename... Args>
	formater(Args&&... args) 
		: values{std::unique_ptr<value_base>(make_value_holder(args))...,}
	{}
	
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

// Implementation
void formater_base::value_format::parse(std::istream& in)
{
	if (in.peek() != ':') return;
	in.ignore(1);
	std::string format_str;
	static std::string specifier_chars = "doxXeEfF";
	static std::string flag_chars = "+-#0";
	static std::string align_chars = "<>=";
	while (in.peek() != '}') {
		format_str += in.peek();
		in.ignore(1);
	}
	
	// read specifier
	if (specifier_chars.find(format_str.back()) != std::string::npos) {
		specifier = format_str.back();
		format_str.pop_back();
	}
	
	// read precision and width
	if (format_str.back() == '.') {
		throw std::logic_error("Precision value expected after '.'");
	}
	
	std::string number_str;
	while (std::isdigit(format_str.back()) || format_str.back() == '.') {
		number_str = format_str.back() + number_str;
		format_str.pop_back();
	}
	
	if (number_str.front() == '0') {
		format_str += '0';
		number_str.erase(0, 1);
	}
	
	std::stringstream number_stream(number_str);
	number_stream >> width;
	if (number_stream.peek() == '.') {
		number_stream.ignore(1);
	}
	number_stream >> precision;
	
	// read flag
	if (flag_chars.find(format_str.back()) != std::string::npos) {
		flag = format_str.back();
		format_str.pop_back();
	}
	
	// read align
	if (align_chars.find(format_str.back()) != std::string::npos) {
		align = format_str.back();
		format_str.pop_back();
	}
	
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
		case '0' : os << std::setfill ('0'); break;
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



template <typename... Args>
std::string make_string (const std::string& str, Args&&... args)
{
	formater<sizeof...(args)> f(args...);
	return f.parse(str);
}

#endif // FORMAT_STRING_H