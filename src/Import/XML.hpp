#pragma once

namespace tinyxml2 {
  class XMLDocument;
}

namespace Import {

  struct XML {
    XML(const char* filePath);
    virtual ~XML();

    tinyxml2::XMLDocument* doc;
  };
}