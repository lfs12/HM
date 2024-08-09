#include "PnpPrinter.hpp"

std::map<std::string, std::string> StatusPNP::StatusPNP1 = {
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

std::map<std::string, std::string> StatusPNP::StatusPNP2 = {
    {"2", "Error y/o falla de impresora"},
    {"3", "Impresora fuera de línea"},
    {"14", "Impresora sin papel para imprimir"}};

std::map<std::string, std::string> StatusPNP::StatusPNP3 = {
    {"0", "Error de comprobación de memoria fiscal. Junto con el Bit 7 indica que la memoria fiscal está llena"},
    {"1", "Error de comprobación de memoria de trabajo"},
    {"3", "Comando no reconocido."},
    {"4", "Campo de datos invalido"},
    {"5", "Comando no válido para estado fiscal"},
    {"6", "Desbordamiento de totales"},
    {"7", "Memoria fiscal llena"},
    {"8", "Memoria fiscal casi llena"},

    {"11", "Es necesario hacer un cierre de la jornada fiscal o Se han enviado el número máximo de ítems en una facture fiscal y se debe cerrar."},
    {"12", "Factura fiscal abierta"},
    {"13", "Documento no fiscal abierto"}};

std::map<std::string, std::string> PnpPrinter::error = {
    {"0", "ERRORES EN CAMPOS DE DATOS"},
    {"1", "ERROR CAMPO 1"},
    {"2", "ERROR CAMPO 2"},
    {"3", "ERROR CAMPO 3"},
    {"4", "ERROR CAMPO 4 o error de tasas de impuesto"},
    {"5", "ERROR CAMPO 5"},
    {"6", "ERROR CAMPO 6"},
    {"7", "ERROR CAMPO 7"},
    {"8", "ERROR CAMPO 8"},
    {"9", "ERROR CAMPO 9"},
    {"A", "ERROR CAMPO 10"},
    {"B", "ERROR CAMPO 11"},
    {"C", "ERROR CAMPO 12"},
    {"D", "ERROR CAMPO 13"},
    {"E", "ERROR CAMPO 14"},
    {"F", "ERROR CAMPO 15"},
    {"10", "ERROR CAMPO 16"},
    {"11", "ERROR CAMPO 17"},
    {"12", "ERROR CAMPO 18"},
    {"13", "ERROR CAMPO 19"},
    {"14", "ERRORES BUS L2C"},
    {"15", "LINEA L2C"},
    {"16", "BCC RAM"},
    {"17", "BCC ROM 0"},
    {"18", "BCC ROM 4"},
    {"1E", "ERROR COMANDO"},
    {"1F", "ERROR AL VERIFICAR COMANDO"},
    {"20", "SECUENCIA"},
    {"28", "ERRORES DE IMPRESIÓN"},
    {"29", "ERROR AL IMPRIMIR"},
    {"46", "ERRORES DE TOTALES"},
    {"47", "DESBORDE DE TOTALES"},
    {"5A", "ERRORES CRITICO"},
    {"5B", "LIMITE DE MEMORIA FISCAL"},
    {"5C", "ERROR EN LOS VALORES DE LA RAM"},
    {"5F", "ERROR EN TRAMA DE DATOS (BCC)"},
    {"60", "LA TRAMA INCLUYE UN ESC"},
    {"61", "ERROR EN FORMATO DE DATOS AL REALIZAR UN Z"},
    {"62", "ERROR EQUIPO SIN FISCALIZAR"},
    {"63", "ERROR EN SUBTOTAL"},
    {"64", "ABRIR_CF"},
    {"65", "SON NECESARIOS LOS DATOS DEL CLIENTE"},
    {"68", "ES NECESARIO UN REPORTE Z"},
    {"6E", "TEXTO_CF"},
    {"6F", "MÁX. CANTIDAD DE LINEAS DE TEXTO NO FISCAL"},
    {"78", "ITEM_CF"},
    {"79", "TASA NO VALIDA"},
    {"7C", "DESBORDE CANTIDAD x MONTO"},
    {"7D", "MONTO MÁX. x ARTICULO"},
    {"82", "CERRAR_CF"},
    {"83", "SUBTOTAL CERO"},
    {"8C", "NO USADO"},
    {"8D", "NO USADO"},
    {"8F", "EMITIENDO UN PAGO EN UNA DEVOLUCIÓN"},
    {"90", "NO USADO"},
    {"91", "NO USADO"},
    {"96", "REPORTE DE MEMORIA FISCAL"},
    {"97", "NO SE ENCONTRÓ EL REPORTE SOLICITADO"},
    {"98", "NO SE ENCONTRÓ EL REPORTE SOLICITADO"},
    {"99", "EL EQUIPO NO ESTÁ CERTIFICADO"},
    {"A0", "RELOJ"},
    {"A1", "PERIODO NO VALIDO. FALLA RESPECTO A LA RAM"},
    {"A2", "PERIODO NO VALIDO. FALLA RESPECTO A LA ROM"},
    {"AA", "DIRECCIÓN"},
    {"AB", "ERROR AL ESCRIBIR DIRECCIÓN"},
    {"B4", "SLIP"},
    {"B5", "ERROR EN SLIP CHEQUE"},
    {"B6", "ERROR EN SLIP ENDOSO"},
    {"B7", "COMANDO NO VALIDO. SLIP ACTIVO"},
    {"BE", "CORTA PAPEL"},
    {"BF", "ERROR EN COMANDO"}};

PnpPrinter::PnpPrinter(std::string cport) : PrinterInterface::PrinterInterface(cport) {}

void PnpPrinter::refrescarConfiguracion()
{
    if (this->device && this->device->model == "PF220")
        this->maxLength = 20;
    else
        this->maxLength = 38;

    this->lineasTextoNF = 4;
}

std::string PnpPrinter::readPort()
{
    std::string bytes;
    int timeOut = 0;

    std::string error = "ERROR";
    std::string fin = this->END;

    bool buscando = true;
    while (buscando && timeOut < 25)
    {
        timeOut++;
        memset(this->buf, 0, 4095);
        int n = RS232_PollComport(this->cport_nr, this->buf, 4095);

        if (n > 0)
        {
            OATPP_LOGD("", "received %i bytes: %s\n", n, (char *)buf);
            bytes += std::string(buf, buf + n);
        }

        if (bytes.find(error) != std::string::npos ||
            bytes.find(fin) != std::string::npos)
        {
            buscando = false;
            OATPP_LOGD("", "\t\t\t\t (ERROR): %s \t\t\t (FIN): %s \t\t\t timeOut=%d\n",
                       (bytes.find(error) != std::string::npos) ? "TRUE" : "false",
                       (bytes.find(fin) != std::string::npos) ? "TRUE" : "false",
                       timeOut);
        }
        else
        {
            if (bytes.find(this->MAS_TIEMPO) != std::string::npos)
            {
                timeOut = 0;
                if (bytes.size() < 5)
                    bytes.clear();
                this->sleep(2000);
            }
            this->sleep(200);
        }
    }

    return bytes;
}

std::string PnpPrinter::ejecutarComando(oatpp::DTO *dto)
{
    PrinterDto *data = ((PrinterDto *)dto);
    std::string resultado, cmd = data->cmd.getValue("");
    nlohmann::json jsonResponse;

    StatusPNP status = leerStatus();

    if (!status.sts1.empty())
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, status.sts1);

    if (status.sts3.find("Error critico") != std::string::npos)
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, status.sts3);

    if (status.sts3.find("Factura fiscal en curso") != std::string::npos)
        cerrarDocumentoFiscal(false);

    if (status.sts3.find("Documento no fiscal en curso") != std::string::npos)
        cerrarDocumentoNoFiscal();

    if ("reset" == cmd)
    {
        resultado = reset();
    }
    else if ("reporteX-ver" == cmd)
    {
        resultado = reporteX(false);
    }
    else if ("reporteX-imprimir" == cmd || "reporte_x" == cmd)
    {
        resultado = reporteX(true);
    }
    else if ("reporteZ-ver" == cmd)
    {
        resultado = reporteZ();
    }
    else if ("reporteZ-imprimir" == cmd || "reporte_z" == cmd)
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
        documentoNoFiscal(data);
    }
    else if ("status" == cmd)
    {
        status = leerStatus();
        nlohmann::json statusJson;
        std::string ss = (!status.sts3.empty()) ? "STATUS: " + status.sts3 : "";
        ss += (!status.sts1.empty()) ? "IMPRESORA: " + status.sts2 : "";
        ss += (!status.sts2.empty()) ? "FISCAL: " + status.sts3 : "";
        statusJson["status"] = ss;
        resultado = statusJson.dump();
    }
    else if ("status2" == cmd)
    {
        leerStatus2();
        resultado = "";
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
std::string PnpPrinter::reset()
{
    std::string comando = this->hex2ascii("0708090A0B0C0D0E0F1011121314151617");
    enviarComando(comando);

    return "";
}
std::string PnpPrinter::cerrarDocumentoFiscal() { throw; }

void PnpPrinter::cerrarDocumentoFiscal(bool igtf)
{
    std::string cmd = (igtf) ? "U" : "T";
    std::string comando = "E" + this->SEPARADOR + cmd + this->SEPARADOR;
    comando = prepararComando(comando);
    enviarComando(comando);
}
std::string PnpPrinter::cerrarDocumentoNoFiscal()
{
    std::string comando = "J"; // Cerrar Documento no fiscal
    prepararYenviarComando(comando);

    return "";
}

void PnpPrinter::anularDocumentoFiscal() { throw; }

std::string PnpPrinter::reporteX(bool imprimir)
{
    std::string comando, resultado;
    comando = "9" + this->SEPARADOR;
    comando += "X" + this->SEPARADOR;
    comando += ((imprimir) ? "T" : "S") + this->SEPARADOR;

    comando = prepararComando(comando);
    resultado = enviarComando(comando);
    resultado = analizarReporteXZ("reporte_x", resultado);

    return resultado;
}
std::string PnpPrinter::reporteZ()
{
    bool imprimir = true;
    std::string comando, resultado;
    comando = "9" + this->SEPARADOR;
    comando += "Z" + this->SEPARADOR;
    comando += ((imprimir) ? "T" : "S") + this->SEPARADOR;

    comando = prepararComando(comando);
    resultado = enviarComando(comando);
    resultado = analizarReporteXZ("reporte_z", resultado);

    return resultado;
}

std::string PnpPrinter::textoFiscal(std::string comentario) // COMENTARIO
{
    std::string comando;
    comando = "A";
    comando += this->SEPARADOR + comentario.substr(0, 40); // campo 1
    comando += this->SEPARADOR + "A";                      // campo 2

    comando = prepararComando(comando);
    enviarComando(comando);

    return "";
}
std::string PnpPrinter::documentoNoFiscal(PrinterDto *dat)
{
    auto items = dat->textoNF.get();
    std::string texto;
    try
    {
        std::string comando = "H"; // Abrir Documento no fiscal
        prepararYenviarComando(comando);

        for (auto i = items->begin(); i != items->end(); ++i)
        {
            comando = "I"; // Imprimir texto en documento no fiscal
            comando += this->SEPARADOR + i->getValue("").substr(0, 38);

            prepararYenviarComando(comando);
        }
        cerrarDocumentoNoFiscal();
    }
    catch (const std::exception &e)
    {
        cerrarDocumentoNoFiscal();
        throw;
    }
    return "";
}

std::string PnpPrinter::leerFechaYhora()
{
    std::string comando, respuesta, resultado, anio, mes, dia;
    comando = "Y";
    comando = prepararComando(comando);
    respuesta = enviarComando(comando);
    if (respuesta.size() > 0)
    {
        anio = respuesta.substr(14, 2);
        mes = respuesta.substr(16, 2);
        dia = respuesta.substr(18, 2);

        resultado = "20" + anio + "/" + mes + "/" + dia;
        resultado += " " + respuesta.substr(21, 4);
        resultado.insert(13, ":");
    }
    return resultado;
}

std::string items(oatpp::DTO::List<oatpp::Object<Item>>) { throw; }

std::string PnpPrinter::facturar(PrinterDto *dat, bool facturar)
{
    std::string fechaTimestamp = leerFechaYhora();
    std::string respuesta, resultado, comando = "@";

    OATPP_COMPONENT(oatpp::Object<PrinterConfig>, printerConfig);
    OATPP_ASSERT_HTTP((printerConfig && printerConfig->pnpConfig), Status::CODE_400, "Debe configurar las tasas en printer_config.json");
    OATPP_ASSERT_HTTP(printerConfig->pnpConfig->tasas->size() > 0, Status::CODE_400, "Debe configurar los tasas en printer_config.json");

    Tasas::Wrapper tasas = nullptr;
    for (auto &&t : *printerConfig->pnpConfig->tasas.get())
    {
        if (t->serial.getValue("") == this->device->serial.getValue(""))
        {
            tasas = t;
            OATPP_LOGE("TASAS ENCONTRADA...", "%s", t->serial.getValue("").c_str());
            break;
        }
    }
    if (!tasas)
    {
        for (auto &&t : *printerConfig->pnpConfig->tasas.get())
        {
            if (t->serial.getValue("").empty() || t->serial.getValue("") == "*")
            {
                tasas = t;
                break;
            }
        }
    }
    printDTO(tasas);

    try
    {
        bool imprimirCabecera = dat->factura->cliente->nombre.getValue("").size() <= this->maxLength;
        if (imprimirCabecera)
        {
            comando += this->SEPARADOR + dat->factura->cliente->nombre.getValue("").substr(0, 38); // campo 1
            comando += this->SEPARADOR + dat->factura->cliente->rif.getValue("").substr(0, 12);    // campo 2
        }
        else // Si cadena es menor se imprime la cabecera
        {
            comando += this->SEPARADOR + ""; // campo 1
            comando += this->SEPARADOR + ""; // campo 2
        }

        comando += this->SEPARADOR + (facturar ? this->NO_USADO : this->fillNumbToString(dat->factura->notaFiscal->factura.getValue(0), 8, 0)); // campo 3
        comando += this->SEPARADOR + (facturar ? this->NO_USADO : (dat->factura->notaFiscal->serialFiscal.getValue("")));                       // campo 4                                                                   // campo 4
        comando += this->SEPARADOR + (facturar ? this->NO_USADO : (this->formatearFecha(dat->factura->notaFiscal->fecha.getValue(""))));        // campo 5
        comando += this->SEPARADOR + (facturar ? this->NO_USADO : (dat->factura->notaFiscal->hora.getValue("")));                               // campo 6                                                            // campo 6
        comando += this->SEPARADOR + (facturar ? "E" : "D");                                                                                    // campo 7
        comando += this->SEPARADOR + this->NO_USADO;                                                                                            // campo 8
        comando += this->SEPARADOR + this->NO_USADO;                                                                                            // campo 9

        prepararYenviarComando(comando);

        if (imprimirCabecera)
        {
            auto aux = dat->factura->cabecera.get();
            for (auto i = aux->begin(); i != aux->end(); i++)
            {
                std::string info = i->getValue("");
                textoFiscal(info);
                // this->sleep(500); // REVISAR: sleep en cabecera
            }
            if (aux != nullptr && !aux->empty())
                textoFiscal(".");
        }

        double baseImponible = 0.0;
        for (auto &&i : *dat->factura->items)
        {
            if (i->producto.getValue("").find("SERVICIO (") == std::string::npos)
                baseImponible = baseImponible + (i->cantidad * i->precio);
        }

        auto item = dat->factura->items.get();
        auto descuentoGlobal = 0.0;
        double porcentajeGlobal = 0.0;

        for (auto i = item->begin(); i != item->end(); i++)
        {
            std::string tasa = "E";

            if (i->get()->tasa == "E")
                tasa = "0";
            else if (i->get()->tasa == "G")
                tasa = std::to_string(int(tasas->G.getValue(0) * 100));
            else if (i->get()->tasa == "R")
                tasa = std::to_string(int(tasas->R.getValue(0) * 100));
            else if (i->get()->tasa == "A")
                tasa = std::to_string(int(tasas->A.getValue(0) * 100));
            else if (i->get()->tasa == "P")
                tasa = "0001";

            comando = "B";
            double precio = i->get()->precio.getValue(0.0);

            if (i->get()->producto.getValue("").find("SERVICIO (") == std::string::npos)
            {

                if (i->get()->descuento.getValue(0.0) != 0.0)
                {
                    if (i->get()->tipo_descuento == "monto")
                    {
                        precio = precio - i->get()->descuento;
                        textoFiscal("DESCUENTO POR " + numbToString(i->get()->descuento.getValue(0.0)) + " BS");
                    }
                    else if (i->get()->tipo_descuento == "porcentaje")
                    {
                        precio = precio * (1 - (i->get()->descuento / 100));
                        textoFiscal("DESCUENTO POR (" + numbToString(i->get()->descuento.getValue(0.0)) + "%) POR " + numbToString(i->get()->precio.getValue(0.0) - precio) + " BS");
                    }
                }

                if (dat->factura->descuento->descuento.getValue(0.0) != 0.0)
                {
                    if (dat->factura->descuento->tipo_descuento == "monto")
                    {
                        double porcentaje = dat->factura->descuento->descuento * 100 / baseImponible;
                        descuentoGlobal = descuentoGlobal + (precio - (precio * (1 - (porcentaje / 100))));
                        precio = precio * (1 - (porcentaje / 100));
                    }
                    else if (dat->factura->descuento->tipo_descuento == "porcentaje")
                    {
                        descuentoGlobal = descuentoGlobal + (precio * (1 - (dat->factura->descuento->descuento / 100)));

                        precio = precio * (1 - (dat->factura->descuento->descuento / 100));
                    }
                }
            }
            else
            {
                if (i->get()->descuento.getValue(0.0) != 0.0)
                {
                    if (i->get()->tipo_descuento == "monto")
                    {
                        precio = precio - i->get()->descuento;
                        textoFiscal("DESCUENTO POR " + numbToString(i->get()->descuento.getValue(0.0)) + " BS");
                    }
                    else if (i->get()->tipo_descuento == "porcentaje")
                    {
                        precio = precio * (1 - (i->get()->descuento / 100));
                        textoFiscal("DESCUENTO POR (" + numbToString(i->get()->descuento.getValue(0.0)) + "%) POR " + numbToString(i->get()->precio.getValue(0.0) - precio) + " BS");
                    }
                }

                if (dat->factura->descuento->descuento.getValue(0.0) != 0.0)
                {
                    if (dat->factura->descuento->tipo_descuento == "monto")
                    {
                        double porcentaje = dat->factura->descuento->descuento * 100 / baseImponible;
                        descuentoGlobal = descuentoGlobal + (precio - (precio * (1 - (porcentaje / 100))));
                        precio = precio * (1 - (porcentaje / 100));
                    }
                    else if (dat->factura->descuento->tipo_descuento == "porcentaje")
                    {
                        descuentoGlobal = descuentoGlobal + (precio * (1 - (dat->factura->descuento->descuento / 100)));

                        precio = precio * (1 - (dat->factura->descuento->descuento / 100));
                    }
                }
            }

            std::string comentario = i->get()->texto.getValue("");
            if (!comentario.empty())
            {
                for (size_t i = 0; i < comentario.size() && (i / this->maxLength) < this->lineasTextoNF; i += this->maxLength)
                    textoFiscal(comentario.substr(i, this->maxLength));
            }

            comando += this->SEPARADOR + i->get()->producto.getValue("").substr(0, this->maxLength); // campo 1
            comando += this->SEPARADOR + this->fillNumbToString(i->get()->cantidad, 7, 3);           // campo 2
            comando += this->SEPARADOR + this->fillNumbToString(precio, 12, 2);                      // campo 3
            comando += this->SEPARADOR + tasa;                                                       // campo 4
            comando += this->SEPARADOR + "M";                                                        // campo 5
            comando += this->SEPARADOR + this->NO_USADO;                                             // campo 6
            comando += this->SEPARADOR + ((facturar) ? this->NO_USADO : "D");                        // campo 7
            comando += this->SEPARADOR + this->NO_USADO;                                             // campo 8

            comando = prepararComando(comando);
            enviarComando(comando);
        }
        if (descuentoGlobal > 0.0)
        {
            if (dat->factura->descuento->tipo_descuento == "PORCENTAJE")
            {
                dat->factura->pie->push_back("DESCUENTO POR (" + numbToString(dat->factura->descuento->descuento.getValue(0.0)) + "%" + ") POR" + numbToString(descuentoGlobal) + " BS");
            }
            else
            {
                dat->factura->pie->push_back("DESCUENTO POR " + numbToString(descuentoGlobal) + " BS");
            }
        }

        std::string cierre = pagos(dat->factura->pagos);
        std::string rawSubtotal = subtotal(dat->factura->pagos);
        auto doc_type = true ? 1:0;
        resultado = analizarFactura(dat->cmd.getValue(""), fechaTimestamp, rawSubtotal, cierre, doc_type);

        std::string linea_comentario = "49" + this->SEPARADOR + dat->linea_comentario.getValue("");
        prepararYenviarComando(linea_comentario);
        
        imprimirPieDePagina(dat, imprimirCabecera);
        bool contieneIgtf = contieneIGTF(dat->factura->pagos);
        cerrarDocumentoFiscal(contieneIgtf);
    }
    catch (const std::exception &e)
    {
        cerrarDocumentoFiscal(false);
        throw;
    }
    return resultado;
}

void PnpPrinter::descuento(std::list<oatpp::data::mapping::type::DTO::Object<Item>>::iterator i)
{
    std::string comando;
    if (i->get()->descuento.getValue(0) != 0 &&
        (i->get()->tipo_descuento == "monto" || i->get()->tipo_descuento == "porcentaje"))
    {
        comando = "D";
        std::string tasa = "E";

        if (i->get()->tasa == "E")
            tasa = "0000";
        if (i->get()->tasa == "P")
            tasa = "0001";
        if (i->get()->tasa == "G")
            tasa = "1600";
        else if (i->get()->tasa == "R")
            tasa = "800";
        else if (i->get()->tasa == "A")
            tasa = "3100";

        comando += this->SEPARADOR + i->get()->producto.getValue("").substr(0, 20);       // campo 1
        comando += this->SEPARADOR + this->fillNumbToString(i->get()->descuento, 4, 2);   // campo 2
        comando += this->SEPARADOR + ((i->get()->tipo_descuento == "monto") ? "D" : "P"); // campo 3
        comando += this->SEPARADOR + tasa;                                                // campo 4

        comando = prepararComando(comando);
        enviarComando(comando);
    }
}

std::string PnpPrinter::pagos(oatpp::DTO::List<oatpp::Object<Pago>> pagos)
{
    std::string comando, respuesta;
    comando = "E"; // item

    bool IGTF = false;
    float amountIGTF = 0;

    if (pagos.get() != nullptr)
    {
        auto item = pagos.get();

        for (auto i = item->begin(); i != item->end(); i++)
        {
            if (i->get()->impuesto != nullptr && i->get()->impuesto->igtf == true)
            {
                IGTF = true;
                // if(i->get() != nullptr )
                amountIGTF += i->get()->monto;
            }
        }
    }

    if (IGTF)
        comando += this->SEPARADOR + "B" + this->SEPARADOR + this->fillNumbToString(amountIGTF, 12, 2);
    else
        comando += this->SEPARADOR + "A" + this->SEPARADOR;

    comando += this->SEPARADOR;
    comando = prepararComando(comando);
    respuesta = enviarComando(comando);

    return respuesta;
}
void PnpPrinter::cabecera(oatpp::DTO::List<oatpp::String>) { throw; }
void PnpPrinter::pie(oatpp::DTO::List<oatpp::String>) { throw; }
void PnpPrinter::imprimirPieDePagina(PrinterDto *dat, bool cabeceraImpresa)
{
    std::string texto;
    int itemSize = dat->factura->pagos != nullptr ? dat->factura->pagos->size() : 0;
    int caracteresPorLinea = 40, lineasMax = 10;
    textoFiscal(texto);

    if (!cabeceraImpresa)
    {
        auto items = dat->factura->cabecera;
        std::string info;
        for (auto i = items->begin(); i != items->end(); ++i)
        {
            info += " " + i->getValue("");
        }
        texto = "RAZON SOCIAL: " + dat->factura->cliente->nombre.getValue("") + " RIF: " + dat->factura->cliente->rif + info;
        for (auto i = dat->factura->pie->begin(); i != dat->factura->pie->end(); i++)
        {
            texto += " " + i->getValue("");
        }

        for (size_t i = 0; i < texto.size() && (i / caracteresPorLinea) < lineasMax; i += caracteresPorLinea)
            textoFiscal(texto.substr(i, caracteresPorLinea));

        for (auto i = dat->factura->pie->begin(); i != dat->factura->pie->end(); i++)
        {
            texto = i->getValue("");
            for (size_t i = 0; i < texto.size() && (i / caracteresPorLinea) < lineasMax; i += caracteresPorLinea)
                textoFiscal(texto.substr(i, caracteresPorLinea));
        }
    }
    else
    {
        for (auto i = dat->factura->pie->begin(); i != dat->factura->pie->end(); i++)
        {
            texto = i->getValue("");
            for (size_t i = 0; i < texto.size() && (i / caracteresPorLinea) < lineasMax; i += caracteresPorLinea)
                textoFiscal(texto.substr(i, caracteresPorLinea));
        }
    }
}

std::string PnpPrinter::prepararComando(std::string comando)
{
    std::string result = this->STX + this->getSecuencia() + comando + this->END;
    std::string chk = this->checksumBCC(result);
    result += chk;

    return result;
}

std::string PnpPrinter::enviarComando(std::string comando)
{
    std::string respuesta;

    writePort(comando);
    // REVISAR: sleep en enviar comando
    this->sleep(200);

    respuesta = this->readPort();
    this->writeLog(comando, respuesta, "PNP");

    if (respuesta.empty())
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "SE HA PERDIDO COMUNICACIÓN CON EL DISPOSITIVO");

    if (respuesta.find("ERROR") != std::string::npos)
    {
        std::string err = obtenerError(respuesta);
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, err);
    }

    return respuesta;
}

void PnpPrinter::reimprimir(PrinterDto *dat) {}

StatusPNP PnpPrinter::leerStatus()
{
    std::string comando = "8" + this->SEPARADOR + "N";
    std::string respuesta;
    respuesta = prepararYenviarComando(comando);

    StatusPNP status;
    if (respuesta.find("ERROR") != std::string::npos)
        return status;

    try
    {
        // sts1 : estado impresora
        // sts2 : estado fiscal
        // sts3 : status de la impresora sts1
        auto campos = this->explode(respuesta.substr(0, respuesta.find(this->END, 0)), this->SEPARADOR[0]);

        if (campos.size() > 12 && respuesta[0] == this->STX[0])
        {
            status.sts1 = campos[1];
            status.sts2 = campos[2];
            status.sts3 = campos[4];

            auto sts3_aux = status.StatusPNP1.find(status.sts3);

            status.sts3 = (sts3_aux == status.StatusPNP1.end()) ? status.sts3 : sts3_aux->second;

            std::stringstream s1, s2;
            s1 << std::hex << status.sts1;
            s2 << std::hex << status.sts2;

            status.sts1.clear();
            status.sts2.clear();

            unsigned n1, n2;
            s1 >> n1;
            s2 >> n2;

            std::bitset<16> b1(n1), b2(n2);

            std::string sb1 = b1.to_string();
            std::string sb2 = b2.to_string();

            std::reverse(sb1.begin(), sb1.end());
            std::reverse(sb2.begin(), sb2.end());

            for (int i = 0; i < 16; i++)
            {
                if (sb1[i] == '1')
                {
                    auto it = status.StatusPNP2.find(std::to_string(i));
                    status.sts1 += (it != status.StatusPNP2.end()) ? it->second + "\n" : "";
                }
                if (sb2[i] == '1')
                {
                    auto it = status.StatusPNP3.find(std::to_string(i));
                    status.sts2 += (it != status.StatusPNP3.end()) ? it->second + "\n" : "";
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

void PnpPrinter::leerStatus2()
{
    std::string comando = "8" + this->SEPARADOR + "W";
    std::string respuesta;
    respuesta = prepararYenviarComando(comando);
}

std::string PnpPrinter::checksumBCC(std::string ascii)
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

std::string PnpPrinter::getSecuencia()
{
    if (this->secuencia >= 127)
        this->secuencia = 32;
    std::stringstream stream;
    stream << std::hex << this->secuencia;
    std::string result(stream.str());
    this->secuencia++;

    return this->hex2ascii(result);
}

std::string PnpPrinter::leerSerial()
{
    std::string comando = this->hex2ascii("80") + this->SEPARADOR;
    std::string respuesta;

    respuesta = enviarComando(prepararComando(comando));
    auto campos = explode(respuesta, this->SEPARADOR[0]);

    if (campos.size() >= 5)
    {
        auto item = campos.begin();
        std::advance(item, 3);

        respuesta = item->c_str();
    }

    return respuesta;
}

std::string PnpPrinter::leerRIF()
{
    std::string comando = this->hex2ascii("80") + this->SEPARADOR;
    std::string respuesta;

    respuesta = enviarComando(prepararComando(comando));
    auto campos = explode(respuesta, this->SEPARADOR[0]);

    if (campos.size() >= 5)
    {
        auto item = campos.begin();
        std::advance(item, 4);

        respuesta = item->c_str();
    }

    return respuesta;
}

std::string PnpPrinter::items(oatpp::DTO::List<oatpp::Object<Item>>) { throw; }

std::string PnpPrinter::subtotal(oatpp::DTO::List<oatpp::Object<Pago>>)
{
    std::string respuesta, comando = "C";

    comando += this->SEPARADOR + this->NO_USADO;
    comando += this->SEPARADOR + this->NO_USADO;

    comando = prepararComando(comando);
    respuesta = enviarComando(comando);
    return respuesta;
}

std::string PnpPrinter::analizarFactura(std::string a, std::string b, std::string c)
{ throw; }
std::string PnpPrinter::analizarFactura(std::string cmd, std::string fecha, std::string rawSubtotal, std::string rawCierre, int doc) 
{
    nlohmann::json respuesta;

    auto rawCamposSubtotal = explode(rawSubtotal, this->SEPARADOR[0]);

    std::vector<std::string> camposSubtotal = {
        {"no_utilizado"}, // 0 STX
        {"no_utilizado"}, // 1
        {"no_utilizado"},
        {"no_utilizado"},
        {"no_utilizado"},
        {"total_venta_e"},    // 5
        {"total_venta_g"},    // 6
        {"no_utilizado"},     // 7 tasa_imponible_estandar_a
        {"total_impuesto_g"}, // 8
        {"total_venta_r"},    // 9
        {"no_utilizado"},     // 10 tasa_imponible_b
        {"total_impuesto_a"}, // 11
        {"no_utilizado"},     // 12
        {"total_venta_a"},    // 13 total_impuesto_a
        {"total_impuesto_p"}, // 14
        {"no_utilizado"},     // 15
        {"subtotal"},         // 16
        {"no_utilizado"}};    // 17

    respuesta["factura"]["subtotales"]["total_impuesto_r"] = 0.0;

    int i = 0;
    for (auto it = rawCamposSubtotal.begin(); it != rawCamposSubtotal.end(); ++it, i++)
    {
        try
        {
            if (camposSubtotal[i] != "no_utilizado")
            {
                try
                {
                    OATPP_LOGD("...", "%s%s", camposSubtotal[i].c_str(), it->c_str());
                    respuesta["factura"]["subtotales"][camposSubtotal[i]] = round2decimals(std::stoi(std::string(it->c_str())) / 100.);
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << '\n';
                }
            }
        }
        catch (const std::exception &e)
        {
            OATPP_LOGE("Analizar Factura", " %s: %s", camposSubtotal, e.what());
        }
    }

    auto rawCamposCierre = explode(rawCierre, this->SEPARADOR[0]);

    OATPP_LOGI("LOS CAMPOS DE CIERRE: ", "");

    for (const auto &elemento : rawCamposCierre)
    {
        std::cout << elemento << " ";
    }

    OATPP_LOGI("FIN DE LOS CAMPOS DE CIERRE: ", "");
    std::cout << std::endl;
    std::vector<std::string> camposCierre = {
        {"no_utilizado"},
        {"no_utilizado"},
        {"no_utilizado"},
        {"no_utilizado"}, // cantidad_facturas_desde_ultimo_z
        {"nro"},
        {"no_utilizado"}, // contador_notas_de_credito
        {"igtf"}};

    i = 0;
    for (auto it = rawCamposCierre.begin(); it != rawCamposCierre.end(); ++it, i++)
    {
        if (camposCierre[i] != "no_utilizado" && camposCierre[i] != "igtf" && camposCierre[i] != "nro")
           {
               if(doc == 1)
               { 
                 respuesta["factura"][camposCierre[i]] = stof(std::string(it->c_str())) / 100L;
               }else if(doc == 0){
                 respuesta["Nota_de_Credito"][camposCierre[i]] = stof(std::string(it->c_str())) / 100L;
               }
           }
        if (camposCierre[i] == "nro")
           {
               if(doc == 1){
                 respuesta["factura"]["nro"] = atoi(it->c_str());
               }else if(doc == 0){
                 respuesta["Nota_de_Credito"]["nro"] = atoi(it->c_str());
               }
           }
        if (camposCierre[i] == "igtf")
            {
                if(doc == 1){
                    respuesta["factura"]["subtotales"]["total_igtf"] = round2decimals(std::stoi(std::string(it->c_str())) / 100.);
                }else if(doc == 0)
                {
                    respuesta["Nota_de_Credito"]["subtotales"]["total_igtf"] = round2decimals(std::stoi(std::string(it->c_str())) / 100.);
                }
            }
    }
    respuesta["cmd"] = cmd;
    respuesta["serialFiscal"] = this->device->serial;
    respuesta["fecha"] = fecha;
    respuesta["rawSubtotal"] = rawSubtotal;
    respuesta["rawFactura"] = rawCierre;

    return respuesta.dump();
}
std::string PnpPrinter::analizarReporteXZ(std::string cmd, std::string rawReporteXZ)
{
    nlohmann::json respuesta;

    respuesta["cmd"] = cmd;
    respuesta["rawResponse"] = rawReporteXZ;

    return respuesta.dump();
}
std::string PnpPrinter::obtenerError(std::string data)
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