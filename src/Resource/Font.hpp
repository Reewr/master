#pragma once

#include <map>

#include "../OpenGLHeaders.hpp"
#include <mmm.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../Resource/Resource.hpp"

class Program;
class Texture;

class Font : public Resource {
public:
  //! Represents a character within the font.
  struct Glyph {
    Glyph();
    Glyph(FT_GlyphSlot& gs, const mmm::vec2& offset, const mmm::vec2& size);
    mmm::vec2 advance;
    mmm::vec2 bitmapSize;
    mmm::vec2 bitmapLoc;
    mmm::vec2 tc;
  };

  Font();

  //! Destructor
  //! If it is the last font object alive, it will deinitalize FreeType
  ~Font();

  // Default handler, calls load with 12
  bool load(ResourceManager*);

  //! Loads a font from file and converts it to a OpenGL texture
  bool load(int size);

  // unloads all the resources
  void unload();

  //! Gets a glyph, if it does not exists loads
  //! the entire bitmap for that size
  const Font::Glyph& getGlyph(unsigned int c, int size);

  //! Returns the metric values (Line spacing etc) for character size
  const mmm::vec2& getMetrics(int size);

  //! Returns a texture, it it does not exists,
  //! loads the entire bitmap for that size
  Texture* getTexture(int size);

private:
  struct Page {
    Page();
    ~Page();

    Texture*  texture;
    mmm::vec2 metrics;
    std::map<unsigned int, Glyph> glyphs;

  private:
    Page(const Page& p);
    Page& operator=(const Page&);
  };

  std::map<unsigned int, Page> mPages;
  FT_Face mFace;

  static int        numFonts;
  static FT_Library fontLib;
};
