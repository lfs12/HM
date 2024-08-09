#include "PrinterInterface.hpp"

#include <map>
#include <list>
#include <bitset>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
 
#include "dto/PrinterDto.hpp"
#include "oatpp/web/protocol/http/Http.hpp"

class StatusNetsoft
{
public:
    StatusNetsoft(){};
    std::string sts1, sts2, sts3;
    static std::map<std::string, std::string> StatusNetsoft1;
    static std::map<std::string, std::string> StatusNetsoft2;
    static std::map<std::string, std::string> StatusNetsoft3;
};

class NetsoftPrinter : public PrinterInterface
{
private:
    typedef oatpp::web::protocol::http::Status Status;

    static std::map<std::string, std::string> error;

    int secuencia = 32;
    int tiempoAdicional = 0;

    std::string
        STX = this->hex2ascii("02"),        // Inicio de trama
        END = this->hex2ascii("03"),        // Fin de trama
        SEPARADOR = this->hex2ascii("1C"),  // Separador
        MAS_TIEMPO = this->hex2ascii("12"), // Esperar mas tiempo
        NO_USADO = "";

    virtual std::string readPort();
    virtual std::string ejecutarComando(oatpp::DTO *);
    virtual std::string prepararComando(std::string);
    virtual std::string enviarComando(std::string);

    virtual std::string facturar(PrinterDto *, bool);
    virtual std::string items(oatpp::DTO::List<oatpp::Object<Item>>);
    virtual std::string subtotal(oatpp::DTO::List<oatpp::Object<Pago>>);
    virtual std::string pagos(oatpp::DTO::List<oatpp::Object<Pago>>);
    virtual void cabecera(oatpp::DTO::List<oatpp::String>);
    virtual void pie(oatpp::DTO::List<oatpp::String>);
    void pie(PrinterDto *dto);

    virtual std::string cerrarDocumentoFiscal();
    virtual void anularDocumentoFiscal();

    virtual std::string documentoNoFiscal(PrinterDto *);
    virtual std::string cerrarDocumentoNoFiscal();

    virtual std::string reporteX(bool);
    virtual std::string reporteZ();

    virtual std::string leerFechaYhora();

    virtual std::string leerSerial();
    virtual std::string leerRIF();

    std::string analizarFactura(std::string cmd, double igtf, std::string fecha, std::string rawSubtotal, std::string rawCierre);
    virtual std::string analizarFactura(std::string cmd, std::string fecha, std::string rawSubtotal);
    virtual std::string analizarReporteXZ(std::string cmd, std::string rawReporteXZ);

    std::string obtenerError(std::string);
    std::string getSecuencia();
    void cierreParcialDocumentoFiscal();
    void imprimirProgramacion();

    std::string reset();
    StatusNetsoft leerStatus();
    void descuentoGlobal(double tasaE, double tasaG, double tasaR, double tasaA, double tasaP, oatpp::Object<Descuento> descuento);
    std::string abrirNotaCredito(PrinterDto *);
    std::string textoFiscal(std::string);
    std::string itemFactura(PrinterDto *);
    std::string totalFactura(PrinterDto *);

    std::string checksumBCC(std::string cadena);

public:
    NetsoftPrinter(std::string);
    virtual void refrescarConfiguracion();
};
