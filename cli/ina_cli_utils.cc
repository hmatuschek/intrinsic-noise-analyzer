#include "ina_cli_utils.hh"
#include <string>
#include <utils/logger.hh>
#include <cstdlib>


using namespace iNA;



bool ina_cli_string2Double(const std::string &string, double &value) {
  const char *ptr = string.c_str();  char *endPtr = (char *)ptr;
  value = strtod(ptr, &endPtr);
  if (0 == value && ptr==endPtr) {
    return false;
  }
  return true;
}

bool ina_cli_string2Int(const std::string &string, long int &value) {
  const char *ptr = string.c_str();  char *endPtr = (char *)ptr;
  value = strtol(ptr, &endPtr, 10);
  if (0 == value && ptr==endPtr) {
    return false;
  }
  return true;
}


bool ina_cli_parseRange(const std::string &range, double &from, double &to, size_t &steps)
{
  bool success=true;
  size_t idx_1 = range.find_first_of(':');
  if (idx_1 == std::string::npos) {
    Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
    message << "Invalid reange format in '" << range << "'. Must be FROM:TO[:STEPS]";
    Utils::Logger::get().log(message);
    return false;
  }
  success = success && ina_cli_string2Double(range.substr(0, idx_1), from);
  size_t idx_2 = range.find_first_of(':', idx_1+1);
  if (idx_2 != std::string::npos) {
    success = success && ina_cli_string2Double(range.substr(idx_1+1, idx_2-1-idx_1), to);
    long int tmp;
    success = success && ina_cli_string2Int(range.substr(idx_2+1), tmp); steps = tmp;
    if (0 > tmp) { success = false; }
  } else {
    long int tmp;
    success = success && ina_cli_string2Int(range.substr(idx_1+1), tmp); steps = tmp;
    if (0 > tmp) { success = false; }
  }
  return success;
}
