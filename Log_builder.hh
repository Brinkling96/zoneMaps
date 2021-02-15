#include <string> 
#include <fstream>

#ifndef LOG_BUILDER_HH
#define LOG_BUILDER_HH

int addHeaderToLog(std::ofstream* file, std::string fileName, std::string input_file, std::string test_case);
std::string buildDataline(std::string input_file);

#endif

