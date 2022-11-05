#ifndef CONTAINER_H
#define CONTAINER_H

#include <iostream>
#include <vector>

#include "recursive_mutex.h"
template <typename T>
class Container {
 public:
  void Add(const T& elem) {
    mutex_.Lock();
    contents_.push_back(elem);
    mutex_.Unlock();
  }

  void AddAll(const std::vector<T>& elems) {
    mutex_.Lock();
    for (T elem : elems) {
      Add(elem);
    }
    mutex_.Unlock();
  }

  void Show() {
    mutex_.Lock();
    std::cout << '[' << contents_[0];
    for (size_t i = 1; i < contents_.size(); i++) {
      std::cout << ", " << contents_[i] ;
    }
    std::cout << ']' << std::endl;
    mutex_.Unlock();
  }

 private:
  std::vector<T> contents_;
  RecursiveMutex mutex_;
};

#endif  // CONTAINER_H
