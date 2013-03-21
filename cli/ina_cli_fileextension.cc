#include "ina_cli_fileextension.hh"


std::string getFileExtension(const std::string &path) {
  size_t idx = path.find_last_of('.');
  if (idx == std::string::npos) { return ""; }
  return path.substr(idx+1);
}


