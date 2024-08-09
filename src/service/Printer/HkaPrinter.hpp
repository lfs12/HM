#ifndef HKaPrinter_hpp
#define HKaPrinter_hpp

#include "PrinterInterface.hpp"
#include "../PrinterService.hpp"

#include <map>
#include <vector>
#include <iostream>

#include "dto/PrinterDto.hpp"
#include "oatpp/web/protocol/http/Http.hpp"
class StatusHka
{
public:
    StatusHka(){};
    std::string sts1, sts2;
    static std::map<std::string, std::string> StatusHka1;
    static std::map<std::string, std::string> StatusHka2;
    std::string getDescripcionStatusHka1();
    std::string getDescripcionStatusHka2();

private:
    std::string getDescripcionStatus(std::string, std::map<std::string, std::string>);
};

class HkaPrinter : public PrinterInterface
{
private:
    typedef oatpp::web::protocol::http::Status Status;
    int tiempoAdicional = 0;

    std::string
        STX = this->hex2ascii("02"),       // Inicio de trama
        END = this->hex2ascii("03"),       // Fin de trama
        ENQ = this->hex2ascii("05"),       // Solicitud de status y error
        ACK = this->hex2ascii("06"),       // Reconocimiento del comando
        NAK = this->hex2ascii("15"),       // Error
        SEPARADOR = this->hex2ascii("0A"); // Separador

    virtual std::string readPort();
    virtual std::string ejecutarComando(oatpp::DTO *);
    virtual std::string prepararComando(std::string);
    virtual std::string enviarComando(std::string);
    std::string enviarComando(std::string comando, bool log);

    virtual std::string facturar(PrinterDto *dat, bool facturar);
    std::string facturarNotaDebito(PrinterDto *);  // Añadir pago a factura
    std::string facturarNotaCredito(PrinterDto *); // Devolución
    virtual std::string items(oatpp::DTO::List<oatpp::Object<Item>>);
    virtual std::string subtotal(oatpp::DTO::List<oatpp::Object<Pago>>);
    virtual std::string pagos(oatpp::DTO::List<oatpp::Object<Pago>>);
    std::string pagos(oatpp::DTO::List<oatpp::Object<Pago>> pagos, int , int );

    virtual void cabecera(oatpp::DTO::List<oatpp::String>);
    virtual void pie(oatpp::DTO::List<oatpp::String>);
    virtual std::string cerrarDocumentoFiscal();
    virtual void anularDocumentoFiscal();

    virtual std::string documentoNoFiscal(PrinterDto *);
    std::string documentoNoFiscalSF(PrinterDto *dat);
    std::string documentoNoFiscalCF(PrinterDto *dat);

    virtual std::string cerrarDocumentoNoFiscal();

    virtual std::string reporteX(bool = true);
    virtual std::string reporteZ();

    virtual std::string leerFechaYhora();

    virtual std::string leerSerial();
    virtual std::string leerRIF();
   // virtual std::string leerRif();

    virtual std::string analizarFactura(std::string, std::string, std::string);
    std::string analizarFactura(std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string);
    virtual std::string analizarReporteXZ(std::string cmd, std::string rawReporteXZ);
    void setFlag();
    void comentario(oatpp::DTO::List<oatpp::String>texto );

    void imprimirProgramacion();

    StatusHka leerStatus();
    void comentario(std::string, bool);
    void descuento(std::list<oatpp::data::mapping::type::DTO::Object<Item>>::iterator);
    void descuentoGlobal(oatpp::Object<Descuento> descuento);

    void reimprimir(PrinterDto *);
    std::vector<std::string> getFlags();

    std::string checksumLRC(std::string cadena);

public:
   // HkaPrinter(std::string);
    HkaPrinter(std::string, std::string="8N1");

    virtual void refrescarConfiguracion();
};

#endif /* HkaPrinter_hpp */
