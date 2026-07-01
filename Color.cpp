#include "Color.h"

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
    std::vector<double> parsedColors = rhs.hexToDecimal(input);
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

  throw std::invalid_argument("Color Conversion Error: In function "
                              "hexCharToDecimal, input char is invalid");
  return 0;
}

bool Color::checkRange(double red, double green, double blue) const {
  if (red < 0 || red > 255) {
    return false;
  }
  if (green < 0 || green > 255) {
    return false;
  }
  if (blue < 0 || blue > 255) {
    return false;
  }
  return true;
}

bool Color::isBright(int red, int green, int blue) const {
  // 382 is 255 * 1.5 rounded down which is the same
  // as checking if the average of the values is greater than 127
  if (red + green + blue < 382)
    return false;
  else
    return true;
}

inline double Color::averageNumber(double number1, double number2) {
  return (number1 + number2) / 2.0;
}

Color::Color() : redChannel{0}, greenChannel{0}, blueChannel{0} {}

Color::Color(const std::vector<double> &colors) {
  if (colors.size() != 3)
    throw std::invalid_argument("Color Initialization Error: Vector argument "
                                "does not have exactly 3 inputs");
  redChannel = colors.at(0);
  greenChannel = colors.at(1);
  blueChannel = colors.at(2);
}

Color::Color(double red, double green, double blue)
    : redChannel{red}, greenChannel{green}, blueChannel{blue} {
  if (!checkRange(red, green, blue)) {
    throw std::invalid_argument(
        "Color Initialization Error: Channel color out of bounds");
  }
}

std::string Color::getHexColor() const {
  std::string hexColor{};
  hexColor = decimalToHex(std::round(redChannel)) +
             decimalToHex(std::round(greenChannel)) +
             decimalToHex(std::round(blueChannel));

  return hexColor;
}

std::string Color::getAnsiCode() const {
  int red = std::round(redChannel);
  int green = std::round(greenChannel);
  int blue = std::round(blueChannel);

  // give the ansi escape code for rgb background
  std::string backGroundEscapeCode = "\033[48;2;" + std::to_string(red) + ";" +
                                     std::to_string(green) + ";" +
                                     std::to_string(blue) + "m";

  std::string foreGroundEscapeCode = (isBright(red, green, blue))
                                         ? "\033[38;2;0;0;0m"
                                         : "\033[38;2;255;255;255m";

  return foreGroundEscapeCode + backGroundEscapeCode;
}

std::vector<double> Color::getDecimalColor() const {
  return {redChannel, greenChannel, blueChannel};
}

bool Color::setColor(double red, double green, double blue) {
  if (!checkRange(red, green, blue))
    return false;

  redChannel = red;
  greenChannel = green;
  blueChannel = blue;
  return true;
}

Color Color::averageColor(
    const Color &color1,
    const Color &color2 /*, std::string colorSpace = "srgb" */) {
  std::vector<double> decimalColor1 = color1.getDecimalColor();
  std::vector<double> decimalColor2 = color2.getDecimalColor();
  std::vector<double> averageVector{};

  for (int i{0}; i < 3; ++i) {
    averageVector.push_back(
        averageNumber(decimalColor1.at(i), decimalColor2.at(i)));
  }

  Color average{averageVector};
  return average;
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
    std::vector<double> parsedColors = hexToDecimal(hexString);
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
