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
  : Text(font, text, mmm::vec2(0), 300) {
    mFont3DProgram = mAsset->rManager()->get<Program>("Program::Font3D");
    setPosition(position);
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
 *   Sets the position but adjusts it so that the text is aligned
 *   to the middle of the coordinate.
 *
 * @param position
 */
void Text3D::setPosition(const mmm::vec3& position) {
  mPosition = mmm::vec3(position.xy - getNormalizedSize() / 2, position.z);
}

/**
 * @brief
 *   Draws the 3D text
 */
void Text3D::draw(mmm::vec3 offset) {
  Camera* camera        = mAsset->camera();
  const mmm::mat4& proj = camera->projection();
  const mmm::mat4& view = camera->view();

  mmm::vec3 cameraRight = {view[0][0], view[0][1], view[0][2]};
  mmm::vec3 cameraUp    = {view[1][0], view[1][1], view[1][2]};

  mFont3DProgram->bind();
  mFont3DProgram->setUniform("MVP", proj * view * mmm::mat4::identity);
  mFont3DProgram->setUniform("cameraRight", cameraRight);
  mFont3DProgram->setUniform("cameraUp", cameraUp);
  mFont3DProgram->setUniform("worldPosition", mPosition + offset);

  if (mHasBackgroundColor) {
    mFont3DProgram->setUniform("isBackground", true);

    glBindVertexArray(mVAOBackground);
    glDrawArrays(GL_TRIANGLES, 0, mNumVerticesBackground);
    glBindVertexArray(0);
  }

  mTextFont->getTexture(mCharacterSize)->bind(0);
  mFont3DProgram->setUniform("isBackground", false);

  glBindVertexArray(mVAO);
  glDrawArrays(GL_TRIANGLES, 0, mNumVertices);
  glBindVertexArray(0);
}
