#include "Text3D.hpp"

#include "../Utils/Asset.hpp"
#include "../Utils/CFG.hpp"
#include "../Camera/Camera.hpp"
#include "../Resource/ResourceManager.hpp"
#include "../Resource/Texture.hpp"
#include "../Resource/Font.hpp"
#include "../GLSL/Program.hpp"
#include "../Utils/Utils.hpp"

Text3D::Text3D(const std::string& font,
       const std::string& text,
       const mmm::vec3&   position)
  : Text(font, text, mmm::vec2(0), 300)
  , mPosition(position) {
    mFont3DProgram = mAsset->rManager()->get<Program>("Program::Font3D");
  }

/**
 * @brief
 *   Returns the size of the text in normalized coordinates between -1 and 1.
 *
 *   Since the text size is in pixels on the screen, it has to be normalized
 *   into a value between -1 and 1 where the maxmimum screen resolution
 *   is used.
 *
 * @return
 */
mmm::vec2 Text3D::getNormalizedSize() {
  mmm::vec2 halfScreenRes = mAsset->cfg()->graphics.res * 0.5;
  return (size() - halfScreenRes) / halfScreenRes;
}

/**
 * @brief
 *   Draws the 3D text
 */
void Text3D::draw() {
  Camera* camera        = mAsset->camera();
  const mmm::mat4& proj = camera->projection();
  const mmm::mat4& view = camera->view();

  mmm::vec3 cameraRight = {view[0][0], view[0][1], view[0][2]};
  mmm::vec3 cameraUp    = {view[1][0], view[1][1], view[1][2]};

  Utils::assertGL();
  mFont3DProgram->bind();
  Utils::assertGL();
  mFont3DProgram->setUniform("MVP", proj * view * mmm::mat4::identity);
  Utils::assertGL();
  mFont3DProgram->setUniform("cameraRight", cameraRight);
  Utils::assertGL();
  mFont3DProgram->setUniform("cameraUp", cameraUp);
  Utils::assertGL();
  mFont3DProgram->setUniform("worldPosition", mPosition);
  // mFont3DProgram->setUniform("size", getNormalizedSize());

  Utils::assertGL();
  if (mHasBackgroundColor) {
    mFont3DProgram->setUniform("isBackground", true);

  Utils::assertGL();
    glBindVertexArray(mVAOBackground);
  Utils::assertGL();
    glDrawArrays(GL_TRIANGLES, 0, mNumVerticesBackground);
  Utils::assertGL();
    glBindVertexArray(0);
  Utils::assertGL();
  }

  Utils::assertGL();
  mTextFont->getTexture(mCharacterSize)->bind(0);
  Utils::assertGL();
  mFont3DProgram->setUniform("isBackground", false);
  Utils::assertGL();

  Utils::assertGL();
  glBindVertexArray(mVAO);
  Utils::assertGL();
  glDrawArrays(GL_TRIANGLES, 0, mNumVertices);
  Utils::assertGL();
  glBindVertexArray(0);
  Utils::assertGL();
}
