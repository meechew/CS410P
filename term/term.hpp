#ifndef TERM_HPP
#define TERM_HPP

#include<memory>
#include <utility>
#include<vector>
#include<string>
#include<stack>
#include<iostream>
#include<exception>
#include "sub.hpp"
#include "term_iterator.hpp"

/////////////////////////////////////////////////////////////////
//
// forward declarations;
//
/////////////////////////////////////////////////////////////////

// This just makes life a lot more convenient
template<typename T>
using term_ptr = std::shared_ptr<term<T>>;

template<typename T>
using rule = std::pair<term_ptr<T>, term_ptr<T>>;

/////////////////////////////////////////////////////////////////
//
// class definitions
//
// The overall structure is pretty simple
// variable <: term
// literal  <: term
// function <: term
//
// Each term can have subterms in it.
//
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// term
//
/////////////////////////////////////////////////////////////////

template<typename T>
class term {
private:
  term_ptr<T> Root = nullptr;
public:

  typedef T                                     value_type;
  typedef T*                                    pointer;
  typedef T&                                    reference;
  typedef size_t                                size_type;
  typedef ptrdiff_t                             difference_type;

  typedef std::bidirectional_iterator_tag       iterator_category;
  typedef term_iterator<T>                      iterator;
  typedef term_iterator<const T>                const_iterator;

  typedef std::reverse_iterator<iterator>       reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  term(): left(nullptr), right(nullptr) {Root = std::shared_ptr<term<T>>(this);}
  explicit term(term<T>*): left(nullptr), right(nullptr)  {}
  explicit term(term_ptr<T>&): left(nullptr), right(nullptr)  {}

  iterator begin()          {return term_iterator<T>(Root, true);}
  iterator end()            {return term_iterator<T>(Root, false);}
  const_iterator cbegin()   {return term_iterator<const T>(Root, true);}
  const_iterator cend()     {return term_iterator<const T>(Root, false);}

  reverse_iterator rbegin() {return std::reverse_iterator<iterator>(end());}
  reverse_iterator rend()   {return std::reverse_iterator<iterator>(begin());}
  const_reverse_iterator crbegin()
  {return std::reverse_iterator<const_iterator>(cend());}
  const_reverse_iterator crend()
  {return std::reverse_iterator<const_iterator>(cbegin());};

  std::ostream& operator<<(std::ostream &out) {return print(out);};
  virtual std::ostream& print(std::ostream &out) { return out;};

  term<T>& GetRoot() {return *Root;}
  term_ptr<T> left = nullptr;
  term_ptr<T> right = nullptr;

  void AddChildren(int argc, std::vector<term_ptr<T>> argv) {
    switch (argc) {
      case 1:
        left = argv.front();
        right = nullptr;
        return;
      case 2:
        left = argv.front();
        right = argv.back();
        return;
    }
  }
};

template<typename T>
class variable : public term<T> {
private:
  std::string Var;
public:
  variable(){}
  variable(std::string argv){
    Var = std::move(argv);
  }

  std::ostream& print(std::ostream &out) override {
    out << Var;
    return out;
  }
};

template<typename T>
class literal : public term<T> {
private:
  T Val;
public:
  literal(){};
  literal(T v): Val(v){}

  std::ostream& print(std::ostream &out) override {
    out << Val;
    return out;
  }
};

template<typename T>
class function : public term<T> {
private:
  T Func;
  std::string Type;
  int Argc;
  std::vector<term_ptr<T>> Argv;
public:
  function(){};
  function(std::string type, int argc, std::vector<term_ptr<T>> argv)
  : Type(type), Argc(argc), Argv(argv) { term<T>::AddChildren(argc, argv); }

  std::ostream& print(std::ostream &out) override {
    if(term<T>::left) term<T>::left->print(out);
    out << ' ' << Type << ' ';
    if(term<T>::right) term<T>::right->print(out);
    return out;
  }
};

/////////////////////////////////////////////////////////////////
//
// unify
//
/////////////////////////////////////////////////////////////////

template<typename T>
bool unify(const term<T>& t1, const term<T>& t2, Sub<T>& sigma)
{
  return true;
}

/////////////////////////////////////////////////////////////////
//
// reduce
//
/////////////////////////////////////////////////////////////////

template<typename T>
term_ptr<T> reduce(term_ptr<T> t, const std::vector<rule<T>>& rules)
{
  return t;
}

/////////////////////////////////////////////////////////////////
//
// rewrite
//
/////////////////////////////////////////////////////////////////

template<typename T>
term_ptr<T> rewrite(term_ptr<T> t, term<T>& rhs, std::vector<int> path, const Sub<T>& sigma)
{
  return t;
}

/////////////////////////////////////////////////////////////////
//
// operator<<
//
/////////////////////////////////////////////////////////////////

template<typename T>
std::ostream& operator<<(std::ostream& out, term<T>& t)
{
  return t.print(out);
}

#endif // TERM_HPP
