#include "PrinterInterface.hpp"

#include <map>
#include <vector>

#include <iostream>
#include <fstream>

#include <sstream>
#include <bitset>
#include <string>
#include <cmath>
#include <curl/curl.h>

#include "dto/PrinterDto.hpp"
//#include "../AppService.hpp"
#include "oatpp/web/protocol/http/Http.hpp"
#include <nlohmann/json.hpp>



class StatusEcrs
{
public:
 // AppService servAppService;
    StatusEcrs(){};
    std::string sts1, sts2, sts3;
    static std::map<std::string, std::string> StatusEcrs1;
    static std::map<std::string, std::string> StatusEcrs2;
    static std::map<std::string, std::string> StatusEcrs3;
};

class EcrsPrinter : public PrinterInterface
{
private:
    typedef oatpp::web::protocol::http::Status Status;
    int tiempoAdicional = 0;
    std::string
        SEPARADOR = this->hex2ascii("1C"),
        STX = this->hex2ascii("02"), // Comienzo de paquete
        END = this->hex2ascii("03"), // Final del paquete (END)
        ACK = this->hex2ascii("06"), // Respuesta positiva
        BEL = this->hex2ascii("07"), // Respuesta intermedia
        NAK = this->hex2ascii("15"), // Respuesta negativa
        NO_USADO = "";

    virtual std::string readPort();
    virtual std::string ejecutarComando(oatpp::DTO *);
    virtual std::string prepararComando(std::string);
    virtual std::string enviarComando(std::string);
    
    std::string facturar(PrinterDto *, bool);
    std::string facturarNotaDebito(PrinterDto *); 
    virtual std::string items(oatpp::DTO::List<oatpp::Object<Item>>);
    virtual std::string subtotal(oatpp::DTO::List<oatpp::Object<Pago>> );
    virtual std::string pagos(oatpp::DTO::List<oatpp::Object<Pago>> );
    std::string pagos2(oatpp::DTO::List<oatpp::Object<Pago>> pagos, float amountRelationPrice);
    virtual void cabecera(oatpp::DTO::List<oatpp::String>);
    virtual void pie(oatpp::DTO::List<oatpp::String>);
    virtual std::string cerrarDocumentoFiscal();
    virtual void anularDocumentoFiscal();
    
    virtual std::string documentoNoFiscal(PrinterDto *);
    virtual std::string cerrarDocumentoNoFiscal();
    
    virtual std::string reporteX(bool = true);
    virtual std::string reporteZ();

    virtual std::string leerFechaYhora();
    
    virtual std::string leerSerial();
    virtual std::string leerRIF();

    virtual std::string analizarFactura(std::string, std::string, std::string);
    std::string analizarFactura(std::string,int, std::string, std::string, int);
    virtual std::string analizarReporteXZ(std::string, std::string);

    void lineaComentario(std::string);
    // void descuentoGlobal(double tasaE, double tasaG, double tasaR, double tasaA, double tasaP, oatpp::Object<Descuento> descuento);
    // double descuento(std::list<oatpp::data::mapping::type::DTO::Object<Item>>::iterator);
    StatusEcrs leerStatus();
    void configurarFuente();
    oatpp::Object<PageDto<oatpp::Object<Device>>> getAllDevices();
    std::vector<std::string> splitString(const std::string& str, char delimiter);

public:
    EcrsPrinter(std::string);
    virtual void refrescarConfiguracion();
    std::string configPrinterMP();
};
