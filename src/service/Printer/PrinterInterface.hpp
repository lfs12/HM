#ifndef PrinterInterface_hpp
#define PrinterInterface_hpp

#include "../../dto/PrinterConfigDto.hpp"
#include "../SerialConnection.hpp"
#include "Tools.hpp"
class PrinterInterface : public SerialConnection
{
protected:
    virtual std::string readPort() = 0;
    virtual std::string ejecutarComando(oatpp::DTO *) = 0;
    virtual std::string prepararComando(std::string) = 0;
    virtual std::string enviarComando(std::string) = 0;
    std::string prepararYenviarComando(std::string);

    virtual std::string facturar(PrinterDto *, bool) = 0;
    virtual std::string items(oatpp::DTO::List<oatpp::Object<Item>>) = 0;
    virtual std::string subtotal(oatpp::DTO::List<oatpp::Object<Pago>>) = 0;
    virtual std::string pagos(oatpp::DTO::List<oatpp::Object<Pago>>) = 0;
    virtual void cabecera(oatpp::DTO::List<oatpp::String>) = 0;
    virtual void pie(oatpp::DTO::List<oatpp::String>) = 0;
    bool contieneIGTF(oatpp::DTO::List<oatpp::Object<Pago>>);
    double cantidadPagoIGTF(oatpp::List<oatpp::Object<Pago>>);
    virtual std::string cerrarDocumentoFiscal() = 0;
    virtual void anularDocumentoFiscal() = 0;

    virtual std::string documentoNoFiscal(PrinterDto *) = 0;
    virtual std::string cerrarDocumentoNoFiscal() = 0;

    virtual std::string reporteX(bool) = 0;
    virtual std::string reporteZ() = 0;

    virtual std::string leerFechaYhora() = 0;

    virtual std::string leerSerial() = 0;
    virtual std::string leerRIF() = 0;

    virtual std::string analizarFactura(std::string cmd, std::string fecha, std::string rawSubtotal) = 0;
    virtual std::string analizarReporteXZ(std::string cmd, std::string rawReporteXZ) = 0;
    std::string formatearFecha(std::string fechaISO, bool ddmmyyyy = true);
    
    // TODO: PrinterIntrface Agregar Reset
public:
    PrinterInterface(std::string cport, std::string config="8N1");
    int maxLength; // Ancho del papel
    int lineasTextoNF;
};

#endif
