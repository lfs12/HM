#ifndef SerialConnection_hpp
#define SerialConnection_hpp

#include "lib/rs232.hpp"
#include "iomanip"
#include <sstream>
#include <map>
#include <vector>

#include "../dto/ConfigDto.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/core/Types.hpp"

class SerialConnection
{
protected:
    typedef oatpp::web::protocol::http::Status Status;

    int cport_nr, // Nro Port Serial
        bdrate;   // Tasa de baudios
    unsigned char buf[4096];
    std::string mode;

    void writePort(std::string);
    std::string readPort();

    std::string ascii_to_hex(const std::string &input);
    std::string int_to_hex(int numb);
    std::string hex2ascii(std::string);
    std::string string_to_upper(std::string &);
    size_t strpos(const std::string &, const std::string &);

    unsigned char hexval(unsigned char c);

    std::string float64_to_string(double numb);

    std::string fillString(std::string cadena, unsigned int length, char caracterParaCompletar = ' ');
    std::string fillNumbToString(double num, int length, int n_decimal, bool replacePunto = false);
    std::string fillNumbToString(int num, int length);
    std::string ltrim(std::string s);
    std::string rtrim(std::string s);
    std::string trim(std::string s);
    std::vector<std::string> explode(std::string const &s, char delim);
    std::string numbToString(double numb);
    double round2decimals(double numero);
    std::string getDateNow();
    std::string getTimeNow();
    std::string removeWordFromString(std::string, std::string&);
    void sleep(int);
    std::string getSerialData(std::string rawData, std::string begin = "" , std::string end = "");
    oatpp::data::mapping::type::Void printDTO(const oatpp::data::mapping::type::Void &variant);

public:
    void writeLog(std::string comando, std::string respuesta, std::string fileName);
    std::mutex mutex; // Validar si el dispositivo esta en uso
    Device *device;
    
    //mosca aqui 8E1 con dispositivo externo
    //mosca aqui 8N1 con dispositivo externo
    SerialConnection(std::string,  std::string = "8N1", int = 9600);
    bool OpenPort();
    void closePort();
    virtual std::string ejecutarComando(oatpp::DTO *) = 0;
    virtual std::string leerFechaYhora() = 0;
    virtual std::string leerSerial() = 0;
    virtual std::string leerRIF() = 0;
    
    virtual void refrescarConfiguracion() = 0;
};
#endif // SerialConnection.hpp
