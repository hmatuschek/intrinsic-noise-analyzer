#ifndef INA_CLI_UTILS_HH
#define INA_CLI_UTILS_HH

#include <string>

/** Translates a string to double value. */
bool ina_cli_string2Double(const std::string &string, double &value);

/** Translates a string to integer value. */
bool ina_cli_string2Int(const std::string &string, long int &value);

/** Parses a range specifier given as a string.
 * FROM:TO[:STEPS], Where FROM & TO are floating point numbers and STEPS is an integer. */
bool ina_cli_parseRange(const std::string &range, double &from, double &to, size_t &steps);


#endif // INA_CLI_UTILS_HH
