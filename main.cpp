#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>

struct Delimiters {
  const std::string d_prefix;
  const std::string d_delimiter;
  const std::string d_postfix;

  const std::string prefix() const { return d_prefix; }
  const std::string delimiter() const { return d_delimiter; }
  const std::string postfix() const { return d_postfix; }

  Delimiters(const char * pref, const char * delim, const char * post)
  : d_prefix(pref), d_delimiter(delim), d_postfix(post)
  {}
};


template <typename C>
std::string ContToStr(C& cont, const Delimiters& d)
{
  std::stringstream out;
  out.write(d.prefix().c_str(), d.prefix().size());
  for (typename C::iterator it = cont.begin(); it != cont.end();)
  {
    out << *it;
    if (++it != cont.end()) out.write(d.delimiter().c_str(), d.delimiter().size());
  }
  out.write(d.postfix().c_str(), d.postfix().size());

  return out.str();
}


template <typename T>
std::ostream& operator<< (std::ostream& os,  T& cont)
{
  os << ContToStr(cont, {"[", ", ", "]"});  
  return os;
}

template <typename K, typename V>
std::ostream& operator<< (std::ostream& os,  std::pair<K, V>& pair)
{
  os << "(" << pair->first << ": " << pair->second << ")";  
  return os;
}



int main()
{
  std::vector<int> v;
  v.push_back(1);
  v.push_back(2);
  v.push_back(3);

  std::cout << v << std::endl;

  std::list<int> l;
  l.push_back(1);
  l.push_back(2);
  l.push_back(3);

  std::cout << l << std::endl;

  std::map<int, std::string> m;
  m[1] = "one";
  m[2] = "two";
  m[3] = "three";

  std::cout << m << std::endl;
}