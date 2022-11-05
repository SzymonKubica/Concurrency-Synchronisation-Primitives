#ifndef RECURSIVE_MUTEX_BASE_H
#define RECURSIVE_MUTEX_BASE_H

#include <cassert>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <thread>
class RecursiveMutex {
 public:
  RecursiveMutex() : number_of_acquisitions_(0){}

  void Lock() {
    std::unique_lock<std::mutex> lock(mutex_);
    std::thread::id myself = std::this_thread::get_id();

    if(owner_.has_value() && owner_.value() == myself) {
      number_of_acquisitions_++;
      return;
    }

    // !!! That while is very important because once we wake up everyone they still
    // need to check that the lock is not held by anyone else. !!!
    while(number_of_acquisitions_ != 0) {
      // Wait until the lock is not held by anyone.
      condition_.wait(lock, [this]() {return !owner_.has_value();});
    }

    // After waiting acquire the lock.
    assert(!owner_.has_value() && "Lock should not be held when we acquire it.");
    assert(number_of_acquisitions_ == 0 &&
           "Before changing the owner of the lock no-one can be holding it.");
    owner_ = myself;
    number_of_acquisitions_ = 1;
  }

  void Unlock() {
    std::unique_lock<std::mutex> lock(mutex_);
    std::thread::id myself = std::this_thread::get_id();

    // Check that we don't call unlock on an unlocked lock.
    assert(owner_.has_value() && "The lock must be held before it can be released.");

    // Check that only the lock holder can unlock it.
    assert(owner_ == myself && "Only the owner of the lock can unlock it.");

    // Check that the number of times the lock was acquired is definitely positive.
    assert(number_of_acquisitions_ > 0);

    number_of_acquisitions_--;

    if (number_of_acquisitions_ == 0) {
      owner_.reset();
      condition_.notify_all();
    }
  }

 private:
  std::mutex mutex_;
  std::condition_variable condition_;
  std::optional<std::thread::id> owner_;
  int number_of_acquisitions_;
};

#endif  // RECURSIVE_MUTEX_BASE_H
