
#include "Tools.hpp"

bool Tools::invalidChar(char c)
{
    return !(c >= 0 && c < 128);
}
void Tools::stripUnicode(std::string &str)
{
    str.erase(std::remove_if(str.begin(), str.end(), invalidChar), str.end());
}

std::string Tools::cleanString(std::string text)
{
    replace(text, "Á", "A");
    replace(text, "É", "E");
    replace(text, "Í", "I");
    replace(text, "Ó", "O");
    replace(text, "Ú", "U");
    replace(text, "Ñ", "N");

    replace(text, "á", "a");
    replace(text, "é", "e");
    replace(text, "í", "i");
    replace(text, "ó", "o");
    replace(text, "ú", "u");
    replace(text, "ñ", "n");

    replace(text, "Ä", "A");
    replace(text, "Ë", "E");
    replace(text, "Ï", "I");
    replace(text, "Ö", "O");
    replace(text, "Ü", "U");

    replace(text, "ä", "a");
    replace(text, "ë", "e");
    replace(text, "ï", "i");
    replace(text, "ö", "o");
    replace(text, "ü", "u");

    replace(text, "À", "A");
    replace(text, "È", "E");
    replace(text, "Ì", "I");
    replace(text, "Ò", "O");
    replace(text, "Ù", "U");

    replace(text, "à", "a");
    replace(text, "è", "e");
    replace(text, "ì", "i");
    replace(text, "ò", "o");
    replace(text, "ù", "u");

    stripUnicode(text);
    return text;
}

std::string Tools::intToDateString(int64_t myInt)
{
    // OATPP_LOGI("servicio Tools", "Entrando a intToDateString %f", myInt);

    std::int64_t time_date_stamp = (myInt / 1000);
    std::time_t temp = time_date_stamp;
    std::tm *t = std::gmtime(&temp);
    std::stringstream ss; // or if you're going to print, just input directly into the output stream
    ss << std::put_time(t, "%d.%m.%Y %H:%M:%S");
    std::string stringDate = ss.str();

    // OATPP_LOGI("servicio Tools", "Saliendo de intToDateString");
    return stringDate;
}

bool Tools::findRegex(std::string regex, std::string textToFind, std::string text)
{
    text = Tools::removeMultipleSpace(text);
    std::smatch m;
    std::regex e(regex);
    std::string strIt = text;
    bool res = false;
    while (std::regex_search(strIt, m, e))
    {

        for (auto x : m)
        {
            if (Tools::trim(x) == textToFind)
            {
                res = true;
                break;
            }
            auto parts = Tools::explode(x, ' ');
            if (parts.size() == 2 && ((Tools::trim(parts[1]) == textToFind) || (Tools::trim(parts[0]) == textToFind)))
            {
                res = true;
                break;
            }
        }
        strIt = m.suffix().str();
    }
    return res;
}

bool Tools::fileExists(const std::string &name)
{
    std::ifstream f(name.c_str());
    return f.good();
}

std::string Tools::replace(std::string subject, const std::string &search,
                           const std::string &replace)
{
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos)
    {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return subject;
}

void Tools::sleep(int sec)
{
    std::this_thread::sleep_for(std::chrono::seconds(sec));
}

std::string Tools::fillNumber(double n, int precision, int with)
{
    double temp;
    std::stringstream stream;
    if (modf(n, &temp) != 0 && precision == 0)
    {
        stream << std::setw(with) << std::fixed << std::setprecision(2) << n;
    }
    else
    {
        stream << std::setw(with) << std::fixed << std::setprecision(precision) << n;
    }
    std::string s = stream.str();
    std::replace(s.begin(), s.end(), '.', ',');
    return s;
}

std::string Tools::fillString(std::string cadena, unsigned int length, char caracterParaCompletar, bool fillLeft)
{
    std::string respuesta;
    if (cadena.length() > length)
    {
        respuesta = cadena;
    }
    else
    {
        respuesta = fillLeft ? std::string(length - cadena.size(), caracterParaCompletar) + cadena : cadena + std::string(length - cadena.size(), caracterParaCompletar);
    }
    return respuesta;
}

std::string Tools::getDateNow()
{
    std::time_t rawtime;
    std::tm *timeinfo;
    char buffer[20];

    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);

    std::strftime(buffer, 20, "%d/%m/%Y", timeinfo);
    return buffer;
}

std::string Tools::getTimeNow()
{
    std::time_t rawtime;
    std::tm *timeinfo;
    char buffer[20];

    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);

    std::strftime(buffer, 20, "%I:%M %p", timeinfo);
    return buffer;
}

bool Tools::BothAreSpaces(char lhs, char rhs) { return (lhs == rhs) && (lhs == ' '); }
std::string Tools::removeMultipleSpace(std::string str)
{

    std::string::iterator new_end = std::unique(str.begin(), str.end(), BothAreSpaces);
    str.erase(new_end, str.end());

    return str;
}

std::string Tools::replaceSubstring(std::string find, std::string replace, std::string var)
{
    std::string res = var;
    res = std::regex_replace(res, std::regex(find), replace);
    OATPP_LOGD("", ".%s. .%s.", var.c_str(), res.c_str());
    return res;
}

void Tools::printDTO(const oatpp::data::mapping::type::Void &variant)
{
    auto serializeConfig = oatpp::parser::json::mapping::Serializer::Config::createShared();
    auto deserializeConfig = oatpp::parser::json::mapping::Deserializer::Config::createShared();

    /* enable beautifier */
    serializeConfig->useBeautifier = true;

    /* create json object mapper with serializer config */
    auto jsonObjectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared(serializeConfig, deserializeConfig);
    oatpp::String json = jsonObjectMapper->writeToString(variant);
    OATPP_LOGD("", "Request Obj: \n%s", json.getValue("").c_str());
}

std::string Tools::dtoToString(const oatpp::data::mapping::type::Void &variant)
{
  auto jsonObjectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
  oatpp::String json = jsonObjectMapper->writeToString(variant);
  return json;
}

std::vector<std::string> Tools::explode(std::string const &s, char delim)
{
    std::vector<std::string> result;
    std::istringstream iss(s);

    for (std::string token; std::getline(iss, token, delim);)
    {
        result.push_back(std::move(token));
    }
    if (result.empty())
        result.push_back(s);

    return result;
}

std::string Tools::ltrim(std::string s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
                                    { return !std::isspace(ch); }));
    return s;
}

std::string Tools::rtrim(std::string s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
                         { return !std::isspace(ch); })
                .base(),
            s.end());
    return s;
}

std::string Tools::trim(std::string s)
{
    return ltrim(rtrim(s));
}

std::string Tools::string_to_upper(std::string &string)
{
    for (auto &c : string)
        c = toupper(c);

    return string;
}
