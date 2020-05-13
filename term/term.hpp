#ifndef TERM_HPP
#define TERM_HPP

#include<memory>
#include<vector>
#include<string>
#include<algorithm>
#include<iostream>
#include<exception>
#include "sub.hpp"

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

template<typename T>
class term {
private:
  typedef term<T> T_Type;
  typedef term<T> *T_Ptr;
  typedef term<T> &T_Ref;
  typedef size_t T_Size;
  typedef prtdiff_t T_dif;
  
  typedef term_iterator<T> iterator;
  typedef term_iterator<const T> const_iterator;

  std::ostream &operator<<(std::ostream &out);
public:

};

template<typename T>
class variable : public term<T> {
private:
  std::string Var;
public:
  variable(){}
  variable(std::string argv){
    Var = argv;
  }
  void print(std::ostream &out){
    out << Var;
  }
};

template<typename T>
class literal : public term<T> {
private:
  term<T> Val;
public:
  literal(){};
  literal(T argv){
    Val = argv;
  }
  void print(std::ostream &out){
    out << Val;
  }
};

template<typename T>
class function : public term<T> {
private:
  term<T> Fun;
  std::string Type;
  int Argc;
  std::vector<std::shared_ptr<term<T>>> Argv;
public:
  function(){};
  function(std::string type, int argc, std::vector<std::shared_ptr<term<T>>> argv){

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
}

/////////////////////////////////////////////////////////////////
//
// reduce
//
/////////////////////////////////////////////////////////////////

Template<typename T>
term_ptr<T> reduce(term_ptr<T> t, const std::vector<rule<T>>& rules)
{
}

/////////////////////////////////////////////////////////////////
//
// rewrite
//
/////////////////////////////////////////////////////////////////

template<typename T>
term_ptr<T> rewrite(term_ptr<T> t, term<T>& rhs, std::vector<int> path, const Sub<T>& sigma)
{
}

/////////////////////////////////////////////////////////////////
//
// operator<<
//
/////////////////////////////////////////////////////////////////

template<typename T>
std::ostream& operator<<(std::ostream& out, const term<T>& t)
{
}


template<typename T>
class term_iterator {
private:
  std::vector<std::shared_ptr<term<T>>> Pos;
public:
  term_iterator<T>() {}
  term_iterator<T>(const term_iterator<T> &i): Pos(i.Pos) {}
  term_iterator<T>(std::shared_ptr<term<T>> p) {
    Pos.push_back(p);
  }
  T& operator*() const;
  T* operator->() const;
  term_iterator& operator++();
  term_iterator& operator--();

  term_iterator operator++(int){
    tree_iterator<T> tmp(*this);
    ++this;
    return tmp;
  }

  term_iterator operator--(int) {
    tree_iterator<T> tmp(*this);
    --this;
    return tmp;
  }
  

  term_iterator& operator+=(unsigned int n) {
    for(int i = 0; i < n; i++) --*this;
    return *this;
  }
  
  term_iterator& operator-=(unsigned int n) {
    for(int i = 0; i < n; i++) ++*this;
    return *this;
  }


};


#endif // TERM_HPP
