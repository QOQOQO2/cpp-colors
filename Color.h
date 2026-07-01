#ifndef COLOR_H
#define COLOR_H

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// https://github.com/QOQOQO2/cpp-colors

/* -- todo board --
 TODO: add conversions to different colorspaces
 TODO: add averaging for the different colorspaces
 TODO: add << support for the different colorspaces
 TODO: add a alpha channel
 TODO: change how you store the colors into linear srgb from 0 to 1 instead
 TODO: add a operator-() function to invert the color

 NOTE: colorspaces to add:
  linear srgb (lsrgb)
  srgb
  oklab
  oklch
  hsv/hsl
  cmyk
*/

// enum class colorSpace {};

class Color {
  friend std::ostream &operator<<(std::ostream &os, const Color &rhs);
  friend std::istream &operator>>(std::istream &in, Color &rhs);

private:
  // Define the variables for the hex color
  double redChannel{};
  double greenChannel{};
  double blueChannel{};

  std::string decimalToHex(int decimal) const;
  std::vector<double> hexToDecimal(const std::string &hex) const;
  int hexCharToDecimal(char c) const;
  bool checkRange(double red, double green, double blue) const;
  bool isBright(int red, int green, int blue) const;
  inline static double averageNumber(double number1, double number2);

public:
  inline static const std::string resetAnsiCode{"\033[0m"};

  Color();
  Color(const std::vector<double> &colors);
  Color(double red, double green, double blue);

  std::string getHexColor() const;
  std::string getAnsiCode() const;
  std::vector<double> getDecimalColor() const;
  bool setColor(double red, double green, double blue);

  static Color
  averageColor(const Color &color1, const Color &color2
               /*, std::string colorSpace = "lsrgb" */); // TODO: have an
                                                         // option to select
                                                         // different
                                                         // colorspaces for
                                                         // averaging the color
  Color operator+(const Color &rhs) const;
  bool operator==(const Color &rhs) const; // Check equivalance to another color
  bool operator==(const char *s) const;    // Check equivalance to a c-string
  Color &operator=(const char *s);         // TODO: have a variable change which
                                           // colorspace to average the color in
};

#endif // COLOR_H
