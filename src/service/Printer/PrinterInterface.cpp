#include "PrinterInterface.hpp"

PrinterInterface::PrinterInterface(std::string cport, std::string config) : SerialConnection::SerialConnection(cport, config)
{
}

std::string PrinterInterface::prepararYenviarComando(std::string comando)
{
    return enviarComando(prepararComando(comando));
}

bool PrinterInterface::contieneIGTF(oatpp::List<oatpp::Object<Pago>> pagos)
{
    bool IGTF = false;

    if (pagos.get() != nullptr)
    {
        for (auto i = pagos->begin(); i != pagos->end(); i++)
        {
            if (i->get()->impuesto != nullptr && i->get()->impuesto->igtf == true)
            {
                IGTF = true;
                break;
            }
        }
    }
    return IGTF;
}

double PrinterInterface::cantidadPagoIGTF(oatpp::List<oatpp::Object<Pago>> pagos)
{
    double cantidad = 0;

    if (pagos.get() != nullptr)
    {
        for (auto i = pagos->begin(); i != pagos->end(); i++)
        {
            if (i->get()->impuesto != nullptr && i->get()->impuesto->igtf == true)
            {
                cantidad += i->get()->monto.getValue(0);
            }
        }
    }
    return cantidad;
}

std::string PrinterInterface::formatearFecha(std::string fechaISO, bool ddmmyyyy)
{
    std::string fecha;
    if (fechaISO.size() == 10)
    {
        fecha = fechaISO.substr(8, 2); // dia
        fecha += "/";
        fecha += fechaISO.substr(5, 2); // mes
        fecha += "/";
        if (ddmmyyyy)
        {
            fecha += fechaISO.substr(0, 4); // año
        }
        else
        {
            fecha += fechaISO.substr(2, 2); // año
        }
    }

    return fecha;
}