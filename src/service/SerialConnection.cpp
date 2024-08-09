#include "SerialConnection.hpp"

#include "lib/rs232.hpp"
#include "dto/PrinterDto.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <iomanip>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>

#include "oatpp/network/Server.hpp"
#include "oatpp/core/base/Environment.hpp"
#include "oatpp/core/Types.hpp"

SerialConnection::SerialConnection(std::string cport, std::string mode, int bdrate)
{
    this->mode = mode;
    this->cport_nr = RS232_GetPortnr(cport.c_str());
    this->bdrate = bdrate;
}

bool SerialConnection::OpenPort()
{
    bool result = true;
    result = RS232_OpenComport(this->cport_nr, this->bdrate, this->mode.c_str(), 0);

    return !result;
}

void SerialConnection::closePort()
{
    RS232_CloseComport(this->cport_nr);
}

void SerialConnection::writePort(std::string msg)
{
    OATPP_LOGD("SerialConnection", "writePort-> %s", msg.c_str());
    RS232_cputs(this->cport_nr, msg);
}

std::string SerialConnection::readPort()
{
    std::string bytes;

    memset(this->buf, 0, 4095);
    int n = RS232_PollComport(this->cport_nr, this->buf, 4095);

    if (n > 0)
    {
        OATPP_LOGD("", "received %i bytes: %s", n, (char *)buf);
        bytes += std::string(buf, buf + n);
    }

    return bytes;
}

std::string SerialConnection::ascii_to_hex(const std::string &input)
{
    static const char hex_digits[] = "0123456789ABCDEF";

    std::string output;
    output.reserve(input.length() * 2);
    for (unsigned char c : input)
    {
        output.push_back(hex_digits[c >> 4]);
        output.push_back(hex_digits[c & 15]);
    }
    return output;
}

std::string SerialConnection::int_to_hex(int numb)
{
    std::stringstream ss;
    ss << std::hex << numb;
    std::string result(ss.str());

    string_to_upper(result);

    return result;
}

std::string SerialConnection::hex2ascii(const std::string in)
{
    int len = in.length();
    std::string out;
    for (int i = 0; i < len; i += 2)
    {
        std::string byte = in.substr(i, 2);
        char chr = (char)(int)strtol(byte.c_str(), nullptr, 16);
        out.push_back(chr);
    }
    return out;
}

std::string SerialConnection::string_to_upper(std::string &string)
{
    for (auto &c : string)
        c = toupper(c);

    return string;
}

std::string SerialConnection::getSerialData(std::string rawData, std::string begin, std::string end)
{
    begin = (begin.empty()) ? this->hex2ascii("02") : begin;
    end = (end.empty()) ? this->hex2ascii("03") : end;

    auto f1 = rawData.find(begin);
    auto f2 = rawData.find(end);

    OATPP_ASSERT_HTTP(f1 != std::string::npos, Status::CODE_400, "TX: Error inicio de comando no encontrado");
    OATPP_ASSERT_HTTP(f2 != std::string::npos, Status::CODE_400, "TX: Error fin de comando no encontrado");
    // OATPP_LOGD("getSerialData", "%s", rawData.substr(f1, f2 - 1).c_str());
    return rawData.substr(f1, f2);
}

std::string SerialConnection::fillNumbToString(double num, int length, int n_decimal, bool replacePunto)
{
    std::ostringstream stream;
    std::string str;

    stream << std::fixed;
    stream << std::setprecision(n_decimal);
    stream << num;

    str = stream.str();
    if (replacePunto)
        std::replace(str.begin(), str.end(), '.', ','); // replace all 'x' to 'y'
    else
        str.erase(std::remove(str.begin(), str.end(), '.'), str.end());

    stream.str("");
    stream << std::setw(length) << std::setfill('0') << str;

    return stream.str();
}

std::string SerialConnection::fillNumbToString(int num, int length)
{
    std::ostringstream stream;
    std::string str;

    stream << num;

    str = stream.str();
    str.erase(std::remove(str.begin(), str.end(), '.'), str.end());

    stream.str("");
    stream << std::setw(length) << std::setfill('0') << str;

    return stream.str();
}

std::string SerialConnection::fillString(std::string cadena, unsigned int length, char caracterParaCompletar)
{
    std::string respuesta;

    if (cadena.length() > length)
    {
        respuesta = cadena.substr(0, length);
    }
    else
    {
        respuesta = cadena + std::string(length - cadena.size(), caracterParaCompletar);
    }
    return respuesta;
}

std::string SerialConnection::ltrim(std::string s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
                                    { return !std::isspace(ch); }));
    return s;
}

std::string SerialConnection::rtrim(std::string s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
                         { return !std::isspace(ch); })
                .base(),
            s.end());
    return s;
}

std::string SerialConnection::trim(std::string s)
{
    return ltrim(rtrim(s));
}

std::vector<std::string> SerialConnection::explode(std::string const &s, char delim)
{
    std::vector<std::string> result;
    std::istringstream iss(s);

    for (std::string token; std::getline(iss, token, delim);)
    {
        result.push_back(std::move(token));
    }

    return result;
}

void SerialConnection::sleep(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

std::string SerialConnection::numbToString(double numb)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << numb;
    std::string s = stream.str();
    std::replace(s.begin(), s.end(), '.', ','); // replace all 'x' to 'y'

    return s;
}

double SerialConnection::round2decimals(double numero)
{
    numero *= 100;
    numero = round(numero);
    numero /= 100;

    return numero;
}

void SerialConnection::writeLog(std::string comando, std::string respuesta, std::string fileName)
{
    try
    {
        std::ofstream file;
        file.open("log" + fileName + ".txt", std::ios::out | std::ios::app | std::ios::binary);
        file << "\n\nTIMESTAMP: " + getDateNow() + " " + getTimeNow();
        file << "\nTX=" + comando;
        file << "\n\tRX=" + respuesta;
        file.close();
    }
    catch (...)
    {
        OATPP_LOGE("writeLog", "ERROR writing log%s", fileName.c_str());
    }
}

std::string SerialConnection::getDateNow()
{
    std::time_t rawtime;
    std::tm *timeinfo;
    char buffer[20];

    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);

    std::strftime(buffer, 20, "%d/%m/%Y", timeinfo);
    return buffer;
}

std::string SerialConnection::getTimeNow()
{
    std::time_t rawtime;
    std::tm *timeinfo;
    char buffer[20];

    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);

    std::strftime(buffer, 20, "%H:%M:%S %p", timeinfo);
    return buffer;
}

std::string SerialConnection::removeWordFromString(std::string word, std::string &string)
{

    auto n = string.find(word);
    if (n != std::string::npos)
    {
        string.erase(n, word.length());
    }
    return string;
}

oatpp::data::mapping::type::Void SerialConnection::printDTO(const oatpp::data::mapping::type::Void &variant)
{
    auto serializeConfig = oatpp::parser::json::mapping::Serializer::Config::createShared();
    auto deserializeConfig = oatpp::parser::json::mapping::Deserializer::Config::createShared();

    /* enable beautifier */
    serializeConfig->useBeautifier = true;

    /* create json object mapper with serializer config */
    auto jsonObjectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared(serializeConfig, deserializeConfig);
    oatpp::String json = jsonObjectMapper->writeToString(variant);
    OATPP_LOGD("", "%s", json.getValue("").c_str());
    return variant;
}