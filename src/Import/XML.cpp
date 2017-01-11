#include "XML.hpp"

#include <sstream>
#include <tinyxml2.h>

#include "../Utils/Utils.hpp"

Import::XML::XML(const char* filePath) {
  doc = new tinyxml2::XMLDocument();
  doc->LoadFile(filePath);

  if (doc->Error()) {
    std::stringstream ss;

    ss << "unable to pares or open '" << filePath << "'" << std::endl;

    if (doc->GetErrorStr1() != NULL)
      ss << "XMLDocument error: " << doc->GetErrorStr1() << std::endl;
    if (doc->GetErrorStr2() != NULL)
      ss << "XMLDocument error: " << doc->GetErrorStr2() << std::endl;

    throw Error(ss.str().c_str());
  }
}

Import::XML::~XML() {
  delete doc;
}
