#ifndef INA_CLI_SAVETABLE_H
#define INA_CLI_SAVETABLE_H

#include <eigen3/Eigen/Eigen>
#include <utils/option_parser.hh>

int saveTable(const std::string &header, Eigen::MatrixXd &table, iNA::Utils::Opt::Parser &option_parser);

#endif // INA_CLI_SAVETABLE_H
