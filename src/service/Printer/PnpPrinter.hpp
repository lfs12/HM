#ifndef PnpPrinter_hpp
#define PnpPrinter_hpp

#include "PrinterInterface.hpp"

#include <map>
#include <vector>

#include <iostream>
#include <fstream>

#include <sstream>
#include <bitset>
#include <string>
#include <cmath>

#include "dto/PrinterDto.hpp"
#include "oatpp/web/protocol/http/Http.hpp"
#include <nlohmann/json.hpp>

class StatusPNP
{
public:
    StatusPNP(){};
    std::string sts1, sts2, sts3;
    static std::map<std::string, std::string> StatusPNP1;
    static std::map<std::string, std::string> StatusPNP2;
    static std::map<std::string, std::string> StatusPNP3;
};

class PnpPrinter : public PrinterInterface
{
private:
    typedef oatpp::web::protocol::http::Status Status;
    static std::map<std::string, std::string> error;

    int secuencia = 32;

    std::string
        SEPARADOR = this->hex2ascii("1C"),
        STX = this->hex2ascii("02"),        // Inicio de trama
        END = this->hex2ascii("03"),        // Fin de trama
        MAS_TIEMPO = this->hex2ascii("12"), // Solicitud mas tiempo
        NO_USADO = "";

    virtual std::string readPort();
    virtual std::string ejecutarComando(oatpp::DTO *);
    virtual std::string prepararComando(std::string);
    virtual std::string enviarComando(std::string);

    virtual std::string facturar(PrinterDto *, bool);
    std::string facturarNotaDebito(PrinterDto *);                       // Añadir pago a factura
    virtual std::string items(oatpp::DTO::List<oatpp::Object<Item>>);
    virtual std::string subtotal(oatpp::DTO::List<oatpp::Object<Pago>> );
    virtual std::string pagos(oatpp::DTO::List<oatpp::Object<Pago>> );
    virtual void cabecera(oatpp::DTO::List<oatpp::String>);
    virtual void pie(oatpp::DTO::List<oatpp::String>);
    virtual std::string cerrarDocumentoFiscal();
    virtual void cerrarDocumentoFiscal(bool igtf);                      // REFACTORIZAR
    virtual void anularDocumentoFiscal();

    virtual std::string documentoNoFiscal(PrinterDto*);
    virtual std::string cerrarDocumentoNoFiscal();

    virtual std::string reporteX(bool);
    virtual std::string reporteZ();

    virtual std::string leerFechaYhora();

    virtual std::string leerSerial();
    virtual std::string leerRIF();

    virtual std::string analizarFactura(std::string, std::string, std::string);
    std::string analizarFactura(std::string, std::string, std::string, std::string, int);
    virtual std::string analizarReporteXZ(std::string, std::string);
    
    void leerStatus2();

    std::string getSecuencia();
    void imprimirPieDePagina(PrinterDto *, bool);
    std::string textoFiscal(std::string);
    void imprimirProgramacion(PrinterDto *);
    std::string reset();
    std::string status(PrinterDto *);
    std::string checksumBCC(std::string);
    std::string leerProgramacion();
    void descuento(std::list<oatpp::data::mapping::type::DTO::Object<Item>>::iterator);
    void reimprimir(PrinterDto *dat);
    StatusPNP leerStatus();
    std::string obtenerError(std::string data);
public:
    PnpPrinter(std::string);
    virtual void refrescarConfiguracion();
};

#endif /* PnpPrinter_hpp */