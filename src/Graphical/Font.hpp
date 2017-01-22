#pragma once

#include <map>
#include <string>
#include <vector>

#include "../OpenGLHeaders.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../Math/Math.hpp"
#include "Texture.hpp"
#include "../Resource/Resource.hpp"


class Program;


class Font : public Resource {
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

  Font();

  //! Destructor
  //! If it is the last font object alive, it will deinitalize FreeType
  ~Font();

  // Default handler, calls load with 12
  bool load();

  //! Loads a font from file and converts it to a OpenGL texture
  bool load(int size);

  // unloads all the resources
  void unload();

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
  FT_Face     mFace;

  static int        numFonts;
  static FT_Library fontLib;
};