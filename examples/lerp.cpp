#include "../Color.h"
#include <chrono>
#include <iostream>
#include <thread>

int main() {
  Color red{255, 0, 0};
  Color green{0, 255, 0};
  Color blue{0, 0, 255};

  for (;;) {
    for (size_t i{0}; i < 255; i += 3) {
      std::cout << "\r";
      std::cout << Color::lerpColor(red, green, i / 255.0);
      std::cout << std::flush;
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    for (size_t i{0}; i < 255; i += 3) {
      std::cout << "\r";
      std::cout << Color::lerpColor(green, blue, i / 255.0);
      std::cout << std::flush;
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    for (size_t i{0}; i < 255; i += 3) {
      std::cout << "\r";
      std::cout << Color::lerpColor(blue, red, i / 255.0);
      std::cout << std::flush;
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  }
  return 0;
}
