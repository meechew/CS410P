// Created by CBunt on 02 May 2020.
//

#ifndef TERM_TERM_ITERATOR_HPP
#define TERM_TERM_ITERATOR_HPP

template<typename T>
class term_iterator {
private:
  term<T>* Pos;
public:
  T& operator*() const;
  T* operator->() const;
  term_iterator& operator++();
  term_iterator& operator--();
  term_iterator operator++(int);
  term_iterator operator--(int);
  term_iterator& operator+=(unsigned int);
  term_iterator& operator-=(unsigned int);
};

#endif //TERM_TERM_ITERATOR_HPP
