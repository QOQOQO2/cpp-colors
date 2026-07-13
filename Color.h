#ifndef COLOR_H
#define COLOR_H

#include <functional>
#include <iosfwd>
#include <map>
#include <string>
#include <vector>

// https://github.com/QOQOQO2/cpp-colors

/* -- todo board --
1. TODO: ✓ add conversions to different colorspaces
2. TODO: ✓ add averaging for the different colorspaces
3. TODO: add << support for the different colorspaces
4. TODO: ✓ change how you store the colors into linear srgb from 0 to 1 instead
5. TODO: ✓ add a operator-() function to invert the color
*/

namespace clr {
enum ColorSpace { lsRGB, sRGB, Oklab, Oklch, HSV, CMYK };

class Color {
  friend std::ostream &operator<<(std::ostream &os, const Color &rhs);
  friend std::istream &operator>>(std::istream &in, Color &rhs);

private:
  // Define the variables for lsRGB format
  double redChannel, greenChannel, blueChannel;
  static constexpr double PI{3.1415926535897932384626433};

  std::string decimalToHex(int decimal) const;
  std::vector<double> hexToDecimal(const std::string &hex) const;
  int hexCharToDecimal(char c) const;
  bool checkRangelsRGB(double red, double green, double blue) const;
  bool isBright(const std::vector<double> &col) const;
  inline static double averageNumber(double a, double b) {
    return (a + b) / 2.0;
  }
  inline static double lerp(double a, double b, double t) {
    return (1 - t) * a + t * b;
  }
  inline static double clamp(double x) {
    return (x >= 1) ? 1 : ((x <= 0) ? 0 : x);
  }

  /* ----- Color conversions ------ */
  static double singlesRGBtolsRGB(double sRGB);
  static double singlelsRGBtosRGB(double lsRGB);

  static std::vector<double> sRGBtolsRGB(const std::vector<double> &sRGB);
  static std::vector<double> lsRGBtosRGB(const std::vector<double> &lsRGB);
  static std::vector<double> HSVtolsRGB(const std::vector<double> &HSV);
  static std::vector<double> lsRGBtoHSV(const std::vector<double> &lsRGB);
  static std::vector<double> CMYKtolsRGB(const std::vector<double> &CMYK);
  static std::vector<double> lsRGBtoCMYK(const std::vector<double> &lsRGB);
  static std::vector<double> OklabtolsRGB(const std::vector<double> &Oklab);
  static std::vector<double> lsRGBtoOklab(const std::vector<double> &lsRGB);
  static std::vector<double> OklchtolsRGB(const std::vector<double> &Oklch);
  static std::vector<double> lsRGBtoOklch(const std::vector<double> &lsRGB);

  struct HubConverters {
    std::function<std::vector<double>(const std::vector<double> &)> toHub;
    std::function<std::vector<double>(const std::vector<double> &)> fromHub;
  };

  static const std::map<ColorSpace, HubConverters> registry;

public:
  inline static const std::string resetAnsiCode{"\033[0m"};
  inline static ColorSpace defaultAverageColorSpace{lsRGB};

  Color();
  Color(const std::vector<double> &colors, ColorSpace colorSpace = sRGB);
  Color(double red, double green, double blue);
  Color(const char *s);

  std::string getHexColor() const;
  std::string getAnsiCode() const;
  std::vector<double> getDecimalColor() const;
  std::vector<double> getlsRGBColor() const;

  static Color averageColor(const Color &color1, const Color &color2,
                            ColorSpace colorSpace = defaultAverageColorSpace);
  static Color lerpColor(const Color &color1, const Color &color2, double t,
                         ColorSpace colorSpace = defaultAverageColorSpace);
  static Color invertColor(const Color &color,
                           ColorSpace colorSpace = defaultAverageColorSpace);
  Color operator+(const Color &rhs) const;
  Color operator-() const;
  bool operator==(const Color &rhs) const; // Check equivalance to another color
  bool operator==(const char *s) const;    // Check equivalance to a c-string
  Color &operator=(const char *s);

  static std::vector<double> convertColor(ColorSpace from, ColorSpace to,
                                          const std::vector<double> &values);
};
} // namespace clr

#endif // COLOR_H
