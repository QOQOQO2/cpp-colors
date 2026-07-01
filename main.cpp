#include "Color.h"
#include <iostream>

int main() {
  // Color test{300, 0, 0}; // This will throw a std::invalid_argument error
  Color color1, color2;

  // std::cout uses rhs.getAnsiCode() << "#" << rhs.getHexColor() <<
  // Color::resetAnsiCode.
  //
  // Calling getAnsiCode() returns the ansi escape code
  // to color text.
  //
  // Calling getHexColor returns the color in the format
  // "XXXXXX".
  //
  // Color::resetAnsiCode is a const where if you print it it will
  // reset the terminal colors back to the default colors.
  std::cout << "Enter a color: #";

  // std::cin can take in "XXX", "#XXX", "XXXXXX", or "#XXXXXX"
  std::cin >> color1;
  std::cout << "Enter another color: #";
  std::cin >> color2;

  // Valid arguments for == is another Color, "XXX", "#XXX", "XXXXXX", or
  // "#XXXXXX"
  if (color1 == color2)
    std::cout << "You entered the same color\n";

  if (color1 == "FFF")
    std::cout << "Your first color is white\n";

  if (color2 == "#000000")
    std::cout << "Your second color is black\n";

  // + uses standard sRGB and returns another Color type
  std::cout << color1 << " + " << color2 << " = " << color1 + color2 << "\n";
  return 0;
}
