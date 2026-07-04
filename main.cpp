#include "Color.h"
#include <iomanip>
#include <iostream>

int main() {
  // Color test{300, 0, 0}; // This will throw a std::invalid_argument error
  Color color1, color2;

  std::cout << "Enter a color: #";

  // std::cin can take in "XXX", "#XXX", "XXXXXX", or "#XXXXXX"
  std::cin >> color1;
  std::cout << "Enter another color: #";
  std::cin >> color2;

  // std::cout uses rhs.getAnsiCode() << "#" << rhs.getHexColor() <<
  // Color::resetAnsiCode.
  //
  // Calling getAnsiCode() returns a std::string of the ansi escape code
  // to color text.
  //
  // Calling getHexColor() returns a std::string of the color in the format
  // "XXXXXX".
  //
  // Color::resetAnsiCode is a const std::string where if you print it it will
  // reset the terminal colors back to the default colors.
  std::cout << "You entered " << color1 << " and " << color2 << "\n";

  // Valid arguments for == are: another Color, "XXX", "#XXX", "XXXXXX", or
  // "#XXXXXX"
  if (color1 == color2)
    std::cout << "You entered the same color\n";

  if (color1 == "FFF")
    std::cout << "Your first color is white\n";

  if (color2 == "#000000")
    std::cout << "Your second color is black\n";

  // + uses linear sRGB and returns another Color type
  std::cout << color1 << " + " << color2 << " = " << color1 + color2 << "\n";

  // You can also specify which color space to use for averaging with the
  // averageColor() function
  std::cout << color1 << " + " << color2 << " = "
            << Color::averageColor(color1, color2, sRGB) << " in sRGB\n\n";

  // You can convert colors into different formats
  // getDecimalColor() return the format in sRGB
  std::vector<double> linearColor =
      Color::convertColor(sRGB, lsRGB, color1.getDecimalColor());

  std::vector<double> hsvColor =
      Color::convertColor(sRGB, HSV, color1.getDecimalColor());

  std::vector<double> cmykColor =
      Color::convertColor(sRGB, CMYK, color1.getDecimalColor());

  std::cout << std::fixed << std::setprecision(2);

  std::cout << color1 << " in linear sRGB is " << color1.getAnsiCode();
  std::cout << linearColor.at(0) << " ";
  std::cout << linearColor.at(1) << " ";
  std::cout << linearColor.at(2);
  std::cout << Color::resetAnsiCode << "\n\n";

  std::cout << color1 << " in HSV is " << color1.getAnsiCode();
  std::cout << hsvColor.at(0) << "H ";
  std::cout << hsvColor.at(1) << "S ";
  std::cout << hsvColor.at(2) << "V";
  std::cout << Color::resetAnsiCode << "\n\n";

  std::cout << color1 << " in CMYK is " << color1.getAnsiCode();
  std::cout << cmykColor.at(0) << "C ";
  std::cout << cmykColor.at(1) << "M ";
  std::cout << cmykColor.at(2) << "Y ";
  std::cout << cmykColor.at(3) << "K ";
  std::cout << Color::resetAnsiCode << "\n\n";

  // You can lerp between colors
  std::cout << "Enter the t value to lerp " << color1 << " and " << color2
            << ": ";
  double t{};
  std::cin >> t;
  std::cout << "The lerped color is " << Color::lerpColor(color1, color2, t)
            << "\n";

  return 0;
}
