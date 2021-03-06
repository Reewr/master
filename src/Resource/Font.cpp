#include "Font.hpp"

#include "../GLSL/Program.hpp"
#include "../Resource/Texture.hpp"
#include "../Utils/Utils.hpp"

int        Font::numFonts = 0;
FT_Library Font::fontLib;

Font::Glyph::Glyph() {}

Font::Glyph::Glyph(FT_GlyphSlot&    gs,
                   const mmm::vec2& offset,
                   const mmm::vec2& size) {

  advance.x = gs->advance.x >> 6;
  advance.y = gs->advance.y >> 6;

  bitmapSize.x = gs->bitmap.width;
  bitmapSize.y = gs->bitmap.rows;

  bitmapLoc.x = gs->bitmap_left;
  bitmapLoc.y = gs->bitmap_top;

  tc.x = offset.x / size.x;
  tc.y = offset.y / size.y;
}

Font::Page::Page() : texture(nullptr) {
  texture = new Texture();
}

Font::Page::~Page() {
  if (texture != nullptr) {
    delete texture;
    texture = nullptr;
  }
}

/**
 * @brief
 *   Creates a new font class. If this is the first
 *   instance of a Font it initializes the FreeType
 *   library and throws error if this does not work
 */
Font::Font() : Logging::Log("Font") {
  if (numFonts == 0) {
    if (FT_Init_FreeType(&fontLib))
      throw std::runtime_error("FreeType not initialized correctly");
    mLog->debug("Loaded FreeType");
  }

  numFonts++;
}

/**
 * @brief
 *   Desctructs a font, if all other fonts are destructed
 *   it will deinitialize the font library
 */
Font::~Font() {
  numFonts--;

  if (numFonts <= 0) {
    FT_Done_FreeType(fontLib);
    mLog->debug("Unloaded FreeType");
  }
}

/**
 * @brief
 *   Unloads the font from memory, clearing all
 *   of its loaded data.
 */
void Font::unload() {
  for (auto& page : mPages) {
    page.second.glyphs.clear();
    page.second.texture->unload();
  }

  mPages.clear();
}

/**
 * @brief
 *   Loads the font using font-size 12 as initial size
 *
 * @return
 */
bool Font::load(ResourceManager*) {
  return load(12);
}

/**
 * @brief
 *   Loads the font using the given font-size
 *
 * @param size
 *
 * @return
 */
bool Font::load(int size) {
  mLog->debug("Loading size: {}", std::to_string(size));

  if (mPages.count(size) > 0) {
    mLog->debug("Size already loaded: {}", std::to_string(size));
    return true;
  }

  if (FT_New_Face(fontLib, mFilename.c_str(), 0, &mFace))
    return false;

  mPages[size];
  Page& page = mPages[size];

  FT_Set_Char_Size(mFace, size * 64, 0, 96, 0);
  FT_GlyphSlot g = mFace->glyph;

  unsigned int rowW = 0;
  unsigned int rowH = 0;
  unsigned int w    = 0;
  unsigned int h    = 0;

  FT_ULong charCode;
  FT_UInt  glyphIndex;

  // goes through the different characters, loading them
  // into memory and figuring out the size of the texture
  // that should hold it
  charCode = FT_Get_First_Char(mFace, &glyphIndex);
  while (glyphIndex != 0) {
    if (charCode > 255) {
      w = mmm::max(w, rowW);
      h += rowH;
      rowW = 0;
      rowH = 0;
      break;
    }

    if (FT_Load_Char(mFace, charCode, FT_LOAD_RENDER)) {
      fprintf(stderr, "%s\n", "Cannot load character");
      continue;
    }

    if (rowW + g->bitmap.width + 1 >= 1024) {
      w = mmm::max(w, rowW);
      h += rowH;
      rowW = 0;
      rowH = 0;
    }

    rowW += g->bitmap.width + 1;
    rowH = mmm::max(rowH, g->bitmap.rows);

    charCode = FT_Get_Next_Char(mFace, charCode, &glyphIndex);
  }

  w = mmm::max(w, rowW);
  h += rowH;

  // Creates a texture with the size that was found out
  // in the previous loop
  Utils::getGLError();
  page.texture->createTexture(mmm::vec2(w, h), 0, GL_RED, GL_RED);
  page.texture->setFilename(filename() + ":" + std::to_string(size));
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  page.texture->clampToEdge();
  page.texture->linear();

  int offsetX  = 0;
  int offsetY  = 0;
  int maxWidth = 0;

  rowH = 0;

  // go through the same character again, but this time
  // use the newly created texture and add the characters
  // to that texture
  charCode = FT_Get_First_Char(mFace, &glyphIndex);
  Utils::getGLError("Before loading");
  while (glyphIndex != 0) {
    if (charCode > 255)
      break;

    if (FT_Load_Char(mFace, charCode, FT_LOAD_RENDER)) {
      fprintf(stderr, "%s\n", "Cannot load character");
      continue;
    }

    if (offsetX + g->bitmap.width + 1 >= 1024) {
      offsetY += rowH;
      rowH    = 0;
      offsetX = 0;
    }

    page.texture->changeSubTex(mmm::vec2(offsetX, offsetY),
                               mmm::vec2(g->bitmap.width, g->bitmap.rows),
                               g->bitmap.buffer);

    page.glyphs[charCode] =
      Font::Glyph(g, mmm::vec2(offsetX, offsetY), mmm::vec2(w, h));
    maxWidth = mmm::max(page.glyphs[charCode].bitmapSize.x, maxWidth);

    rowH = mmm::max(rowH, g->bitmap.rows);
    offsetX += g->bitmap.width + 1;

    charCode = FT_Get_Next_Char(mFace, charCode, &glyphIndex);
  }

  page.metrics = mmm::vec2(maxWidth, mFace->size->metrics.height >> 6);
  FT_Done_Face(mFace);
  return true;
}

/**
 * @brief
 *   Returns a glyph for a specific character 'c'
 *   that has the size 'size'. If the 'size' isnt
 *   loaded, it will be loaded
 *
 * @param c
 * @param size
 *
 * @return
 */
const Font::Glyph& Font::getGlyph(unsigned int c, int size) {
  if (mPages.count(size) == 0)
    load(size);

  return mPages[size].glyphs[c];
}

/**
 * @brief
 *   Returns the font metrics for a specific size
 *
 * @param size
 *
 * @return
 */
const mmm::vec2& Font::getMetrics(int size) {
  if (mPages.count(size) == 0)
    load(size);

  return mPages[size].metrics;
}

/**
 * @brief
 *   Returns a font for a specific size
 *
 * @param size
 *
 * @return
 */
Texture* Font::getTexture(int size) {
  if (mPages.count(size) == 0)
    load(size);

  return mPages[size].texture;
}
