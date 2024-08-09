#ifndef HkaBascule_hpp
#define HkaBascule_hpp

#include "../SerialConnection.hpp"
#include "../AppService.hpp"

#include <iostream>
#include <fstream>

#include "dto/BasculeDto.hpp"
#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/web/server/api/ApiController.hpp"

class HkaBascule : public SerialConnection
{

private:
    typedef oatpp::web::protocol::http::Status Status;

    std::string
        STX = "02", // Inicio de trama
        END = "03", // Fin de trama
        ENQ = "05", // Solicitud de status y error
        ACK = "06", // Reconocimiento del comando
        NAK = "15"; // No reconocimiento del comando (Error)

    std::string enviarComando(std::string);
    std::string readPort();

public:
    HkaBascule(std::string);
    virtual std::string ejecutarComando(oatpp::DTO *);
    virtual std::string leerFechaYhora();
    virtual std::string leerSerial();
    virtual std::string leerRIF();
    virtual void refrescarConfiguracion();

    std::string getPeso();
    std::string getPesoPrecio();
    std::string getPlu();
    std::string setPlu();
};

#endif // Fin HkaBascule.hpp
