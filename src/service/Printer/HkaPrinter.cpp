#include "HkaPrinter.hpp"

std::string StatusHka::getDescripcionStatusHka1()
{
    OATPP_LOGI("entrnado a", "StatusHka::getDescripcionStatusHka1");
    OATPP_LOGI("saliendo de", "StatusHka::getDescripcionStatusHka1");
    return this->getDescripcionStatus(this->sts1, this->StatusHka1);
}

std::string StatusHka::getDescripcionStatusHka2()
{
    OATPP_LOGI("entrnado a", "StatusHka::getDescripcionStatusHka2");
    OATPP_LOGI("saliendo de", "StatusHka::getDescripcionStatusHka2");
    return this->getDescripcionStatus(this->sts2, this->StatusHka2);
}

std::string StatusHka::getDescripcionStatus(std::string hexCode, std::map<std::string, std::string> sts)
{
    OATPP_LOGI("entrnado a", "StatusHka::getDescripcionStatus");

    auto it = sts.find(hexCode);
    std::string value;

    if (it == sts.end())
    {
        value = "ERROR: Status de la impresora: Codigo desconocido " + hexCode;
    }
    else
    {
        value = it->second;
    }
    OATPP_LOGI("saliendo de", "StatusHka::getDescripcionStatus");
    return value;
}

std::map<std::string, std::string> StatusHka::StatusHka1 = {
    {"40", "Modo Entrenamiento y en Espera"},
    {"41", "Modo Entrenamiento y en medio de una Transacción Fiscal"},
    {"42", "Modo Entrenamiento y en medio de una Transacción No fiscal"},
    {"60", "Modo Fiscal y en Espera"},
    {"61", "Modo Fiscal y en medio de una Transacción Fiscal"},
    {"62", "Modo Fiscal y en medio de una Transacción No fiscal"},
    {"69", "Modo Fiscal con la MF llena y en medio de una Transacción Fiscal"},
    {"68", "Modo Fiscal con la MF llena y en Espera"},
    {"6A", "Modo Fiscal con la MF llena y en Transacción No fiscal"}};

std::map<std::string, std::string> StatusHka::StatusHka2 = {
    {"40", "Ningún error"},
    // {"48", "Error gaveta"},
    {"41", "Error sin papel"},
    {"42", "Error mecánico de la impresora / papel"},
    {"43", "Error mecánico de la impresora / fin de papel"},
    {"60", "Error fiscal"},
    {"64", "Error en la memoria fiscal"},
    {"6C", "Error memoria fiscal llena"}};

HkaPrinter::HkaPrinter(std::string cport, std::string config) : PrinterInterface::PrinterInterface(cport, config)
{

    this->maxLength = 40;
    this->lineasTextoNF = 8;
}

void HkaPrinter::refrescarConfiguracion()
{
    OATPP_LOGI("entrnado a", " HkaPrinter::refrescarConfiguracion");

    // if (string_to_upper(this->device->brand.getValue("")).find("PP9") != std::string::npos)
    if (string_to_upper(this->device->model.getValue("")).find("PP9") != std::string::npos)
    {
        this->maxLength = 34;
    }
    else
    {
        this->maxLength = 40;
    }
    OATPP_LOGI("saliendo de", " HkaPrinter::refrescarConfiguracion");
}

std::string HkaPrinter::readPort()
{
    OATPP_LOGI("entrnado a", "HkaPrinter::readPort");

    std::string bytes = "";
    int timeOut = 0;

    std::string comandoReconocido = this->ACK;
    std::string error = this->NAK;
    std::string fin = this->END;

    bool buscando = true;
    while (buscando && timeOut < 15 + this->tiempoAdicional)
    {
        timeOut++;
        memset(this->buf, 0, 4095);
        int n = RS232_PollComport(this->cport_nr, this->buf, 4095);

        if (n > 0)
        {
            OATPP_LOGD("", "received %i bytes: %s\n", n, (char *)buf);
            bytes += std::string((char *)buf, n);
        }

        if (bytes.find(comandoReconocido) != std::string::npos ||
            bytes.find(error) != std::string::npos ||
            bytes.find(fin) != std::string::npos)
        {
            buscando = false;
            OATPP_LOGD("", "\t\t(OK): %s \t\t (ERROR): %s \t\t (FIN): %s \t\t timeOut=%d",
                       (bytes.find(comandoReconocido) != std::string::npos) ? "TRUE" : "false",
                       (bytes.find(error) != std::string::npos) ? "TRUE" : "false",
                       (bytes.find(fin) != std::string::npos) ? "TRUE" : "false",
                       timeOut);
        }
        else
        {
            this->sleep(150);
        }
    }
    OATPP_LOGI("saliendo de", "HkaPrinter::readPort");
    return bytes;
}

std::string HkaPrinter::ejecutarComando(oatpp::DTO *dto)
{
    OATPP_LOGI("entrnado a HkaPrinter::ejecutarComando", "");
    PrinterDto *data = ((PrinterDto *)dto);
    std::string resultado, cmd = data->cmd.getValue("");

    StatusHka status = leerStatus();
    OATPP_LOGD("", "Status: %s - %s", status.getDescripcionStatusHka1().c_str(), status.getDescripcionStatusHka2().c_str());

    auto auxx = StatusHka::StatusHka2.begin();
    std::advance(auxx, 1);

    if (status.getDescripcionStatusHka2() != StatusHka::StatusHka2.begin()->second && status.getDescripcionStatusHka2() != auxx->second)
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, status.getDescripcionStatusHka2());

    if (status.sts1 == "41" || status.sts1 == "61")
    {
        try
        {
            anularDocumentoFiscal();
        }
        catch (...)
        {
        }
        this->sleep(500);
    }

    if (status.sts1 == "42" || status.sts1 == "62")
    {
        try
        {
            anularDocumentoFiscal();
        }
        catch (...)
        {
        }
        try
        {
            cerrarDocumentoNoFiscal();
        }
        catch (...)
        {
        }
        this->sleep(500);
    }

    nlohmann::json jsonResponse;

    if ("reporteX-ver" == cmd ||
        "reporteX-imprimir" == cmd ||
        "reporte_x" == cmd)
    {
        resultado = reporteX();
    }
    else if ("reporteZ-ver" == cmd ||
             "reporteZ-imprimir" == cmd ||
             "reporte_z" == cmd)
    {
        resultado = reporteZ();
    }
    else if ("facturar" == cmd)
    {
        OATPP_LOGI("cmd=facturar", "");
        resultado = facturar(data, true);
    }
    else if ("notaCredito" == cmd)
    {
        resultado = facturar(data, false);
    }
    // else if ("notaDebito" == cmd)
    // {
    //     notaDebito(data);
    // }
    else if ("textoNF" == cmd)
    {
        resultado = documentoNoFiscal(data);
    }
    else if ("imprimirProgramacion" == cmd)
    {
        imprimirProgramacion();
    }
    else if ("reimprimir" == cmd)
    {
        reimprimir(data);
    }
    else if ("status" == cmd)
    {
        leerStatus();
    }
    else if ("fechaYhora" == cmd)
    {
        jsonResponse["fechaYhora"] = leerFechaYhora();
    }
    else if ("flag" == cmd)
    {
        setFlag();
    }
    else if ("serial" == cmd)
    {
        jsonResponse["serial"] = leerSerial();
    }
    else
    {
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "Comando desconocido.");
    }

    resultado = resultado.empty() ? jsonResponse.dump() : resultado;
    OATPP_LOGI("saliendo de HkaPrinter::ejecutarComando", "");
    return resultado;
}

std::string HkaPrinter::prepararComando(std::string comando)
{
    OATPP_LOGI("entrando a HkaPrinter::prepararComando", "");

    std::string result;
    std::string data = comando + this->END;
    std::string chk = this->checksumLRC(data);

    result = this->STX + data + chk;
    OATPP_LOGI("saliendo de HkaPrinter::prepararComando", "");
    return result;
}

std::string HkaPrinter::enviarComando(std::string comando)
{

    return enviarComando(comando, true);
}

std::string HkaPrinter::enviarComando(std::string comando, bool log)
{

    std::string respuesta;
    writePort(comando);
    respuesta = this->readPort();

    if (log)
    {
        this->writeLog(comando, respuesta, this->device->brand);
    }

    if (respuesta.size() == 0)
    {
        std::string _comando = this->ENQ;
        std::string _respuesta;

        writePort(_comando);
        _respuesta = this->readPort();
        OATPP_LOGE("enviarComando", "COMANDO SIN RESPUESTA, SOLICITANDO STATUS");

        if (_respuesta.empty())
            throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "SE HA PERDIDO COMUNICACIÓN CON EL DISPOSITIVO");
        else
        {
            OATPP_LOGE("enviarComando", "OMITIENDO ERROR DE COMUNICACION");
        }
    }
    if (respuesta == this->NAK)
    {
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "SE HA ENCONTRADO UN ERROR AL ENVIAR EL COMANDO");
    }

    return respuesta;
}

std::string HkaPrinter::facturar(PrinterDto *dat, bool factura)
{
    OATPP_LOGI("entrando a HkaPrinter::facturar", "")
    OATPP_COMPONENT(oatpp::Object<PrinterConfig>, printerConfig);
    OATPP_ASSERT_HTTP((printerConfig && printerConfig->hkaConfig), Status::CODE_400, "Debe configurar los metodos de pago en printer_config.json");
    OATPP_ASSERT_HTTP(printerConfig->hkaConfig->formasPago->size() > 0, Status::CODE_400, "Debe configurar los metodos de pago en printer_config.json");

    FormasPago::Wrapper formasPago = nullptr;
    for (auto &&metodos : *printerConfig->hkaConfig->formasPago.get())
    {
        if (metodos->serial.getValue("") == this->device->serial.getValue(""))
        {
            formasPago = metodos;
            OATPP_LOGE("METODOS DE PAGO ENCONTRADOS...", "%s", metodos->serial.getValue("").c_str());
            break;
        }
    }
    if (!formasPago)
    {
        for (auto &&metodos : *printerConfig->hkaConfig->formasPago.get())
        {
            if (metodos->serial.getValue("").empty() || metodos->serial.getValue("") == "*")
            {
                formasPago = metodos;
                break;
            }
        }
    }
    printDTO(formasPago);
    std::string auxMetodo;
    for (size_t i = 0; i < dat->factura->pagos->size(); i++)
    {
        auxMetodo = dat->factura->pagos[i]->metodo.getValue("");
        auxMetodo = string_to_upper(auxMetodo);
        dat->factura->pagos[i]->metodo_id = -99;
        OATPP_LOGD(".....", "-%d--%s---", i, auxMetodo.c_str());
        if (auxMetodo == formasPago->fp_01.getValue("."))
            dat->factura->pagos[i]->metodo_id = 1;
        else if (auxMetodo == formasPago->fp_02.getValue("."))
            dat->factura->pagos[i]->metodo_id = 2;
        else if (auxMetodo == formasPago->fp_03.getValue("."))
            dat->factura->pagos[i]->metodo_id = 3;
        else if (auxMetodo == formasPago->fp_04.getValue("."))
            dat->factura->pagos[i]->metodo_id = 4;
        else if (auxMetodo == formasPago->fp_05.getValue("."))
            dat->factura->pagos[i]->metodo_id = 5;
        else if (auxMetodo == formasPago->fp_06.getValue("."))
            dat->factura->pagos[i]->metodo_id = 6;
        else if (auxMetodo == formasPago->fp_07.getValue("."))
            dat->factura->pagos[i]->metodo_id = 7;
        else if (auxMetodo == formasPago->fp_08.getValue("."))
            dat->factura->pagos[i]->metodo_id = 8;
        else if (auxMetodo == formasPago->fp_09.getValue("."))
            dat->factura->pagos[i]->metodo_id = 9;
        else if (auxMetodo == formasPago->fp_10.getValue("."))
            dat->factura->pagos[i]->metodo_id = 10;
        else if (auxMetodo == formasPago->fp_11.getValue("."))
            dat->factura->pagos[i]->metodo_id = 11;
        else if (auxMetodo == formasPago->fp_12.getValue("."))
            dat->factura->pagos[i]->metodo_id = 12;
        else if (auxMetodo == formasPago->fp_13.getValue("."))
            dat->factura->pagos[i]->metodo_id = 13;
        else if (auxMetodo == formasPago->fp_14.getValue("."))
            dat->factura->pagos[i]->metodo_id = 14;
        else if (auxMetodo == formasPago->fp_15.getValue("."))
            dat->factura->pagos[i]->metodo_id = 15;
        else if (auxMetodo == formasPago->fp_16.getValue("."))
            dat->factura->pagos[i]->metodo_id = 16;
        else if (auxMetodo == formasPago->fp_17.getValue("."))
            dat->factura->pagos[i]->metodo_id = 17;
        else if (auxMetodo == formasPago->fp_18.getValue("."))
            dat->factura->pagos[i]->metodo_id = 18;
        else if (auxMetodo == formasPago->fp_19.getValue("."))
            dat->factura->pagos[i]->metodo_id = 19;
        else if (auxMetodo == formasPago->fp_20.getValue("."))
            dat->factura->pagos[i]->metodo_id = 20;
        else if (auxMetodo == formasPago->fp_21.getValue("."))
            dat->factura->pagos[i]->metodo_id = 21;
        else if (auxMetodo == formasPago->fp_22.getValue("."))
            dat->factura->pagos[i]->metodo_id = 22;
        else if (auxMetodo == formasPago->fp_23.getValue("."))
            dat->factura->pagos[i]->metodo_id = 23;
        else if (auxMetodo == formasPago->fp_24.getValue("."))
            dat->factura->pagos[i]->metodo_id = 24;
        else
            OATPP_LOGE("ERROR METODO NO REGISTRADO", "**%d****%s******", i, auxMetodo.c_str());
    }
    double baseImponible = 0, pDescuento = 0;
    double descc;
    std::string resultado;
    auto flags = this->getFlags();
    // validar metodos de pago
    for (size_t i = 0; i < dat->factura->pagos->size(); i++)
    {
        if (dat->factura->pagos[i]->metodo_id == nullptr)
            throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "Factura.Pagos[n].metodo_id es nulo");

        if (!(dat->factura->pagos[i]->metodo_id > 0 && dat->factura->pagos[i]->metodo_id < 20) && flags[50] == "00")
            throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "Factura.Pagos[n].metodo_id invalido, se espera un valor entre 1 y 19, (PAGOS EN MONEDA NACIONAL), " + std::to_string(dat->factura->pagos[i]->metodo_id.getValue(0)) + "." + std::to_string(i));

        if (!(dat->factura->pagos[i]->metodo_id > 0 && dat->factura->pagos[i]->metodo_id < 25) && flags[50] == "01")
            throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "Factura.Pagos[n].metodo_id invalido, se espera un valor entre 1 y 24, (PAGOS EN MONEDA NACIONAL Y DIVISAS)" + std::to_string(dat->factura->pagos[i]->metodo_id.getValue(0)) + "." + std::to_string(i));
    }
    try
    {
        std::string comando;
        if ((dat->factura->cliente->rif.getValue("") != "" && factura) || (!factura))
        {
            comando = "iR*";
            if (factura)
                comando += dat->factura->cliente->rif.getValue("");
            else
                comando += dat->factura->cliente->rif.getValue("").empty() ? "N/D" : dat->factura->cliente->rif.getValue("");
            comando = prepararYenviarComando(comando);
        }
        if (dat->factura->cliente->nombre.getValue("") != "")
        {
            comando = "iS*";
            comando += dat->factura->cliente->nombre.getValue("").substr(0, this->maxLength);
            comando = prepararYenviarComando(comando);

            if (dat->factura->cliente->nombre.getValue("").size() > this->maxLength)
            {
                dat->factura->cabecera->push_front(dat->factura->cliente->nombre->substr(this->maxLength));
            }
        }
        if (!factura)
        {
            comando = "iF*";
            comando += this->fillNumbToString(dat->factura->notaFiscal->factura.getValue(0), 11);
            prepararYenviarComando(comando);

            comando = "iD*";
            comando += dat->factura->notaFiscal->fecha.getValue("");
            prepararYenviarComando(comando);

            comando = "iI*";
            comando += dat->factura->notaFiscal->serialFiscal.getValue("");
            prepararYenviarComando(comando);
        }

        cabecera(dat->factura->cabecera);

        int precioLength = 11;
        int precioDecimalLength = 2;

        if (flags[21] == "00")
        {
            precioLength = 11; // 8 enteros, 2 decimales, 1 coma
            precioDecimalLength = 2;
        }
        else if (flags[21] == "01")
        {
            precioLength = 11; // 7 enteros, 3 decimales, 1 coma
            precioDecimalLength = 3;
        }
        else if (flags[21] == "02")
        {
            precioLength = 11; // 6 enteros, 4 decimales, 1 coma
            precioDecimalLength = 4;
        }
        else if (flags[21] == "11")
        {
            precioLength = 11; // 9 enteros, 1 decimal, 1 coma
            precioDecimalLength = 1;
        }
        else if (flags[21] == "12")
        {
            precioLength = 10; // 10 enteros, 0 decimales
            precioDecimalLength = 0;
        }
        else if (flags[21] == "30")
        {
            precioLength = 17; // 14 enteros, 2 decimales, 1 coma
            precioDecimalLength = 2;
        }

        //! checar esto

        if (dat->factura->descuento != nullptr)
        {
            for (auto &&i : *dat->factura->items)
            {
                if (i->producto.getValue("").find("SERVICIO (") == std::string::npos)
                    baseImponible = baseImponible + (i->cantidad * i->precio);
            }
            if (dat->factura->descuento->tipo_descuento.getValue("") == "monto")
            {
                pDescuento = (dat->factura->descuento->descuento * 100) / baseImponible;
                dat->factura->descuento->tipo_descuento = "porcentaje";
                dat->factura->descuento->descuento = pDescuento;
            }
            // for (auto &&i : *dat->factura->items)
            // {
            //     if (i->producto.getValue("").find("SERVICIO (") == std::string::npos)
            //     {
            //         double monto = (i->precio * pDescuento) / 100;
            //         i->descuento = monto;
            //         i->tipo_descuento = "monto";
            //     }
            // }
        }

        auto item = dat->factura->items.get();
        for (auto i = item->begin(); i != item->end(); i++)
        {
            std::string comentario = i->get()->texto.getValue("");
            if (!comentario.empty())
            {
                for (size_t i = 0; i < comentario.size() && (i / this->maxLength) < this->lineasTextoNF; i += this->maxLength)
                    this->comentario(comentario.substr(i, this->maxLength), true);
            }

            std::string tasa;
            if (i->get()->tasa == "E")
                tasa = "0";
            if (i->get()->tasa == "G")
                tasa = "1";
            else if (i->get()->tasa == "R")
                tasa = "2";
            else if (i->get()->tasa == "A")
                tasa = "3";

            comando = "G";
            comando += (factura ? "F" : "C");
            comando += "+";
            comando += tasa;

            comando += this->fillNumbToString(i->get()->precio, precioLength, precioDecimalLength, true) + "||";
            comando += this->fillNumbToString(i->get()->cantidad, 12, 3, true) + "||";

            if (!i->get()->codigo.getValue("").empty())
                comando += "|" + i->get()->codigo.getValue("") + "|";
            comando += i->get()->producto.getValue("").substr(0, 120);

            comando = prepararYenviarComando(comando);
            descuento(i);
        }
        subtotal(dat->factura->pagos);
        descuentoGlobal(dat->factura->descuento);
        pagos(dat->factura->pagos, precioLength, precioDecimalLength);
        pie(dat->factura->pie);

        // solicutud de acumulados en tasas
        auto s25 = this->getSerialData(prepararYenviarComando("S25")); // subtotal + igtf
        auto s2E = this->getSerialData(prepararYenviarComando("S2E")); // acumulado tasa e
        auto s21 = this->getSerialData(prepararYenviarComando("S21")); // acumulado tasa G
        auto s22 = this->getSerialData(prepararYenviarComando("S22")); // acumulado tasa R
        auto s23 = this->getSerialData(prepararYenviarComando("S23")); // acumulado tasa A
        auto s1 = this->getSerialData(prepararYenviarComando("S1"));   // datos de la ultima factura
        // OATPP_LOGD("vars...", "\ns25: %s.\ns2E: %s.\ns21: %s.\ns22: %s.\ns23: %s.\ns1: %s.", s25.c_str(), s2E.c_str(), s21.c_str(), s22.c_str(), s23.c_str(), s1.c_str());

        std::string serial = leerSerial();
        if (flags[50] == "01")
            prepararYenviarComando("199");

        resultado = analizarFactura(dat->cmd.getValue(""), s1, s2E, s21, s22, s23, s25, serial);
    }
    catch (const std::exception &e)
    {
        anularDocumentoFiscal();
        throw;
    }
    return resultado;
}

std::string HkaPrinter::pagos(oatpp::DTO::List<oatpp::Object<Pago>> pagos, int precioLength, int precioDecimalLength)
{
    if (string_to_upper(this->device->model.getValue("")).find("PP9") != std::string::npos)
    {
        precioLength++;
    }
    std::string comando, respuesta;
    if (pagos->size() == 0)
    {
        comando = "101"; // item
        prepararYenviarComando(comando);
    }
    else
    {
        for (auto i = pagos->begin(); i != pagos->end(); i++)
        {
            comando = "2";

            comando += this->fillNumbToString(i->get()->metodo_id, 2, 0);
            comando += this->fillNumbToString(i->get()->monto, precioLength, precioDecimalLength);

            prepararYenviarComando(comando);
        }
    }

    return "";
}

void HkaPrinter::anularDocumentoFiscal()
{
    std::string comando = "7";
    prepararYenviarComando(comando);
}

std::string HkaPrinter::documentoNoFiscal(PrinterDto *dat)
{
    if (dat->textoNF)
    {
        printDTO(dat->textoNF);
        documentoNoFiscalSF(dat);
    }
    else if (dat->textoNFF)
    {
        printDTO(dat->textoNFF);
        documentoNoFiscalCF(dat);
    }
    else
    {
        OATPP_ASSERT_HTTP(false, Status::CODE_400, "Debe configurar los metodos de pago en printer_config.json");
    }

    return "";
}
std::string HkaPrinter::documentoNoFiscalCF(PrinterDto *dat)
{
    auto items = dat->textoNFF.get();
    std::string texto, formato, tipo;

    for (auto i = items->begin(); i != items->end(); ++i)
    {
        texto = i->get()->text.getValue("");
        formato = i->get()->formato.getValue("");

        if (formato == "logo")
        {
            tipo = " "
                   "@LOGO";
        }
        else if (formato == "negrita")
        {
            tipo = "*";
        }
        else if (formato == "centrado")
        {
            tipo = "!";
        }
        else if (formato == "negrita_centrado")
        {
            tipo = "¡";
        }
        else if (formato == "negrita_centrado_doble_a")
        {
            tipo = "$";
        }
        else if (formato == "expandido")
        {
            tipo = ">";
        }
        else
            tipo = " ";

        std::string comando = (i->get() == items->back().get()) ? "81" : "80";
        comando += tipo;
        comando += texto;

        comando = prepararYenviarComando(comando);
    }
    return "";
}

std::string HkaPrinter::documentoNoFiscalSF(PrinterDto *dat)
{
    auto items = dat->textoNF.get();
    std::string texto, formato, tipo;

    for (auto i = items->begin(); i != items->end(); ++i)
    {
        texto = i->get()->substr(0, maxLength);
        texto = texto;
        tipo = " ";

        std::string comando = (i->get() == items->back().get()) ? "81" : "80";
        comando += tipo;
        comando += texto;

        comando = prepararYenviarComando(comando);
    }
    return "";
}

std::string HkaPrinter::cerrarDocumentoNoFiscal()
{
    std::string comando = this->hex2ascii("80202E");

    comando = prepararYenviarComando(comando);
    return "";
}

std::string HkaPrinter::reporteX(bool imprimir)
{
    std::string comando = "I0X";
    try
    {
        this->tiempoAdicional = 100;
        prepararYenviarComando(comando);
        this->tiempoAdicional = 0;
    }
    catch (const std::exception &e)
    {
        this->tiempoAdicional = 0;
        throw;
    }

    return "";
}

std::string HkaPrinter::reporteZ()
{
    std::string comando = "I0Z";
    try
    {
        this->tiempoAdicional = 100;
        prepararYenviarComando(comando);
        this->tiempoAdicional = 0;
    }
    catch (const std::exception &e)
    {
        this->tiempoAdicional = 0;
        throw;
    }
    return "";
}

std::string HkaPrinter::leerFechaYhora()
{
    std::string comando = "a", respuesta;
    respuesta = prepararYenviarComando(comando);
    return respuesta;
}

std::string HkaPrinter::leerSerial()
{
    std::string comando = "S5";
    std::string respuesta;

    respuesta = enviarComando(prepararComando(comando), false);

    if (respuesta.length() > 25)
        respuesta = respuesta.substr(15, 10);

    return respuesta;
}

std::string HkaPrinter::leerRIF()
{
    std::string comando = "S5";
    std::string respuesta;

    respuesta = enviarComando(prepararComando(comando), false);

    if (respuesta.length() > 25)
        respuesta = respuesta.substr(3, 11);

    return respuesta;
}

std::string HkaPrinter::analizarFactura(std::string a, std::string b, std::string c) { throw; }

std::string HkaPrinter::analizarFactura(std::string cmd, std::string rawS1, std::string raw2E, std::string raw21, std::string raw22, std::string raw23, std::string raw25, std::string serial)
{
    int nroFactura = -1;
    nlohmann::json respuesta;
    try
    {
        respuesta["factura"]["subtotales"]["total_impuesto_e"] = round2decimals(stof(explode(raw2E, '\n')[1]) / 100);
        respuesta["factura"]["subtotales"]["total_impuesto_g"] = round2decimals(stof(explode(raw21, '\n')[1]) / 100);
        respuesta["factura"]["subtotales"]["total_impuesto_r"] = round2decimals(stof(explode(raw22, '\n')[1]) / 100);
        respuesta["factura"]["subtotales"]["total_impuesto_a"] = round2decimals(stof(explode(raw23, '\n')[1]) / 100);

        respuesta["factura"]["subtotales"]["total_venta_e"] = round2decimals(stof(explode(explode(raw2E, '\n')[0], ' ')[1]) / 100);
        respuesta["factura"]["subtotales"]["total_venta_g"] = round2decimals(stof(explode(explode(raw21, '\n')[0], ' ')[1]) / 100);
        respuesta["factura"]["subtotales"]["total_venta_r"] = round2decimals(stof(explode(explode(raw22, '\n')[0], ' ')[1]) / 100);
        respuesta["factura"]["subtotales"]["total_venta_a"] = round2decimals(stof(explode(explode(raw23, '\n')[0], ' ')[1]) / 100);

        respuesta["factura"]["subtotales"]["total_igtf"] = round2decimals(stof(explode(raw25, '\n')[1]) / 100);
        respuesta["factura"]["subtotales"]["subtotal"] = round2decimals(stof(explode(raw25, '\n')[2]) / 100);

        //////////////////////////////////////////////////////////////////////////////////////////////////
        auto rawCamposS1 = explode(rawS1, this->SEPARADOR[0]);

        std::string fecha = rawCamposS1[15] + rawCamposS1[14];
        if (fecha.size() > 11)
        {
            fecha = "20" + fecha.substr(4, 2) + "/" + fecha.substr(2, 2) + "/" + fecha.substr(0, 2) + " " + fecha.substr(6, 2) + ":" + fecha.substr(8, 2);
        }

        respuesta["factura"]["nro"] = explode(raw25, '\n')[6] == "1" ? stoi(rawCamposS1[2]) : stoi(rawCamposS1[6]);
        respuesta["cmd"] = cmd;
        respuesta["serialFiscal"] = serial;
        respuesta["fecha"] = fecha;
    }
    catch (std::exception &e)
    {
        printf("ERROR analizarFactura %s\n", e.what());
    }
    return respuesta.dump();
}

std::string HkaPrinter::analizarReporteXZ(std::string cmd, std::string rawReporteXZ) { throw; };

void HkaPrinter::imprimirProgramacion()
{
    std::string comando = "D";
    prepararYenviarComando(comando);
}

std::string HkaPrinter::facturarNotaDebito(PrinterDto *dat) // Añadir pago a factura
{
    std::string comando;

    comando = "iR*";
    comando += dat->factura->cliente->rif.getValue("");
    prepararYenviarComando(comando);

    comando = "iS*";
    comando += dat->factura->cliente->nombre.getValue("");
    prepararYenviarComando(comando);

    comando = "iF*";
    comando += this->fillNumbToString(dat->factura->notaFiscal->factura.getValue(0), 11);
    prepararYenviarComando(comando);

    comando = "iD*";
    comando += dat->factura->notaFiscal->fecha.getValue("");
    prepararYenviarComando(comando);

    comando = "iI*";
    comando += dat->factura->notaFiscal->serialFiscal.getValue("");
    prepararYenviarComando(comando);

    auto aux = dat->factura->cabecera.get(); // REVISAR cabecera - info
    int n = 0;
    for (auto i = aux->begin(); i != aux->end(); i++)
    {
        std::string info = i->getValue("");

        comando = "i";
        comando += this->fillNumbToString(n++, 2, 0);

        comando += info;
        prepararYenviarComando(comando);
    }

    auto item = dat->factura->items.get();
    for (auto i = item->begin(); i != item->end(); i++)
    {
        std::string text = i->get()->texto.getValue("");
        if (!text.empty())
        {
            comentario(text, false);
        }

        std::string tasa;

        if (i->get()->tasa == "E")
            tasa = "0";
        if (i->get()->tasa == "G")
            tasa = "1";
        else if (i->get()->tasa == "R")
            tasa = "2";
        else if (i->get()->tasa == "A")
            tasa = "3";

        comando = "GD+";
        comando += tasa;

        comando += this->fillNumbToString(i->get()->precio, 11, 2, true) + "||";
        comando += this->fillNumbToString(i->get()->cantidad, 12, 3, true) + "||";

        if (!i->get()->codigo.getValue("").empty())
            comando += "|" + i->get()->codigo.getValue("") + "|";
        comando += i->get()->producto.getValue("");

        prepararYenviarComando(comando);

        descuentoGlobal(dat->factura->descuento);
    }

    pagos(dat->factura->pagos);
    return "";
}

void HkaPrinter::comentario(std::string comentario, bool esFactura) // COMENTARIO
{
    std::string comando = esFactura ? "@" : "A";
    comando += comentario;
    prepararYenviarComando(comando);
}

void HkaPrinter::descuento(std::list<oatpp::data::mapping::type::DTO::Object<Item>>::iterator i)
{
    std::string comando;
    if (i->get()->descuento.getValue(0) != 0 &&
        (i->get()->tipo_descuento == "monto" || i->get()->tipo_descuento == "porcentaje"))
    {
        comando = "G";
        comando += (i->get()->tipo_descuento == "monto") ? "M" : "P";
        comando += "-*";
        if (i->get()->tipo_descuento == "monto")
            comando += this->fillNumbToString(i->get()->descuento.getValue(0), 11, 2, true);
        else
            comando += this->fillNumbToString(i->get()->descuento.getValue(0), 4, 2, true);

        prepararYenviarComando(comando);
    }
}

void HkaPrinter::descuentoGlobal(oatpp::Object<Descuento> descuento)
{
    std::string comando;
    if (descuento->descuento.getValue(0) != 0 &&
        (descuento->tipo_descuento == "monto" || descuento->tipo_descuento == "porcentaje"))
    {
        comando = "G";
        comando += (descuento->tipo_descuento == "monto") ? "M" : "P";
        comando += "-*";
        if (descuento->tipo_descuento == "monto")
            comando += this->fillNumbToString(descuento->descuento.getValue(0), 11, 2, true);
        else
            comando += this->fillNumbToString(descuento->descuento.getValue(0), 4, 2, true);

        prepararYenviarComando(comando);
    }
}

void HkaPrinter::reimprimir(PrinterDto *dat)
{
    std::string comando = "R";
    auto reimprimir = dat->reimprimir.get();

    if (reimprimir == nullptr)
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "ERROR OBJETO REIMPRIMIR ES NULL");

    if (reimprimir->doc.get() == nullptr)
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "ERROR OBJETO REIMPRIMIR INCOMPLETO");
    else
    {
        std::string doc = reimprimir->doc.getValue("").c_str();

        if (doc == "factura")
            comando += "F";
        else if (doc == "notaDebito")
            comando += "D";
        else if (doc == "notaCredito")
            comando += "C";
        else if (doc == "reporte_z")
            comando += "Z";
        else if (doc == "ultimo")
        {
            comando += "U";
            if (reimprimir->numb.get() != nullptr)
                throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "ERROR REIMPRIMIR ULTIMO NO PUEDE CONTENER EL CAMPO numb");
        }
    }
    if (reimprimir->doc.getValue("").c_str() != "ultimo" && reimprimir->numb.get() == nullptr)
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "ERROR OBJETO REIMPRIMIR INCOMPLETO");

    comando += this->fillNumbToString(reimprimir->numb.getValue(0), 7);
    comando += this->fillNumbToString(reimprimir->numb.getValue(0), 7);

    prepararYenviarComando(comando);
}

std::string HkaPrinter::checksumLRC(std::string ascii)
{
    int chkdec = 0;

    for (int i = 0; i < ascii.length(); i++)
    {
        if (int((unsigned char)ascii[i]) == 194)
            continue;

        chkdec ^= (int((unsigned char)ascii[i]));
        // std::cout<< ((int((unsigned char)ascii[i]))) <<"-";
    }

    std::string chkhex = this->int_to_hex(chkdec);

    string_to_upper(chkhex);
    // OATPP_LOGI("checksumLRC", "\t\tchkdec: %d\t\tchkhex: %s\t\tchkdec: %d\t\thex2ascii(chkhex) %s\t\t", chkdec, chkhex.c_str(), chkdec, this->hex2ascii(chkhex).c_str());
    return this->hex2ascii(chkhex);
}

StatusHka HkaPrinter::leerStatus()
{
    std::string comando = this->ENQ;
    std::string respuesta;

    respuesta = this->getSerialData(enviarComando(comando));
    StatusHka status;

    std::string hex = ascii_to_hex(respuesta);
    OATPP_LOGD("HEX ", "%s", hex.c_str());
    OATPP_LOGD("RESPUESTA ", "%s", respuesta.c_str());
    OATPP_LOGD("HEX.KENGHT", "%d", hex.length());

    if (hex.length() == 6)
    {
        status.sts1 = hex.substr(2, 2);
        status.sts2 = hex.substr(4, 2);

        OATPP_LOGD("sts1", "%d", status.sts1.c_str());
        OATPP_LOGD("sts2", "%d", status.sts2.c_str());
    }

    // for (int i = 0; i < hex.length(); i += 2)
    // {
    //     std::string part = hex.substr(i, 2);

    //     switch (i / 2)
    //     {
    //     case 0:
    //     {
    //         status.sts1 = part;
    //         break;
    //     }
    //     case 1:
    //     {
    //         status.sts2 = part;
    //         break;
    //     }
    //     default:
    //         break;
    //     }
    // }
    return status;
}

std::vector<std::string> HkaPrinter::getFlags()
{
    std::vector<std::string> flags;
    std::string comando, respuesta, aux;
    comando = "S3";
    respuesta = prepararYenviarComando(comando);
    auto campos = explode(respuesta, this->SEPARADOR[0]);
    int pos = 0;
    if (campos.size() == 5)
        pos = 3;
    else if (campos.size() == 6)
        pos = 4;
    OATPP_LOGD("CAMPOS.SIZE()", "%d", campos.size());
    for (size_t i = 0; i < campos.size(); i++)
    {
        OATPP_LOGD("campos[", "%d] %s", i, campos[i].c_str());
    }

    for (size_t i = 0; i < campos[pos].size(); i += 2)
    {
        aux = campos[pos].substr(i, 2);
        flags.push_back(aux);
    }

    OATPP_LOGI("Flags", " %d", flags.size());
    OATPP_ASSERT_HTTP(flags.size() >= 63, Status::CODE_400, "ERROR AL OBTENER FLAGS");
    return flags;
}

void HkaPrinter::setFlag()
{
    std::string comando, respuesta;
    comando = "PJ2130";
    respuesta = prepararYenviarComando(comando);

    comando = "PJ5001";
    respuesta = prepararYenviarComando(comando);

    comando = "PJ6317";
    respuesta = prepararYenviarComando(comando);
}

std::string HkaPrinter::items(oatpp::DTO::List<oatpp::Object<Item>>) { throw; }
std::string HkaPrinter::pagos(oatpp::DTO::List<oatpp::Object<Pago>> pagos) { throw; }

std::string HkaPrinter::subtotal(oatpp::DTO::List<oatpp::Object<Pago>> pagos)
{
    std::string comando, respuesta;
    comando = "3";
    respuesta = prepararYenviarComando(comando);
    return "";
}
void HkaPrinter::cabecera(oatpp::DTO::List<oatpp::String> cabecera)
{
    comentario(cabecera);
}
void HkaPrinter::pie(oatpp::DTO::List<oatpp::String> pie)
{
    comentario(pie);
}
void HkaPrinter::comentario(oatpp::DTO::List<oatpp::String> texto)
{
    std::string comando, info, linea;
    int n = 0;
    for (auto i = texto->begin(); i != texto->end() && n < 10; i++)
    {
        info = i->getValue("");
        linea = "";
        do
        {
            if (info.empty())
                continue;
            linea = info.substr(0, this->maxLength);

            comando = "i";
            comando += this->fillNumbToString(n++, 2, 0);
            comando += linea;

            comando = prepararYenviarComando(comando);

            if (info.size() > this->maxLength)
            {
                info = trim(info.substr(this->maxLength, std::string::npos));
            }
            else
            {
                info = "";
            }
        } while (!info.empty() && n < 10);
    }
}
std::string HkaPrinter::cerrarDocumentoFiscal() { throw; }
