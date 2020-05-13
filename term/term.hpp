#ifndef TERM_HPP
#define TERM_HPP

#include<memory>
#include<vector>
#include<string>
#include<stack>
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
struct node
{
  T Value;
  std::shared_ptr<node<T>> left = nullptr;
  std::shared_ptr<node<T>> right = nullptr;
  node<T>(T v) : Value(v), left(nullptr), right(nullptr) {}
  node<T>() = delete;
};

/////////////////////////////////////////////////////////////////
//
// iterator
//
/////////////////////////////////////////////////////////////////

template<typename T>
class term_iterator {
private:
  std::stack<std::shared_ptr<node<T>>> Path;
  std::shared_ptr<node<T>> Root;
public:
  typedef term<T>                         value_type;
  typedef term<T>*                        pointer;
  typedef term<T>&                        reference;
  typedef size_t                          size_type;
  typedef ptrdiff_t                       difference_type;
  typedef std::bidirectional_iterator_tag iterator_category;
  
  term_iterator<T>() = delete;
  term_iterator<T>(std::shared_ptr<node<T>> n, bool begin);
  term_iterator<T>(const term_iterator<T>& i) : Path(i._path), Root(i.Root) {}

  term<T>& operator*() {return Path.top()->Value;}
  term<T>* operator->() {return &Path.top()->Value;}
  term_iterator& operator++();
  term_iterator& operator--();

  term_iterator operator++(int){
    term_iterator<T> tmp(*this);
    ++this;
    return tmp;
  }

  term_iterator operator--(int) {
    term_iterator<T> tmp(*this);
    --this;
    return tmp;
  }

  term_iterator& operator-=(unsigned int n) {
    for(int i = 0; i < n; i++) ++*this;
    return *this;
  }

  term_iterator& operator+=(unsigned int n) {
    for(int i = 0; i < n; i++) --*this;
    return *this;
  }

  bool operator==(const term_iterator<T>& Next) const
  {return Path == Next.Path;}
  bool operator!=(const term_iterator<T>& Next) const
  {return Path != Next.Path;}
};

template<typename T>
term_iterator<T>::term_iterator(std::shared_ptr<node<T>> n, bool begin)
{
  Root = n;
  if(begin)
  {
    for(; n->left; n = n->left)
    {
      Path.push(n);
    }
    Path.push(n);
  }
}

template<typename T>
term_iterator<T>& term_iterator<T>::operator++()
{
  if(!Path.empty())
  {
    if(Path.top()->right)
    {
      Path.push(Path.top()->right);
      while(Path.top()->left)
      {
        Path.push(Path.top()->left);
      }
    }
    else
    {
      std::shared_ptr<node<T>> child = Path.top();
      Path.pop();
      while(!Path.empty() && Path.top()->right == child)
      {
        child = Path.top();
        Path.pop();
      }
    }
  }
  return *this;
}

template<typename T>
term_iterator<T>& term_iterator<T>::operator--()
{
  if(!Path.empty())
  {
    if(Path.top()->left)
    {
      Path.push(Path.top()->left);
      while(Path.top()->right)
      {
        Path.push(Path.top()->right);
      }
    }
    else
    {
      std::shared_ptr<node<T>> child = Path.top();
      Path.pop();
      while(!Path.empty() && Path.top()->left == child)
      {
        child = Path.top();
        Path.pop();
      }
    }
  }
  else
  {
    Path.push(Root);
    while(Path.top()->right)
    {
      Path.push(Path.top()->right);
    }
  }
  return *this;
}
/////////////////////////////////////////////////////////////////
//
// term
//
/////////////////////////////////////////////////////////////////

template<typename T>
class term {
private:
  typedef term<T>                               value_type;
  typedef term<T>*                              pointer;
  typedef term<T>&                              reference;
  typedef size_t                                size_type;
  typedef ptrdiff_t                             difference_type;
  
  typedef std::bidirectional_iterator_tag       iterator_category;
  typedef term_iterator<T>                      iterator;
  typedef term_iterator<const T>                const_iterator;

  typedef std::reverse_iterator<iterator>       reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  std::shared_ptr<T> Elem = nullptr;
  std::ostream &operator<<(std::ostream &out);
  std::shared_ptr<term<T>> InsertElem (const T &elem, std::shared_ptr<term<T>> pos);
public:
  term() {Elem = nullptr;};

  iterator begin()          {return term_iterator<T>();}
  iterator end()            {return term_iterator<T>();}
  const_iterator cbegin()   {return term_iterator<const T>();}
  const_iterator cend()     {return term_iterator<const T>();}

  reverse_iterator rbegin() {return std::reverse_iterator<iterator>(end());}
  reverse_iterator rend()   {return std::reverse_iterator<iterator>(begin());}
  const_reverse_iterator crbegin()
  {return std::reverse_iterator<const_iterator>(cend());}
  const_reverse_iterator crend()
  {return std::reverse_iterator<const_iterator>(cbegin());}

  virtual void print(std::ostream &out) = 0;
  void insert(const T &e)   {Elem = InsertElem(e, Elem);}
};

template<typename T>
std::shared_ptr<term<T>> InsertElem(const T &elem, std::shared_ptr<term<T>> pos) {
  if(!elem)
    return std::make_shared<term<T>>(term<T>(elem));
  if(elem < pos->Value)
    pos->left = InsertElem(elem, pos->left);
  if(elem < pos->Value)
    pos->right = InsertElem(elem, pos->right);
}


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
  T Val;
public:
  literal(){};
  literal(T v){
    Val = v;
  }
  void print(std::ostream &out){
    out << Val;
  }
};

template<typename T>
class function : public term<T> {
private:
  T Value;
  std::string Type;
  int Argc;
  std::vector<std::shared_ptr<term<T>>> Argv;
  std::shared_ptr<function<T>> left = nullptr;
  std::shared_ptr<function<T>> right = nullptr;
public:
  function(){};
  function(std::string type, int argc, std::vector<std::shared_ptr<term<T>>> argv)
  : Type(type), Argc(argc), Argv(argv){}
  void print(std::ostream &out) {
    out << Type;
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

template<typename T>
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

#endif // TERM_HPP
