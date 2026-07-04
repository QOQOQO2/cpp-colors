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
  std::string hexString{hex};
  if (!hexString.empty() && hexString.at(0) == '#')
    hexString = hexString.substr(1);

  if (hexString.size() != 6 && hexString.size() != 3)
    return {0, 0, 0};

  double red{};
  double green{};
  double blue{};

  if (hexString.size() == 6) {
    // clang-format off
    red   =  hexCharToDecimal(hexString.at(0)) * 16;
    red   += hexCharToDecimal(hexString.at(1));
    green =  hexCharToDecimal(hexString.at(2)) * 16;
    green += hexCharToDecimal(hexString.at(3));
    blue  =  hexCharToDecimal(hexString.at(4)) * 16;
    blue  += hexCharToDecimal(hexString.at(5));
    // clang-format on
  } else {
    // its 17 because 11 in hex is 17 in decimal
    // clang-format off
    red   = hexCharToDecimal(hexString.at(0)) * 17;
    green = hexCharToDecimal(hexString.at(1)) * 17;
    blue  = hexCharToDecimal(hexString.at(2)) * 17;
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

  if (temp.at(0) + temp.at(1) + temp.at(2) < 382)
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
  double r = singlelsRGBtosRGB(clamp(lsRGB.at(0))) / 255.0;
  double g = singlelsRGBtosRGB(clamp(lsRGB.at(1))) / 255.0;
  double b = singlelsRGBtosRGB(clamp(lsRGB.at(2))) / 255.0;

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
  double r = singlelsRGBtosRGB(clamp(lsRGB.at(0))) / 255.0;
  double g = singlelsRGBtosRGB(clamp(lsRGB.at(1))) / 255.0;
  double b = singlelsRGBtosRGB(clamp(lsRGB.at(2))) / 255.0;

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

std::vector<double> Color::OklabtolsRGB(const std::vector<double> &Oklab) {
  double L = Oklab.at(0);
  double A = Oklab.at(1);
  double B = Oklab.at(2);
  double l = 0;
  double m = 0;
  double s = 0;
  double r = 0;
  double g = 0;
  double b = 0;

  // clang-format off
  static constexpr double invoklab[3][3]{
    {1, 0.3963377774, 0.2158037573},
    {1, -0.1055613458, -0.0638541728},
    {1, -0.0894841775, -1.2914855480}};

  static constexpr double invlms[3][3]{
    {4.0767416621, -3.3077115913, 0.2309699292},
    {-1.2684380046, 2.6097574011, -0.3413193965},
    {-0.0041960863, -0.7034186147, 1.7076147010}};
  // clang-format on

  l = (L) + (invoklab[0][1] * A) + (invoklab[0][2] * B);
  m = (L) + (invoklab[1][1] * A) + (invoklab[1][2] * B);
  s = (L) + (invoklab[2][1] * A) + (invoklab[2][2] * B);

  l = l * l * l;
  m = m * m * m;
  s = s * s * s;

  r = (invlms[0][0] * l) + (invlms[0][1] * m) + (invlms[0][2] * s);
  g = (invlms[1][0] * l) + (invlms[1][1] * m) + (invlms[1][2] * s);
  b = (invlms[2][0] * l) + (invlms[2][1] * m) + (invlms[2][2] * s);

  return {clamp(r), clamp(g), clamp(b)};
}

std::vector<double> Color::lsRGBtoOklab(const std::vector<double> &lsRGB) {
  double r = lsRGB.at(0);
  double g = lsRGB.at(1);
  double b = lsRGB.at(2);
  double l = 0;
  double m = 0;
  double s = 0;
  double L = 0;
  double A = 0;
  double B = 0;

  // clang-format off
  static constexpr double lms[3][3]{
    {0.4122214708, 0.5363325363, 0.0514459929},
    {0.2119034982, 0.6806995451, 0.1073969566},
    {0.0883024619, 0.2817188376, 0.6299787005}};

  static constexpr double oklab[3][3]{
    {0.2104542553,  0.7936177850, -0.0040720468},
    {1.9779984951, -2.4285922050,  0.4505937099},
    {0.0259040371,  0.7827717662, -0.8086757660}};
  // clang-format on

  l = (lms[0][0] * r) + (lms[0][1] * g) + (lms[0][2] * b);
  m = (lms[1][0] * r) + (lms[1][1] * g) + (lms[1][2] * b);
  s = (lms[2][0] * r) + (lms[2][1] * g) + (lms[2][2] * b);

  l = std::cbrt(l);
  m = std::cbrt(m);
  s = std::cbrt(s);

  L = (oklab[0][0] * l) + (oklab[0][1] * m) + (oklab[0][2] * s);
  A = (oklab[1][0] * l) + (oklab[1][1] * m) + (oklab[1][2] * s);
  B = (oklab[2][0] * l) + (oklab[2][1] * m) + (oklab[2][2] * s);

  return {L, A, B};
}

std::vector<double> Color::OklchtolsRGB(const std::vector<double> &Oklch) {
  double l = Oklch.at(0);
  double C = Oklch.at(1);
  double h = Oklch.at(2);

  h *= PI / 180;

  double a = C * std::cos(h);
  double b = C * std::sin(h);

  return OklabtolsRGB({l, a, b});
}

std::vector<double> Color::lsRGBtoOklch(const std::vector<double> &lsRGB) {
  std::vector<double> Oklab = lsRGBtoOklab(lsRGB);

  double l = Oklab.at(0);
  double a = Oklab.at(1);
  double b = Oklab.at(2);

  double C = std::sqrt(a * a + b * b);
  double h = std::atan2(b, a);

  h *= 180 / PI;
  if (h < 0) {
    h += 360;
  }

  return {l, C, h};
}

/* ---------------------------------- */

Color::Color() : redChannel{0}, greenChannel{0}, blueChannel{0} {}

Color::Color(const std::vector<double> &col, ColorSpace colorSpace) {

  if (colorSpace != CMYK && col.size() != 3)
    throw std::length_error(
        "Color Initialization Error: Input vector size is invalid");

  if (colorSpace == CMYK && col.size() == 4)
    throw std::length_error(
        "Color Initialization Error: Input vector size is invalid");

  std::vector<double> temp = convertColor(colorSpace, lsRGB, col);

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

Color::Color(const char *s) : Color(hexToDecimal(s), sRGB) {}

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

std::vector<double> Color::getlsRGBColor() const {
  return {redChannel, greenChannel, blueChannel};
}

Color Color::averageColor(const Color &color1, const Color &color2,
                          ColorSpace colorSpace) {

  if (colorSpace == HSV || colorSpace == Oklch)
    throw std::domain_error("Color Manipulation Error: not allowed to average "
                            "cylindrical color spaces (e.g. HSV)");

  std::vector<double> decimalColor1 =
      convertColor(lsRGB, colorSpace, color1.getlsRGBColor());
  std::vector<double> decimalColor2 =
      convertColor(lsRGB, colorSpace, color2.getlsRGBColor());
  std::vector<double> averageVector{};

  for (size_t i{0}; i < decimalColor1.size(); ++i) {
    averageVector.push_back(
        averageNumber(decimalColor1.at(i), decimalColor2.at(i)));
  }

  return Color{averageVector, colorSpace};
}

Color Color::lerpColor(const Color &color1, const Color &color2, double t,
                       ColorSpace colorSpace) {
  if (t < 0 || t > 1) {
    throw std::out_of_range(
        "Color Manipulation Error: t value of lerp is invalid");
  }
  std::vector<double> decimalColor1 =
      convertColor(lsRGB, colorSpace, color1.getlsRGBColor());
  std::vector<double> decimalColor2 =
      convertColor(lsRGB, colorSpace, color2.getlsRGBColor());
  std::vector<double> lerpVector{};

  for (size_t i{0}; i < decimalColor1.size(); ++i) {
    lerpVector.push_back(lerp(decimalColor1.at(i), decimalColor2.at(i), t));
  }

  return Color{lerpVector, colorSpace};
}

Color Color::invertColor(const Color &color, ColorSpace colorSpace) {
  std::vector<double> colorValues =
      convertColor(lsRGB, colorSpace, color.getlsRGBColor());

  switch (colorSpace) {
  case lsRGB:
    colorValues.at(0) = 1 - colorValues.at(0);
    colorValues.at(1) = 1 - colorValues.at(1);
    colorValues.at(2) = 1 - colorValues.at(2);
    break;

  case sRGB:
    colorValues.at(0) = 255 - colorValues.at(0);
    colorValues.at(1) = 255 - colorValues.at(1);
    colorValues.at(2) = 255 - colorValues.at(2);
    break;

  case CMYK:
    colorValues.at(0) = 1 - colorValues.at(0);
    colorValues.at(1) = 1 - colorValues.at(1);
    colorValues.at(2) = 1 - colorValues.at(2);
    colorValues.at(3) = 1 - colorValues.at(3);
    break;

  case Oklab:
    // colorValues.at(0) = 1 - colorValues.at(0);
    colorValues.at(1) = -colorValues.at(1);
    colorValues.at(2) = -colorValues.at(2);
    break;

  case HSV:
    colorValues.at(0) = std::fmod(colorValues.at(0) + 180, 360);
    colorValues.at(1) = 1 - colorValues.at(1);
    // colorValues.at(2) = 1 - colorValues.at(2);
    break;

  case Oklch:
    colorValues.at(0) = 1 - colorValues.at(0);
    // colorValues.at(1) = 1 - colorValues.at(1);
    colorValues.at(2) = std::fmod(colorValues.at(2) + 180, 360);
    break;
  }

  return Color{colorValues, colorSpace};
}

Color Color::operator+(const Color &rhs) const {
  return Color::averageColor(*this, rhs, defaultAverageColorSpace);
}

Color Color::operator-() const {
  return Color::invertColor(*this, defaultAverageColorSpace);
}

bool Color::operator==(const Color &rhs) const {
  if (redChannel == rhs.redChannel && greenChannel == rhs.greenChannel &&
      blueChannel == rhs.blueChannel)
    return true;
  return false;
}

bool Color::operator==(const char *s) const {
  try {
    std::vector<double> parsedColors =
        convertColor(sRGB, lsRGB, hexToDecimal(s));
    return (redChannel == parsedColors.at(0) &&
            greenChannel == parsedColors.at(1) &&
            blueChannel == parsedColors.at(2));
  } catch (const std::invalid_argument &) {
    return false;
  }
}

Color &Color::operator=(const char *s) {
  *this = Color{hexToDecimal(s)};
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
  { ColorSpace::lsRGB, { [](const auto& v) { return v; }, [](const auto& v) { return v; } } },
  { ColorSpace::sRGB,  { Color::sRGBtolsRGB,  Color::lsRGBtosRGB } },
  { ColorSpace::HSV,   { Color::HSVtolsRGB,   Color::lsRGBtoHSV } },
  { ColorSpace::CMYK,  { Color::CMYKtolsRGB,  Color::lsRGBtoCMYK} },
  { ColorSpace::Oklab, { Color::OklabtolsRGB, Color::lsRGBtoOklab } },
  { ColorSpace::Oklch, { Color::OklchtolsRGB, Color::lsRGBtoOklch} }
    // clang-format on
};
