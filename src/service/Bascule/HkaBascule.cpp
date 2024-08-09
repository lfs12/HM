#include "HkaBascule.hpp"

HkaBascule::HkaBascule(std::string cport) : SerialConnection::SerialConnection(cport)
{
}

void HkaBascule::refrescarConfiguracion() {}

std::string HkaBascule::ejecutarComando(oatpp::DTO *dto)
{
    BasculeDto *data = ((BasculeDto *)dto);
    std::string cmd = data->cmd.getValue("");

    std::string respuesta;
    if ("peso" == cmd)
    {
        respuesta = getPeso();
    }
    else if ("peso-precio" == cmd)
    {
        respuesta = getPesoPrecio();
    }
    else if ("getPlu" == cmd)
    {
        respuesta = getPlu();
    }
    else if ("setPlu" == cmd)
    {
        respuesta = setPlu();
    }
    else
    {
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "Comando desconocido.");
    }
    return respuesta;
}

std::string HkaBascule::leerSerial()
{
    OATPP_LOGD("", "HkaBascule::leerSerial -> No implementado");
    return "";
}
std::string HkaBascule::leerFechaYhora()
{
    OATPP_LOGD("", "HkaBascule::leerFechaYhora -> No implementado");
    return "";
}
std::string HkaBascule::leerRIF()
{
    throw;
}

std::string HkaBascule::getPeso()
{
    std::string respuesta, comando = "05";
    comando = this->hex2ascii(comando);
    respuesta = enviarComando(comando);
    if (respuesta == this->hex2ascii(this->ACK))
    {
        comando = "11";
        comando = this->hex2ascii(comando);
        respuesta = enviarComando(comando);
        OATPP_LOGD("", "respuesta: %s", respuesta.c_str());

        if (respuesta.size() == 15)
        {
            auto peso = Peso::createShared();

            peso->estable = respuesta[2] == 'S';
            char signo = respuesta[3];
            peso->peso = this->trim(signo + this->trim(respuesta.substr(4, 8)));
            return AppService::DtoToString(peso);
        }
    }

    throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "ERROR: No se pudo comunicar con la balanza");
}

std::string HkaBascule::getPesoPrecio()
{
    std::string resultado, respuesta, comando = "05";
    comando = this->hex2ascii(comando);
    respuesta = enviarComando(comando);
    if (respuesta == this->hex2ascii(this->ACK))
    {
        comando = "12";
        comando = this->hex2ascii(comando);
        respuesta = enviarComando(comando);

        auto peso = Peso::createShared();
        if (respuesta.size() == 37)
        {
            peso->estable = respuesta[13] == 'S';
            char signo = respuesta[14];
            peso->peso = this->trim(signo + this->trim(respuesta.substr(15, 8))).c_str();
            peso->precio = this->trim(respuesta.substr(26, 8)).c_str();
            peso->total = this->trim(respuesta.substr(2, 8)).c_str();

            resultado = AppService::DtoToString(peso);
        }
        else if (respuesta.size() == 10 && respuesta.find("FFFFFFFF") != std::string::npos)
        {
            resultado = this->getPeso();
        }
    }
    else
    {
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "ERROR: No se pudo comunicar con la balanza");
    }

    return resultado;
}
std::string HkaBascule::setPlu()
{
    throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "ERROR: Función en desarrollo");

    std::string resultado, respuesta, comando = "76";
    comando = this->hex2ascii(comando);

    respuesta = enviarComando(comando);
    exit(1);
    if (respuesta == this->hex2ascii(this->ACK) || true)
    {
        comando = "1100000000EF";
        comando = this->hex2ascii(comando);
        respuesta = enviarComando(comando);
        if (respuesta == this->hex2ascii(this->ACK) || true)
        {
            comando = "77f900e00400002b5c21";
            comando = this->hex2ascii(comando);
            respuesta = enviarComando(comando);
            if (respuesta == this->hex2ascii(this->ACK) || true)
            {
                comando = "3300000000cd";
                comando = this->hex2ascii(comando);
                respuesta = enviarComando(comando);

                OATPP_LOGD("", "respuesta: %s", respuesta.c_str());

                if (respuesta.size() == 15)
                {
                    auto peso = Peso::createShared();

                    peso->estable = respuesta[2] == 'S';
                    char signo = respuesta[3];
                    peso->peso = this->trim(signo + this->trim(respuesta.substr(4, 8)));
                    resultado = AppService::DtoToString(peso);
                }
            }
            else
                OATPP_LOGD("", "3...");
        }
        else
            OATPP_LOGD("", "2...");
    }
    else
        OATPP_LOGD("", "1...");

    return resultado;
}
std::string HkaBascule::getPlu()
{
    throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "ERROR: Función en desarrollo");

    std::string resultado, respuesta, comando = "05";
    comando = this->hex2ascii(comando);
    respuesta = enviarComando(comando);

    if (respuesta == this->hex2ascii(this->ACK))
    {
        comando = "11";
        comando = this->hex2ascii(comando);
        respuesta = enviarComando(comando);
        OATPP_LOGD("", "respuesta: %s", respuesta.c_str());

        if (respuesta.size() == 15)
        {
            auto peso = Peso::createShared();

            peso->estable = respuesta[2] == 'S';
            char signo = respuesta[3];
            peso->peso = this->trim(signo + this->trim(respuesta.substr(4, 8)));
            resultado = AppService::DtoToString(peso);
        }
    }
    else
    {
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "ERROR: No se pudo comunicar con la balanza");
    }

    return resultado;
}
std::string HkaBascule::readPort()
{
    std::string bytes;
    int timeOut = 0;

    std::string comandoReconocido = this->hex2ascii(this->ACK);
    std::string inicio = this->hex2ascii(this->STX);
    std::string fin = this->hex2ascii(this->END);

    bool buscando = true;
    while (buscando && timeOut < 10)
    {
        timeOut++;
        memset(this->buf, 0, 4095);
        int n = RS232_PollComport(this->cport_nr, this->buf, 4095);

        if (n > 0)
        {
            OATPP_LOGD("", "received %i bytes: %s\n", n, (char *)buf);
            bytes += std::string(buf, buf + n);
        }

        if (bytes.find(comandoReconocido) != std::string::npos ||
            bytes.find(fin) != std::string::npos ||
            bytes.find(inicio) != std::string::npos)
        {
            OATPP_LOGD("", "\t\t\t\t(OK): %s             STX: %s              END: %s             timeOut=%d\n",
                       (bytes.find(comandoReconocido) != std::string::npos) ? "TRUE" : "false",
                       (bytes.find(inicio) != std::string::npos) ? "TRUE" : "false",
                       (bytes.find(fin) != std::string::npos) ? "TRUE" : "false",
                       timeOut);
            buscando = false;
        }
        else
        {
            this->sleep(100);
        }
    }

    return bytes;
}

std::string HkaBascule::enviarComando(std::string comando)
{
    std::string respuesta = "";

    writePort(comando);
    respuesta = this->readPort();
    this->writeLog(comando, respuesta, "BALANZA");
    return respuesta;
}
