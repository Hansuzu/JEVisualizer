#include <fstream>
#include <sstream>
#include <vector>

namespace configReader{
  void readConfig(std::istream& co, std::vector<std::pair<std::pair<std::string, std::string>, std::string> >& ans);
  void readConfig(std::string co,  std::vector<std::pair<std::pair<std::string, std::string>, std::string> >& ans);
  void readConfigFromFile(const char* fname,  std::vector<std::pair<std::pair<std::string, std::string>, std::string> >& ans);
}