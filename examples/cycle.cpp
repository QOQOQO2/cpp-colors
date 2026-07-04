#include "../Color.h"
#include <chrono>
#include <iostream>
#include <thread>

int main() {
  for (;;) {
    for (size_t i{0}; i < 360; ++i) {
      std::cout << "\r";
      std::cout << Color{{0.776, 0.1092, static_cast<double>(i)}, Oklch};
      std::cout << std::flush;
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
  return 0;
}
