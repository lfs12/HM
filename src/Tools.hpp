#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <iostream>
#include <regex>
#include <sstream>
#include <chrono>
#include <ctime>
#include <thread>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <functional>
#include <iostream>
#include <map>

#include "nlohmann/json.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

using json = nlohmann::json;

class Tools
{
private:
    static bool BothAreSpaces(char lhs, char rhs);

public:
    static void stripUnicode(std::string &str);
    static bool invalidChar(char c);
    static std::string cleanString(std::string text);
    static std::string intToDateString(int64_t myInt);
    static bool findRegex(std::string regex, std::string tagToFind, std::string text);
    static bool fileExists(const std::string &name);
    static std::string replace(std::string subject, const std::string &search, const std::string &replace);
    static std::string fillNumber(double n, int precision = 2, int width = 4);
    static std::string fillString(std::string cadena, unsigned int length, char caracterParaCompletar = ' ', bool fillLeft = false);
    static std::string removeMultipleSpace(std::string in);
    static std::string replaceSubstring(std::string find, std::string replace, std::string var);
    static void printDTO(const oatpp::data::mapping::type::Void &variant);
    static std::vector<std::string> explode(std::string const &s, char delim);
    static std::string ltrim(std::string s);
    static std::string rtrim(std::string s);
    static std::string trim(std::string s);
    static std::string string_to_upper(std::string &string);
    static std::string dtoToString(const oatpp::data::mapping::type::Void &variant);

    static void sleep(int sec);
    static std::string getDateNow();
    static std::string getTimeNow();
    inline std::tm localtime_xp(std::time_t timer);
};

#endif // TOOLS_HPP
