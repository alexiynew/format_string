#ifndef FORMAT_STRING_H
#define FORMAT_STRING_H

#include <sstream>
#include <exception>
#include <memory>
#include <cctype>


// forvard declaration
// Here should be description
template <typename... Args>
std::string make_string (const std::string& str, Args&&... args);

	
class formater_base {
protected:
	formater_base() {}
	virtual ~formater_base() {}
	virtual void print_value(std::istream& in, std::ostream& out, size_t index) = 0;

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
				throw std::logic_error("You can not switch from manual fild indexing mode to automatic"); 
			}
			mode = indexing_mode::automatic;
			index = next_index++;
		} else if (std::isdigit(in.peek())) {
			if (mode == indexing_mode::automatic) { 
				throw std::logic_error("You can not switch from automatic fild indexing mode to manual"); 
			}
			mode = indexing_mode::manual;
			in >> index;
		} else 	{ 
			throw std::logic_error("Index number expected"); 
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
				print_value(in, out, index);
	
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
	
	void print_value(std::istream& in, std::ostream& out, size_t index) override
	{
		if (index >= N) {
			throw std::logic_error("Wrong index"); 
		}

		//char next_char = in.peek();
		
		values[index]->print_to(out);
	}

	// give access to private methods
	template <typename... Args>
	friend std::string make_string (const std::string& str, Args&&... args);
};



template <typename... Args>
std::string make_string (const std::string& str, Args&&... args)
{	
	formater<sizeof...(args)> f(args...);
	return f.parse(str);
}

#endif // FORMAT_STRING_H