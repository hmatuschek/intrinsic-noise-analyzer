#include "ina_cli_savetable.hh"
#include "ina_cli_fileextension.hh"

#include <fstream>
#include <utils/matexport.hh>
#include <utils/logger.hh>

using namespace iNA;
using namespace iNA::Utils;


int saveTableCSV(std::ostream &file, const std::string &header, Eigen::MatrixXd &table);
int saveTableMAT(std::ostream &file, const std::string &header,
                 const std::string &table_name, Eigen::MatrixXd &table);


int saveTable(const std::string &header, const std::string &table_name,
              Eigen::MatrixXd &table, iNA::Utils::Opt::Parser &option_parser)
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
    return saveTableCSV(file, header, table);
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
    return saveTableMAT(file, header, table_name, table);
  }

  if (option_parser.has_flag("output-stdout")) {
    return saveTableCSV(std::cout, header, table);
  }

  if (option_parser.has_option("output"))
  {
    filename = option_parser.get_option("output").front();
    std::string extension = getFileExtension(filename);
    if ("csv" == extension) {
      std::fstream file(filename.c_str(), std::ios_base::out);
      if (! file.is_open()) {
        Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
        message << "Can not open file: " << filename;
        Utils::Logger::get().log(message);
        return -1;
      }
      return saveTableCSV(file, header, table);
    } else if ("mat" == extension) {
      std::fstream file(filename.c_str(), std::ios_base::out);
      if (! file.is_open()) {
        Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
        message << "Can not open file: " << filename;
        Utils::Logger::get().log(message);
        return -1;
      }
      return saveTableMAT(file, header, table_name, table);
    }
    Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
    message << "Unrecognized file extension: " << extension;
    Utils::Logger::get().log(message);
    return -1;
  }

  // This should not happen:
  Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
  message << "Internal error: Unknown output command!";
  Utils::Logger::get().log(message);
  return -1;
}


int saveTableCSV(std::ostream &file, const std::string &header, Eigen::MatrixXd &table)
{
  file << header;
  for (int i=0; i<table.rows(); i++) {
    file << table(i,0);
    for (int j=1; j<table.cols(); j++) {
      file << "\t" << table(i,j);
    }
    file << std::endl;
  }

  return 0;
}


int saveTableMAT(std::ostream &file, const std::string &header,
                 const std::string &table_name, Eigen::MatrixXd &table)
{
  MatFile matfile;
  matfile.add(header);
  matfile.add(table_name, table);
  matfile.serialize(file);
  return 0;
}

