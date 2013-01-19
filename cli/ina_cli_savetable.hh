#ifndef INA_CLI_SAVETABLE_H
#define INA_CLI_SAVETABLE_H

#include <eigen3/Eigen/Eigen>
#include <utils/option_parser.hh>
#include <string>
#include <vector>

int saveTable(const std::string &header, const std::string &table_name,
              Eigen::MatrixXd &table, iNA::Utils::Opt::Parser &option_parser);

int saveTable(const std::string &header, const std::vector<std::string> &columns,
              const std::string &table_name, Eigen::MatrixXd &table,
              iNA::Utils::Opt::Parser &option_parser);

#endif // INA_CLI_SAVETABLE_H
