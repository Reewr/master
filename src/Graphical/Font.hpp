#ifndef GRAPHICAL_FONT_HPP
#define GRAPHICAL_FONT_HPP

#include <map>
#include <string>
#include <vector>

#include "../OpenGLHeaders.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../Math/Math.hpp"
#include "Texture.hpp"


class Program;


class Font {
public:
  //! Represents a character within the font.
  struct Glyph {
    Glyph();
    Glyph(FT_GlyphSlot& gs, const vec2& offset, const vec2& size);
    vec2 advance;
    vec2 bitmapSize;
    vec2 bitmapLoc;
    vec2 tc;
  };

  //! Only Constructor - Initializes FreeType & loads font from file
  Font(const std::string filename);

  //! Destructor
  //! If it is the last font object alive, it will deinitalize FreeType
  ~Font();

  //! Loads a font from file and converts it to a OpenGL texture
  void loadFromFile(const std::string& filename, int size);

  //! Gets a glyph, if it does not exists loads
  //! the entire bitmap for that size
  const Font::Glyph& getGlyph(char c, int size);

  //! Returns a texture size, if it does not exists loads
  //! the entire bitmap for that size
  const vec2& getTextureSize(int size);

  //! Returns the metric values (Line spacing etc) for character size
  const vec2& getMetrics(int size);

  //! Returns a texture, it it does not exists,
  //! loads the entire bitmap for that size
  Texture& getTexture(int size);

private:
  struct Page {
    Page();
    vec2    texSize;
    Texture texture;
    vec2    metrics;
    std::map<char, Glyph> glyphs;
  };

  std::map<unsigned int, Page> mPages;
  std::string mFilename;
  FT_Face     mFace;

  static int        numFonts;
  static FT_Library fontLib;
};

#endif