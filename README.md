# Tuples

A tuple is a fixed sized list of items - that's an array! Yes, if all items are of the same type - if not you need a tuple. (Well, or void* but please don't tell me)

```C++
tuple<int, string, double> user;    // create tuple
user << 42 | "sally" | 47.11;       // fill with data (100% typesave)

int id; string name; double weight;
user >> id | name | weight;         // get the data (100% typesave)
```

Whenever you would create a small struct or class to hold values a tuple may be an elegant and less noisy solution. Ok, lets see what this beauty has to offer:

## CREATION
Tuples can be created using plain old types (be carefull with C arrays, or better avoid them) or other structures - like stl containers or even tuples. The only requirement is that these types are default constructable. This is because tuples are always created empty and filled with data using the <<operator.

```C++
tuple<int> t1;
tuple<int, double> t2;
tuple<int, double, string> t3;
```

## GET / SET
You can use the templated get and set methods of tuples to access the stored values

```C++
t3.set<2>("foo")
cout << t3.get<2>() << endl
```
This is typesave and fast but clumsy. Its most often better to use the syntax shown in the initial example, but if you want to access single values there is also another way:

## ACCESSOR TYPES

```C++
typedef tuple<int, string> user;

user foo;

typedef user::accessor<0> id;
typedef user::accessor<1> name;

(id)user = 1;
(name)user = "boing";

cout << (name)user << endl;
```
You don't want this for simple one-use tuples but it's a good thing for more complex setups.

## COMPARISSON
Tuples can be compared to one another using all the standard operators like ==, <=, etc. Comparisson is done item by item and uses the operators of the compared values (which have to be defined of course). If otherwise equal a shorter tuple is less than the longer one.

## ACCEPT
As tuples consist of different types, there is no way to write an iterator for these containers. If you want to process all items inside a tuple use the visitor pattern.

```C++
struct outputter {
  template<class V> void operator () (V& value, int i) const {
   cout << value << " ";
  }
};

// somewhere else
user.accept(outputter());
```
