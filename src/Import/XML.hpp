#pragma once

namespace tinyxml2 {
struct XMLDocument;
}

namespace Import {

struct XML {
  XML(const char* filePath);
  virtual ~XML();

  tinyxml2::XMLDocument* doc;
};
}