#include "Font.hpp"

#include "../GLSL/Program.hpp"
#include "../Utils/Utils.hpp"

int        Font::numFonts = 0;
FT_Library Font::fontLib;

Font::Glyph::Glyph() {}

Font::Glyph::Glyph(FT_GlyphSlot& gs, const vec2& offset, const vec2& size) {

  advance.x = gs->advance.x >> 6;
  advance.y = gs->advance.y >> 6;

  bitmapSize.x = gs->bitmap.width;
  bitmapSize.y = gs->bitmap.rows;

  bitmapLoc.x = gs->bitmap_left;
  bitmapLoc.y = gs->bitmap_top;

  tc.x = offset.x / size.x;
  tc.y = offset.y / size.y;
}

Font::Page::Page() {}

/**
 * @brief
 *   Creates a new font class. If this is the first
 *   instance of a Font it initializes the FreeType
 *   library and throws error if this does not work
 *
 * @param filename
 */
Font::Font(const std::string filename) {
  if (numFonts == 0 && FT_Init_FreeType(&fontLib))
    throw Error("FreeType not initialized correctly");

  numFonts++;
  mFilename = filename;
}

/**
 * @brief
 *   Desctructs a font, if all other fonts are destructed
 *   it will deinitialize the font library
 */
Font::~Font() {
  numFonts--;

  if (numFonts <= 0)
    FT_Done_FreeType(fontLib);

  for (auto page : mPages)
    page.second.glyphs.clear();

  mPages.clear();
}

/**
 * @brief
 *   Loads a font from filename
 *
 * @param filename
 * @param size
 */
void Font::loadFromFile(const std::string& filename, int size) {
  if (mPages.count(size) > 0)
    return;
  if (FT_New_Face(fontLib, filename.c_str(), 0, &mFace))
    return;

  mFilename = filename;
  mPages[size];
  Page& page = mPages[size];

  FT_Set_Pixel_Sizes(mFace, 0, size);
  FT_GlyphSlot g = mFace->glyph;

  int rowW = 0;
  int rowH = 0;
  int w    = 0;
  int h    = 0;

  // goes through the different characters, loading them
  // into memory and figuring out the size of the texture
  // that should hold it
  for (int i = 32; i < 128; i++) {
    if (FT_Load_Char(mFace, i, FT_LOAD_RENDER)) {
      fprintf(stderr, "%s\n", "Cannot load character");
      continue;
    }

    if (rowW + g->bitmap.width + 1 >= 1024) {
      w = std::max(w, rowW);
      h += rowH;
      rowW = 0;
      rowH = 0;
    }

    rowW += g->bitmap.width + 1;
    rowH = std::max(rowH, g->bitmap.rows);
  }

  w = std::max(w, rowW);
  h += rowH;

  // Creates a texture with the size that was found out
  // in the previous loop
  Utils::getGLError();
  page.texture.createTexture(vec2(w, h), 0, GL_RED, GL_RED);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  page.texture.clampToEdge().linear();

  int offsetX = 0;
  int offsetY = 0;

  rowH = 0;

  // go through the same character again, but this time
  // use the newly created texture and add the characters
  // to that texture
  for (int i = 32; i < 128; i++) {
    if (FT_Load_Char(mFace, i, FT_LOAD_RENDER))
      continue;

    if (offsetX + g->bitmap.width + 1 >= 1024) {
      offsetY += rowH;
      rowH    = 0;
      offsetX = 0;
    }

    page.texture.changeSubTex(vec2(offsetX, offsetY),
                              vec2(g->bitmap.width, g->bitmap.rows),
                              g->bitmap.buffer);

    page.glyphs[i] = Font::Glyph(g, vec2(offsetX, offsetY), vec2(w, h));

    rowH = std::max(rowH, g->bitmap.rows);
    offsetX += g->bitmap.width + 1;
  }

  page.texSize = vec2(w, h);
  page.metrics = vec2(0, mFace->size->metrics.height >> 6);
  FT_Done_Face(mFace);
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
const Font::Glyph& Font::getGlyph(char c, int size) {
  if (mPages.count(size) == 0)
    loadFromFile(mFilename, size);

  return mPages[size].glyphs[c];
}

/**
 * @brief
 *   Gets the texture size of a font with a specific size
 *
 * @param size
 *
 * @return
 */
const vec2& Font::getTextureSize(int size) {
  if (mPages.count(size) == 0)
    loadFromFile(mFilename, size);

  return mPages[size].texSize;
}

/**
 * @brief
 *   Returns the font metrics for a specific size
 *
 * @param size
 *
 * @return
 */
const vec2& Font::getMetrics(int size) {
  if (mPages.count(size) == 0)
    loadFromFile(mFilename, size);

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
Texture& Font::getTexture(int size) {
  if (mPages.count(size) == 0)
    loadFromFile(mFilename, size);

  return mPages[size].texture;
}
