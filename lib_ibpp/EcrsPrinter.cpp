#include "EcrsPrinter.hpp"

std::map<std::string, std::string> StatusEcrs::StatusEcrs1 = {
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

std::map<std::string, std::string> StatusEcrs::StatusEcrs2 = {
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

std::map<std::string, std::string> StatusEcrs::StatusEcrs3 = {
    // {"0", "MECANISMO DE IMPRESIÓN: FUERA DE LINEA"},
    {"1", "TAPA ABIERTA"},
    {"2", "SENSOR DE TEMPERATURA DEL CABEZAL: ALTA"},
    {"3", "ERROR NO RECUPERABLE: ERROR"},
    {"4", "CORTADORA DE PAPEL: ERROR"},
    {"5", "BUFFER OVERFLOW: ERROR"},
    {"6", "SENSOR DE FIN DE PAPEL: SIN PAPEL"},
    {"7", "SENSOR DE AUSENCIA DE PAPEL: SIN PAPEL"},
};

EcrsPrinter::EcrsPrinter(std::string cport) : PrinterInterface::PrinterInterface(cport) {}

void EcrsPrinter::refrescarConfiguracion()
{
    OATPP_LOGI("Refrescando configuracion","...");
    this->maxLength = 48;
    this->lineasTextoNF = 4;
    OATPP_LOGI("->","------------[HECHO]---------------");
}

std::string EcrsPrinter::readPort()
{
    OATPP_LOGI("LEYENDO PUERTO","...");
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
    OATPP_LOGI("->","------------[HECHO]-----------");
    return bytes;
}

std::string EcrsPrinter::ejecutarComando(oatpp::DTO *dto)
{
    OATPP_LOGI("EJECUTANDO COMANDO","...");
    PrinterDto *data = ((PrinterDto *)dto);
    std::string resultado = "", cmd = data->cmd.getValue("");
    OATPP_LOGI("Comando a ejecutar: %s", cmd.c_str());
    nlohmann::json jsonResponse;

    StatusEcrs status = leerStatus();

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
    else if ("notaDebito" == cmd)
    {
        resultado = facturarNotaDebito(data);
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
    else if ("config_formasPago" == cmd)
    {
        jsonResponse["configMethodsPay"] = configPrinterMP();
    }
    else
    {
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "Comando desconocido.");
    }
    resultado = resultado.empty() ? jsonResponse.dump() : resultado;
    OATPP_LOGI("-COMANDO EJECUTADO","-");
    return resultado;
}

void EcrsPrinter::configurarFuente()
{
    OATPP_LOGI("CONFIGURANDO FUENTE","...");
    std::string comando = ":230";
    OATPP_LOGI("->FUENTE CONFIGURADA","<-");
    prepararYenviarComando(comando);
}

std::string EcrsPrinter::cerrarDocumentoFiscal()
{
    OATPP_LOGI("CERRANDO DOCUMENTO FISCAL","...");
    std::string comando = "F5";
    prepararYenviarComando(comando);
    OATPP_LOGI("CERRANDO DOCUMENTO FISCAL","");
    return "";
}

void EcrsPrinter::anularDocumentoFiscal()
{
    OATPP_LOGI("ANULANDO DOCUMENTO FISCAL","...");
    std::string comando = "F6";
    prepararYenviarComando(comando);
    OATPP_LOGI("->DOCUMENTO FISCAL ANULADO","<-");
}

std::string EcrsPrinter::cerrarDocumentoNoFiscal()
{
    OATPP_LOGI("CERRANDO DOCUMENTO FISCAL","...");
    std::string comando = "N3", respuesta;
    try
    {
        prepararYenviarComando(comando);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    OATPP_LOGI("->DOCUMENTO FISCAL CERRADO","<-");
    return "";
}

std::string EcrsPrinter::reporteX(bool imprimir)
{
    OATPP_LOGI("GENERANDO REPORTE X","...");
    std::string comando, resultado;
    comando = "R0" + this->SEPARADOR + "0";

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

    OATPP_LOGI("->","REPORTE X GENERADO<-");
    return resultado;
}

std::string EcrsPrinter::reporteZ()
{
    OATPP_LOGI("GENERANDO REPORTE Z","...");
    std::string comando, resultado;
    comando = "R0" + this->SEPARADOR + "1";

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
    OATPP_LOGI("->REPORTE Z GENERADO","<-");
    return resultado;
}

std::string EcrsPrinter::documentoNoFiscal(PrinterDto *dat)
{
    OATPP_LOGI("GENERANDO DOCUMENTO NO FISCAL","...");
    auto items = dat->textoNF.get();
    std::string texto, respuesta;

    //START ABRIR DOCUMENTO NO FISCAL
    std::string comando = "N0";
    prepararYenviarComando(comando);
    //END FISCAL DOC 

    //START Printer line in Document No Fiscal
    for (auto i = items->begin(); i != items->end(); ++i)
    {
        comando = "N1" + this->SEPARADOR;
        comando += i->getValue(""); //linea de comentario
        comando += this->SEPARADOR + "0"; //Tamaño (opcional)
        comando += this->SEPARADOR + "0"; // Alineación: IZQUIERDA (“0”), CENTRADO (“1”) o DERECHA (“2”)
        comando += this->SEPARADOR + "0"; //Atributo: DEFAULT(0), FONTB (0x1), BOLD (0x2), UNDERLINE (0x4) y/o REVERSE (0x8)
        prepararYenviarComando(comando);
    }
    //END  Printer line in Document No Fiscal
    respuesta = cerrarDocumentoNoFiscal();
    OATPP_LOGI("->","DOCUMENTO NO FISCAL GENERADO<-");
    return "DOCUMENTO NO FISCAL GENERADO EXITOSAMENTE!";
}

void EcrsPrinter::lineaComentario(std::string texto)
{
    OATPP_LOGI("CREANDO LINEA DE COMENTARIO","...");
    std::string comando = "F7" + this->SEPARADOR + fillString(texto.substr(0, 48), 48);

    prepararYenviarComando(comando);
    OATPP_LOGI("->","LINEA DE COMENTARIO CREADA<-");
}

std::string EcrsPrinter::leerFechaYhora()
{
    OATPP_LOGI("LEYENDO FECHA Y HORA","...");
    std::string comando, respuesta, resultado, anio, mes, dia, hora;
    comando = "T0";
    respuesta = prepararYenviarComando(comando);
    auto respuesta_ = splitString(respuesta, '\x1c');
    if (respuesta.size() > 12)
    {
        anio = respuesta_[1];
        OATPP_LOGI("TESTING YEAR ", "===========HERE======%s======HERE==========", anio.c_str());
        anio = anio.substr(0,4);
        mes = respuesta_[1]; mes = mes.substr(4,2);
        dia = respuesta_[1]; dia = dia.substr(6);
        hora = respuesta_[2];

        resultado = anio + "/" + mes + "/" + dia;
        hora.insert(2, ":");
        hora.insert(5, ":");
        resultado += " " + hora;
    }
    OATPP_LOGI("->","LECTURA DE FECHA Y HORA TERMINADA <-");
    return resultado;
}

std::string EcrsPrinter::configPrinterMP(){
        //CONFIGURAR METODOS DE PAGO
        OATPP_COMPONENT(oatpp::Object<PrinterConfig>, printerConfig);
        OATPP_ASSERT_HTTP((printerConfig && printerConfig->ecrs), Status::CODE_400, "Debe configurar los metodos de pago en printer_config.json");
        OATPP_ASSERT_HTTP(printerConfig->ecrs->formasPago->size() > 0, Status::CODE_400, "Debe configurar los metodos de pago en printer_config.json");
        FormasPago::Wrapper formasPago = nullptr;
        for (auto &&metodos : *printerConfig->ecrs->formasPago.get())
        {
            if (metodos->serial.getValue("") == this->device->serial.getValue(""))
            {
                formasPago = metodos;
                OATPP_LOGD("METODOS DE PAGO ENCONTRADOS...", "%s", metodos->serial.getValue("").c_str());
                break;
            }
        }
        if (!formasPago)
        {
            for (auto &&metodos : *printerConfig->ecrs->formasPago.get())
            {
                if (metodos->serial.getValue("").empty() || metodos->serial.getValue("") == "*")
                {
                    formasPago = metodos;
                    break;
                }
            }
        }

        for (size_t i = 0; i <= 14; i++)
        {
            if(i==0){
                auto fp1 = formasPago->fp_01.getValue("EFECTIVO");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "0";
                cmd += this->SEPARADOR + fp1;
                auto resOMP = prepararYenviarComando(cmd);
                //START: VALIDAR "CONFIG METODOS DE PAGO DISPONIBLE"
                auto resOMP_split = splitString(resOMP, '\x1c');
                OATPP_LOGI("Validation Config MP [res_split]:", "%s", resOMP_split[0].substr(1,4).c_str());
                 if(resOMP_split[0].substr(1,4) == "1542"){
                    OATPP_ASSERT_HTTP(false, Status::CODE_400, "ERROR: PRIMERO DEBE GENERAR UN 'REPORTE Z'");
                 }
                 //END: VALIDAR "CONFIG METODOS DE PAGO DISPONIBLE"
            }
            if(i==1){
                auto fp2 = formasPago->fp_02.getValue("DEBITO");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "1";
                cmd += this->SEPARADOR + fp2;
                prepararYenviarComando(cmd);
            }
            if(i==2){
                auto fp3 = formasPago->fp_03.getValue("CREDITO");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "2";
                cmd += this->SEPARADOR + fp3;
                prepararYenviarComando(cmd);
            }
            if(i==3){
                auto fp4 = formasPago->fp_04.getValue("PAGO MOVIL");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "3";
                cmd += this->SEPARADOR + fp4;
                prepararYenviarComando(cmd);
            }
            if(i==4){
                auto fp5 = formasPago->fp_05.getValue("DIVISA");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "4";
                cmd += this->SEPARADOR + fp5;
                prepararYenviarComando(cmd);
            }
            if(i==5){
                auto fp6 = formasPago->fp_06.getValue("ZELLE");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "5";
                cmd += this->SEPARADOR + fp6;
                prepararYenviarComando(cmd);
            }
            if(i==6){
                auto fp7 = formasPago->fp_07.getValue("CESTA TICKECT");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "6";
                cmd += this->SEPARADOR + fp7;
                prepararYenviarComando(cmd);
            }
            if(i==7){
                auto fp8 = formasPago->fp_08.getValue("CUPON");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "7";
                cmd += this->SEPARADOR + fp8;
                prepararYenviarComando(cmd);
            }
            if(i==8){
                auto fp9 = formasPago->fp_09.getValue("VISA");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "8";
                cmd += this->SEPARADOR + fp9;
                prepararYenviarComando(cmd);
            }
            if(i==9){
                auto fp10 = formasPago->fp_10.getValue("CIRRUS");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "9";
                cmd += this->SEPARADOR + fp10;
                prepararYenviarComando(cmd);
            }
            if(i==10){
                auto fp11 = formasPago->fp_11.getValue("AMERICAN EXPRESS");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "10";
                cmd += this->SEPARADOR + fp11;
                prepararYenviarComando(cmd);
            }
            if(i==11){
                auto fp12 = formasPago->fp_12.getValue("CHEQUE");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "11";
                cmd += this->SEPARADOR + fp12;
                prepararYenviarComando(cmd);
            }
            if(i==12){
                auto fp13 = formasPago->fp_13.getValue("DINNERS CLUB");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "12";
                cmd += this->SEPARADOR + fp13;
                prepararYenviarComando(cmd);
            }
            if(i==13){
                auto fp14 = formasPago->fp_14.getValue("TARJETA REGALO");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "13";
                cmd += this->SEPARADOR + fp14;
                prepararYenviarComando(cmd);
            }
        }
        
        
        printDTO(formasPago);

    //FINALIZAR CONFIGURACION

//    //OBTENER CONFIG METHODS PAY
//    std::string pm[13];
//    for (size_t i = 0; i < 14; i++)
//    {
//     std::string cmd = "C9" + this->SEPARADOR + std::to_string(static_cast<int>(i));
//     auto resCmd = prepararYenviarComando(cmd);
//     pm[i] = resCmd;
//    }
//    //FINALIZAR OBTENER CONFIG METHODS PAY
    return "Success!";
}

std::string EcrsPrinter::facturarNotaDebito(PrinterDto *dat)
{
    OATPP_LOGI("INICIANDO DOCUMENTO FISCAL", "CMD == %s", dat->cmd.get()->c_str());

    //INICIALIZAR FACTURA
    std::string fechaTimestamp = leerFechaYhora();
    std::string resultado, comando = "F0" + this->SEPARADOR;
    double tasaE, tasaP, tasaG, tasaA, tasaR;
    tasaE = tasaP = tasaG = tasaA = tasaR = 0;
    std::string rawSubtotal;

    auto device = this->getAllDevices();
    //START VALIDACIONES TAMAÑO DE CARACTERES
    OATPP_ASSERT_HTTP(dat->factura->cliente->nombre.getValue("").size() <= 100, Status::CODE_400, "NOMBRE DE CLIENTE NO DEBE SUPERAR LOS 100 CARACTERES!");
    OATPP_ASSERT_HTTP(dat->factura->cliente->rif.getValue("").size() <= 11, Status::CODE_400, "RIF DE CLIENTE NO DEBE SUPERAR LOS 11 CARACTERES!");
    if(dat->factura->notaFiscal != nullptr)
    OATPP_ASSERT_HTTP(std::to_string(static_cast<int>(dat->factura->notaFiscal->factura.getValue(0))).size() <= 8, Status::CODE_400, "NUMERO DE FACTURA EN NOTA FISCAL NO DEBE SUPERAR LOS 8 CARACTERES!");
    //END VALIDACIONES TAMAÑO DE CARACTERES

    OATPP_LOGI("el cliente es > ", "%s", dat->factura->cliente->nombre.getValue("").c_str());
    try
    {
        comando += "2";
            OATPP_LOGI("========[ESTE ES EL COMANDO PARA GENERAR UN DOCUMENTO FISCAL]: %s", comando.c_str())
            if(device->items[0]->model.getValue("") == "DTP80-i"){
             
              //START CONSTRUIR CABECERA
              comando += this->SEPARADOR + dat->factura->cliente->nombre.getValue("");
              comando += this->SEPARADOR + dat->factura->cliente->rif.getValue("");
                auto numFactuRef = fillNumbToString(dat->factura->notaFiscal->factura.getValue(0), 8);
                comando += this->SEPARADOR + numFactuRef; //Numero factura referencial
                auto fechaFactuRef = dat->factura->notaFiscal->fecha.getValue("");
                std::string fecha_convertida = fechaFactuRef.substr(8, 2) + fechaFactuRef.substr(5, 2) + fechaFactuRef.substr(0, 4);
                comando += this->SEPARADOR + fecha_convertida; //Fecha factura referencial
                auto serialFiscalRef = dat->factura->notaFiscal->serialFiscal.getValue("");
                comando += this->SEPARADOR + serialFiscalRef; //Serial fiscal de referencia
                comando += this->SEPARADOR + "0"; //Imprimir logo (true||false)
                comando += this->SEPARADOR + "";
                prepararYenviarComando(comando);

                if(dat->linea_comentario.getValue("")!=""){
                    //START LINEA COMENTARIO
                    comando = "F7" + this->SEPARADOR;
                    comando += dat->linea_comentario.getValue("");
                    comando += this->SEPARADOR + "0";
                    comando += this->SEPARADOR + "1";
                    comando += this->SEPARADOR + "0x8";
                    prepararYenviarComando(comando);
                    //END LINEA COMENTARIO
                }
                //END CONSTRUIR CABECERA

              //START ITEMS
              auto items = dat->factura->items.get();
              for(auto i = items->begin(); i != items->end(); i++){

                //START VALIDACIONES ITEMS CHAR/DIGIT NUM
                OATPP_ASSERT_HTTP(i->get()->codigo.getValue("").size() <= 30, Status::CODE_400, "CODIGO PRODUCTO ITEM NO DEBE SUPERAR LOS 30 CARACTERES!");
                auto auxCantidadItem = std::to_string(i->get()->cantidad.getValue(0));
                OATPP_ASSERT_HTTP((auxCantidadItem.size()-6)-1 <= 11, Status::CODE_400, "CANTIDAD PRODUCTO ITEM NO DEBE SUPERAR LOS 11 DIGITOS!");
                auto auxPrecioItem = std::to_string(i->get()->precio.getValue(0)); 
                OATPP_ASSERT_HTTP(auxPrecioItem.size()-3 <= 14, Status::CODE_400, "PRECIO PRODUCTO ITEM NO DEBE SUPERAR LOS 14 DIGITOS!");
                //END VALIDACIONES ITEMS CHAR/DIGIT NUM

                comando = "F1" + this->SEPARADOR + "0"; 
                comando += this->SEPARADOR + i->get()->producto;
                comando += this->SEPARADOR + fillString(i->get()->codigo, 30);
                auto cantidadI = fillNumbToString(i->get()->cantidad, 11);
                comando += this->SEPARADOR + cantidadI;
                OATPP_LOGI("CANTIDAD ITEM ===================>", "%s", cantidadI.c_str());
                comando += this->SEPARADOR + "";
                auto precioI = fillNumbToString(i->get()->precio, 14, 2); 
                OATPP_LOGI("PRECIO ITEM ===================>", "%s", precioI.c_str());
                comando += this->SEPARADOR + precioI;
                if(i->get()->tasa == "E"){
                    comando += this->SEPARADOR + "0";
                }else if(i->get()->tasa == "G"){
                    comando += this->SEPARADOR + "1";
                }else if(i->get()->tasa == "R"){
                    comando += this->SEPARADOR + "2";
                }else if(i->get()->tasa == "A"){
                    comando += this->SEPARADOR + "3";
                }else if(i->get()->tasa == "P"){
                    comando += this->SEPARADOR + "4";
                }else{
                    OATPP_ASSERT_HTTP(false, Status::CODE_400, "Error: Tipo de impuesto no registrado");
                }
                comando += this->SEPARADOR + "2";
                comando += this->SEPARADOR + "0";
                prepararYenviarComando(comando);
                //START DESCUENTO SOBRE ITEM (SI 'descuento' > 0)
                auto descuentoI = fillNumbToString(i->get()->descuento, 16, 2);
                OATPP_LOGI("Descuento ==>", "%d", descuentoI);
                if(std::stoi(descuentoI) > 0){
                    OATPP_LOGI("DESCUENTO --->", "[Mayor que cero]");
                    OATPP_LOGI("TIPO DE DESCUENTO --->", "%s", i->get()->tipo_descuento->c_str());
                    if(i->get()->tipo_descuento == "monto"){
                        comando = "F1" + this->SEPARADOR + "2"; 
                        comando += this->SEPARADOR + "Descuento sobre Item";
                        comando += this->SEPARADOR + fillString(i->get()->codigo, 30);
                        auto cantidadI = fillNumbToString(i->get()->cantidad, 11);
                        comando += this->SEPARADOR + cantidadI;
                        OATPP_LOGI("CANTIDAD ITEM ===================>", "%s", cantidadI.c_str());
                        comando += this->SEPARADOR;
                        comando += this->SEPARADOR + descuentoI;
                        if(i->get()->tasa == "E"){
                            comando += this->SEPARADOR + "0";
                        }else if(i->get()->tasa == "G"){
                            comando += this->SEPARADOR + "1";
                        }else if(i->get()->tasa == "R"){
                            comando += this->SEPARADOR + "2";
                        }else if(i->get()->tasa == "A"){
                            comando += this->SEPARADOR + "3";
                        }else if(i->get()->tasa == "P"){
                            comando += this->SEPARADOR + "4";
                        }else{
                            OATPP_ASSERT_HTTP(false, Status::CODE_400, "Error: Tipo de impuesto no registrado");
                        }
                        comando += this->SEPARADOR + "2";
                        comando += this->SEPARADOR + "0";
                        prepararYenviarComando(comando);
                    }else if(i->get()->tipo_descuento == "porcentaje"){
                        comando = "F1" + this->SEPARADOR + "2";
                        comando += this->SEPARADOR + "Descuento sobre Item";
                        comando += this->SEPARADOR;
                        comando += this->SEPARADOR + "0";
                        comando += this->SEPARADOR;
                        comando += this->SEPARADOR + descuentoI;
                        comando += this->SEPARADOR + "1";
                        comando += this->SEPARADOR + "2";
                        comando += this->SEPARADOR + "0";
                        prepararYenviarComando(comando);
                        
                    }
                }    
              }
              //END ITEMS

              //START DESCUENTO GLOBAL
              auto descuentoDG = fillNumbToString(dat->factura->descuento->descuento, 16, 2);
              OATPP_LOGD("VALOR DESCUENTO GLOBAL (conversión a entero 'printlang') ==>","%s", descuentoDG.c_str());
              auto tipo_descuento_DG = dat->factura->descuento->tipo_descuento.getValue("");
              OATPP_LOGI("TIPO DESCUENTO GLOBAL ===> ", tipo_descuento_DG.c_str());
              if(std::stoi(descuentoDG) > 0){
                if(tipo_descuento_DG == "porcentaje"){
                    comando = "F3" + this->SEPARADOR;
                    comando += "0";
                    comando += this->SEPARADOR + "Desc global %";
                    comando += this->SEPARADOR + descuentoDG;
                    prepararYenviarComando(comando);
                }else if(tipo_descuento_DG == "monto"){
                    comando = "F3" + this->SEPARADOR;
                    comando += "1";
                    comando += this->SEPARADOR + "Desc global";
                    comando += this->SEPARADOR + descuentoDG;
                    prepararYenviarComando(comando);
                }
              }
              //END DESCUENTO GLOBAL

              //START SUBTOTAL PARCIAL
              comando = "F2" + this->SEPARADOR + "0";
              prepararYenviarComando(comando);
              //END SUBTOTAL PARCIAL
              
              //START SUBTOTAL DEFINITIVO (TOTAL)
              rawSubtotal = subtotal(dat->factura->pagos);
              //END SUBTOTAL DEFINITIVO (TOTAL)

              //START PAGOS
              pagos(dat->factura->pagos);
              //END PAGOS
              
              //START LINEA COMENTARIO
              comando = "F7" + this->SEPARADOR;
              comando += dat->factura->pie[0].getValue("");
              comando += this->SEPARADOR + "0";
              comando += this->SEPARADOR + "1";
              comando += this->SEPARADOR + "0x8";
              prepararYenviarComando(comando);
              //END LINEA COMENTARIO
            }
        cerrarDocumentoFiscal();
       
        //ANALIZAR FACTURA EMITIDA
        auto respReport = prepararYenviarComando("R9");
        auto splitRes_f = splitString(respReport, '\x1c');
        auto recent_invoice = splitRes_f[1];
        OATPP_LOGI("Ultima factura emitida ==> %s", recent_invoice.c_str()); 
        //FIN DE ANALISIS DE FACTURA EMITIDA

        resultado = analizarFactura(dat->cmd.getValue(""), std::stoi(recent_invoice), fechaTimestamp, rawSubtotal, 2);
    }
    catch (...)
    {
        anularDocumentoFiscal();
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "HA OCURRIDO UN ERROR");
    }
    return resultado;
}

// Función para obtener el valor de "price_old" en USD
double getPriceOldUSD(const std::string& data) {
    // Encuentra la posición de "usd"
    size_t pos_usd = data.find("\"usd\"");
    if (pos_usd == std::string::npos) {
        std::cerr << "No se encontró la moneda USD en los datos proporcionados." << std::endl;
        return -1; // Retorna un valor negativo para indicar un error
    }

    // Encuentra la posición de "price_old"
    size_t pos_price_old = data.find("price_old", pos_usd);
    if (pos_price_old == std::string::npos) {
        std::cerr << "No se encontró el valor de price_old para USD en los datos proporcionados." << std::endl;
        return -1; // Retorna un valor negativo para indicar un error
    }

    // Encuentra el valor de price_old en USD
    size_t pos_colon = data.find(":", pos_price_old);
    size_t pos_comma = data.find(",", pos_price_old);
    std::string price_old_str = data.substr(pos_colon + 1, pos_comma - pos_colon - 1);

    // Convierte el valor a double y devuelve
    return std::stod(price_old_str);
}

// Función de escritura de datos recibidos
size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* data) {
    data->append((char*)contents, size * nmemb);
    return size * nmemb;
}

double getCurrentRate(){

    CURL* curl;
    CURLcode res;
    std::string data;
    double price_old_usd = 0.00;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    
    
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://pydolarvenezuela-api.vercel.app/api/v1/dollar/page?page=bcv");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Error al realizar la solicitud: " << curl_easy_strerror(res) << std::endl;
        } else {
            std::cout << "Datos recibidos: " << data << std::endl;
            price_old_usd = getPriceOldUSD(data);
            if (price_old_usd != -1) {
                std::cout << "El valor de price_old en USD es: " << price_old_usd << std::endl;
            }
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return price_old_usd;
}

std::string EcrsPrinter::facturar(PrinterDto *dat, bool docFiscal)
{
    OATPP_LOGI("INICIANDO DOCUMENTO FISCAL", "CMD == %s", dat->cmd.get()->c_str());

    //INICIALIZAR FACTURA
    std::string fechaTimestamp = leerFechaYhora();
    std::string resultado, comando = "F0" + this->SEPARADOR;
    double tasaE, tasaP, tasaG, tasaA, tasaR;
    tasaE = tasaP = tasaG = tasaA = tasaR = 0;
    std::string rawSubtotal;
    std::string resFdoc;
    auto device = this->getAllDevices();
    int docType = 0;

    //START VALIDACIONES TAMAÑO DE CARACTERES
    OATPP_ASSERT_HTTP(dat->factura->cliente->nombre.getValue("").size() <= 100, Status::CODE_400, "NOMBRE DE CLIENTE NO DEBE SUPERAR LOS 100 CARACTERES!");
    OATPP_ASSERT_HTTP(dat->factura->cliente->rif.getValue("").size() <= 11, Status::CODE_400, "RIF DE CLIENTE NO DEBE SUPERAR LOS 11 CARACTERES!");
    if(dat->factura->notaFiscal != nullptr)
    OATPP_ASSERT_HTTP(std::to_string(dat->factura->notaFiscal->factura.getValue(0)).size() <= 8, Status::CODE_400, "NUMERO DE FACTURA EN NOTA FISCAL NO DEBE SUPERAR LOS 8 CARACTERES!");
    //END VALIDACIONES TAMAÑO DE CARACTERES 

    OATPP_LOGI("el cliente es > ", "%s", dat->factura->cliente->nombre.getValue("").c_str());
    try
    {
        comando += (docFiscal == true ? "0" : (docFiscal == false ? "1" : "2"));
            OATPP_LOGI("========[ESTE ES EL COMANDO PARA GENERAR UN DOCUMENTO FISCAL]: %s", comando.c_str())
            if(device->items[0]->model.getValue("") == "DTP80-i"){
             
              //START CONSTRUIR CABECERA
              comando += this->SEPARADOR + dat->factura->cliente->nombre.getValue("");
              comando += this->SEPARADOR + dat->factura->cliente->rif.getValue("");
              if(docFiscal == false){
                docType = 1;
                auto numFactuRef = fillNumbToString(dat->factura->notaFiscal->factura.getValue(0), 8);
                comando += this->SEPARADOR + numFactuRef; //Numero factura referencial
                auto fechaFactuRef = dat->factura->notaFiscal->fecha.getValue("");
                std::string fecha_convertida = fechaFactuRef.substr(8, 2) + fechaFactuRef.substr(5, 2) + fechaFactuRef.substr(0, 4);
                comando += this->SEPARADOR + fecha_convertida; //Fecha factura referencial
                auto serialFiscalRef = dat->factura->notaFiscal->serialFiscal.getValue("");
                comando += this->SEPARADOR + serialFiscalRef; //Serial fiscal de referencia
                comando += this->SEPARADOR + "0"; //Imprimir logo (true||false)
                comando += this->SEPARADOR + "";
                resFdoc = prepararYenviarComando(comando);
                //END CONSTRUIR CABECERA
                OATPP_LOGI("LINEA COMENTARIO =>", "%s", dat->linea_comentario.getValue("").c_str());
                if(dat->linea_comentario.getValue("")!=""){
                    OATPP_LOGI("LINEA COMENTARIO", "[No esta vacia]");
                    //START LINEA COMENTARIO
                    comando = "F7" + this->SEPARADOR;
                    comando += dat->linea_comentario.getValue("");
                    comando += this->SEPARADOR + "0";
                    comando += this->SEPARADOR + "1";
                    comando += this->SEPARADOR + "0x8";
                    prepararYenviarComando(comando);
                    //END LINEA COMENTARIO
                }
              }else{
                comando += this->SEPARADOR;
                //Print NUM FACT
                comando += this->SEPARADOR;
                //Print DATE FACT REF
                comando += this->SEPARADOR;
                //Print Serial fiscal REF
                comando += this->SEPARADOR + "0";
                comando += this->SEPARADOR + "";
                resFdoc = prepararYenviarComando(comando);
                //END CONSTRUIR CABECERA
              }


              //START ITEMS
              auto items = dat->factura->items.get();
              for(auto i = items->begin(); i != items->end(); i++){

                //START VALIDACIONES ITEMS CHAR/DIGIT NUM
                OATPP_ASSERT_HTTP(i->get()->codigo.getValue("").size() <= 30, Status::CODE_400, "CODIGO PRODUCTO ITEM NO DEBE SUPERAR LOS 30 CARACTERES!");
                auto auxCantidadItem = std::to_string(i->get()->cantidad.getValue(0));
                OATPP_ASSERT_HTTP((auxCantidadItem.size()-6)-1 <= 11, Status::CODE_400, "CANTIDAD PRODUCTO ITEM NO DEBE SUPERAR LOS 11 DIGITOS!");
                auto auxPrecioItem = std::to_string(i->get()->precio.getValue(0)); 
                OATPP_ASSERT_HTTP(auxPrecioItem.size()-3 <= 14, Status::CODE_400, "PRECIO PRODUCTO ITEM NO DEBE SUPERAR LOS 14 DIGITOS!");
                //END VALIDACIONES ITEMS CHAR/DIGIT NUM

                comando = "F1" + this->SEPARADOR + "0"; 
                comando += this->SEPARADOR + i->get()->producto;
                comando += this->SEPARADOR + fillString(i->get()->codigo, 30);
                auto cantidadI = fillNumbToString(i->get()->cantidad, 11);
                comando += this->SEPARADOR + cantidadI;
                OATPP_LOGI("CANTIDAD ITEM ===================>", "%s", cantidadI.c_str());
                comando += this->SEPARADOR + "";
                auto precioI = fillNumbToString(i->get()->precio, 14, 2); 
                OATPP_LOGI("PRECIO ITEM ===================>", "%s", precioI.c_str());
                comando += this->SEPARADOR + precioI;
                if(i->get()->tasa == "E"){
                    comando += this->SEPARADOR + "0";
                }else if(i->get()->tasa == "G"){
                    comando += this->SEPARADOR + "1";
                }else if(i->get()->tasa == "R"){
                    comando += this->SEPARADOR + "2";
                }else if(i->get()->tasa == "A"){
                    comando += this->SEPARADOR + "3";
                }else if(i->get()->tasa == "P"){
                    comando += this->SEPARADOR + "4";
                }else{
                    OATPP_ASSERT_HTTP(false, Status::CODE_400, "Error: Tipo de impuesto no registrado");
                }
                comando += this->SEPARADOR + "2";
                comando += this->SEPARADOR + "0";
                prepararYenviarComando(comando);
                //START DESCUENTO SOBRE ITEM (SI 'descuento' > 0)
                auto descuentoI = fillNumbToString(i->get()->descuento, 16, 2);
                OATPP_LOGI("Descuento ==>", "%d", descuentoI);
                if(std::stoi(descuentoI) > 0){
                    OATPP_LOGI("DESCUENTO --->", "[Mayor que cero]");
                    OATPP_LOGI("TIPO DE DESCUENTO --->", "%s", i->get()->tipo_descuento->c_str());
                    if(i->get()->tipo_descuento == "monto"){
                        comando = "F1" + this->SEPARADOR + "2"; 
                        comando += this->SEPARADOR + "Descuento sobre Item";
                        comando += this->SEPARADOR + fillString(i->get()->codigo, 30);
                        auto cantidadI = fillNumbToString(i->get()->cantidad, 11);
                        comando += this->SEPARADOR + cantidadI;
                        OATPP_LOGI("CANTIDAD ITEM ===================>", "%s", cantidadI.c_str());
                        comando += this->SEPARADOR;
                        comando += this->SEPARADOR + descuentoI;
                        if(i->get()->tasa == "E"){
                            comando += this->SEPARADOR + "0";
                        }else if(i->get()->tasa == "G"){
                            comando += this->SEPARADOR + "1";
                        }else if(i->get()->tasa == "R"){
                            comando += this->SEPARADOR + "2";
                        }else if(i->get()->tasa == "A"){
                            comando += this->SEPARADOR + "3";
                        }else if(i->get()->tasa == "P"){
                            comando += this->SEPARADOR + "4";
                        }else{
                            OATPP_ASSERT_HTTP(false, Status::CODE_400, "Error: Tipo de impuesto no registrado");
                        }
                        comando += this->SEPARADOR + "2";
                        comando += this->SEPARADOR + "0";
                        prepararYenviarComando(comando);
                    }else if(i->get()->tipo_descuento == "porcentaje"){
                        comando = "F1" + this->SEPARADOR + "2";
                        comando += this->SEPARADOR + "Descuento sobre Item";
                        comando += this->SEPARADOR;
                        comando += this->SEPARADOR + "0";
                        comando += this->SEPARADOR;
                        comando += this->SEPARADOR + descuentoI;
                        comando += this->SEPARADOR + "1";
                        comando += this->SEPARADOR + "2";
                        comando += this->SEPARADOR + "0";
                        prepararYenviarComando(comando);
                        
                    }
                }    
              }
              //END ITEMS

              //START DESCUENTO GLOBAL
              auto descuentoDG = fillNumbToString(dat->factura->descuento->descuento, 16, 2);
              OATPP_LOGD("VALOR DESCUENTO GLOBAL (conversión a entero 'printlang') ==>","%s", descuentoDG.c_str());
              auto tipo_descuento_DG = dat->factura->descuento->tipo_descuento.getValue("");
              OATPP_LOGI("TIPO DESCUENTO GLOBAL ===> ", tipo_descuento_DG.c_str());
              if(std::stoi(descuentoDG) > 0){
                if(tipo_descuento_DG == "porcentaje"){
                    comando = "F3" + this->SEPARADOR;
                    comando += "0";
                    comando += this->SEPARADOR + "Desc global %";
                    comando += this->SEPARADOR + descuentoDG;
                    prepararYenviarComando(comando);
                }else if(tipo_descuento_DG == "monto"){
                    comando = "F3" + this->SEPARADOR;
                    comando += "1";
                    comando += this->SEPARADOR + "Desc global";
                    comando += this->SEPARADOR + descuentoDG;
                    prepararYenviarComando(comando);
                }
              }
              //END DESCUENTO GLOBAL

              //START SUBTOTAL PARCIAL
              comando = "F2" + this->SEPARADOR + "0";
              prepararYenviarComando(comando);
              //END SUBTOTAL PARCIAL
              
              //START SUBTOTAL DEFINITIVO
              rawSubtotal = subtotal(dat->factura->pagos);
              //END SUBTOTAL DEFINITIVO

              //START PAGOS
              pagos(dat->factura->pagos);
              //END PAGOS

              //START LINEA COMENTARIO
              comando = "F7" + this->SEPARADOR;
              comando += dat->factura->pie[0].getValue("");
              comando += this->SEPARADOR + "0";
              comando += this->SEPARADOR + "1";
              comando += this->SEPARADOR + "0x8";
              prepararYenviarComando(comando);
              //END LINEA COMENTARIO
            }
        cerrarDocumentoFiscal();

        //ANALIZAR FACTURA EMITIDA
        auto splitRes_f = splitString(resFdoc, '\x1c');
        auto recent_invoice = splitRes_f[1];
        OATPP_LOGI("Ultima factura emitida ==> %s", recent_invoice.c_str()); 
        //FIN DE ANALISIS DE FACTURA EMITIDA
        resultado = analizarFactura(dat->cmd.getValue(""), std::stoi(recent_invoice), fechaTimestamp, rawSubtotal, docType);
    }
    catch (...)
    {
        anularDocumentoFiscal();
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "HA OCURRIDO UN ERROR");
    }
    return resultado;
}

void deep(){
//         void EcrsPrinter::descuentoGlobal(double tasaE, double tasaG, double tasaR, double tasaA, double tasaP, oatpp::Object<Descuento> descuento)
// {
//     OATPP_LOGI("CREANDO DESCUENTO GLOBAL","...");
//     double porcentaje = 0, subtotal;
//     subtotal = tasaE + tasaG + tasaR + tasaA + tasaP;

//     if (descuento == nullptr || descuento->tipo_descuento.getValue("").empty() || descuento->descuento.getValue(0) == 0)
//         return;

//     if (descuento->descuento.getValue(0) >= subtotal && descuento->tipo_descuento.getValue("") == "monto")
//         return;

//     if (descuento->tipo_descuento.getValue("") == "monto")
//     {
//         porcentaje = descuento->descuento.getValue(0) * 100 / subtotal;
//     }
//     else
//         porcentaje = descuento->descuento.getValue(0);

//     // Descuento por porcentaje
//     tasaE = tasaE * porcentaje / 100;
//     tasaG = tasaG * porcentaje / 100;
//     tasaR = tasaR * porcentaje / 100;
//     tasaA = tasaA * porcentaje / 100;
//     tasaP = tasaP * porcentaje / 100;

//     std::string comando = "F3" + this->SEPARADOR;
//     std::string descripcion;

//     descripcion = "DESCUENTO ";
//     if (descuento->tipo_descuento.getValue("") == "porcentaje")
//     {
//         descripcion += "(" + numbToString(descuento->descuento.getValue(0)) + "%)";
//     }

//     comando += fillString(descripcion, 100);
//     comando += fillNumbToString(tasaE, 12, 2);
//     comando += fillNumbToString(tasaG, 12, 2);
//     comando += fillNumbToString(tasaR, 12, 2);
//     comando += fillNumbToString(tasaA, 12, 2);
//     comando += fillNumbToString(tasaP, 12, 2);

//     prepararYenviarComando(comando);
//     OATPP_LOGI("-> DESCUENTO GLOABAL REALIZADO","<-");
// }

// double EcrsPrinter::descuento(std::list<oatpp::data::mapping::type::DTO::Object<Item>>::iterator i)
// {
//     OATPP_LOGI("entrando a EcrsPrinter::descuento:  ", "");
//     double descuento = 0, precio, nuevoPrecio = 0;
//     std::string comando;

//     if (i->get()->descuento.getValue(0) != 0 &&
//         (i->get()->tipo_descuento == "monto" || i->get()->tipo_descuento == "porcentaje"))
//     {
//         comando = "F3" + this->SEPARADOR;
//         std::string descripcion, tasa = "E";

//         if (i->get()->tasa == "E")
//             tasa = "0";
//         else if (i->get()->tasa == "G")
//             tasa = "1";
//         else if (i->get()->tasa == "R")
//             tasa = "2";
//         else if (i->get()->tasa == "A")
//             tasa = "3";
//         else if (i->get()->tasa == "P")
//             tasa = "4";

//         descuento = i->get()->descuento.getValue(0);
//         precio = i->get()->precio.getValue(0) * i->get()->cantidad.getValue(0);

//         descripcion = "DESCUENTO ";

//         if (i->get()->tipo_descuento == "porcentaje")
//         {
//             descripcion += "(" + numbToString(i->get()->descuento.getValue(0)) + "%)";
//             nuevoPrecio = (precio * i->get()->descuento.getValue(0) / 100);
//         }
//         else
//         {
//             nuevoPrecio = i->get()->descuento.getValue(0);
//         }

//         if (precio - nuevoPrecio <= 0 || fillNumbToString(precio - nuevoPrecio, 10, 2) == "0000000000")
//             return 0;

//         comando += fillString(descripcion, 100);
//         comando += this->fillNumbToString(nuevoPrecio, 10, 2);
//         comando += tasa;

//         prepararYenviarComando(comando);
//     }
//     OATPP_LOGI("saliendo de EcrsPrinter::descuento:  ", "");
//     return round2decimals(nuevoPrecio);
// }
}

std::string EcrsPrinter::items(oatpp::DTO::List<oatpp::Object<Item>> items) { throw; }

std::string EcrsPrinter::subtotal(oatpp::DTO::List<oatpp::Object<Pago>> pagos)
{
    OATPP_LOGD("PROCESANDO SUBTOTAL", "...");
    auto device = this->getAllDevices();
    std::string comando, rawSubtotal;
    bool igtf = contieneIGTF(pagos);
    float amountIGTF = cantidadPagoIGTF(pagos);

    OATPP_LOGI("amountIGTF : "," %F",amountIGTF);


    if (igtf)
    {
        OATPP_LOGD("IGTF IN SUBTOTAL ==>", "TRUE")
        comando = "F2" + this->SEPARADOR;
        comando += "1";
        comando += this->SEPARADOR + fillNumbToString(amountIGTF, 16, 2);
        comando += this->SEPARADOR + "1";
        comando += this->SEPARADOR + "S";
    }
    else
    {
        comando = "F2" + this->SEPARADOR;
        comando += "1";
    }

    rawSubtotal = prepararYenviarComando(comando);
    OATPP_LOGD("SUBTOTTAL PROCESADO", "---[HECHO]---");
    return rawSubtotal;
}

std::string EcrsPrinter::pagos(oatpp::DTO::List<oatpp::Object<Pago>> pagos)
{
    OATPP_LOGD("PROCESANDO PAGOS", "...");
    std::string comando, rawSubtotal;

    if (!pagos.get()->empty())
    {
        OATPP_LOGI("Condicion--->", "PAGOS NOT EMPTY");
        auto item = pagos.get();
        auto countI = 0;
        OATPP_LOGI("--->", "get pagos success!");
        for (auto i = item->begin(); i != item->end(); i++)
        {
            countI++;
            OATPP_LOGI("Iterando pagos ITEM--->", "%d", countI);
            OATPP_LOGI("Item element MONTO--->", "%f", i->get()->monto.getValue(0.0));

            float monto_item = i->get()->monto.getValue(0.00); // Tu número flotante
            
            // // Obtener la parte decimal del número
            // float parte_decimal = monto_item - floor(monto_item);
            // std::cout << "Parte Decimal monto: " << parte_decimal << std::endl;
            // // Redondear solo si la parte decimal es mayor o igual a 0.09
            // if (parte_decimal >= 0.095 && parte_decimal <= 0.10) {
            //     monto_item = monto_item + 0.01;
            //      std::cout << "CEIL: " << monto_item << std::endl;
            // }
            // std::cout << "Número redondeado: " << monto_item << std::endl;

            i->get()->monto = monto_item;

            if (i->get()->impuesto == nullptr || i->get()->impuesto->igtf.getValue(false) == false)
            {
                OATPP_LOGI("Condicion--->", "SIN APLICAR IGTF");
                comando = "F4" + this->SEPARADOR;
                comando += "0";
                auto metodo_id = static_cast<int>(i->get()->metodo_id);
                OATPP_LOGI("MetodoID=>", "%d", metodo_id-1);
                comando += this->SEPARADOR + std::to_string(metodo_id-1); 
                comando += this->SEPARADOR + "Pago";
                auto montoP = fillNumbToString(i->get()->monto, 16, 2);
                OATPP_LOGI("Monto=>", "%s", montoP.c_str());
                comando += this->SEPARADOR + montoP;
                prepararYenviarComando(comando);
            }else if(i->get()->impuesto->igtf == true){
                OATPP_LOGI("Condicion--->", "APLICANDO IGTF");
                comando = "F11" + this->SEPARADOR;
                comando += "0";
                comando += this->SEPARADOR + "Pago";
                auto montoP = fillNumbToString(i->get()->monto, 16, 2);
                OATPP_LOGI("Monto=>", "%s", montoP.c_str());
                comando += this->SEPARADOR + montoP;
                auto tasaP = 36.50;
                if(i->get()->impuesto->tasa_cambio != nullptr) tasaP = i->get()->impuesto->tasa_cambio;
                OATPP_ASSERT_HTTP(std::to_string(tasaP).length() <= 12, Status::CODE_400, "ERROR: LA TASA DE LA MONEDA EXTRANJERA NO DEBE SUPERAR LOS 12 DIGITOS!");
                comando += this->SEPARADOR + fillNumbToString(tasaP, 12, 6);
                comando += this->SEPARADOR + "$";
                auto metodo_id = static_cast<int>(i->get()->metodo_id);
                OATPP_LOGI("MetodoID=>", "%d", metodo_id-1);
                comando += this->SEPARADOR + std::to_string(metodo_id-1);
                prepararYenviarComando(comando); 
            }
        }
    }
    OATPP_LOGD("PAGOS PROCESADOS", "---[HECHO]--");

    return "success";
}

std::string EcrsPrinter::pagos2(oatpp::DTO::List<oatpp::Object<Pago>> pagos, float amountRelationPrice)
{
    OATPP_LOGD("PROCESANDO PAGOS", "...");
    std::string comando, rawSubtotal;

    if (pagos.get() != nullptr)
    {
        auto item = pagos.get();
        for (auto i = item->begin(); i != item->end(); i++)
        {
            if (i->get()->impuesto->igtf == false && i->get()->monto == amountRelationPrice)
            {
                OATPP_LOGI("Condicion--->", "SIN APLICAR IGTF");
                comando = "F4" + this->SEPARADOR;
                comando += "0";
                auto metodo_id = static_cast<int>(i->get()->metodo_id);
                OATPP_LOGI("MetodoID=>", "%d", metodo_id-1);
                comando += this->SEPARADOR + std::to_string(metodo_id-1);
                comando += this->SEPARADOR + "Pago";
                auto montoP = fillNumbToString(i->get()->monto, 16, 2);
                OATPP_LOGI("Monto=>", "%s", montoP.c_str());
                comando += this->SEPARADOR + montoP;
                prepararYenviarComando(comando);
            }else if(i->get()->impuesto->igtf == true && i->get()->monto == amountRelationPrice){
                OATPP_LOGI("Condicion--->", "APLICANDO IGTF");
                comando = "F11" + this->SEPARADOR;
                comando += "0";
                comando += this->SEPARADOR + "Pago";
                auto montoP = fillNumbToString(i->get()->monto, 16, 2);
                OATPP_LOGI("Monto=>", "%s", montoP.c_str());
                comando += this->SEPARADOR + montoP;
                auto tasaP = 36.50;
                if(i->get()->impuesto->tasa_cambio != nullptr) tasaP = i->get()->impuesto->tasa_cambio;
                comando += this->SEPARADOR + fillNumbToString(tasaP, 12, 6);
                comando += this->SEPARADOR + "$";
                auto metodo_id = static_cast<int>(i->get()->metodo_id);
                OATPP_LOGI("MetodoID=>", "%d", metodo_id-1);
                comando += this->SEPARADOR + std::to_string(metodo_id-1);
                prepararYenviarComando(comando); 
            }
        }
    }
    OATPP_LOGD("PAGOS PROCESADOS", "---[HECHO]--");

    return "success";
}

void EcrsPrinter::cabecera(oatpp::DTO::List<oatpp::String> cabecera)
{
    OATPP_LOGI("entrando a EcrsPrinter::cabecera:  ", "");
    for (auto i = cabecera->begin(); i != cabecera->end(); i++)
    {
        if (!i->getValue("").empty())
        {
            lineaComentario(i->getValue(""));
        }
    }
    lineaComentario("");
    auto device = this->getAllDevices();
    if (device->items[0]->model.getValue("") == "DTP80-i" || device->items[0]->model.getValue("") == "DTP80-i")
    {
        lineaComentario("Articulo                          Monto");
    }
    else
    {
        lineaComentario("Articulo                                   Monto");
    }

    lineaComentario(fillString("", 48, '-'));

    OATPP_LOGI("saliendo de EcrsPrinter::cabecera:  ", "");
}

void EcrsPrinter::pie(oatpp::DTO::List<oatpp::String> pie)
{
    OATPP_LOGI("entrando a EcrsPrinter::pie: ", "");
    std::string texto;
    int caracteresPorLinea = 48;
    int lineasMax = 10;
    for (auto i = pie->begin(); i != pie->end(); i++)
        texto += i->getValue("") + ". ";

    this->trim(texto);

    for (size_t i = 0; i < texto.size() && (i / caracteresPorLinea) < lineasMax; i += caracteresPorLinea)
        lineaComentario(trim(texto.substr(i, caracteresPorLinea)));

    OATPP_LOGI("saliendo de EcrsPrinter::pie: ", "");
}

std::string EcrsPrinter::prepararComando(std::string comando)
{

    return this->STX + comando + this->END;
}

std::string EcrsPrinter::enviarComando(std::string comando)
{
    OATPP_LOGI("SERVICIO ECRS: ", "------START: enviarComando(cmd)---------");
    OATPP_LOGI("P1 ==> %s", device->ip.getValue("").c_str());
    OATPP_LOGI("P2 ==> %s", device->port.getValue("").c_str());

    if(OpenPort()){
        OATPP_LOGI("PUERTO ABIERTO", "---[SerialComunication]---");
        std::string respuesta;
    
        writePort(comando);
    
        respuesta = this->readPort();
        this->writeLog(comando, respuesta, "ECRS");
    
        if (respuesta.empty())
            throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "SE HA PERDIDO COMUNICACIÓN CON EL DISPOSITIVO");
    
        if (respuesta.find(this->NAK) != std::string::npos)
            throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "SE HA ENCONTRADO UN ERROR AL ENVIAR EL COMANDO");
        return respuesta;
    }else
    {
        OATPP_LOGI("Winsock", "---[SocketComunication]---");
        std::string respuesta;
           OATPP_LOGI("Comando ->","%s", comando.c_str());
        
           // Inicializar Winsock
           WSADATA wsaData;
           if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
               std::cerr << "Error al inicializar Winsock" << std::endl;
           }
        
           // Crear un objeto de socket
           SOCKET cliente = socket(AF_INET, SOCK_STREAM, 0);
           if (cliente == INVALID_SOCKET) {
               std::cerr << "Error al crear el socket" << std::endl;
               WSACleanup();
           }
        
           // Conectar al emulador de impresora fiscal en el puerto 3010
           sockaddr_in serverAddr;
           serverAddr.sin_family = AF_INET;
           serverAddr.sin_addr.s_addr = inet_addr( device->ip.getValue("").c_str() );
           serverAddr.sin_port = htons( std::stoi(device->port.getValue("").c_str()) );
           if (connect(cliente, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
               std::cerr << "Error al conectar al servidor" << std::endl;
               closesocket(cliente);
               WSACleanup();
                throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "SE HA PERDIDO COMUNICACIÓN CON EL DISPOSITIVO");
           }
        
           // Formar la trama de comando para obtener el estado
           const char* comando_obtener_estado = comando.c_str(); // Utilizar const char* en lugar de std::string
           OATPP_LOGI("Comando preparado: %s", comando_obtener_estado);
           if (send(cliente, comando_obtener_estado, strlen(comando_obtener_estado), 0) == SOCKET_ERROR) { // Utilizar strlen para obtener la longitud del comando
               std::cerr << "Error al enviar el comando" << std::endl;
               closesocket(cliente);
               WSACleanup();
           }
        
           // Recibir la respuesta del emulador
           char datos_recibidos[1024];
           int bytesRecibidos = recv(cliente, datos_recibidos, sizeof(datos_recibidos), 0);
           if (bytesRecibidos == SOCKET_ERROR) {
               std::cerr << "Error al recibir datos" << std::endl;
           } else {
               // Asignar los datos recibidos a la cadena respuesta utilizando la longitud recibida
               respuesta = std::string(datos_recibidos, bytesRecibidos);
               OATPP_LOGI("Respuesta del emulador: %s", respuesta.c_str());
           }
        
           // Cerrar la conexión
           closesocket(cliente);
           WSACleanup();
           // //END WEBSOCKET
           OATPP_LOGI("SERVICIO ECRS: ", "------END: enviarComando(cmd)---------");
           return respuesta; // Devolver directamente la cadena respuesta
    }
}


// StatusEcrs EcrsPrinter::leerStatus()
// {
//     OATPP_LOGI("entrando a EcrsPrinter::leerStatus: ", "");
//     StatusEcrs status;
//     std::string comando;
//     std::string respuesta;

//     comando = "C0" + this->SEPARADOR;
//     respuesta = prepararYenviarComando(comando);
//     if (respuesta.size() == 11)
//     {
//         auto mapEstado = status.StatusEcrs2.find(respuesta.substr(7, 1));
//         OATPP_LOGD("MAPA DE ESTADOS 1 => ", "%s", mapEstado->first.c_str());
//         OATPP_LOGD("MAPA DE ESTADOS 2 => ", "%s", mapEstado->second.c_str());
//         status.sts2 = (mapEstado == status.StatusEcrs2.end() ? "DESCONOCIDO" : mapEstado->second);
//         OATPP_LOGD("MAPA DE ESTADO DOS  => ", "%s", status.sts2.c_str());
//     }
//     std::cout<<"RESPUESTA ANY POSITION => "<<respuesta[3]<<std::endl; 
//     if (respuesta[3] == '0' && respuesta[4] == '0')
//     {
//         OATPP_LOGI("Lectura de status (COND 0) ===>", "CUMPLE LA CONDICION");
//         try
//         {
//             status.sts3 = respuesta.substr(1, 4);
//             OATPP_LOGD("STATUS 3 -in try- ===>", "%s", status.sts3.c_str());
//             OATPP_LOGI("STATUS3 === %s", status.sts3.c_str());
//             std::stringstream s3;
//             s3 << std::hex << status.sts3;

//             if (status.sts3.find('0'))
//                 status.sts3 = "OK";
//             else
//                 status.sts3.clear();
//             unsigned n3;
//             s3 >> n3;

//             std::bitset<16> b3(n3);

//             std::string sb3 = b3.to_string();

//             std::reverse(sb3.begin(), sb3.end());
//             for (int i = 0; i < 16; i++)
//             {
//                 if (sb3[i] == '1')
//                 {
//                     auto it = status.StatusEcrs3.find(std::to_string(i));
//                     if (it != status.StatusEcrs3.end())
//                     {
//                         if (status.sts3.find("SIN PAPEL") != std::string::npos && (it->second.find("SIN PAPEL") != std::string::npos))
//                             continue;
//                         status.sts3 += (it != status.StatusEcrs3.end()) ? it->second + "\n" : "";
//                     }
//                 }
//             }
//         }
//         catch (...)
//         {
//             OATPP_LOGE("ERROR", "ANALIZANDO STATUS3 (estado mecanismo impresor)");
//         }
//     }

//     OATPP_LOGD("", "Status Fiscal: %s", this->trim(status.sts2).c_str());
//     OATPP_LOGD("", "Status Impresora: %s", this->trim(status.sts3).c_str());
//     OATPP_LOGI("Saliendo de EcrsPrinter::leerStatus: ", "");
//     status.sts3 = "OK";
//     return status;
// }
std::vector<std::string> EcrsPrinter::splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}
StatusEcrs EcrsPrinter::leerStatus()
{
    OATPP_LOGI("START::", "LECTURA DE STATUS INICIADA");
    StatusEcrs status;
    std::string comando;
    std::string respuesta;

    comando = "C0";
    respuesta = prepararYenviarComando(comando);

    auto splitRes = splitString(respuesta, '\x1c');

    auto statusRes = splitRes[1]; //Código de respuesta
    auto statusRes2 = splitRes[2]; // Estado
    auto statusRes3 = splitRes[3]; //Estado bloqueo

    OATPP_LOGD("Status ==>", "%s", statusRes.c_str());
    try
    {
     //Obteniendo 'StatusEcrs1'
     for(const auto& status1: status.StatusEcrs1){
        if(statusRes == "0"){
            status.sts3 = "OK";
        }else if(statusRes == status1.first){
            status.sts3 = status1.second;
        }
     }
    //Obteniendo 'StatusEcrs2'
    for(const auto& status2: status.StatusEcrs2){
        if(statusRes2 == status2.first){
            status.sts2 = status2.second;
        }
     }
    //Obteniendo 'StatusEcrs3'
    for(const auto& status3: status.StatusEcrs3){
        if(statusRes3 == status3.first){
            status.sts1 = status3.second;
        }
     }

    }catch (...){
        OATPP_LOGE("[ERROR]"," FALLO AL INTENTAR OBTENER STATUS");
    }

    OATPP_LOGD("Status_1 Print ==>", "%s", status.sts1.c_str());
    OATPP_LOGD("Status_2 Print ==>", "%s", status.sts2.c_str());
    OATPP_LOGD("Status_3 Print ==>", "%s", status.sts3.c_str());

    OATPP_LOGI("END::", "LECTURA DE STATUS FINALIZADA");

    return status;
}


oatpp::Object<PageDto<oatpp::Object<Device>>> EcrsPrinter::getAllDevices()
{
    OATPP_LOGI("CONSULTANDO LOS DISPOSITIVOS","...");
    auto result = oatpp::Object<PageDto<oatpp::Object<Device>>>::createShared();
    result->items = oatpp::data::mapping::type::DTO::Vector<Device::Wrapper>::createShared();
    result->count = unsigned(0);

    try
    {
        OATPP_COMPONENT(oatpp::Object<PrinterConfig>, printerConfig);
        OATPP_ASSERT_HTTP((printerConfig && printerConfig->devices), Status::CODE_400, "Debe configurar los dispositivos en printer_config.json");
        OATPP_ASSERT_HTTP(printerConfig->devices->size() > 0, Status::CODE_400, "Debe configurar los dispositivos en printer_config.json");

        // std::string url = "http://localhost:10110/info";

        // http::Request request(url);
        // const auto httpResponse = request.send("GET");
        // auto responseCode = httpResponse.status.code;
        // std::string response = std::string(httpResponse.body.begin(), httpResponse.body.end());
        // std::cout << url << " - " << httpResponse.status.code << " - " << httpResponse.status.reason << std::endl;
        // std::cout << response << std::endl;
        // auto response = "{\"app\":\"Liberty\",\"data\":{\"devices\":[{\"brand\":\"PNP\",\"model\":\"PF220\",\"serial\":\"EOG0003063\",\"type\":\"impresora\"}],\"fecha\":\"2022-06-20\",\"licencia\":\"X-Y-Z\",\"producto\":\"POS3DB\",\"razon_social\":\"PRUEBA\",\"rif\":\"J-1234567890\"},\"version\":\"1.0.0\"}";

        // nlohmann::json json = nlohmann::json::parse(response);
        // if (json["data"] != nullptr && json["data"]["devices"] != nullptr)
        // {
        result->count = printerConfig->devices->size();
        result->items = printerConfig->devices;

        // }
        // else
        // {
        //     OATPP_LOGE("getAllDevices", " LIBERTY NO HA SIDO INICIALIZADO");
        // }
    }
    catch (const std::exception &e)
    {
        OATPP_LOGE("getAllDevices", " %s", e.what());
    }
    OATPP_LOGI("->","Consulta de dispositivos finalizada <-");
    return result;
}
std::string EcrsPrinter::leerSerial()
{
    OATPP_LOGI("LECTURA DE SERIAL 'ECRS':  ", "Obteniendo datos de Serializacion...");
    std::string comando = "C2";
    std::string respuesta;
    std::string serial = "";

    respuesta = prepararYenviarComando(comando);
    auto splitRes = splitString(respuesta, '\x1c');
    serial = splitRes[1];

    if (serial != ""){
        respuesta = serial;
    }else{
        OATPP_ASSERT_HTTP(false, Status::CODE_101, "ERROR: lectura de serial");
    }

    OATPP_LOGI("LECTURA DE serial 'ECRS':  ", "-[Obtenidos datos Serializacion <<serial Extraido>> ]-");
    return respuesta;
}

std::string EcrsPrinter::leerRIF()
{
    OATPP_LOGI("LECTURA DE RIF 'ECRS':  ", "Obteniendo datos fiscales...");
    std::string comando = "C3";
    std::string respuesta;
    std::string rif = "";

    respuesta = prepararYenviarComando(comando);
    auto splitRes = splitString(respuesta, '\x1c');
    rif = splitRes[3];

    if (rif != ""){
        respuesta = rif;
    }else{
        OATPP_ASSERT_HTTP(false, Status::CODE_101, "ERROR: lectura de RIF");
    }

    OATPP_LOGI("LECTURA DE RIF 'ECRS':  ", "-[Obtenidos datos fiscales <<RIF Extraido>> ]-");
    return respuesta;
}

std::string EcrsPrinter::analizarFactura(std::string cmd, std::string fecha, std::string rawSubtotal) { throw; }

std::string EcrsPrinter::analizarFactura(std::string cmd, int nroFactura, std::string fecha, std::string rawSubtotal, int doc)
{
    OATPP_LOGI("ANALIZANDO FACTURA","...");
    nlohmann::json respuesta;
    try
    {
        if (rawSubtotal != "")
        {
            auto rawCamposSubtotal = splitString(rawSubtotal, '\x1c');
            if (!rawCamposSubtotal.empty()) {
                rawCamposSubtotal.erase(rawCamposSubtotal.begin());
                rawCamposSubtotal.erase(rawCamposSubtotal.end()-1);
            }
            std::vector<std::string> camposSubtotal = {
                {"Monto Exento"},
                {"Monto Percibido"},
                {"total_venta_g"},
                {"total_impuesto_g"},
                {"total_venta_r"},
                {"total_impuesto_r"},
                {"total_venta_a"},
                {"total_impuesto_a"},
                {"total_venta_p"}, 
                {"total_igtf"},
                {"Monto total del documento"},
                {"Monto Subtotal sin impuesto del documento"},
                {"Cantidad de ítems"},
                {"Cantidad de líneas impresas"},
                };

            int i = 0;
            for (auto it = rawCamposSubtotal.begin(); it != rawCamposSubtotal.end(); ++it, i++)
            {
                try
                {       
                    respuesta["factura"]["subtotales"][camposSubtotal[i]] = round2decimals(stof(std::string(it->c_str())) / 100.);
                        
                }
                catch (const std::exception &e)
                {
                    OATPP_LOGE("Analizar Factura", " %s: %s", it->c_str(), e.what());
                }
            }
        }
        // if(doc == 0){
        //     respuesta["Factura"]["nro"] = nroFactura;
        // }else if(doc == 1){
        //     respuesta["Nota_de_Credito"]["nro"] = nroFactura;
        // }else if(doc == 2){
        //     respuesta["Nota_de_Debito"]["nro"] = nroFactura;
        // }else{
        //     respuesta["Documento"]["nro"] = nroFactura;
        // }
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
    OATPP_LOGI("->ANALISIS DE FACTURA COMPLETADO","<-");
    return respuesta.dump();
}

std::string EcrsPrinter::analizarReporteXZ(std::string cmd, std::string rawReporteXZ)
{
    OATPP_LOGI("ANALIZANDO REPORTE XZ","...");
    nlohmann::json respuesta;

    respuesta["cmd"] = cmd;
    respuesta["rawResponse"] = rawReporteXZ;
    OATPP_LOGI("->","Analisis de reporte xz completado <-");
    return respuesta.dump();
}

