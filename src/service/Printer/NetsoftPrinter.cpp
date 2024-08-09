#include "NetsoftPrinter.hpp"

std::map<std::string, std::string> NetsoftPrinter::error = {
    {"00", "ERRORES EN CAMPO DE DATOS"},
    {"01", "ERROR EN CAMPO 1"},
    {"02", "ERROR EN CAMPO 2"},
    {"03", "ERROR EN CAMPO 3"},
    {"04", "ERROR EN CAMPO 4"},
    {"05", "ERROR EN CAMPO 5"},
    {"06", "ERROR EN CAMPO 6"},
    {"07", "ERROR EN CAMPO 7"},
    {"08", "ERROR EN CAMPO 8"},
    {"09", "ERROR EN CAMPO 9"},
    {"A", "ERROR EN CAMPO 10"},
    {"B", "ERROR EN CAMPO 11"},
    {"C", "ERROR EN CAMPO 12"},
    {"D", "ERROR EN CAMPO 13"},
    {"E", "ERROR EN CAMPO 14"},
    {"F", "ERROR EN CAMPO 15"},
    {"10", "ERROR EN CAMPO 16"},
    {"11", "ERROR EN CAMPO 17"},
    {"12", "ERROR EN CAMPO 18"},
    {"13", "ERROR EN CAMPO 19"},
    {"14", "ERRORES BUS I2C EEPROM"},
    {"15", "ERROR LINEA I2C RTC"},
    {"19", "ERROR RTC NO AVANZA"},
    {"1A", "ERROR DESBORDAMIENTO DIRECCION MF"},
    {"1B", "ERROR EN SERVIDOR WIFI"},
    {"1C", "ERROR INICIANDO SERVIDOR WIFI"},
    {"1D", "ERROR SERVIDOR WIFI OCUPADO CON OTRO CLIENTE"},
    {"20", "SECUENCIA"},
    {"21", "ERROR COMANDO NO VALIDO PARA ESTADO FISCAL"},
    {"22", "ERROR COMANDO NO VALIDO EN MODO ENTRENAMIENTO"},
    {"23", "ERROR COMANDO NO VALIDO WIFI NO CONECTADA"},
    {"24", "ERROR COMANDO NO VALIDO MODO WIFI ACTIVADO"},
    {"25", "ERROR TIMEOUT COMUNICACION WIFI"},
    {"26", "ERROR TIMEOUT COMUNICACIÓN SERIAL"},
    {"28", "ERROR TIMEOUT EN COMUNICACIÓN"},
    {"2A", "ERROR DE COMUNICACIÓN CON LA IMPRESORA"},
    {"32", "ERROR FECHA HORA YA CAMBIADA EN PERIODO ACTUAL"},
    {"33", "ERROR EXISTEN VENTAS EN EL PERIODO ACTUAL"},
    {"34", "ERROR FECHA HORA ANTERIOR AL ULTIMO REPORTE Z EMITIDO"},
    {"35", "ERROR FECHA HORA MAYOR EN MAS DE UN DIA A ULTIMO REPORTE Z EMITIDO"},
    {"36", "ERROR FECHA HORA MENOR A LA ULTIMA FACTURA EMITIDA"},
    {"37", "ERROR FECHA HORA MENOR A LA ULTIMA NOTA DE CREDITO EMITIDA"},
    {"38", "ERROR FECHA HORA MENOR AL ULTIMO DOCUMENTO NO FISCAL EMITIDO"},
    {"3C", "ERROR EN DESCRIPCION DEL ITEM"},
    {"3D", "ERROR EN CANTIDAD ITEM"},
    {"3E", "ERROR CANTIDAD ITEM CERO"},
    {"3F", "ERROR EN MONTO DE ITEM"},
    {"47", "DESBORDE DE TOTALES"},
    {"48", "DESBORDE DE ACUMULADORES EN REPORTE Z"},
    {"49", "PORCENTAJE DESCUENTO MAYOR CIEN"},
    {"4A", "DIVISION ENTRE CERO"},
    {"4B", "DESCRIPTOR DE IVA REPETIDO"},
    {"4C", "DESBORDE DE TOTAL IVA"},
    {"4D", "DESBORDE DE ACUMULADORES IVA EN REPORTE Z"},
    {"4E", "DESBORDE DE TOTAL ACUMULADOR EN REPORTE Z"},
    {"5B", "LIMITE DE MEMORIA FISCAL"},
    {"5C", "ERROR DIRECCIONAMIENTO MEMORIA DE TRABAJO"},
    {"5D", "LIMITE MEMORIA AUDITORA"},
    {"5F", "ERROR EN TRAMA DE DATOS (BCC)"},
    {"60", "TRAMA CON ESC"},
    {"62", "EQUIPO SIN FISCALIZAR"},
    {"63", "ERROR EN SUBTOTAL"},
    {"65", "SE NECESITAN LOS DATOS DEL CLIENTE"},
    {"66", "ERROR EN FORMATO DE CORREO ELECTRÓNICO"},
    {"68", "ES NECESARIO UN REPORTE Z"},
    {"6E", "ERROR EN TEXTO"},
    {"6F", "MAXIMA CANTIDAD DE LINEAS DE TEXTO NO FISCAL"},
    {"70", "ERROR EN TEXTO FISCAL"},
    {"71", "ERROR EN TEXTO NO FISCAL"},
    {"79", "TASA DE IMPUESTO NO VALIDA"},
    {"7A", "MONTO ITEM CERO"},
    {"7B", "TASAS DE IVA NO CONFIGURADAS"},
    {"7C", "DESBORDE DE CANTIDAD POR MONTO"},
    {"7D", "ERROR MONTO IVA CERO"},
    {"7E", "ERROR MONTO MIN ARTICULO"},
    {"7F", "ERROR FALLA IMPRESORA"},
    {"80", "ERROR IMPRESORA SIN PAPEL"},
    {"81", "ERROR IMPRESORA TAPA ABIERTA"},
    {"82", "ERROR MONTO IGTF CERO"},
    {"83", "SUBTOTAL CERO"},
    {"8F", "EMITIENDO UN PAGO EN UNA DEVOLUCION"},
    {"90", "ERROR FACTURA PAGADA"},
    {"91", "ERROR DATOS DEVOLUCION"},
    {"97", "NO SE ENCONTRO REPORTE EN LAS FECHAS SOLICITADAS"},
    {"98", "NO SE ENCONTRO REPORTE CON LOS NUMEROS SOLICITADOS"},
    {"9A", "ERROR RPT MEMORIA FISCAL"},
    {"9B", "ERROR RPT MEMORIA AUDITORA"},
    {"9C", "ERROR RPT MF NO EXISTEN REPORTES CON LOS PARAMETROS"},
    {"9D", "ERROR RPT MA NO EXISTEN DOCUMENTOS CON LOS PARAMETROS"},
    {"A1", "PERIODO NO VALIDO. FALLA RESPECTO A LA RAM"},
    {"A2", "PERIODO NO VALIDO. FALLA RESPECTO A LA ROM"},
    {"BD", "PAGO INVALIDO"},
    {"BE", "CORTA PAPEL"},
    {"BF", "ERROR EN COMANDO"},
    {"C7", "EQUIPO FISCALIZADO"},
    {"C8", "ERROR EN DATOS DE FISCALIZACION"},
    {"C9", "ERROR EN DATOS DE MEMORIA DE TRABAJO"},
    {"CA", "ERROR LEYENDO MEMORIA DE TRABAJO"},
    {"CB", "ERROR FISICO MEMORIA AUDITORA"},
    {"CC", "ERROR FISICO MEMORIA REPORTE MEMORIA FISCAL"},
    {"CD", "ERROR FISICO MEMORIA ESCRIBIENDO REPORTE MEMORIA FISCAL"},
    {"CE", "ERROR LEYENDO ESCRIBIENDO MEMORIA DE TRABAJO"},
    {"CF", "ERROR FISICO MEMORIA DE TRABAJO"},
    {"D0", "ERROR DATOS MEM MA"},
    {"D2", "ERROR INICIALIZANDO MEMORIA DE TRABAJO"},
    {"D3", "ERROR DATOS CAMBIO TASAS IVA"},
    {"DC", "ERROR ABRIENDO MEMORIA AUDITORA"},
    {"DD", "ERROR ESCRIBIENDO EN MEMORIA AUDITORA"},
    {"DE", "ERROR LEYENDO MEMORIA AUDITORA"},
    {"DF", "ERROR LECTURA ESCRITURA MEMORIA AUDITORA"},
    {"E1", "ERROR ESCRIBIENDO FIN EN MEMORIA AUDITORA"},
    {"E6", "ERROR LEYENDO MEMORIA REPORTES FISCALES"},
    {"EB", "ERROR LEYENDO MEMORIA REPORTES AUDITORES"},
    {"F3", "ERROR LECTURA ESCRITURA REPORTE MEMORIA FISCAL"},
    {"FA", "ERROR LECTURA REPORTE MEMORIA AUDITORA FACTURAS"},
    {"FB", "ERROR LECTURA REPORTE MEMORIA AUDITORA NOTAS DE CREDITO"},
    {"FC", "ERROR LECTURA REPORTE MEMORIA AUDITORA DOCUMENTOS NO FISCALES"},
    {"FD", "ERROR ESCRITURA REPORTE MEMORIA AUDITORA FACTURAS"},
    {"FE", "ERROR ESCRITURA REPORTE MEMORIA AUDITORA NOTAS DE CREDITO"},
    {"FF", "ERROR ESCRITURA REPORTE MEMORIA AUDITORA DOCUMENTOS NO FISCALES"}};

std::map<std::string, std::string> StatusNetsoft::StatusNetsoft1 = {
    {"00", "Impresora lista para abrir una factura, abrir un documento no fiscal, hacer un reporte Z o un reporte de memoria fiscal"},
    {"01", "Factura fiscal en curso. Esperando por un ítem, cerrar/cancelar la factura. Solo se admitieran comandos relacionados al la factura fiscal"},
    {"02", "Documento no fiscal en curso. Esperando por línea de texto, cerrar el documento. Solo se admitieran comandos relacionados a documentos no fiscales"},
    {"03", "SLIP activo. Solo se admitieran comandos relacionados a documentos no fiscales o comandos para el formato de cheques"},
    {"04", "Mas de un día desde el último reporte Z. Es necesario un reporte Z. Para poder realizar una venta se deberá efectuar previamente un reporte Z."},
    {"05", "Primeras líneas descriptivas de una factura fiscal impresas"},
    {"08", "Equipo bloqueado a la espera de impresión de cierre Z. Esto ocurre solo en caso de producirse un error durante la impresión de un cierre Z. Por ejemplo, si se acaba el papel cuando se está imprimiendo un cierre Z. Se debe hacer un RESET al equipo."},
    {"10", "Error critico. Error en BCC RAM. Es necesaria la intervención del servicio técnico"},
    {"11", "Error critico. Error en BCC ROM. Es necesaria la intervención del servicio técnico"},
    {"12", "Error critico. Error de formato de FECHA en RAM. Es necesaria la intervención del servicio técnico."},
    {"13", "Error critico. Error de formato de datos al realizar un Z. Es necesaria la intervención del servicio técnico."},
    {"14", "Error critico. Limite de memoria fiscal. Es necesaria la intervención del servicio técnico."}};

std::map<std::string, std::string> StatusNetsoft::StatusNetsoft2 = {
    {"1", "Tapa abierta"},
    {"2", "Error y/o falla de impresora"},
    {"3", "Impresora fuera de línea"},
    {"14", "Impresora sin papel para imprimir"}};

std::map<std::string, std::string> StatusNetsoft::StatusNetsoft3 = {
    {"0", "Error de comprobación de memoria fiscal."},
    {"1", "Error de comprobación de memoria de trabajo"},
    {"2", "Error de comprobación de memoria de auditoria"},
    {"3", "Comando no reconocido."},
    {"4", "Campo de datos invalido"},
    {"5", "Comando no válido para estado fiscal"},
    {"6", "Desbordamiento de totales"},
    {"7", "Memoria fiscal llena"},
    {"8", "Memoria fiscal casi llena"},
    {"9", "Error de comprobación RTC"},
    {"10", "Memoria de auditoria llena"},
    {"11", "Es necesario hacer un cierre de la jornada fiscal (REPORTE Z)."},
    {"12", "Factura fiscal abierta"},
    {"13", "Documento no fiscal abierto"}};

NetsoftPrinter::NetsoftPrinter(std::string cport) : PrinterInterface::PrinterInterface(cport)
{
}

void NetsoftPrinter::refrescarConfiguracion()
{
    this->maxLength = 34;
    this->lineasTextoNF = 4;
}

std::string NetsoftPrinter::readPort()
{
    std::string bytes;
    int timeOut = 0;

    std::string error = "ERROR";
    std::string fin = this->END;

    bool buscando = true;
    while (buscando && timeOut < (25 + this->tiempoAdicional))
    {
        timeOut++;
        memset(this->buf, 0, 4095);
        int n = RS232_PollComport(this->cport_nr, this->buf, 4095);

        if (n > 0)
        {
            OATPP_LOGD("", "received %i bytes: %s", n, (char *)buf);
            bytes += std::string((char *)buf, n);
        }

        if (bytes.find(error) != std::string::npos ||
            bytes.find(fin) != std::string::npos)
        {
            buscando = false;
            OATPP_LOGD("", "\t\t\t (ERROR): %s \t\t\t (FIN): %s \t\t\t timeOut=%d\n",
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

std::string NetsoftPrinter::ejecutarComando(oatpp::DTO *dto)
{
    PrinterDto *data = ((PrinterDto *)dto);
    std::string resultado, cmd = data->cmd.getValue("");

    StatusNetsoft status = leerStatus();

    if (status.sts1.find("Error critico") != std::string::npos)
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, status.sts1);

    //! NO SE PUEDE CONFIAR EN ESTOS STATUS. SON LOS ESTADOS DEL COMANDO ANTERIOR
    // if (!status.sts2.empty())
    //     throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "ESTATUS IMPRESORA: " + status.sts2);

    // if (!status.sts3.empty())
    //     throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "ESTATUS FISCAL: " + status.sts3);

    if (status.sts1.find("Factura fiscal en curso") != std::string::npos)
        anularDocumentoFiscal();

    if (status.sts1.find("Documento no fiscal en curso") != std::string::npos)
        cerrarDocumentoNoFiscal();

    nlohmann::json jsonResponse;

    if ("reporteX-imprimir" == cmd || "reporte_x" == cmd)
    {
        resultado = reporteX(true);
    }
    else if ("reporteZ-imprimir" == cmd || "reporte_z" == cmd)
    {
        resultado = reporteZ();
    }
    else if ("fechaYhora" == cmd)
    {
        jsonResponse["fechaYhora"] = leerFechaYhora();
    }
    else if ("reset" == cmd)
    {
        reset();
    }
    else if ("status" == cmd)
    {
        status = leerStatus();
        nlohmann::json statusJson;
        std::string ss = (!status.sts1.empty()) ? "STATUS: " + status.sts1 : "";
        ss += (!status.sts2.empty()) ? "\nIMPRESORA: " + status.sts2 : "";
        ss += (!status.sts3.empty()) ? "\nFISCAL: " + status.sts3 : "";
        statusJson["status"] = ss;
        resultado = statusJson.dump();
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
    else if ("serial" == cmd)
    {
        jsonResponse["serial"] = leerSerial();
    }
    else if ("rif" == cmd)
    {
        jsonResponse["rif"] = leerRIF();
    }
    else if ("imprimirProgramacion" == cmd)
    {
        imprimirProgramacion();
    }
    else
    {
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "Comando desconocido.");
    }

    resultado = resultado.empty() ? jsonResponse.dump() : resultado;
    return resultado;
}

std::string NetsoftPrinter::prepararComando(std::string comando)
{
    comando = this->STX + this->getSecuencia() + comando + this->END;

    std::string chk = this->checksumBCC(comando);
    comando += chk;

    return comando;
}

std::string NetsoftPrinter::enviarComando(std::string comando)
{
    std::string respuesta;
    writePort(comando);

    respuesta = this->readPort();
    this->writeLog(comando, respuesta, "NETSOFT");

    if (respuesta.empty())
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "SE HA PERDIDO COMUNICACIÓN CON EL DISPOSITIVO");

    if (respuesta.find("ERROR") != std::string::npos)
    {
        std::string err = obtenerError(respuesta);
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, err);
    }
    return respuesta;
}

std::string NetsoftPrinter::facturar(PrinterDto *dat, bool facturar)
{
    std::string fechaTimestamp = leerFechaYhora();
    std::string resultado, comando = "@";
    double tasaE, tasaP, tasaG, tasaA, tasaR;
    tasaE = tasaP = tasaG = tasaA = tasaR = 0;

    try
    {
        bool imprimirCabecera = dat->factura->cliente->nombre.getValue("").size() <= maxLength;
        if (imprimirCabecera)
        {
            comando += this->SEPARADOR + dat->factura->cliente->nombre.getValue("").substr(0, 34); // campo 1
            comando += this->SEPARADOR + dat->factura->cliente->rif.getValue("").substr(0, 12);    // campo 2
        }
        else // Si cadena es menor se imprime la cabecera
        {
            comando += this->SEPARADOR + ""; // campo 1
            comando += this->SEPARADOR + ""; // campo 2
        }

        comando += this->SEPARADOR + (facturar ? this->NO_USADO : this->fillNumbToString(dat->factura->notaFiscal->factura.getValue(0), 8, 0)); // campo 3
        comando += this->SEPARADOR + (facturar ? this->NO_USADO : (dat->factura->notaFiscal->serialFiscal.getValue("")));                       // campo 4                                                                   // campo 4
        comando += this->SEPARADOR + (facturar ? this->NO_USADO : (this->formatearFecha(dat->factura->notaFiscal->fecha.getValue(""), false))); // campo 5
        comando += this->SEPARADOR + (facturar ? this->NO_USADO : (dat->factura->notaFiscal->hora.getValue("")));                               // campo 6                                                            // campo 6
        comando += this->SEPARADOR + (facturar ? "C" : "D");                                                                                    // campo 7
        comando += this->SEPARADOR + this->NO_USADO;                                                                                            // campo 8
        comando += this->SEPARADOR + this->NO_USADO;                                                                                            // campo 9

        prepararYenviarComando(comando);

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

        for (auto i = dat->factura->items->begin(); i != dat->factura->items->end(); i++)
        {
            double precio = i->get()->precio.getValue(0);
            if (i->get()->tasa == "E")
            {
                OATPP_LOGI("PRECIO TASA E: ","%f", precio);
                tasaE += precio;
            }
            else if (i->get()->tasa == "G")
            {
                OATPP_LOGI("PRECIO TASA G: ","%f", precio);
                tasaG += precio;
            }
            else if (i->get()->tasa == "R")
            {
                tasaR += precio;
            }
            else if (i->get()->tasa == "A")
            {
                tasaA += precio;
            }
            else if (i->get()->tasa == "P")
            {
                tasaP += precio;
            }

            float porcentaje = i->get()->descuento.getValue(0);
            if (i->get()->descuento.getValue(0) > 0)
            {
                std::stringstream descuento;
                descuento << "DESCUENTO ";
                descuento.precision(2);

                if (i->get()->tipo_descuento.getValue("") == "monto")
                {
                    porcentaje = porcentaje * 100 / i->get()->precio;
                    descuento << "(Bs " << i->get()->descuento.getValue(0) << ")";
                }
                else
                {
                    descuento << "(" << i->get()->descuento.getValue(0) << "%)";
                }

                precio -= precio * porcentaje / 100;
                textoFiscal(descuento.str());
            }

            comando = "B";
            comando += this->SEPARADOR + i->get()->producto.getValue("").substr(0, 31);    // campo 1
            comando += this->SEPARADOR + this->fillNumbToString(i->get()->cantidad, 7, 3); // campo 2
            comando += this->SEPARADOR + this->fillNumbToString(precio, 16, 2);            // campo 3
            comando += this->SEPARADOR + i->get()->tasa.getValue("");                      // campo 4
            comando += this->SEPARADOR + "M";                                              // campo 5
            comando += this->SEPARADOR + this->NO_USADO;                                   // campo 6
            comando += this->SEPARADOR + ((facturar) ? this->NO_USADO : "D");              // campo 7
            comando += this->SEPARADOR + this->NO_USADO;                                   // campo 8
            comando += this->SEPARADOR + this->NO_USADO;                                   // campo 9

            prepararYenviarComando(comando);

            std::string comentario = i->get()->texto.getValue("");
            if (!comentario.empty())
            {
                for (size_t i = 0; i < comentario.size() && (i / this->maxLength) < this->lineasTextoNF; i += this->maxLength)
                    textoFiscal(comentario.substr(i, this->maxLength));
            }
        }

        descuentoGlobal(tasaE, tasaG, tasaR, tasaA, tasaP, dat->factura->descuento);

        std::string rawSubtotal = subtotal(dat->factura->pagos);
        pagos(dat->factura->pagos);

        cierreParcialDocumentoFiscal();
        pie(dat);
        std::string cierre = cerrarDocumentoFiscal();
        resultado = analizarFactura(dat->cmd.getValue(""), cantidadPagoIGTF(dat->factura->pagos), fechaTimestamp, rawSubtotal, cierre);
    }
    catch (...)
    {
        try
        {
            anularDocumentoFiscal();
        }
        catch (const std::exception &e)
        {
            OATPP_LOGE("ERROR ABORTANDO FACTURA: ", "%s", e.what());
        }
        throw;
    }
    return resultado;
}

std::string NetsoftPrinter::items(oatpp::DTO::List<oatpp::Object<Item>> items) { throw; }

std::string NetsoftPrinter::subtotal(oatpp::DTO::List<oatpp::Object<Pago>>)
{
    std::string respuesta, comando = "C";

    comando += this->SEPARADOR + this->NO_USADO;
    comando += this->SEPARADOR + this->NO_USADO;

    respuesta = prepararYenviarComando(comando);
    return respuesta;
}

std::string NetsoftPrinter::pagos(oatpp::DTO::List<oatpp::Object<Pago>> pagos)
{
    std::string comando, respuesta;

    if (pagos.get() != nullptr)
    {
        for (auto pago = pagos->begin(); pago != pagos->end(); pago++)
        {

            bool ifgt = pago->get()->impuesto.get() != nullptr && pago->get()->impuesto->igtf;
            if (ifgt)
            {
                comando = "D";
                comando += this->SEPARADOR + pago->get()->metodo.getValue("").substr(0, 31);          // Campo 1
                comando += this->SEPARADOR + fillNumbToString(pago->get()->monto.getValue(0), 15, 2); // Campo 2
                comando += this->SEPARADOR + (ifgt ? "I" : "T");                                      // Campo 3
                comando += this->SEPARADOR + (ifgt ? "I" : "E");                                      // Campo 4
                prepararYenviarComando(comando);
            }
        }
    }

    return respuesta;
}

void NetsoftPrinter::descuentoGlobal(double tasaE, double tasaG, double tasaR, double tasaA, double tasaP, oatpp::Object<Descuento> descuento)
{
    double porcentaje = 0, subtotal, subtotal2=0;
    subtotal = tasaE + tasaG + tasaR + tasaA + tasaP;

    OATPP_LOGI("EL SUBTOTAL ES: "," %F",subtotal);
    Tools::printDTO(descuento);

    if (descuento == nullptr || descuento->tipo_descuento.getValue("").empty() || descuento->descuento.getValue(0) == 0)
        return;

    if (descuento->descuento.getValue(0) >= subtotal && descuento->tipo_descuento.getValue("") == "monto")
        return;

    if (descuento->tipo_descuento.getValue("") == "monto")
    {
        subtotal2= subtotal - tasaE;
        porcentaje = (descuento->descuento.getValue(0) * 100) / subtotal2;

        OATPP_LOGI("EL porcentaje ES: "," %F",porcentaje);

        // Descuento por porcentaje
        tasaE = tasaE * porcentaje / 100;
        tasaG = tasaG * porcentaje / 100;
        tasaR = tasaR * porcentaje / 100;
        tasaA = tasaA * porcentaje / 100;
        tasaP = tasaP * porcentaje / 100;
    }
    else
    {
        tasaE = tasaG = tasaR = tasaA = tasaP = descuento->descuento.getValue(0);
    }

    std::string comando, descripcion;

    descripcion = "DESCUENTO ";
    if (descuento->tipo_descuento.getValue("") == "porcentaje")
    {
        descripcion += "(" + numbToString(descuento->descuento.getValue(0)) + "%)";
    }

    comando += fillString(descripcion, 20);
    char tipoDescuento = descuento->tipo_descuento.getValue("") == "monto" ? 'D' : 'P';
    comando = "D";
    comando += this->SEPARADOR + descripcion;                    // Campo 1
    comando += this->SEPARADOR + fillNumbToString(tasaE, 15, 2); // Campo 2
    comando += this->SEPARADOR + tipoDescuento;                  // Campo 3
    comando += this->SEPARADOR + "E";                            // Campo 4
    if (tasaE > 0.0)
        prepararYenviarComando(comando);

    comando = "D";
    comando += this->SEPARADOR + descripcion;                    // Campo 1
    comando += this->SEPARADOR + fillNumbToString(tasaR, 15, 2); // Campo 2
    comando += this->SEPARADOR + tipoDescuento;                  // Campo 3
    comando += this->SEPARADOR + "R";                            // Campo 4
    if (tasaR > 0.0)
        prepararYenviarComando(comando);

    comando = "D";
    comando += this->SEPARADOR + descripcion;                    // Campo 1
    comando += this->SEPARADOR + fillNumbToString(tasaA, 15, 2); // Campo 2
    comando += this->SEPARADOR + tipoDescuento;                  // Campo 3
    comando += this->SEPARADOR + "A";                            // Campo 4
    if (tasaA > 0.0)
        prepararYenviarComando(comando);

    comando = "D";
    comando += this->SEPARADOR + descripcion;                    // Campo 1
    comando += this->SEPARADOR + fillNumbToString(tasaG, 15, 2); // Campo 2
    comando += this->SEPARADOR + tipoDescuento;                  // Campo 3
    comando += this->SEPARADOR + "G";                            // Campo 4
    if (tasaG > 0.0)
        prepararYenviarComando(comando);

    comando = "D";
    comando += this->SEPARADOR + descripcion;                    // Campo 1
    comando += this->SEPARADOR + fillNumbToString(tasaP, 15, 2); // Campo 2
    comando += this->SEPARADOR + tipoDescuento;                  // Campo 3
    comando += this->SEPARADOR + "P";                            // Campo 4
    if (tasaP > 0.0)
        prepararYenviarComando(comando);
}

void NetsoftPrinter::cabecera(oatpp::DTO::List<oatpp::String> cabecera)
{
    for (auto i = cabecera->begin(); i != cabecera->end(); i++)
    {
        textoFiscal(i->getValue(""));
    }
    if (!cabecera->empty())
        textoFiscal(".");
}

void NetsoftPrinter::pie(PrinterDto *dto)
{
    std::string texto;
    int caracteresPorLinea = 64;
    int lineasMax = 20;
    int ii = 0;

    try
    {
        for (auto &&pago : *dto->factura->pagos)
        {
            if (ii++ >= lineasMax)
                break;

            std::string pagoStr = "Bs " +
                                  fillNumbToString(pago->monto.getValue(0.0), 0, 2, true);

            textoFiscal(pago->metodo.getValue("") +
                        std::string((caracteresPorLinea - (pagoStr.size() + pago->metodo.getValue("").size())), ' ') +
                        pagoStr);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    for (auto i = dto->factura->pie->begin(); i != dto->factura->pie->end(); i++)
        texto += i->getValue("") + ". ";

    this->trim(texto);
    for (size_t i = 0; i < texto.size() && (i / caracteresPorLinea) < lineasMax; i += caracteresPorLinea)
    {
        if (ii++ >= lineasMax)
            break;
        textoFiscal(trim(texto.substr(i, caracteresPorLinea)));
    }
}

void NetsoftPrinter::pie(oatpp::DTO::List<oatpp::String> pie)
{
    std::string texto;
    int caracteresPorLinea = 64;
    int lineasMax = 6;
    for (auto i = pie->begin(); i != pie->end(); i++)
        texto += i->getValue("") + ". ";

    this->trim(texto);

    for (size_t i = 0; i < texto.size() && (i / caracteresPorLinea) < lineasMax; i += caracteresPorLinea)
        textoFiscal(trim(texto.substr(i, caracteresPorLinea)));

    try
    {
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

void NetsoftPrinter::cierreParcialDocumentoFiscal()
{
    std::string comando = "E";

    comando += this->SEPARADOR + "A"; // campo 1

    std::string result;
    result = prepararYenviarComando(comando);
}

std::string NetsoftPrinter::cerrarDocumentoFiscal()
{
    std::string comando = "E";

    comando += this->SEPARADOR + "E"; // campo 1

    std::string result;
    result = prepararYenviarComando(comando);
    return result;
}

void NetsoftPrinter::anularDocumentoFiscal()
{
    std::string comando;

    comando = "D";
    comando += this->SEPARADOR + "ANULADO";                  // Campo 1
    comando += this->SEPARADOR + fillNumbToString(0, 15, 2); // Campo 2
    comando += this->SEPARADOR + "C";                        // Campo 3
    comando += this->SEPARADOR + "E";                        // Campo 4
    try
    {
        prepararYenviarComando(comando);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    try
    {
        cerrarDocumentoNoFiscal();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

std::string NetsoftPrinter::documentoNoFiscal(PrinterDto *dat)
{
    nlohmann::json json;
    auto items = dat->textoNF.get();
    std::string texto, respuesta;

    try
    {
        std::string comando = "H"; // Abrir Documento no fiscal
        prepararYenviarComando(comando);

        for (auto i = items->begin(); i != items->end(); ++i)
        {
            // if(i->getValue("") == "") *i = " ";
            if(i->getValue("").empty()) continue;
            comando = "I"; // IMPRIMIR texto en documento no fiscal
            comando += this->SEPARADOR + i->getValue("").substr(0, 64);
            try
            {
                prepararYenviarComando(comando);
            }
            catch (const std::exception &e)
            {
                OATPP_LOGE("ERROR: ", "%s", e.what());
            }
        }
        respuesta = cerrarDocumentoNoFiscal();

        auto campos = explode(respuesta, this->SEPARADOR[0]);
        if (campos.size() > 3)
        {
            auto item = campos.begin();
            std::advance(item, 3);
            respuesta = item->c_str();
            respuesta = respuesta.substr(0, respuesta.find(this->END, 0));

            json["docNoFiscal"] = respuesta;
        }
    }
    catch (const std::exception &e)
    {
        try
        {
            cerrarDocumentoFiscal();
        }
        catch (const std::exception &e)
        {
            OATPP_LOGE("ERROR ABORTANDO DOCUMENTO NO FISCAL: ", "%s", e.what());
        }
        throw;
    }
    return json.dump();
}

void NetsoftPrinter::imprimirProgramacion()
{
    std::string comando = "MS";
    prepararYenviarComando(comando);
}

std::string NetsoftPrinter::cerrarDocumentoNoFiscal()
{
    std::string comando, respuesta;
    comando = "J";
    comando += this->SEPARADOR + "E";

    try
    {
        respuesta = prepararYenviarComando(comando);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return respuesta;
}

std::string NetsoftPrinter::reporteX(bool imprimir)
{
    std::string resultado, comando;
    comando = "9";
    comando += this->SEPARADOR + "X";
    comando += this->SEPARADOR + (imprimir ? "T" : "S");

    this->tiempoAdicional = 100;
    try
    {
        resultado = prepararYenviarComando(comando);
        this->tiempoAdicional = 0;
    }
    catch (...)
    {
        this->tiempoAdicional = 0;
        throw;
    }

    resultado = analizarReporteXZ("reporte_x", resultado);

    return resultado;
}

std::string NetsoftPrinter::reporteZ()
{
    bool imprimir = true;
    std::string resultado, comando = "9";
    comando += this->SEPARADOR + "Z";
    comando += this->SEPARADOR + (imprimir ? "T" : "S");

    this->tiempoAdicional = 100;
    try
    {
        resultado = prepararYenviarComando(comando);
        this->tiempoAdicional = 0;
    }
    catch (...)
    {
        this->tiempoAdicional = 0;
        throw;
    }

    resultado = analizarReporteXZ("reporte_x", resultado);

    return resultado;
}

std::string NetsoftPrinter::leerFechaYhora()
{
    std::string comando, respuesta, resultado, anio, mes, dia, hora;
    comando = "Y";
    respuesta = prepararYenviarComando(comando);

    auto campos = explode(respuesta, this->SEPARADOR[0]);

    if (campos.size() >= 5)
    {
        auto item = campos.begin();
        std::advance(item, 3);

        respuesta = item->c_str();

        if (respuesta.size() == 6)
        {
            anio = respuesta.substr(0, 2);
            mes = respuesta.substr(2, 2);
            dia = respuesta.substr(4, 2);
        }

        std::advance(item, 1);
        respuesta = item->c_str();

        if (respuesta.size() == 11)
        {
            hora = respuesta.substr(0, 4);

            hora.insert(2, ":");
        }
    }

    resultado = "20" + anio + "/" + mes + "/" + dia;
    resultado += " " + hora;
    return resultado;
}

std::string NetsoftPrinter::leerSerial()
{
    std::string comando = this->hex2ascii("80");
    std::string respuesta;
    try
    {
        respuesta = prepararYenviarComando(comando);
        auto datosFiscales = explode(respuesta, this->SEPARADOR[0]);
        if (datosFiscales.size() >= 3)
        {
            auto items = datosFiscales.begin();
            std::advance(items, 3);

            respuesta = items->c_str();
        }
    }
    catch (const std::exception &e)
    {
        if (std::string(e.what()) == "ERROR COMANDO NO VALIDO PARA ESTADO FISCAL")
            return "??????????";

        throw;
    }
    return respuesta;
}

std::string NetsoftPrinter::leerRIF()
{
    std::string comando = this->hex2ascii("80");
    std::string respuesta;
    try
    {
        respuesta = prepararYenviarComando(comando);
        respuesta = this->getSerialData(respuesta);
        auto datosFiscales = explode(respuesta, this->SEPARADOR[0]);

        if (datosFiscales.size() >=4)
        {
            auto items = datosFiscales.begin();
            std::advance(items, 4);

            respuesta = items->c_str();
        }
    }
    catch (const std::exception &e)
    {
        if (std::string(e.what()) == "ERROR COMANDO NO VALIDO PARA ESTADO FISCAL")
            return "??????????";

        throw;
    }
    return respuesta;
}
std::string NetsoftPrinter::analizarFactura(std::string cmd, std::string fecha, std::string rawSubtotal) { throw; };
std::string NetsoftPrinter::analizarFactura(std::string cmd, double igft, std::string fecha, std::string rawSubtotal, std::string rawCierre)
{
    nlohmann::json respuesta;
    try
    {
        if (rawSubtotal.size() > 100)
        {
            rawSubtotal = rawSubtotal.substr(1, rawSubtotal.size() - 3);
            auto rawCamposSubtotal = explode(rawSubtotal, this->SEPARADOR[0]);

            std::vector<std::string> camposSubtotal = {
                {"no_usado"}, //// Inicio cmd...
                {"no_usado"}, //// Estado impresora
                {"no_usado"}, //// Estado fiscal
                {"total_venta_e"},
                {"total_venta_p"},
                {"total_venta_r"},
                {"no_usado"}, //// Tasa Impuesto Reducido
                {"total_impuesto_r"},
                {"total_venta_g"},
                {"no_usado"}, //// Tasa Impuesto General
                {"total_impuesto_g"},
                {"total_venta_a"},
                {"no_usado"}, //// Tasa Impuesto Aumentado
                {"total_impuesto_a"},

                // No se añade a la respuesta, se usa para calcular los totales
                {"desc_total_venta_e"},    // Descuento Venta Exento
                {"desc_total_venta_p"},    // Descuento Venta Percibido
                {"desc_total_venta_r"},    // Descuentos Venta Reducido
                {"no_usado"},              //// Tasa reducido
                {"desc_total_impuesto_r"}, // Descuento Impuesto Reducido
                {"desc_total_venta_g"},    // Descuento Venta General
                {"no_usado"},              //// Tasa general
                {"desc_total_impuesto_g"}, // Descuento Impuesto General
                {"desc_total_venta_a"},    // Descuento Venta Aumentado
                {"no_usado"},              //// Tasa impuesto aumentado
                {"desc_total_impuesto_a"}  // Descuento impuesto Aumentado
            };

            int i = 0;
            double subtotal = 0;
            if (rawCamposSubtotal.size() > 24)
                for (auto it = rawCamposSubtotal.begin(); it != rawCamposSubtotal.end(); ++it, i++)
                {
                    try
                    {
                        if (camposSubtotal.size() == i)
                            break;
                        if (camposSubtotal[i] != "no_usado")
                        {
                            if (camposSubtotal[i].find("desc_") != std::string::npos)
                            {
                                double aux = respuesta["factura"]["subtotales"][camposSubtotal[i].substr(5, std::string::npos)];
                                aux -= round2decimals(stoi(std::string(it->c_str())) / 100.);
                                subtotal += aux;
                                respuesta["factura"]["subtotales"][camposSubtotal[i].substr(5, std::string::npos)] = aux;
                            }
                            else
                            {
                                respuesta["factura"]["subtotales"][camposSubtotal[i]] = round2decimals(stoi(std::string(it->c_str())) / 100.);
                            }
                        }
                    }
                    catch (const std::exception &e)
                    {
                        OATPP_LOGE("Analizar Factura", " %s: %s", camposSubtotal, e.what());
                    }
                }

            try
            {
                double ivaIGTF = 0;
                if (igft > subtotal)
                {
                    ivaIGTF = round2decimals(subtotal * 0.03);
                }
                else
                {
                    ivaIGTF = round2decimals(igft * 0.03);
                }

                respuesta["factura"]["subtotales"]["total_igtf"] = ivaIGTF;

                respuesta["factura"]["subtotales"]["subtotal"] = round2decimals(subtotal + ivaIGTF);
            }
            catch (const std::exception &e)
            {
                OATPP_LOGE("Analizar Factura", "Error calculando subtotales...  %s", e.what());
            }
        }

        if (rawCierre.size() >= 20)
        {
            rawCierre = rawCierre.substr(1, rawCierre.size() - 3);
            auto cierre = explode(rawCierre, this->SEPARADOR[0]);
            if (cierre.size() == 4)
            {
                try
                {
                    respuesta["factura"]["nro"] = stoi(cierre[3]);
                }
                catch (const std::exception &e)
                {
                    OATPP_LOGE("Analizar Factura", " Cierre: %s", e.what());
                }
            }
        }

        respuesta["cmd"] = cmd;
        respuesta["serialFiscal"] = this->device->serial;
        respuesta["fecha"] = fecha;
        respuesta["rawSubtotal"] = rawSubtotal;
        respuesta["rawCierre"] = rawCierre;
    }
    catch (std::exception &e)
    {
        OATPP_LOGE("ERROR ", "analizarFactura %s", e.what());
    }
    return respuesta.dump();
};

std::string NetsoftPrinter::analizarReporteXZ(std::string cmd, std::string rawReporteXZ)
{
    nlohmann::json respuesta;

    respuesta["cmd"] = cmd;
    respuesta["rawResponse"] = rawReporteXZ;

    return respuesta.dump();
};

std::string NetsoftPrinter::reset()
{
    std::string comando = this->hex2ascii("0708090A0B0C0D0E0F1011121314151617");

    std::string result;
    prepararYenviarComando(comando);

    return "";
}

std::string NetsoftPrinter::textoFiscal(std::string comentario)
{
    std::string comando;
    comando = "A";
    comando += this->SEPARADOR + comentario.substr(0, 64); // campo 1
    comando += this->SEPARADOR + this->NO_USADO;           // campo 2

    prepararYenviarComando(comando);

    return "";
}

std::string NetsoftPrinter::checksumBCC(std::string ascii)
{
    std::string chkhex, chkstr;
    int chkdec = 0;

    for (int i = 0; i < ascii.size(); i++)
    {
        if (int((unsigned char)ascii[i]) == 194)
            continue;

        chkdec += (int((unsigned char)ascii[i]));
    }
    chkhex = this->int_to_hex(chkdec);
    for (int i = 0; i < (4 - chkhex.size()); i++)
        chkstr += '0';
    chkstr += (chkhex);
    return chkstr;
}

std::string NetsoftPrinter::getSecuencia()
{
    if (this->secuencia >= 127)
        this->secuencia = 32;

    std::string result ="";
    result += char(secuencia);
    this->secuencia++;
    return result;
}

std::string NetsoftPrinter::obtenerError(std::string data)
{
    data = data.substr(0, data.find(this->END, 0));
    auto res = explode(data, this->SEPARADOR[0]);
    std::string codigo, respuesta;

    for (auto &&i : res)
    {
        if (i.find("ERROR") != std::string::npos)
        {
            codigo = i.c_str();
            removeWordFromString("ERROR", i);
            respuesta = trim(i);
            break;
        }
    }
    auto it = error.find(respuesta);
    respuesta = (it != error.end()) ? it->second : ("SE HA ENCONTRADO UN ERROR AL ENVIAR EL COMANDO: " + codigo);

    return respuesta;
}

StatusNetsoft NetsoftPrinter::leerStatus()
{
    try
    {
        leerRIF();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    StatusNetsoft status;
    std::string comando = "8" + this->SEPARADOR + "N";
    std::string respuesta;

    try
    {
        respuesta = enviarComando(prepararComando(comando));
        if (respuesta.find("ERROR") != std::string::npos)
            return status;

        // sts1 : status de la impresora sts1
        // sts2 : estado impresora
        // sts3 : estado fiscal
        auto campos = this->explode(respuesta.substr(0, respuesta.find(this->END, 0)), this->SEPARADOR[0]);

        if (campos.size() > 12 && respuesta[0] == this->STX[0])
        {
            status.sts1 = campos[4];
            status.sts2 = campos[1];
            status.sts3 = campos[2];

            auto sts1_aux = status.StatusNetsoft1.find(status.sts1);

            status.sts1 = (sts1_aux == status.StatusNetsoft1.end()) ? status.sts3 : sts1_aux->second;

            std::stringstream s2, s3;
            s2 << std::hex << status.sts2;
            s3 << std::hex << status.sts3;

            status.sts2.clear();
            status.sts3.clear();

            unsigned n2, n3;
            s2 >> n2;
            s3 >> n3;

            std::bitset<16> b2(n2), b3(n3);

            std::string sb2 = b2.to_string();
            std::string sb3 = b3.to_string();

            std::reverse(sb2.begin(), sb2.end());
            std::reverse(sb3.begin(), sb3.end());

            for (int i = 0; i < 16; i++)
            {
                if (sb2[i] == '1')
                {
                    auto it = status.StatusNetsoft2.find(std::to_string(i));
                    status.sts2 += (it != status.StatusNetsoft2.end()) ? it->second + "\n" : "";
                }
                if (sb3[i] == '1')
                {
                    auto it = status.StatusNetsoft3.find(std::to_string(i));
                    status.sts3 += (it != status.StatusNetsoft3.end()) ? it->second + "\n" : "";
                }
            }
        }

        OATPP_LOGD("", "status.sts1: %s", this->trim(status.sts1).c_str());
        OATPP_LOGD("", "status.sts2: %s", this->trim(status.sts2).c_str());
        OATPP_LOGD("", "status.sts3: %s", this->trim(status.sts3).c_str());
    }
    catch (const std::exception &e)
    {
        OATPP_LOGD("leerStatus", "%s", e.what());
    }
    return status;
}