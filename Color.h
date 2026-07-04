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
4. TODO: add a alpha channel
5. TODO: ✓ change how you store the colors into linear srgb from 0 to 1 instead
6. TODO: add a operator-() function to invert the color

 NOTE: colorspaces to add:
  linear srgb (lsrgb) ✓
  srgb ✓
  oklab
  oklch
  hsv ✓
  cmyk ✓
*/

enum ColorSpace { lsRGB, sRGB, Oklab, Oklch, HSV, CMYK };

class Color {
  friend std::ostream &operator<<(std::ostream &os, const Color &rhs);
  friend std::istream &operator>>(std::istream &in, Color &rhs);

private:
  // Define the variables for lsRGB format
  double redChannel, greenChannel, blueChannel;

  std::string decimalToHex(int decimal) const;
  std::vector<double> hexToDecimal(const std::string &hex) const;
  int hexCharToDecimal(char c) const;
  bool checkRangelsRGB(double red, double green, double blue) const;
  bool isBright(const std::vector<double> &col) const;
  inline static double averageNumber(double number1, double number2);
  inline static double lerp(double number1, double number2, double t);
  inline static double clamp(double value);

  /* ----- Color conversions ------ */
  static double singlesRGBtolsRGB(double sRGB);
  static double singlelsRGBtosRGB(double lsRGB);

  static std::vector<double> sRGBtolsRGB(const std::vector<double> &sRGB);
  static std::vector<double> lsRGBtosRGB(const std::vector<double> &lsRGB);
  static std::vector<double> HSVtolsRGB(const std::vector<double> &HSV);
  static std::vector<double> lsRGBtoHSV(const std::vector<double> &lsRGB);
  static std::vector<double> CMYKtolsRGB(const std::vector<double> &CMYK);
  static std::vector<double> lsRGBtoCMYK(const std::vector<double> &lsRGB);

  struct HubConverters {
    std::function<std::vector<double>(const std::vector<double> &)> toHub;
    std::function<std::vector<double>(const std::vector<double> &)> fromHub;
  };

  static const std::map<ColorSpace, HubConverters> registry;

public:
  inline static const std::string resetAnsiCode{"\033[0m"};

  Color();
  Color(const std::vector<double> &colors);
  Color(double red, double green, double blue);

  std::string getHexColor() const;
  std::string getAnsiCode() const;
  std::vector<double> getDecimalColor() const;

  static Color averageColor(const Color &color1, const Color &color2,
                            ColorSpace colorSpace = lsRGB);
  static Color lerpColor(const Color &color1, const Color &color2, double t,
                         ColorSpace colorSpace = lsRGB);
  Color operator+(const Color &rhs) const;
  bool operator==(const Color &rhs) const; // Check equivalance to another color
  bool operator==(const char *s) const;    // Check equivalance to a c-string
  Color &operator=(const char *s);         // TODO: have a variable change which

  static std::vector<double> convertColor(ColorSpace from, ColorSpace to,
                                          const std::vector<double> &values);
};

#endif // COLOR_H
