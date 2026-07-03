#include "Color.h"
#include <cmath>
#include <iostream>
#include <stdexcept>

std::ostream &operator<<(std::ostream &os, const Color &rhs) {
  os << rhs.getAnsiCode() << "#" << rhs.getHexColor() << Color::resetAnsiCode;
  return os;
}

std::istream &operator>>(std::istream &in, Color &rhs) {
  std::string input{};
  in >> input;

  if (!input.empty() && input.at(0) == '#')
    input = input.substr(1); // Check if the user put a # in front of the
                             // string

  try {
    std::vector<double> parsedColors =
        Color::convertColor(sRGB, lsRGB, rhs.hexToDecimal(input));
    rhs.redChannel = parsedColors.at(0);
    rhs.greenChannel = parsedColors.at(1);
    rhs.blueChannel = parsedColors.at(2);
  } catch (const std::invalid_argument &) {
    in.setstate(std::ios::failbit); // if the hex to decimal conversion fails
                                    // then tell iostream that it failed
  }

  return in;
}

std::string Color::decimalToHex(int decimal) const {
  if (decimal < 0 || decimal > 255) {
    return "00";
  }
  char hexNumbers[]{'0', '1', '2', '3', '4', '5', '6', '7',
                    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

  std::string hex{};

  hex = hexNumbers[(decimal % 16)] + hex;
  decimal /= 16;
  hex = hexNumbers[(decimal % 16)] + hex;

  return hex;
}

std::vector<double> Color::hexToDecimal(const std::string &hex) const {
  if (hex.size() != 6 && hex.size() != 3)
    return {0, 0, 0};

  double red{};
  double green{};
  double blue{};

  if (hex.size() == 6) {
    // clang-format off
    red   =  hexCharToDecimal(hex.at(0)) * 16;
    red   += hexCharToDecimal(hex.at(1));
    green =  hexCharToDecimal(hex.at(2)) * 16;
    green += hexCharToDecimal(hex.at(3));
    blue  =  hexCharToDecimal(hex.at(4)) * 16;
    blue  += hexCharToDecimal(hex.at(5));
    // clang-format on
  } else {
    // its 17 because 11 in hex is 17 in decimal
    // clang-format off
    red   = hexCharToDecimal(hex.at(0)) * 17;
    green = hexCharToDecimal(hex.at(1)) * 17;
    blue  = hexCharToDecimal(hex.at(2)) * 17;
    // clang-format on
  }
  return {red, green, blue};
}

int Color::hexCharToDecimal(char c) const {
  if (c <= '9' && c >= '0')
    return c - '0';
  if (c <= 'F' && c >= 'A')
    return c - 'A' + 10;
  if (c <= 'f' && c >= 'a')
    return c - 'a' + 10;

  throw std::invalid_argument("color conversion error: in function "
                              "hexCharToDecimal, input char is invalid");
  return 0;
}

bool Color::checkRangelsRGB(double red, double green, double blue) const {
  if (red < 0 || red > 1) {
    return false;
  }
  if (green < 0 || green > 1) {
    return false;
  }
  if (blue < 0 || blue > 1) {
    return false;
  }
  return true;
}

bool Color::isBright(const std::vector<double> &col) const {
  std::vector<double> temp = col;

  if (temp.at(0) + temp.at(1) + temp.at(2) < 1.5f)
    return false;
  else
    return true;
}

inline double Color::averageNumber(double number1, double number2) {
  return (number1 + number2) / 2.0;
}

inline double Color::lerp(double number1, double number2, double t) {
  return (1 - t) * number1 + t * number2;
}

/* ---------------------------------- */

double Color::singlesRGBtolsRGB(double sRGB) {
  double col = sRGB;
  col /= 255.0;
  if (col <= 0.04045) {
    col /= 12.92;
  } else {
    col = std::pow((col + 0.055) / 1.055, 2.4);
  }
  return col;
}

double Color::singlelsRGBtosRGB(double lsRGB) {
  double col = lsRGB;

  if (col <= 0.0031308) {
    col *= 12.92;
  } else {
    col = 1.055 * std::pow(col, 1.0 / 2.4) - 0.055;
  }

  col *= 255.0;

  return col;
}

std::vector<double> Color::sRGBtolsRGB(const std::vector<double> &sRGB) {
  std::vector<double> col = sRGB;

  for (size_t i{0}; i < 3 && i < col.size(); ++i) {
    col.at(i) /= 255.0;

    if (col.at(i) <= 0.04045) {
      col.at(i) /= 12.92;
    } else {
      col.at(i) = std::pow((col.at(i) + 0.055) / 1.055, 2.4);
    }
  }

  return col;
}

std::vector<double> Color::lsRGBtosRGB(const std::vector<double> &lsRGB) {
  std::vector<double> col = lsRGB;

  for (size_t i{0}; i < 3 && i < col.size(); ++i) {
    if (col.at(i) <= 0.0031308) {
      col.at(i) *= 12.92;
    } else {
      col.at(i) = 1.055 * std::pow(col.at(i), 1.0 / 2.4) - 0.055;
    }

    col.at(i) *= 255.0;
  }

  return col;
}

/* ---------------------------------- */

Color::Color() : redChannel{0}, greenChannel{0}, blueChannel{0} {}

Color::Color(const std::vector<double> &col) {
  if (col.size() != 3)
    throw std::invalid_argument("Color Initialization Error: Vector argument "
                                "does not have exactly 3 inputs");

  std::vector<double> temp = convertColor(sRGB, lsRGB, col);

  redChannel = temp.at(0);
  greenChannel = temp.at(1);
  blueChannel = temp.at(2);
}

Color::Color(double red, double green, double blue)
    : redChannel{singlesRGBtolsRGB(red)},
      greenChannel{singlesRGBtolsRGB(green)},
      blueChannel{singlesRGBtolsRGB(blue)} {
  if (!checkRangelsRGB(redChannel, greenChannel, blueChannel)) {
    throw std::invalid_argument(
        "Color Initialization Error: Channel color out of bounds");
  }
}

std::string Color::getHexColor() const {
  std::string hexColor{};
  hexColor = decimalToHex(std::round(singlelsRGBtosRGB(redChannel))) +
             decimalToHex(std::round(singlelsRGBtosRGB(greenChannel))) +
             decimalToHex(std::round(singlelsRGBtosRGB(blueChannel)));

  return hexColor;
}

std::string Color::getAnsiCode() const {
  int red = std::round(singlelsRGBtosRGB(redChannel));
  int green = std::round(singlelsRGBtosRGB(greenChannel));
  int blue = std::round(singlelsRGBtosRGB(blueChannel));

  // give the ansi escape code for rgb background
  std::string backGroundEscapeCode = "\033[48;2;" + std::to_string(red) + ";" +
                                     std::to_string(green) + ";" +
                                     std::to_string(blue) + "m";

  std::string foreGroundEscapeCode =
      (isBright(lsRGBtosRGB({redChannel, greenChannel, blueChannel})))
          ? "\033[38;2;0;0;0m"
          : "\033[38;2;255;255;255m";

  return foreGroundEscapeCode + backGroundEscapeCode;
}

std::vector<double> Color::getDecimalColor() const {
  return convertColor(lsRGB, sRGB, {redChannel, greenChannel, blueChannel});
}

Color Color::averageColor(const Color &color1, const Color &color2,
                          ColorSpace colorSpace) {
  std::vector<double> decimalColor1 =
      convertColor(sRGB, colorSpace, color1.getDecimalColor());
  std::vector<double> decimalColor2 =
      convertColor(sRGB, colorSpace, color2.getDecimalColor());
  std::vector<double> averageVector{};

  for (size_t i{0}; i < 3; ++i) {
    averageVector.push_back(
        averageNumber(decimalColor1.at(i), decimalColor2.at(i)));
  }

  Color average{convertColor(colorSpace, sRGB, averageVector)};
  return average;
}

Color Color::lerpColor(const Color &color1, const Color &color2, double t,
                       ColorSpace colorSpace) {
  if (t < 0 || t > 1) {
    throw std::invalid_argument(
        "Color Manipulation Error: t value of lerp is invalid");
    return {};
  }
  std::vector<double> decimalColor1 =
      convertColor(lsRGB, colorSpace, color1.getDecimalColor());
  std::vector<double> decimalColor2 =
      convertColor(lsRGB, colorSpace, color2.getDecimalColor());
  std::vector<double> lerpVector{};

  for (size_t i{0}; i < 3; ++i) {
    lerpVector.push_back(lerp(decimalColor1.at(i), decimalColor2.at(i), t));
  }

  Color temp{convertColor(colorSpace, lsRGB, lerpVector)};
  return temp;
}

Color Color::operator+(const Color &rhs) const {
  return Color::averageColor(*this, rhs);
}

bool Color::operator==(const Color &rhs) const {
  if (redChannel == rhs.redChannel && greenChannel == rhs.greenChannel &&
      blueChannel == rhs.blueChannel)
    return true;
  return false;
}

bool Color::operator==(const char *s) const {
  std::string hexString(s);
  if (!hexString.empty() && hexString.at(0) == '#')
    hexString = hexString.substr(1);

  try {
    std::vector<double> parsedColors =
        convertColor(sRGB, lsRGB, hexToDecimal(hexString));
    return (redChannel == parsedColors.at(0) &&
            greenChannel == parsedColors.at(1) &&
            blueChannel == parsedColors.at(2));
  } catch (const std::invalid_argument &) {
    return false;
  }
}

Color &Color::operator=(const char *s) {
  std::string hexString(s);
  if (!hexString.empty() && hexString.at(0) == '#')
    hexString = hexString.substr(1);

  *this = Color{hexToDecimal(hexString)};
  return *this;
}

std::vector<double> Color::convertColor(ColorSpace from, ColorSpace to,
                                        const std::vector<double> &values) {
  // FIX: make this work with more colorspaces with the from and to variables
  if (from == to)
    return values;
  if (from == sRGB) {
    return sRGBtolsRGB(values);
  } else if (from == lsRGB) {
    return lsRGBtosRGB(values);
  } else {
    throw std::invalid_argument("Color Conversion Error: The from and/or to "
                                "values in convertColor function is invalid");
    return values;
  }
}
