#include "SewooPrinter.hpp"

std::map<std::string, std::string> StatusSewoo::StatusSewoo1 = {
    {"0", "Último comando ejecutado exitosamente"},
    {"257 ", "Comando inválido en el estado actual"},
    {"258 ", "Comando inválido en el documento actual"},
    {"259 ", "Jumper de programación conectado"},
    {"261 ", "Comando sólo aceptado con el jumper de servicio"},
    {"263 ", "Comando inválido"},
    {"265 ", "Falta un campo obligatorio en el comando"},
    {"272 ", "Longitud de un campo del comando inválida"},
    {"273 ", "Valor de un campo del comando inválido o fuera de rango"},
    {"274 ", "Tasa de impuesto inactiva"},
    {"514 ", "Mecanismo impresor fuera de línea"},
    {"516 ", "Mecanismo impresor sin papel"},
    {"517 ", "Tasa de baudios inválida"},
    {"769 ", "Error en la fiscalización"},
    {"770 ", "Error en comando de cambio de fecha"},
    {"771 ", "Fecha inválida"},
    {"1025", "Número de línea de encabezado/cola inválido"},
    {"1026", "RUC no puede ser modificado"},
    {"1281", "Memoria de transacciones llena"},
    {"1283", "Memoria de transacciones desconectada"},
    {"1284", "Error de lectura o escritura de Memoria de transacciones"},
    {"1285", "Memoria de transacciones inválida"},
    {"1537", "Comando inválido fuera de la jornada fiscal"},
    {"1538", "No se ha iniciado la jornada fiscal"},
    {"1539", "Memoria fiscal llena"},
    {"1540", "Memoria fiscal desconectada"},
    {"1541", "Memoria fiscal inválida"},
    {"1542", "Se requiere emitir un reporte Z"},
    {"1543", "No se encuentra el documento"},
    {"1544", "Periodo fiscal sin datos"},
    {"1545", "Periodo auditado sin datos"},
    {"1546", "No hay más datos disponibles"},
    {"1547", " No se pueden emitir 2 Z el mismo día"},
    {"1548", "No se pudo grabar Reporte Z"},
    {"1793", "Total debe ser mayor a cero"},
    {"2049", "Límite de líneas de comentario en documento fiscal"},
    {"2305", "Límite de líneas en documento no fiscal"},
    {"65535", "Error desconocido"}};

std::map<std::string, std::string> StatusSewoo::StatusSewoo2 = {
    {"0", "EN ESPERA"},
    {"1", "INICIO VENTA"},
    {"2", "VENTA"},
    {"3", "SUBTOTAL"},
    {"4", "PAGO"},
    {"5", "FIN VENTA"},
    {"6", "NO FISCAL"},
    {"7", "PROGRAMACIÓN"},
    {"8", "ERROR"},
    {"9", "INICIO DEVOLUCIÓN"},
    {"10", "DEVOLUCIÓN"}};

std::map<std::string, std::string> StatusSewoo::StatusSewoo3 = {
    // {"0", "MECANISMO DE IMPRESIÓN: FUERA DE LINEA"},
    {"1", "TAPA ABIERTA"},
    {"2", "SENSOR DE TEMPERATURA DEL CABEZAL: ALTA"},
    {"3", "ERROR NO RECUPERABLE: ERROR"},
    {"4", "CORTADORA DE PAPEL: ERROR"},
    {"5", "BUFFER OVERFLOW: ERROR"},
    {"6", "SENSOR DE FIN DE PAPEL: SIN PAPEL"},
    {"7", "SENSOR DE AUSENCIA DE PAPEL: SIN PAPEL"},
};

// SewooPrinter::SewooPrinter(std::string cport) : PrinterInterface::PrinterInterface(cport) {}
SewooPrinter::SewooPrinter(std::string cport, std::string config) : PrinterInterface::PrinterInterface(cport, config) {}

void SewooPrinter::refrescarConfiguracion()
{
    this->maxLength = 48;
    this->lineasTextoNF = 4;
}

std::string SewooPrinter::readPort()
{
    std::string bytes;
    int timeOut = 0;

    std::string error = this->NAK;
    std::string fin = this->END;
    std::string OK = this->ACK;
    bool buscando = true;

    while (buscando && timeOut < (25 + this->tiempoAdicional))
    {
        timeOut++;
        memset(this->buf, 0, 4095);
        int n = RS232_PollComport(this->cport_nr, this->buf, 4095);

        if (n > 0)
        {
            OATPP_LOGD("", "received %i bytes: %s", n, (char *)buf);
            bytes += std::string(buf, buf + n);
        }

        if (bytes.find(error) != std::string::npos ||
            bytes.find(fin) != std::string::npos ||
            bytes.find(OK) != std::string::npos)
        {
            buscando = false;
            OATPP_LOGD("", "\t\t\t\t (OK): %s \t\t\t (ERROR): %s \t\t\t (FIN): %s \t\t\t timeOut=%d\n",
                       (bytes.find(OK) != std::string::npos) ? "TRUE" : "false",
                       (bytes.find(error) != std::string::npos) ? "TRUE" : "false",
                       (bytes.find(fin) != std::string::npos) ? "TRUE" : "false",
                       timeOut);
        }
        else
        {
            this->sleep(150);
        }
    }

    return bytes;
}

std::string SewooPrinter::ejecutarComando(oatpp::DTO *dto)
{
    PrinterDto *data = ((PrinterDto *)dto);
    std::string resultado = "", cmd = data->cmd.getValue("");
    nlohmann::json jsonResponse;

    StatusSewoo status = leerStatus();

    if (!(status.sts3.find("OK") != std::string::npos))
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, status.sts3);

    if ((status.sts2.find("ERROR") != std::string::npos)) // ! TRY TO RESET
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, status.sts2);

    if (!(status.sts2.find("EN ESPERA") != std::string::npos) && !(status.sts2.find("NO FISCAL") != std::string::npos))
        anularDocumentoFiscal();

    if (status.sts2.find("NO FISCAL") != std::string::npos)
        cerrarDocumentoNoFiscal();

    if ("reporteX-ver" == cmd || "reporteX-imprimir" == cmd || "reporte_x" == cmd)
    {
        resultado = reporteX(data);
    }
    else if ("reporteZ-ver" == cmd || "reporteZ-imprimir" == cmd || "reporte_z" == cmd)
    {
        resultado = reporteZ();
    }
    else if ("facturar" == cmd)
    {
        resultado = facturar(data, true);
    }
    else if ("notaCredito" == cmd)
    {
        resultado = facturar(data, false);
    }
    else if ("textoNF" == cmd)
    {
        resultado = documentoNoFiscal(data);
    }
    else if ("status" == cmd)
    {
        status = leerStatus();
        jsonResponse["estado_fiscal"] = status.sts2;
        jsonResponse["estado_impresora"] = status.sts3;
    }
    else if ("fuente" == cmd)
    {
        configurarFuente();
    }
    else if ("fechaYhora" == cmd)
    {
        jsonResponse["fechaYhora"] = leerFechaYhora();
    }
    else if ("serial" == cmd)
    {
        jsonResponse["serial"] = leerSerial();
    }
    else if ("rif" == cmd)
    {
        jsonResponse["rif"] = leerRIF();
    }
    else
    {
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "Comando desconocido.");
    }
    resultado = resultado.empty() ? jsonResponse.dump() : resultado;
    return resultado;
}

void SewooPrinter::configurarFuente()
{
    std::string comando = ":230";
    prepararYenviarComando(comando);
}

std::string SewooPrinter::cerrarDocumentoFiscal()
{
    std::string comando = "M";
    prepararYenviarComando(comando);
    return "";
}

void SewooPrinter::anularDocumentoFiscal()
{
    std::string comando = "Q";
    prepararYenviarComando(comando);
}

std::string SewooPrinter::cerrarDocumentoNoFiscal()
{
    std::string comando = "T", respuesta;
    try
    {
        prepararYenviarComando(comando);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return "";
}

std::string SewooPrinter::reporteX(bool imprimir)
{
    std::string comando, resultado;
    comando = "X";

    this->tiempoAdicional = 100;
    try
    {
        resultado = prepararYenviarComando(comando);
        this->tiempoAdicional = 0;
    }
    catch (...)
    {
        this->tiempoAdicional = 0;
    }

    resultado = analizarReporteXZ("reporte_x", resultado);

    return resultado;
}

std::string SewooPrinter::reporteZ()
{
    std::string comando, resultado;
    comando = "Z";

    this->tiempoAdicional = 100;
    try
    {
        resultado = prepararYenviarComando(comando);
        this->tiempoAdicional = 0;
    }
    catch (...)
    {
        this->tiempoAdicional = 0;
    }

    resultado = analizarReporteXZ("reporte_z", resultado);
    return resultado;
}

std::string SewooPrinter::documentoNoFiscal(PrinterDto *dat)
{
    auto items = dat->textoNF.get();
    std::string texto, respuesta;

    std::string comando = "S"; // Abrir Documento no fiscal
    prepararYenviarComando(comando);

    for (auto i = items->begin(); i != items->end(); ++i)
    {
        lineaComentario(i->getValue(""));
    }

    respuesta = cerrarDocumentoNoFiscal();

    return "";
}

void SewooPrinter::lineaComentario(std::string texto)
{
    std::string comando = "V" + fillString(texto.substr(0, 48), 48);

    prepararYenviarComando(comando);
}

std::string SewooPrinter::leerFechaYhora()
{
    std::string comando, respuesta, resultado, anio, mes, dia, hora;
    comando = "f";
    respuesta = prepararYenviarComando(comando);

    if (respuesta.size() > 12)
    {
        anio = respuesta.substr(5, 4);
        mes = respuesta.substr(3, 2);
        dia = respuesta.substr(1, 2);
        hora = respuesta.substr(9, 4);

        resultado = anio + "/" + mes + "/" + dia;
        hora.insert(2, ":");
        resultado += " " + hora;
    }
    return resultado;
}

std::string SewooPrinter::facturar(PrinterDto *dat, bool facturar)
{
    std::string fechaTimestamp = leerFechaYhora();
    std::string resultado, comando = "L&";
    double tasaE, tasaP, tasaG, tasaA, tasaR;
    tasaE = tasaP = tasaG = tasaA = tasaR = 0;

    double baseImponible = 0, pDescuento = 0;
    double descc;

    try
    {
        comando += (facturar ? "1" : "2");
        bool imprimirCabecera = dat->factura->cliente->nombre.getValue("").size() <= maxLength;
        if (imprimirCabecera)
        {
            comando += this->SEPARADOR + fillString(dat->factura->cliente->nombre.getValue(""), (dat->factura->cliente->nombre.getValue("").size() > 34 ? dat->factura->cliente->nombre.getValue("").size() : 34));
            comando += this->SEPARADOR + fillString(dat->factura->cliente->rif.getValue(""), 14);
        }
        else // Si cadena es menor se imprime la cabecera
        {
            comando += this->SEPARADOR + fillString("", 34);
            comando += this->SEPARADOR + fillString("", 14);
        }

        std::string fecha;
        if (!facturar)
        {
            fecha = (dat->factura->notaFiscal->fecha.getValue(""));
            fecha.erase(std::remove(fecha.begin(), fecha.end(), '/'), fecha.end());
        }

        comando += this->SEPARADOR + (facturar ? this->NO_USADO : fecha);
        comando += this->SEPARADOR + (facturar ? this->NO_USADO : this->fillNumbToString(dat->factura->notaFiscal->factura.getValue(0), 8));
        comando += this->SEPARADOR + (facturar ? this->NO_USADO : (dat->factura->notaFiscal->serialFiscal.getValue("")));

        std::string respuesta = prepararYenviarComando(comando);

        if (imprimirCabecera)
        {
            cabecera(dat->factura->cabecera);
        }
        else
        {
            for (int i = dat->factura->cabecera->size() - 1; i >= 0; i--)
            {
                dat->factura->pie->push_front(dat->factura->cabecera[i].getValue(""));
            }
            dat->factura->pie->push_front("RIF: " + dat->factura->cliente->rif.getValue(""));
            dat->factura->pie->push_front("RAZON SOCIAL: " + dat->factura->cliente->nombre.getValue(""));
        }

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
            }
            for (auto &&i : *dat->factura->items)
            {
                if (i->producto.getValue("").find("SERVICIO (") == std::string::npos)
                {
                    double monto = (i->precio * pDescuento) / 100;
                    i->descuento = monto;
                    i->tipo_descuento = "monto";
                }
            }
        }

        auto item = dat->factura->items.get();
        for (auto i = item->begin(); i != item->end(); i++)
        {

            double precio = i->get()->precio.getValue(0) * i->get()->cantidad.getValue(0);
            if (i->get()->producto.getValue("").find("SERVICIO (") != std::string::npos)
            {
                if (pDescuento > 0)
                {
                    double descueento = 0;
                    descueento = ((i->get()->precio.getValue(0) * i->get()->cantidad.getValue(0)) * pDescuento) / 100;
                    precio = i->get()->precio.getValue(0) * i->get()->cantidad.getValue(0); //- descueento;
                    precio = precio - descueento;
                }
            }

            std::string comentario = i->get()->texto.getValue("");
            if (!comentario.empty())
            {
                for (size_t i = 0; i < comentario.size() && (i / this->maxLength) < this->lineasTextoNF; i += this->maxLength)
                    lineaComentario(comentario.substr(i, this->maxLength));
            }

            // Imprimir Cantidad como comentario
            if (i->get()->cantidad.getValue(0) != 1)
            {
                lineaComentario(numbToString(i->get()->cantidad.getValue(0)) + " X " + numbToString(i->get()->precio.getValue(0)));
            }

            std::string tasa = "0";

            if (i->get()->tasa == "E")
            {
                tasaE += precio;
                tasa = "0";
            }
            else if (i->get()->tasa == "G")
            {
                tasaG += precio;
                tasa = "1";
            }
            else if (i->get()->tasa == "R")
            {
                tasaR += precio;
                tasa = "2";
            }
            else if (i->get()->tasa == "A")
            {
                tasaA += precio;
                tasa = "3";
            }
            else if (i->get()->tasa == "P")
            {
                tasaP += precio;
                tasa = "4";
            }

            comando = "H1";
            comando += fillString(i->get()->producto.getValue(""), 20);
            comando += this->fillNumbToString(precio, 12, 2);
            comando += tasa;

            prepararYenviarComando(comando);
            double desc = descuento(i);

            // Descontar Valor del descuento a subtotales
            if (i->get()->tasa == "E")
                tasaE -= desc;
            else if (i->get()->tasa == "G")
                tasaG -= desc;
            else if (i->get()->tasa == "R")
                tasaR -= desc;
            else if (i->get()->tasa == "A")
                tasaA -= desc;
            else if (i->get()->tasa == "P")
                tasaP -= desc;
        }

        //    descuentoGlobal(tasaE, tasaG, tasaR, tasaA, tasaP, dat->factura->descuento);

        std::string rawSubtotal = subtotal(dat->factura->pagos);
        pagos(dat->factura->pagos);

        pie(dat->factura->pie);
        std::string nFactura = prepararYenviarComando("Y2");
        int nroFactura = 0;
        if (nFactura.size() > 1 && nFactura[0] == this->STX[0])
        {
            auto factuCampos = explode(nFactura, '*');
            if (factuCampos.size() > 28)
            {
                if (facturar)
                {
                    nroFactura = stoi(factuCampos[22]);
                }
                else
                {
                    nroFactura = stoi(factuCampos[28]);
                }
            }
        }
        OATPP_LOGW("NFACTURA", "%s", nFactura.c_str());
        cerrarDocumentoFiscal();
        resultado = analizarFactura(dat->cmd.getValue(""), nroFactura, fechaTimestamp, rawSubtotal);
    }
    catch (...)
    {
        prepararYenviarComando("Q");
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "HA OCURRIDO UN ERROR");
    }
    return resultado;
}

void SewooPrinter::descuentoGlobal(double tasaE, double tasaG, double tasaR, double tasaA, double tasaP, oatpp::Object<Descuento> descuento)
{
    double porcentaje = 0, subtotal, subtotal2 = 0;
    subtotal = tasaE + tasaG + tasaR + tasaA + tasaP;

    if (descuento == nullptr || descuento->tipo_descuento.getValue("").empty() || descuento->descuento.getValue(0) == 0)
        return;

    if (descuento->descuento.getValue(0) >= subtotal && descuento->tipo_descuento.getValue("") == "monto")
        return;

    if (descuento->tipo_descuento.getValue("") == "monto")
    {
        OATPP_LOGI("EL subtotal ES: ", " %F", subtotal);
        subtotal2 = subtotal - tasaE;
        OATPP_LOGI("EL subtotal2 ES: ", " %F", subtotal2);
        porcentaje = (descuento->descuento.getValue(0) * 100) / subtotal2;

        // descuento->tipo_descuento="porcentaje";
        // descuento->descuento=porcentaje;

        OATPP_LOGI("EL porcentaje ES: ", " %F", porcentaje);
    }
    else
        porcentaje = descuento->descuento.getValue(0);

    OATPP_LOGI("LAS TASAS 1:  -- ", "%F -- %F -- %F -- %F -- %F", tasaE, tasaG, tasaR, tasaA, tasaP);

    // Descuento por porcentaje
    tasaE = tasaE * porcentaje / 100;
    tasaG = tasaG * porcentaje / 100;
    tasaR = tasaR * porcentaje / 100;
    tasaA = tasaA * porcentaje / 100;
    tasaP = tasaP * porcentaje / 100;

    // tasaE=0;

    OATPP_LOGI("LAS TASAS 2:  -- ", "%F -- %F -- %F -- %F -- %F", tasaE, tasaG, tasaR, tasaA, tasaP);

    std::string comando = "\\", descripcion;

    descripcion = "DESCUENTO ";
    if (descuento->tipo_descuento.getValue("") == "porcentaje")
    {
        descripcion += "(" + numbToString(descuento->descuento.getValue(0)) + "%)";
    }

    comando += fillString(descripcion, 20);
    comando += fillNumbToString(tasaE, 12, 2);
    comando += fillNumbToString(tasaG, 12, 2);
    comando += fillNumbToString(tasaR, 12, 2);
    comando += fillNumbToString(tasaA, 12, 2);
    comando += fillNumbToString(tasaP, 12, 2);

    prepararYenviarComando(comando);
}

double SewooPrinter::descuento(std::list<oatpp::data::mapping::type::DTO::Object<Item>>::iterator i)
{
    double descuento = 0, precio, nuevoPrecio = 0;
    std::string comando;

    if (i->get()->descuento.getValue(0) != 0 &&
        (i->get()->tipo_descuento == "monto" || i->get()->tipo_descuento == "porcentaje"))
    {
        comando = "[0";
        std::string descripcion, tasa = "E";

        if (i->get()->tasa == "E")
            tasa = "0";
        else if (i->get()->tasa == "G")
            tasa = "1";
        else if (i->get()->tasa == "R")
            tasa = "2";
        else if (i->get()->tasa == "A")
            tasa = "3";
        else if (i->get()->tasa == "P")
            tasa = "4";

        descuento = i->get()->descuento.getValue(0);
        precio = i->get()->precio.getValue(0) * i->get()->cantidad.getValue(0);

        descripcion = "DESCUENTO ";

        if (i->get()->tipo_descuento == "porcentaje")
        {
            descripcion += "(" + numbToString(i->get()->descuento.getValue(0)) + "%)";
            nuevoPrecio = (precio * i->get()->descuento.getValue(0) / 100);
        }
        else
        {
            nuevoPrecio = i->get()->descuento.getValue(0);
        }

        if (precio - nuevoPrecio <= 0 || fillNumbToString(precio - nuevoPrecio, 10, 2) == "0000000000")
            return 0;

        comando += fillString(descripcion, 20);
        comando += this->fillNumbToString(nuevoPrecio, 10, 2);
        comando += tasa;

        prepararYenviarComando(comando);
    }
    return round2decimals(nuevoPrecio);
}

std::string SewooPrinter::items(oatpp::DTO::List<oatpp::Object<Item>> items) { throw; }

std::string SewooPrinter::subtotal(oatpp::DTO::List<oatpp::Object<Pago>> pagos)
{
    std::string comando, rawSubtotal;
    bool igtf = contieneIGTF(pagos);
    float amountIGTF = cantidadPagoIGTF(pagos);

    if (igtf)
    {
        comando = hex2ascii("C4") + "T" + fillNumbToString(amountIGTF, 14, 2);
    }
    else
    {
        comando = hex2ascii("C4") + "1";
    }

    rawSubtotal = prepararYenviarComando(comando);
    return rawSubtotal;
}

std::string SewooPrinter::pagos(oatpp::DTO::List<oatpp::Object<Pago>> pagos)
{
    std::string comando, rawSubtotal;
    bool igtf = contieneIGTF(pagos);
    float amountIGTF = cantidadPagoIGTF(pagos);

    if (pagos.get() != nullptr)
    {
        auto item = pagos.get();
        for (auto i = item->begin(); i != item->end(); i++)
        {
            if (i->get()->impuesto == nullptr || i->get()->impuesto->igtf == false)
            {
                comando = "a1";

                comando += fillString(i->get()->metodo.getValue(""), 20);
                comando += this->fillNumbToString(i->get()->monto, 14, 2);
                prepararYenviarComando(comando);
            }
        }
    }
    if (igtf)
    {
        comando = "a1DIVISA IGTF $       " + fillNumbToString(amountIGTF, 14, 2);
        prepararYenviarComando(comando);
    }

    return "";
}

void SewooPrinter::cabecera(oatpp::DTO::List<oatpp::String> cabecera)
{
    for (auto i = cabecera->begin(); i != cabecera->end(); i++)
    {
        if (!i->getValue("").empty())
        {
            lineaComentario(i->getValue(""));
        }
    }
    lineaComentario("");
    lineaComentario("Articulo                                   Monto");
    lineaComentario(fillString("", 48, '-'));
}

void SewooPrinter::pie(oatpp::DTO::List<oatpp::String> pie)
{
    std::string texto;
    int caracteresPorLinea = 48;
    int lineasMax = 10;
    for (auto i = pie->begin(); i != pie->end(); i++)
        texto += i->getValue("") + ". ";

    this->trim(texto);

    for (size_t i = 0; i < texto.size() && (i / caracteresPorLinea) < lineasMax; i += caracteresPorLinea)
        lineaComentario(trim(texto.substr(i, caracteresPorLinea)));
}

std::string SewooPrinter::prepararComando(std::string comando)
{
    return this->STX + comando + this->END;
}

std::string SewooPrinter::enviarComando(std::string comando)
{
    std::string respuesta;

    writePort(comando);

    respuesta = this->readPort();
    this->writeLog(comando, respuesta, "Sewoo");

    if (respuesta.empty())
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "SE HA PERDIDO COMUNICACIÓN CON EL DISPOSITIVO");

    if (respuesta.find(this->NAK) != std::string::npos)
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "SE HA ENCONTRADO UN ERROR AL ENVIAR EL COMANDO");

    return respuesta;
}

StatusSewoo SewooPrinter::leerStatus()
{
    StatusSewoo status;
    std::string comando;
    std::string respuesta;

    comando = "_";
    respuesta = prepararYenviarComando(comando);

    if (respuesta.size() == 11)
    {
        auto mapEstado = status.StatusSewoo2.find(respuesta.substr(7, 1));
        status.sts2 = (mapEstado == status.StatusSewoo2.end() ? "DESCONOCIDO" : mapEstado->second);
    }

    comando = "e";
    respuesta = prepararYenviarComando(comando);
    if (respuesta.size() == 7)
    {
        try
        {
            status.sts3 = respuesta.substr(1, 4);

            std::stringstream s3;
            s3 << std::hex << status.sts3;

            if (status.sts3 == "0000")
                status.sts3 = "OK";
            else
                status.sts3.clear();

            unsigned n3;
            s3 >> n3;

            std::bitset<16> b3(n3);

            std::string sb3 = b3.to_string();

            std::reverse(sb3.begin(), sb3.end());

            for (int i = 0; i < 16; i++)
            {
                if (sb3[i] == '1')
                {
                    auto it = status.StatusSewoo3.find(std::to_string(i));
                    if (it != status.StatusSewoo3.end())
                    {
                        if (status.sts3.find("SIN PAPEL") != std::string::npos && (it->second.find("SIN PAPEL") != std::string::npos))
                            continue;
                        status.sts3 += (it != status.StatusSewoo3.end()) ? it->second + "\n" : "";
                    }
                }
            }
        }
        catch (...)
        {
            OATPP_LOGE("ERROR", "ANALIZANDO STATUS3 (estado mecanismo impresor)");
        }
    }

    OATPP_LOGD("", "estatus fiscal: %s", this->trim(status.sts2).c_str());
    OATPP_LOGD("", "estatus impresora: %s", this->trim(status.sts3).c_str());

    return status;
}

std::string SewooPrinter::leerSerial()
{
    std::string comando = "\"";
    std::string respuesta;

    respuesta = prepararYenviarComando(comando);

    if (respuesta.length() > 10)
        if (respuesta[0] == this->STX[0])
        {
            respuesta = respuesta.substr(1, 10);
        }
        else
        {
            respuesta = respuesta.substr(0, 10);
        }
    else
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "NO SE HA PODIDO OBTENER EL SERIAL DEL DISPOSITIVO");

    return respuesta;
}

std::string SewooPrinter::leerRIF()
{
    std::string comando = "&";
    std::string respuesta;

    respuesta = prepararYenviarComando(comando);
    auto datosFiscales = explode(respuesta, this->SEPARADOR[0]);

    if (datosFiscales.size() == 6)
    {
        auto item = datosFiscales.begin();
        std::advance(item, 2);

        respuesta = item->c_str();
    }

    return respuesta;
}
std::string SewooPrinter::analizarFactura(std::string cmd, std::string fecha, std::string rawSubtotal) { throw; }

std::string SewooPrinter::analizarFactura(std::string cmd, int nroFactura, std::string fecha, std::string rawSubtotal)
{
    nlohmann::json respuesta;
    try
    {
        if (rawSubtotal.size() > 23)
        {
            rawSubtotal = rawSubtotal.substr(1, rawSubtotal.size() - 3);
            auto rawCamposSubtotal = explode(rawSubtotal, this->SEPARADOR[0]);

            std::vector<std::string> camposSubtotal = {
                {"subtotal"},
                {"total_venta_e"},
                {"total_venta_g"},
                {"total_impuesto_g"},
                {"total_venta_r"},
                {"total_impuesto_r"},
                {"total_venta_a"},
                {"total_impuesto_a"},
                {"no_usado"}, // cantidad items
                {"total_venta_p"},
                {"total_igtf"}};

            int i = 0;
            if (rawCamposSubtotal.size() == 11)
                for (auto it = rawCamposSubtotal.begin(); it != rawCamposSubtotal.end(); ++it, i++)
                {
                    try
                    {
                        if (camposSubtotal[i] != "no_usado")
                        {
                            respuesta["factura"]["subtotales"][camposSubtotal[i]] = round2decimals(stof(std::string(it->c_str())) / 100.);
                        }
                    }
                    catch (const std::exception &e)
                    {
                        OATPP_LOGE("Analizar Factura", " %s: %s", camposSubtotal, e.what());
                    }
                }
        }
        respuesta["factura"]["nro"] = nroFactura;
        respuesta["cmd"] = cmd;
        respuesta["serialFiscal"] = this->device->serial;
        respuesta["fecha"] = fecha;
        respuesta["rawSubtotal"] = rawSubtotal;
    }
    catch (std::exception &e)
    {
        OATPP_LOGE("ERROR ", "analizarFactura %s", e.what());
    }
    return respuesta.dump();
}

std::string SewooPrinter::analizarReporteXZ(std::string cmd, std::string rawReporteXZ)
{
    nlohmann::json respuesta;

    respuesta["cmd"] = cmd;
    respuesta["rawResponse"] = rawReporteXZ;

    return respuesta.dump();
}
