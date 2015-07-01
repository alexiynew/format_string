#include <iostream>
#include <sstream>
#include <exception>
#include <memory>

class formater_base {
protected:
	formater_base() {}
	virtual ~formater_base() {}
	virtual void print_index(std::istream& in, std::ostream& out) const = 0;

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

	template <typename T>
	value_holder<T>* make_value_holder(const T& t)
	{
		return new value_holder<T>(t);
	}

	std::string parse(const std::string& str) const
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
				
				print_index(in, out);
	
				in.get(c);
				if (c != '}') { throw std::logic_error("'}' expected"); }
			} else if (c == '}') {
				if (in.peek() == '}') { 
					out << '}'; 
					in.ignore(1); 
					continue; 
				}
				throw std::logic_error("single '}' in format string"); 
			} else {
				out << c;
			}
		}
		return out.str();
	} 
};

template <size_t N>
class formater : private formater_base {

	template <typename... Args>
	formater(Args&&... args) : values{std::unique_ptr<value_base>(make_value_holder(args))...,}
	{}

	void print_index(std::istream& in, std::ostream& out) const override
	{
		size_t index; 
		in >> index;
		if (index >= N) { throw std::logic_error("Wrong index"); }
		values[index]->print_to(out);
	}

	std::unique_ptr<value_base> values[N];

	template <typename... Args>
	friend std::string make_string (const std::string& str, Args&&... args);
};

template <typename... Args>
std::string make_string (const std::string& str, Args&&... args)
{	
	formater<sizeof...(args)> f(args...);
	return f.parse(str);
}

int main ()
{
	std::cout << make_string("one {0} two {1} three {2}", 1, 0.42, "string") << std::endl;
	std::cout << make_string("one {0} two {0} three {0}", "string") << std::endl;
	std::cout << make_string("one {{{0}}}", "string") << std::endl;
}