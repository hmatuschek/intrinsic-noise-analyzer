#include "ina_cli_savetable.hh"

#include <fstream>
#include <utils/matexport.hh>
#include <utils/logger.hh>

using namespace iNA;
using namespace iNA::Utils;

int saveTableCSV(std::ostream &file, const std::string &header, Eigen::MatrixXd &table);
int saveTableMAT(std::ostream &file, const std::string &header, Eigen::MatrixXd &table);

int saveTable(const std::string &header, Eigen::MatrixXd &table, iNA::Utils::Opt::Parser &option_parser)
{
  std::string filename;
  if (option_parser.has_option("output-csv"))
  {
    filename = option_parser.get_option("output-csv").front();
    std::fstream file(filename.c_str(), std::ios_base::out);
    if (! file.is_open()) {
      Message message = LOG_MESSAGE(Message::ERROR);
      message << "Can not open file: " << filename;
      Logger::get().log(message);
      return -1;
    }
    return -1; //saveTableCSV(file, header, table);
  }

  if (option_parser.has_option("output-mat"))
  {
    filename = option_parser.get_option("output-mat").front();
    std::fstream file(filename.c_str(), std::ios_base::out);
    if (! file.is_open()) {
      Message message = LOG_MESSAGE(Message::ERROR);
      message << "Can not open file: " << filename;
      Logger::get().log(message);
      return -1;
    }
    return -1; //saveTableMAT(file, header, table);
  }

  if (option_parser.has_option("output"))
  {
    /// @bug Implement!
    return -1;
  }
  return -1;
}


