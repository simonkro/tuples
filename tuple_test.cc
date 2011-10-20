#include <iostream>
using std::cout;
using std::endl;

#include <string>
using std::string;

#include <deque>
using std::deque;

#include "../tuple/tuple.h"
using tbd::accu;
using tbd::tuple;

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

struct outputter {
  template<class V> void operator () (V& value, int i) const {
    cout << value << " ";
  }
};

tuple<int, int> divmod(int dividend, int divisor) {
  return accu << dividend / divisor | dividend % divisor;
}

// convert seconds into hours, minutes and seconds
tuple<int, int, int> hms(int seconds) {
  int secs, mins, hours;

  divmod(seconds, 60) >> mins | secs;
  divmod(mins, 60) >> hours | mins;

  return accu << hours | mins | secs;
}

int main() {
  typedef tuple<int, string, double> mytuple;

  tuple<double> double_tuple;
  double d = double_tuple.first();

  int secs, mins, hours;
  hms(3723) >> hours | mins | secs;

  cout << hms(3723).join(", ") << endl;
  cout << (hms(3723) < hms(3724)) << endl;

  // create instances, default constructors will be called of any types that have some.
  // (this implies POTs like int will be uninitialised)
  mytuple testt = mytuple() << 1 | "2" | 3.4;

  // both tuples and rows can be feed with plain values...
  testt << 42 | "foo" | 47.11;

  tuple<__int64, string, double> t = testt;

  cout << t.first() << " " << t.last() << endl;

  // plain old types can be extracted ...
  int i; string s; 
  testt >> i | s | d;

  // single values can be queried using get
  cout << testt.get<0>() << " " << testt.get<1>() << endl;

  // and written using - surprise - set
  testt.set<0>(999);

  // if you define accessor types ...
  typedef tuple<int, string, double> user;
  typedef user::accessor<0> id;
  typedef user::accessor<1> name;
  typedef user::accessor<2> weight;

  // ... this will look much nicer:
  (id)testt = 1;
  (name)testt = "boing";
  (weight)testt = 3.14159265;

  // you can of course access the elements the same way
  cout << (id)testt << ", " << (name)testt << ", " << (weight)testt << endl;

  // accept gives each element to the functor
  testt.accept(outputter());
  
    tuple<int, string, double> x;
  x << 12 | "34.56" | 78.90;

  for(int i = 0; i < 3; i++) {
    cout << x.get<int>(i) << endl;
  }

  std::vector<double> data;
  copy(x.begin<double>(), x.end<double>(), back_inserter(data));

  foreach(string str, make_pair(x.begin<string>(), x.end<string>())) {
    cout << str << " ";
  }

  return 0;
}
