#include <fstream>
#include <sstream>
#include <vector>


#ifndef CONFIG_H
#define CONFIG_H

namespace configReader{
  void readConfig(std::istream& co, std::vector<std::pair<std::pair<std::string, std::string>, std::string> >& ans, bool lower=1);
  void readConfig(std::string co,  std::vector<std::pair<std::pair<std::string, std::string>, std::string> >& ans, bool lower=1);
  void readConfigFromFile(const char* fname,  std::vector<std::pair<std::pair<std::string, std::string>, std::string> >& ans, bool lower=1);
}

#endif