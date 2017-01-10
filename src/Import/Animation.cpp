#include "Animation.hpp"

#include <string>
#include <vector>

#include <tinyxml2.h>

#include "../Utils/Utils.hpp"
#include "XML.hpp"

std::vector<std::string> split (std::string s, std::string delims) {
  std::vector<std::string> ws = {""};

  for (const auto& c: s) {
    if (delims.find(c) != std::string::npos) {
      if (ws.back() == "") continue;
      ws.push_back("");
    } else {
      ws.back() += c;
    }
  }

  if (ws.back() == "")
    ws.pop_back();

  return ws;
}

std::vector<float> map_to_float (std::vector<std::string> ws) {
  std::vector<float> fs;
  fs.resize(ws.size());

  for (unsigned int i = 0; i < ws.size(); i++)
    fs[i] = atof(ws[i].c_str());

  return fs;
}

Import::Animation::Animation (const char* filePath) : XML(filePath) {
  Error err = "Error while parsing '" + std::string(filePath) + "'.";
  int success = tinyxml2::XMLError::XML_SUCCESS;

  tinyxml2::XMLElement* root = this->doc->RootElement();

  if (root->QueryIntAttribute("frame_size", &n) != success) {
    fatalError("missing 'frame_size' attribute in anim file");
    throw err;
  }
  if (root->QueryIntAttribute("frames", &l) != success) {
    fatalError("missing 'frames' attribute in anim file");
    throw err;
  }

  fs.resize(n*l + l);
  float* ptr = &fs[0];

  tinyxml2::XMLElement* frame = root->FirstChildElement("frame");
  for (int i = 0; i < l; ++i) {
    if (frame == NULL) {
      fatalError("inconsistent or bad data in anim file");
      throw err;
    }

    float t = 0;
    if (frame->QueryFloatAttribute ("time", &t) != success) {
      fatalError("missing or malformed time attribute in anim file");
      throw err;
    }
    *ptr = t;
    ptr++;

    std::string s = frame->GetText();
    std::vector<float> f = map_to_float(split(s, " \n\t"));

    for (int j = 0; j < n; ++j) {
      if (f.size() <= (size_t)j) {
        fatalError("incomplete frame data in anim file");
        throw err;
      }

      *ptr = f[j];
      ptr++;
    }

    if (f.size() > (size_t)n)
      warning ("excessive frame data in '",filePath,"'");

    frame = frame->NextSiblingElement("frame");
  }
}
