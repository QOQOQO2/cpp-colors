#include "Color.h"
#include <iomanip>
#include <iostream>

// Everything is in the namespace clr
using namespace clr;

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

  // + uses linear sRGB unless you change it and returns another Color type
  std::cout << color1 << " + " << color2 << " = " << color1 + color2
            << " in linear sRGB\n";

  // You can also specify which color space to use for averaging with the
  // averageColor() function
  // you cannot use cylindrical color spaces like hsv for averaging and lerping
  // The colorspaces are enums named ColorSpace
  std::cout << color1 << " + " << color2 << " = "
            << Color::averageColor(color1, color2, sRGB) << " in sRGB\n";

  // You can change what the default color space that averageColor, lerpColor,
  // invertColor, operator+, and operator+ uses by changing
  // defaultAverageColorSpace
  Color::defaultAverageColorSpace = Oklab;
  std::cout << color1 << " + " << color2 << " = " << color1 + color2
            << " in Oklab\n\n";

  // You can convert colors into different colorSpaces
  // The current color spaces are:
  // lsRGB, sRGB, Oklab, Oklch, HSV, and CMYK
  //
  // getDecimalColor() returns the format in sRGB
  // getlsRGBColor() returns the format in linear sRGB
  std::vector<double> linearColor =
      Color::convertColor(sRGB, lsRGB, color1.getDecimalColor());

  std::vector<double> oklabColor =
      Color::convertColor(sRGB, Oklab, color1.getDecimalColor());

  std::vector<double> oklchColor =
      Color::convertColor(sRGB, Oklch, color1.getDecimalColor());

  std::cout << std::fixed << std::setprecision(3);

  std::cout << color1 << " in linear sRGB is " << color1.getAnsiCode();
  std::cout << linearColor.at(0) << " ";
  std::cout << linearColor.at(1) << " ";
  std::cout << linearColor.at(2);
  std::cout << Color::resetAnsiCode << "\n";

  std::cout << color1 << " in Oklab is " << color2.getAnsiCode();
  std::cout << oklabColor.at(0) << "l ";
  std::cout << oklabColor.at(1) << "a ";
  std::cout << oklabColor.at(2) << "b";
  std::cout << Color::resetAnsiCode << "\n";

  std::cout << color1 << " in Oklch is " << color2.getAnsiCode();
  std::cout << oklchColor.at(0) << "l ";
  std::cout << oklchColor.at(1) << "C ";
  std::cout << oklchColor.at(2) << "h";
  std::cout << Color::resetAnsiCode << "\n\n";

  // You can lerp between colors
  std::cout << "Enter the t value to lerp " << color1 << " and " << color2
            << ": ";
  double t{};
  std::cin >> t;
  // defaultAverageColorSpace is still oklab
  std::cout << "The lerped color is " << Color::lerpColor(color1, color2, t)
            << " in Oklab\n\n";

  // Invert colors
  // Oklab and Oklch will result in the same color
  // sRGB, HSV, and CMYK will result in the same color
  std::cout << color2 << " inverted is " << -color2 << " in Oklab\n";
  std::cout << color2 << " inverted is " << Color::invertColor(color2, HSV)
            << " in HSV\n";

  return 0;
}
