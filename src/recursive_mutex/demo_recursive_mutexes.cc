#include "container.h"

static void ThreadBody(Container<int> &container, int from, int to) {
  std::vector<int> numbers;
  for (int i = from; i < to; i++) {
    numbers.push_back(i);
  }
  container.AddAll(numbers);
  container.Show();
}
int main() {
  Container<int> container;

  // A possible interleaving where thread2 goes first. the lists don't
  // get messed up in the middle.
  std::thread thread1(ThreadBody, std::ref(container), 0, 50);
  std::thread thread2(ThreadBody, std::ref(container), 50, 100);
  std::thread thread3(ThreadBody, std::ref(container), 100, 150);

  thread1.join();
  thread2.join();
  thread3.join();

  return 0;
}
