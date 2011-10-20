/* The MIT License

Copyright (c) 2007 Simon Kroeger (simonkroeger@gmx.de)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */

#ifndef __tuple_h__
#define __tuple_h__

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

namespace tbd {
namespace tuple_details {

//////////////////////// forward declaration

template<class HT, class TT> struct tuple_item;
typedef tuple_item<void, void> tuple_end;

//////////////////////// meta push / pop / last / n-th element

template<class T, class E> struct push_element {
  typedef tuple_item<typename T::head_type, typename push_element<typename T::tail_type, E>::type> type;
};
template<class A> struct push_element<tuple_end, A> { typedef tuple_item<A, tuple_end> type; };

template<class T> struct pop_element {
  typedef tuple_item<typename T::head_type, typename pop_element<typename T::tail_type>::type> type;
};
template<class HT> struct pop_element< tuple_item<HT, tuple_end> > { typedef tuple_end type; };

template<class T> struct last_element {
  typedef typename last_element<typename T::tail_type>::type type;
};
template<class HT> struct last_element< tuple_item<HT, tuple_end> > { 
  typedef tuple_item<HT, tuple_end> type; 
};

template<int C, class T> struct tuple_element {
  typedef typename tuple_element<C - 1, typename T::tail_type>::type type;
};
template<class T> struct tuple_element<0, T> { typedef T type; };

//////////////////////// tuple_item

template<class HT, class TT> struct tuple_item: public TT {
  typedef tuple_item<HT, TT> self;
  enum { size = TT::size + 1 };
  typedef HT head_type;
  typedef TT tail_type;

  tuple_item() {}

  tuple_item(const tuple_item<HT, TT>& other): 
    tail_type(other.tail()), _head(other.head()) {}

  template<class OHT, class OTT> tuple_item(const tuple_item<OHT, OTT>& other): 
    tail_type(other.tail()), _head(other.head()) {}

  head_type& head() { return _head; }
  tail_type& tail() { return *this; }
  const head_type& head() const { return _head; }
  const tail_type& tail() const { return *this; }

  template<class V> typename push_element<self, V>::type operator| (const V& v) {
    return typename push_element<self, V>::type(*this, v);
  }

  template<class OHT, class OTT, class V> explicit tuple_item(const tuple_item<OHT, OTT>& other, const V& v): 
    tail_type(other.tail(), v), _head(other.head()) {}

protected:
  friend struct Accumulator;
  template<class V> explicit tuple_item(const tuple_end&, const V& v): _head(v) {}
private:
  head_type _head;
};

template<> struct tuple_item<void, void> { enum { size = 0 }; };

struct Accumulator {
  template<class V> tuple_item<V, tuple_end> operator<< (const V& v) const  {
    return tuple_item<V, tuple_end>(tuple_end(), v);
  }
};

//////////////////////// comparison

template<class T1, class T2> inline bool eq(const T1& lhs, const T2& rhs) {
  return lhs.head() == rhs.head() && eq(lhs.tail(), rhs.tail());
}
template<class T1> inline bool eq(const T1&, const tuple_end&) { return false; }
template<class T2> inline bool eq(const tuple_end&, const T2&) { return false; }
inline bool eq(const tuple_end&, const tuple_end&) { return true; }

template<class T1, class T2> inline bool neq(const T1& lhs, const T2& rhs) {
  return lhs.head() != rhs.head() || neq(lhs.tail(), rhs.tail());
}
template<class T1> inline bool neq(const T1&, const tuple_end&) { return true; }
template<class T2> inline bool neq(const tuple_end&, const T2&) { return true; }
inline bool neq(const tuple_end&, const tuple_end&) { return false; }

template<class T1, class T2> inline bool lt(const T1& lhs, const T2& rhs) {
  return lhs.head() < rhs.head() || !(rhs.head() < lhs.head()) && lt(lhs.tail(), rhs.tail());
}
template<class T1> inline bool lt(const T1&, const tuple_end&) { return false; }
template<class T2> inline bool lt(const tuple_end&, const T2&) { return true; }
inline bool lt(const tuple_end&, const tuple_end&) { return false; }

template<class T1, class T2> inline bool gt(const T1& lhs, const T2& rhs) {
  return lhs.head() > rhs.head() || !(rhs.head() > lhs.head()) && gt(lhs.tail(), rhs.tail());
}
template<class T1> inline bool gt(const T1&, const tuple_end&) { return true; }
template<class T2> inline bool gt(const tuple_end&, const T2&) { return false; }
inline bool gt(const tuple_end&, const tuple_end&) { return false; }

template<class T1, class T2> inline bool lte(const T1& lhs, const T2& rhs) {
  return lhs.head() <= rhs.head() && ( !(rhs.head() <= lhs.head()) || lte(lhs.tail(), rhs.tail()));
}
template<class T1> inline bool lte(const T1&, const tuple_end&) { return false; }
template<class T2> inline bool lte(const tuple_end&, const T2&) { return true; }
inline bool lte(const tuple_end&, const tuple_end&) { return true; }

template<class T1, class T2> inline bool gte(const T1& lhs, const T2& rhs) {
  return lhs.head() >= rhs.head() && ( !(rhs.head() >= lhs.head()) || gte(lhs.tail(), rhs.tail()));
}
template<class T1> inline bool gte(const T1&, const tuple_end&) { return true; }
template<class T2> inline bool gte(const tuple_end&, const T2&) { return false; }
inline bool gte(const tuple_end&, const tuple_end&) { return true; }

template<class LHT, class LTT, class RHT, class RTT> inline
bool operator==(const tuple_item<LHT, LTT>& l, const tuple_item<RHT, RTT>& r) { return eq(l, r); }

template<class LHT, class LTT, class RHT, class RTT> inline
bool operator!=(const tuple_item<LHT, LTT>& l, const tuple_item<RHT, RTT>& r) { return neq(l, r); }

template<class LHT, class LTT, class RHT, class RTT> inline
bool operator< (const tuple_item<LHT, LTT>& l, const tuple_item<RHT, RTT>& r) { return lt(l, r); }

template<class LHT, class LTT, class RHT, class RTT> inline
bool operator> (const tuple_item<LHT, LTT>& l, const tuple_item<RHT, RTT>& r) { return gt(l, r); }

template<class LHT, class LTT, class RHT, class RTT> inline
bool operator<=(const tuple_item<LHT, LTT>& l, const tuple_item<RHT, RTT>& r) { return lte(l, r); }

template<class LHT, class LTT, class RHT, class RTT> inline
bool operator>=(const tuple_item<LHT, LTT>& l, const tuple_item<RHT, RTT>& r) { return gte(l, r); }

//////////////////////// get /set

template<int C, class HT, class TT> inline typename tuple_element<C, 
  tuple_item<HT, TT> >::type::head_type& tuple_get(tuple_item<HT, TT>& r) {
typedef typename tuple_element<C, tuple_item<HT, TT> >::type te;
  return ((te&)r).head();
}

template<int C, class HT, class TT> inline void tuple_set(tuple_item<HT, TT>& r, 
  const typename tuple_element<C, tuple_item<HT, TT> >::type::head_type& value) {
  typedef typename tuple_element<C, tuple_item<HT, TT> >::type te;
  ((te&)r).head() = value;
}

//////////////////////// accessor

template<int C, class T, class V = typename tuple_element<C, T>::type::head_type> struct tuple_accessor {
  tuple_accessor(T& tuple): _v(tuple_get<C>(tuple)) {}
  tuple_accessor<C, T, V>& operator= (const V& v) { _v = v; return *this; }
  operator V& () { return _v; }
protected:
  V& _v;
};

template<int C, class T> struct tuple_accessor<C, T, std::string> {
  typedef std::string V;
  tuple_accessor(T& t): _v(tuple_get<C>(t)) {}
  tuple_accessor<C, T, V>& operator= (const V& v) { _v = v; return *this; }
  operator V& () { return _v; }
  operator const char* () { return _v.c_str(); }
protected:
  V& _v;
};

//////////////////////// accept

template<class V> void tuple_accept(tuple_end&, V&, int i = 0) {}
template<class V> void tuple_accept(const tuple_end&, V&, int i = 0) {}
template<class T, class V> V& tuple_accept(T& tuple, V& visitor, int i = 0) {
  visitor(tuple.head(), i);
  tuple_accept(tuple.tail(), visitor, i + 1);
  return visitor;
}

template<class V> void tuple_accept(tuple_end&, const V&, int i = 0) {}
template<class V> void tuple_accept(const tuple_end&, const V&, int i = 0) {}
template<class T, class V> const V& tuple_accept(T& tuple, const V& visitor, int i = 0) {
  visitor(tuple.head(), i);
  tuple_accept(tuple.tail(), visitor, i + 1);
  return visitor;
}

//////////////////////// ignore

struct ignore {};

//////////////////////// collector

template<class T, class S = T> struct collector : public collector<T, typename S::tail_type> {
  typedef collector<T, typename S::tail_type> super;
  collector(T& r): super(r) {}

  template<class V> super& operator| (const V& v) { 
    ((S&)(T&)*this).head() = v; 
    return *this; 
  }
  super& operator| (const ignore&) { return *this; }
};

template<class T> struct collector<T, tuple_end> {
  collector(T& t): _tuple(t) {}
  operator T& () { return _tuple; }
protected:
  T& _tuple;
};

template<class HT, class TT, class V> 
collector<tuple_item<HT, TT>, TT> operator<< (tuple_item<HT, TT>& tuple, const V& value) {
  return collector< tuple_item<HT, TT> >(tuple) | value;
}

//////////////////////// distributor

template<class T, class S = T> struct distributor : public distributor<T, typename S::tail_type> {
  typedef distributor<T, typename S::tail_type> super;
  distributor(T& r): super(r) {}

  template<class V> super& operator| (V& t) { 
    t = (V&)((S&)(T&)*this).head(); 
    return *this; 
  }
  super& operator| (const ignore&) { return *this; }
};

template<class T> struct distributor<T, tuple_end> {
  distributor(T& t): _tuple(t) {}
  operator T& () { return _tuple; }
protected:
  T& _tuple;
};

template<class HT, class TT, class V> 
distributor<tuple_item<HT, TT>, TT> operator>> (tuple_item<HT, TT>& tuple, V& value) {
  return distributor< tuple_item<HT, TT> >(tuple) | value;
}

//////////////////////// join

struct joiner {
  joiner(std::string separator): _sep(separator) {}
  template<class T> void operator() (const T& value, int i) {
    if (i) _sstr << _sep;
    _sstr << value;
  }
  std::string str() { return _sstr.str(); }
private:
  std::stringstream _sstr;
  std::string _sep;
};

//////////////////////// converter

template<class Tuple, class To, class Sub> struct convert {
  static To do_it(const Tuple& t) {
    To value;
    std::stringstream _sstr;
    _sstr.precision(15);
    _sstr << ((Sub&)t).head();
    _sstr >> value;
    return value;
  }
};
template<class Tuple, class Sub> struct convert<Tuple, typename Sub::head_type, Sub> {
  static typename Sub::head_type do_it(const Tuple& t) { return ((Sub&)t).head(); }
};

template<class TT, class VT, class T> struct create_func_table {
    typedef VT(*pt_convert)(const TT&);
  static void create(pt_convert* ft) {
    *ft = convert<TT, VT, T>::do_it;
    create_func_table<TT, VT, typename T::tail_type>::create(++ft);
  }
};
template<class TT, class VT> struct create_func_table<TT, VT, tuple_end> {
    typedef VT(*pt_convert)(const TT&);
  static void create(pt_convert*) {}
};
template<class TT, class VT> struct create_func_table<TT, VT, const tuple_end> {
  typedef VT(*pt_convert)(const TT&);
  static void create(pt_convert*) {}
};

template<class TT, class VT> struct converter {
  static VT get(const TT& tuple, int i) {
    if (i >= TT::size) return VT();
    return func_table()[i](tuple);
  }
private:
  typedef VT(*pt_convert)(const TT&);
  converter(); // no instances

  static pt_convert* func_table() {
    static pt_convert ft[TT::size] = {0};
    if (!ft[0]) create_func_table<TT, VT, TT>::create(ft);
    return ft;
  }
};

//////////////////////// iterator

template<class T, class V> struct iterator {
  typedef T tuple_type;
  typedef iterator<T, V> self;
  typedef V value_type;
  typedef value_type& reference;
  typedef value_type* pointer;
  typedef std::input_iterator_tag iterator_category;
  typedef int difference_type;
  typedef converter<tuple_type, value_type> converter;

  iterator(tuple_type& tuple, int pos = 0): _pos(pos), _value(), _tuple(tuple) { 
    ++(*this); 
  }

  self& operator++() {
    _value = converter::get(_tuple, _pos++);
    return *this; 
  }

  value_type& operator*() { return _value; }
  value_type* operator->() { return &_value; }

  bool operator==(const self& other) const { 
    return (&_tuple == &other._tuple) && (_pos == other._pos);
  }
  bool operator!=(const self& other) const {
    return (&_tuple != &other._tuple) || (_pos != other._pos);
  }

private:
  int _pos;
  value_type _value;
  tuple_type& _tuple;
};

//////////////////////// tuple

#if !TBD_TUPLE_MAX_SIZE
#  define TBD_TUPLE_MAX_SIZE 10

#  define TBD_TUPLE_ENUM_PARAMS(param) \
  param##0, param##1, param##2, param##3, param##4, \
  param##5, param##6, param##7, param##8, param##9 

#  define TBD_TUPLE_ENUM_PARAMS_WITH_DEFAULTS(param, def) \
  param##0##def, param##1##def, param##2##def, param##3##def, param##4##def, \
  param##5##def, param##6##def, param##7##def, param##8##def, param##9##def 

#  define TBD_TUPLE_MAKE_TUPLE >::t>::t>::t>::t>::t>::t>::t>::t>::t>::t

#else
#  include <boost/preprocessor/repetition.hpp>

#  define TBD_TUPLE_ENUM_PARAMS(param) \
  BOOST_PP_ENUM_PARAMS(TBD_TUPLE_MAX_SIZE, param)

#  define TBD_TUPLE_ENUM_PARAMS_WITH_DEFAULTS(param, def) \
  BOOST_PP_ENUM_BINARY_PARAMS(TBD_TUPLE_MAX_SIZE, param, def BOOST_PP_INTERCEPT)

#  define TBD_TUPLE_MAKE_TUPLE_PART(z, n, unused) >::t
#  define TBD_TUPLE_MAKE_TUPLE \
  BOOST_PP_REPEAT(TBD_TUPLE_MAX_SIZE, TBD_TUPLE_MAKE_TUPLE_PART, ~)

#endif

template<class T1, class T2> struct mt { typedef tuple_item<T1, T2> t; };
template<class T1> struct mt<T1, void> { typedef tuple_item<T1, tuple_end> t; };
template<> struct mt<void, void> { typedef void t; };

template<TBD_TUPLE_ENUM_PARAMS(class T)> struct make_tuple { 
  typedef TBD_TUPLE_ENUM_PARAMS(typename mt<T), void TBD_TUPLE_MAKE_TUPLE type;
};

template<TBD_TUPLE_ENUM_PARAMS_WITH_DEFAULTS(class S, = void)> 
struct tuple : make_tuple<TBD_TUPLE_ENUM_PARAMS(S)>::type  { 
  typedef tuple<TBD_TUPLE_ENUM_PARAMS(S)> self;
  typedef typename make_tuple<TBD_TUPLE_ENUM_PARAMS(S)>::type value_type;

  tuple() {}
  tuple(const self& t): value_type(t.value()) {}

  template<class HT, class TT> tuple(const tuple_item<HT, TT>& t): value_type(t) {}

  template<int C> struct element { 
    typedef typename tuple_element<C, value_type>::type::head_type type; 
  };

  template<int C> typename element<C>::type& get() { 
    return tuple_get<C>(value()); 
  }
  template<class T> T get(int i) {
    return converter<value_type, T>::get(value(), i);
  }
  template<int C> void set(const typename element<C>::type& v) { 
    return tuple_set<C>(value(), v); 
  }
  typename value_type::head_type& first() { 
    return tuple_get<0>(value()); 
  }
  typename last_element<value_type>::type::head_type& last() { 
    return tuple_get<value_type::size - 1>(value()); 
  }
  template<class V> iterator<value_type, V> begin() {
    return iterator<value_type, V>(value());
  }
  template<class V> iterator<value_type, V> end() {
    return iterator<value_type, V>(value(), value_type::size);
  }

  template<int C> struct accessor : tuple_accessor<C, value_type> { 
    typedef typename element<C>::type V;
    accessor(self& t): tuple_accessor<C, value_type>(t.value()) {}
    accessor<C>& operator= (const V& value) { 
      ((tuple_accessor<C, value_type>&)*this) = value; 
      return *this;
    }
  };

  template<class V> 
  collector<self, typename value_type::tail_type> operator<< (const V& value) {
    return collector<self, value_type>(*this) | value;
  }

  template<class V> 
  distributor<self, typename value_type::tail_type> operator>> (V& value) {
    return distributor<self, value_type>(*this) | value;
  }

  template<class V> V& accept(V& visitor) { 
    return tuple_accept(value(), visitor); 
  }
  template<class V> const V& accept(const V& visitor) { 
    return tuple_accept(value(), visitor); 
  }
  
  std::string join(std::string separator = "") {
    joiner visitor(separator);
    return accept(visitor).str();
  }

  template<TBD_TUPLE_ENUM_PARAMS(class T)> 
  bool operator==(const tuple<TBD_TUPLE_ENUM_PARAMS(T)>& r) { 
    return eq(value(), r.value()); 
  }
  template<TBD_TUPLE_ENUM_PARAMS(class T)> 
  bool operator!=(const tuple<TBD_TUPLE_ENUM_PARAMS(T)>& r) { 
    return neq(value(), r.value()); 
  }
  template<TBD_TUPLE_ENUM_PARAMS(class T)> 
  bool operator< (const tuple<TBD_TUPLE_ENUM_PARAMS(T)>& r) { 
    return lt(value(), r.value()); 
  }
  template<TBD_TUPLE_ENUM_PARAMS(class T)> 
  bool operator> (const tuple<TBD_TUPLE_ENUM_PARAMS(T)>& r) { 
    return gt(value(), r.value()); 
  }
  template<TBD_TUPLE_ENUM_PARAMS(class T)> 
  bool operator<=(const tuple<TBD_TUPLE_ENUM_PARAMS(T)>& r) { 
    return lte(value(), r.value()); 
  }
  template<TBD_TUPLE_ENUM_PARAMS(class T)> 
  bool operator>=(const tuple<TBD_TUPLE_ENUM_PARAMS(T)>& r) { 
    return gte(value(), r.value()); 
  }
  
  value_type& value() {return *this; }
  const value_type& value() const {return*this; }
};

} // namespace tuple_details

using tuple_details::tuple; // lifting tuple to tbd namespace
const tuple_details::Accumulator accu = tuple_details::Accumulator();
const tuple_details::ignore na = tuple_details::ignore();

} // namspace tbd

#endif // __tuple_h__
