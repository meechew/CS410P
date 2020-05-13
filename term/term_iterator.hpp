// Created by CBunt on 13 May 2020.
//

#ifndef TERM_TERM_ITERATOR_HPP
#define TERM_TERM_ITERATOR_HPP
#include<memory>
#include<vector>
#include<stack>
#ifndef NODE
#define NODE
template<typename T>
struct node
{
  T Value;
  std::shared_ptr<node<T>> left = nullptr;
  std::shared_ptr<node<T>> right = nullptr;
  node<T>(T v) : Value(v), left(nullptr), right(nullptr) {}
  node<T>() = delete;
};
#endif

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
  typedef T                               value_type;
  typedef T*                              pointer;
  typedef T&                              reference;
  typedef size_t                          size_type;
  typedef ptrdiff_t                       difference_type;
  typedef std::bidirectional_iterator_tag iterator_category;

  term_iterator<T>() = delete;
  term_iterator<T>(std::shared_ptr<node<T>> n, bool begin);
  term_iterator<T>(const term_iterator<T>& i) : Path(i.Path), Root(i.Root) {}

  //Not sure why but the shared_ptr was returning term<T> not term<T>&
  term<T>& operator*() {return (term<T>&)Path.top()->Value;}
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
#endif //TERM_TERM_ITERATOR_HPP
