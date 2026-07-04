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

inline double Color::clamp(double value) {
  if (value >= 1)
    return 1;
  else if (value <= 0)
    return 0;
  else
    return value;
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

  for (size_t i{0}; i < 3 && i < col.size(); ++i)
    col.at(i) = singlesRGBtolsRGB(col.at(i));

  return col;
}

std::vector<double> Color::lsRGBtosRGB(const std::vector<double> &lsRGB) {
  std::vector<double> col = lsRGB;

  for (size_t i{0}; i < 3 && i < col.size(); ++i)
    col.at(i) = singlelsRGBtosRGB(col.at(i));

  return col;
}

std::vector<double> Color::HSVtolsRGB(const std::vector<double> &HSV) {
  double h = std::fmod(HSV.at(0), 360.0);
  if (h < 0.0)
    h += 360;

  double s = HSV.at(1);
  double v = HSV.at(2);

  double c = v * s;
  double x = c * (1 - std::abs(std::fmod(h / 60, 2) - 1));
  double m = v - c;

  double r = 0, g = 0, b = 0;

  int sector = h / 60;

  // clang-format off
  switch (sector) {
    case 0: // 0 <= H < 60
        r = c; g = x; b = 0.0;
        break;
    case 1: // 60 <= H < 120
        r = x; g = c; b = 0.0;
        break;
    case 2: // 120 <= H < 180
        r = 0.0; g = c; b = x;
        break;
    case 3: // 180 <= H < 240
        r = 0.0; g = x; b = c;
        break;
    case 4: // 240 <= H < 300
        r = x; g = 0.0; b = c;
        break;
    case 5: // 300 <= H < 360
    default:
        r = c; g = 0.0; b = x;
        break;
    // clang-format on
  }

  return {singlesRGBtolsRGB((r + m) * 255.0),
          singlesRGBtolsRGB((g + m) * 255.0),
          singlesRGBtolsRGB((b + m) * 255.0)};
}

std::vector<double> Color::lsRGBtoHSV(const std::vector<double> &lsRGB) {
  double r = clamp(singlelsRGBtosRGB(lsRGB.at(0)) / 255.0);
  double g = clamp(singlelsRGBtosRGB(lsRGB.at(1)) / 255.0);
  double b = clamp(singlelsRGBtosRGB(lsRGB.at(2)) / 255.0);

  double mx = r;
  if (g > mx)
    mx = g;
  if (b > mx)
    mx = b;

  double mn = r;
  if (g < mn)
    mn = g;
  if (b < mn)
    mn = b;

  double df = mx - mn;

  double h = 0;
  if (mx == mn) {
    h = 0;
  } else if (mx == r) {
    h = std::fmod(60 * ((g - b) / df) + 360, 360);
  } else if (mx == g) {
    h = std::fmod(60 * ((b - r) / df) + 120, 360);
  } else if (mx == b) {
    h = std::fmod(60 * ((r - g) / df) + 240, 360);
  }

  double s = (mx == 0) ? 0 : df / mx;
  double v = mx;

  return {h, s, v};
}

std::vector<double> Color::CMYKtolsRGB(const std::vector<double> &CMYK) {
  double c = CMYK.at(0);
  double m = CMYK.at(1);
  double y = CMYK.at(2);
  double k = CMYK.at(3);

  double r = (1 - c) * (1 - k);
  double g = (1 - m) * (1 - k);
  double b = (1 - y) * (1 - k);

  return {singlesRGBtolsRGB(r * 255.0), singlesRGBtolsRGB(g * 255.0),
          singlesRGBtolsRGB(b * 255.0)};
}

std::vector<double> Color::lsRGBtoCMYK(const std::vector<double> &lsRGB) {
  double r = clamp(singlelsRGBtosRGB(lsRGB.at(0)) / 255.0);
  double g = clamp(singlelsRGBtosRGB(lsRGB.at(1)) / 255.0);
  double b = clamp(singlelsRGBtosRGB(lsRGB.at(2)) / 255.0);

  double k = 0;
  if (r >= g && r >= b)
    k = 1 - r;
  else if (g >= r && g >= b)
    k = 1 - g;
  else
    k = 1 - b;

  double c = 0;
  double m = 0;
  double y = 0;
  if (k < 1) {
    c = (1 - r - k) / (1 - k);
    m = (1 - g - k) / (1 - k);
    y = (1 - b - k) / (1 - k);
  }

  return {c, m, y, k};
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
      convertColor(sRGB, colorSpace, color1.getDecimalColor());
  std::vector<double> decimalColor2 =
      convertColor(sRGB, colorSpace, color2.getDecimalColor());
  std::vector<double> lerpVector{};

  for (size_t i{0}; i < 3; ++i) {
    lerpVector.push_back(lerp(decimalColor1.at(i), decimalColor2.at(i), t));
  }

  Color temp{convertColor(colorSpace, sRGB, lerpVector)};
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
  if (from == to)
    return values;

  auto itFrom = registry.find(from);
  auto itTo = registry.find(to);

  if (itFrom == registry.end() || itTo == registry.end()) {
    throw std::invalid_argument(
        "Color Convert Error: Unsupported or invalid color space");
  }

  std::vector<double> lsrgb = itFrom->second.toHub(values);
  return itTo->second.fromHub(lsrgb);
}

const std::map<ColorSpace, Color::HubConverters> Color::registry = {
    // clang-format off
  { ColorSpace::lsRGB, { [](const auto& v) { return v; },     [](const auto& v) { return v; } } },
  { ColorSpace::sRGB,  { Color::sRGBtolsRGB,                Color::lsRGBtosRGB } },
  { ColorSpace::HSV,   { Color::HSVtolsRGB,                 Color::lsRGBtoHSV } },
  { ColorSpace::CMYK,  { Color::CMYKtolsRGB,                Color::lsRGBtoCMYK} }
    // clang-format on
};
