#include "PrinterService.hpp"

std::vector<SerialConnection *> PrinterService::printers = {};
std::mutex PrinterService::mutex;

std::string PrinterService::exeComandPrinter(oatpp::Object<PrinterDto> &dto)
{
    SerialConnection *sc = nullptr;
    validarDTO(dto);
    limpiarDTO(dto);

    // //********SAVED A LOG FILE***********
    // std::string directorioActual = __FILE__;
    // size_t found = directorioActual.find_last_of("/\\");
    // std::string rutaDirectorio = directorioActual.substr(0, found);
    // // Construir la ruta completa al archivo JSON en el directorio "resources"
    // std::string rutaCompleta = "resources\\LOGS.txt";
    // //VALIDACION ARCHIVO LOGS.txt
    // std::ifstream archivoEntrada(rutaCompleta.c_str());
    // if (!archivoEntrada) {
    //     // El archivo no existe, así que lo creamos
    //     std::ofstream nuevoArchivo(rutaCompleta.c_str());
    //     if (nuevoArchivo) {
    //         std::cout << "Se ha creado el archivo LOGS.txt" << std::endl;
    //         // Aquí puedes realizar alguna acción adicional, como escribir en el archivo
    //     } else {
    //         std::cerr << "Error al crear el archivo LOGS.txt" << std::endl;
    //     }
    // } else {
    //     std::cout << "El archivo LOGS.txt existe" << std::endl;
    // }
    // //END VALIDACION

    // OATPP_LOGI("RUTA", "FILE-TXT  %s", rutaCompleta.c_str());
    // std::string rutaBat = "resources\\LOGS.bat";
    
    // //START VALIDATION
    // // Validar si el archivo LOGS.bat existe
    // std::ifstream archivoEntrada2(rutaBat.c_str());
    // if (!archivoEntrada2) {
    //     // El archivo no existe, así que lo creamos
    //     std::ofstream nuevoArchivo2(rutaBat.c_str());
    //     if (nuevoArchivo2) {
    //         std::cout << "Se ha creado el archivo LOGS.bat" << std::endl;
    //         // Aquí puedes realizar alguna acción adicional, como escribir en el archivo
    //     } else {
    //         std::cerr << "Error al crear el archivo LOGS.bat" << std::endl;
    //     }
    // } else {
    //     std::cout << "El archivo LOGS.bat ya existe" << std::endl;
    // }
    // //END VALIDATION

    // std::string cmdBat = "start "+rutaBat;
    // FILE *archivo = std::fopen(rutaCompleta.c_str(), "a");
    // fprintf(archivo, "\n%s\n", "______________START LOG________________");
    // fprintf(archivo, "DTO: %s\n", Tools::dtoToString(dto).c_str());
    // //******************************

    const oatpp::Object<PrinterDto> dtoPtr = dto.getPtr();
    oatpp::DTO *data = dto.get();
    std::string result;

    sc = getDevice(dto);

    if (sc == nullptr || sc->device == nullptr)
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "NO SE ENCONTRO INFORMACIÓN DE LA IMPRESORA");

    try
    {
        sc->mutex.lock();
        
        sc->writeLog(Tools::dtoToString(dto), "RequestDTO", sc->device->brand);
        result = sc->ejecutarComando(data);
        sc->writeLog("ResponseDTO", result, sc->device->brand);
        OATPP_LOGV("RESPONSE", " %s", result.c_str())

        // //********SAVED A LOG FILE***********
        // fprintf(archivo, "RESPONSE: %s\n", result.c_str());
        // fprintf(archivo, "%s\n", "_______________END LOG_______________");
        // std::fclose(archivo);
        // //******************************
        // system(cmdBat.c_str());
        sc->closePort();
        sc->mutex.unlock();
    }
    catch (const std::exception &e)
    {
        OATPP_LOGE("PrinterService...", "%s", e.what());
        sc->closePort();
        sc->mutex.unlock();
        throw;
    }

    return result;
}
SerialConnection *PrinterService::getDevice(oatpp::Object<PrinterDto> &dto)
{
    SerialConnection *sc = nullptr;

    AppService *app = new AppService();
    app->addDevices();

    if (PrinterService::printers.size() == 0)
    {
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "NO SE DETECTARON DISPOSITIVOS");
    }
    else if (PrinterService::printers.size() == 1)
    {
        if (PrinterService::printers[0]->device->type.getValue("") == "impresora")
        {
            sc = PrinterService::printers[0];
        }
    }
    else
    {
        OATPP_ASSERT_HTTP(dto->serial, Status::CODE_500, "EXISTEN MULTIPLES DISPOSITIVOS, DEBE ESPECIFICAR EL SERIAL DE LA IMPRESORA");
        for (int i = 0; i < PrinterService::printers.size(); i++)
        {
            if (PrinterService::printers[i]->device->type.getValue("") == "impresora")
            {
                OATPP_LOGI("\n\tDispositivo", " \n\t%s: Marca: %s \n\tPuerto: %s \n\tSerial: %s", PrinterService::printers[i]->device->type.getValue("").c_str(), PrinterService::printers[i]->device->brand.getValue("").c_str(), PrinterService::printers[i]->device->port.getValue("").c_str(), PrinterService::printers[i]->device->serial.getValue("").c_str());
                if (PrinterService::printers[i]->device->serial.getValue("") == dto->serial.getValue("") && !dto->serial.getValue("").empty())
                {
                    sc = PrinterService::printers[i];
                    break;
                }
            }
        }
    }
    return sc;
}

SerialConnection *PrinterService::selectDevice(oatpp::Object<PrinterDto> &dto)
{
    SerialConnection *sc = nullptr;

    if (PrinterService::printers.size() == 0)
    {
        AppService *app = new AppService();
        app->usingSerialPort--;
        app->addDevices();
        app->usingSerialPort++;
    }

    if (PrinterService::printers.size() == 0)
    {
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "NO SE DETECTARON DISPOSITIVOS");
    }
    else if (PrinterService::printers.size() == 1)
    {
        if (PrinterService::printers[0]->device->type.getValue("") == "impresora")
        {
            OATPP_LOGI("\n\tDispositivo", " \n\t%s: Marca: %s \n\tPuerto: %s \n\tSerial: %s", PrinterService::printers[0]->device->type.getValue("").c_str(), PrinterService::printers[0]->device->brand.getValue("").c_str(), PrinterService::printers[0]->device->port.getValue("").c_str(), PrinterService::printers[0]->device->serial.getValue("").c_str());
            sc = PrinterService::printers[0];
        }
    }
    else
    {
        OATPP_ASSERT_HTTP(dto->serial, Status::CODE_500, "EXISTEN MULTIPLES DISPOSITIVOS, DEBE ESPECIFICAR EL SERIAL DE LA IMPRESORA");
        for (int i = 0; i < PrinterService::printers.size(); i++)
        {
            if (PrinterService::printers[i]->device->type.getValue("") == "impresora")
            {
                OATPP_LOGI("\n\tDispositivo", " \n\t%s: Marca: %s \n\tPuerto: %s \n\tSerial: %s", PrinterService::printers[i]->device->type.getValue("").c_str(), PrinterService::printers[i]->device->brand.getValue("").c_str(), PrinterService::printers[i]->device->port.getValue("").c_str(), PrinterService::printers[i]->device->serial.getValue("").c_str());
                if (PrinterService::printers[i]->device->serial.getValue("") == dto->serial.getValue("") && !dto->serial.getValue("").empty())
                {
                    sc = PrinterService::printers[i];
                    break;
                }
            }
        }
    }
    return sc;
}

std::float_t  PrinterService::monto_valido(oatpp::Object<PrinterDto> &dat){
    OATPP_LOGI("VALIDACION DE MONTO", "[INICIADA]");
    auto items = dat->factura->items.get();
    auto pagos = dat->factura->pagos.get();
    float totalPrices = 0.0;
    float totalAmount = 0.0;
    float descuento = dat->factura->descuento->descuento.getValue(0.0);
    float amountRelationPrice = 0.0;
    bool breakAmount = false;

    for (auto i = items->begin(); i != items->end(); i++)
    {
        totalPrices += i->get()->precio.getValue(0.0) * i->get()->cantidad.getValue(0.0);
    }
    if(descuento!=0.0) totalPrices -= descuento;
    OATPP_LOGI("TOTAL COSTO DE 'ITEMS' ==>", "%f", totalPrices);
    for (auto i = pagos->begin(); i != pagos->end(); i++)
    {
        // auto conIGTF = 0.00;
        // if(i->get()->impuesto != nullptr && i->get()->impuesto->igtf.getValue(false) != false){
        //     conIGTF = i->get()->monto.getValue(0.0) * 0.03;
        //     OATPP_LOGI("ValMonto => contiene IGTF ","%f", conIGTF);
        // } 
        totalAmount += i->get()->monto.getValue(0.0);
        if(i->get()->monto.getValue(0.0) >= totalPrices && breakAmount == false){
            amountRelationPrice = i->get()->monto.getValue(0.0);
            breakAmount = true;
        }
    }
        OATPP_LOGI("TOTAL MONTO DE 'PAGOS' ==>", "%f", totalAmount);
        OATPP_LOGI("Monto sobre Precio (si es '0.0' no existe)==>", "%f", amountRelationPrice);
    if(totalPrices > totalAmount){
        OATPP_ASSERT_HTTP(false, Status::CODE_501, "ERROR: El Monto Total es inferior al Costo Total de productos");
    }
    OATPP_LOGI("VALIDACION DE MONTO", "[TERMINADA]");
    return amountRelationPrice;
}

void PrinterService::validarDTO(oatpp::Object<PrinterDto> &dto)
{
    OATPP_LOGI("Validar DTO", " v1.0")
    OATPP_ASSERT_HTTP(dto, Status::CODE_500, "DTO ES NULL");
    OATPP_ASSERT_HTTP(!dto->cmd.getValue("").empty(), Status::CODE_500, "COMANDO INVALIDO");

    std::string cmd = dto->cmd.getValue("");

    if ("facturar" == cmd || "notaCredito" == cmd || "notaDebito" == cmd)
    {
        OATPP_ASSERT_HTTP(dto->factura, Status::CODE_500, "Factura es nulo");
        OATPP_ASSERT_HTTP(dto->factura->cliente, Status::CODE_500, "Factura.Cliente es nulo");
        OATPP_ASSERT_HTTP(dto->factura->cliente->nombre, Status::CODE_500, "Factura.Cliente.nombre es nulo");
        OATPP_ASSERT_HTTP(dto->factura->cliente->rif, Status::CODE_500, "Factura.Cliente.rif es nulo");
        OATPP_ASSERT_HTTP(dto->factura->items, Status::CODE_500, "Factura.Items es nulo");
        OATPP_ASSERT_HTTP(dto->factura->cabecera, Status::CODE_500, "Factura.cabecera es nulo");
        OATPP_ASSERT_HTTP(dto->factura->pie, Status::CODE_500, "Factura.pie es nulo");

        //START VALIDAR MONTO
        monto_valido(dto);
        //END VALIDAR MONTO

        double total = 0;
        for (auto i = dto->factura->items->begin(); i != dto->factura->items->end(); i++)
        {
            OATPP_ASSERT_HTTP(i->get(), Status::CODE_500, "Factura.Items[n] es nulo");

            OATPP_ASSERT_HTTP(i->get()->codigo, Status::CODE_500, "Factura.Items[n].codigo es nulo");
            OATPP_ASSERT_HTTP(i->get()->producto, Status::CODE_500, "Factura.Items[n].producto es nulo");
            OATPP_ASSERT_HTTP(i->get()->cantidad, Status::CODE_500, "Factura.Items[n].cantidad es nulo");
            OATPP_ASSERT_HTTP(i->get()->precio, Status::CODE_500, "Factura.Items[n].precio es nulo");
            OATPP_ASSERT_HTTP(i->get()->tasa, Status::CODE_500, "Factura.Items[n].tasa es nulo");
            OATPP_ASSERT_HTTP(i->get()->descuento, Status::CODE_500, "Factura.Items[n].descuento es nulo");
            OATPP_ASSERT_HTTP(i->get()->tipo_descuento, Status::CODE_500, "Factura.Items[n].tipo_descuento es nulo");
            OATPP_ASSERT_HTTP(i->get()->texto, Status::CODE_500, "Factura.Items[n].texto es nulo");

            OATPP_ASSERT_HTTP(!i->get()->producto.getValue("").empty(), Status::CODE_500, "Factura.Items[n].producto esta vacio");
            OATPP_ASSERT_HTTP(i->get()->cantidad.getValue(0) != 0, Status::CODE_500, "Factura.Items[n].cantidad es 0");
           // OATPP_ASSERT_HTTP(i->get()->precio != 0, Status::CODE_500, "Factura.Items[n].precio es 0");
            OATPP_ASSERT_HTTP(!i->get()->tasa.getValue("").empty(), Status::CODE_500, "Factura.Items[n].tasa esta vacio");
            total += i->get()->precio * i->get()->cantidad;

            auto tasa = i->get()->tasa.getValue("");
            OATPP_ASSERT_HTTP(tasa == "E" || tasa == "G" || tasa == "R" || tasa == "A" || tasa == "P", Status::CODE_500, "Factura.Items.tasa invalido");

            if (i->get()->descuento.getValue(0) > 0)
            {
                OATPP_ASSERT_HTTP(!i->get()->tipo_descuento.getValue("").empty(), Status::CODE_500, "Factura.Items.tipo_descuento esta vacio");
                OATPP_ASSERT_HTTP(i->get()->tipo_descuento.getValue("") == "monto" || i->get()->tipo_descuento.getValue("") == "porcentaje", Status::CODE_500, "Factura.Items.tipo_descuento invalido");
                if (i->get()->tipo_descuento.getValue("") == "porcentaje")
                {
                    OATPP_ASSERT_HTTP(i->get()->descuento.getValue(0) < 100, Status::CODE_500, "Factura.Items.descuento, no puede ser mayor al 100%");
                }
                else
                {
                    OATPP_ASSERT_HTTP(i->get()->descuento.getValue(0) < (i->get()->precio * i->get()->cantidad), Status::CODE_500, "Factura.Items.descuento, no puede ser mayor al total");
                }
            }
        }

        OATPP_ASSERT_HTTP(dto->factura->descuento, Status::CODE_500, "Factura.descuento es nulo");
        if (dto->factura->descuento->descuento.getValue(0) > 0)
        {
            OATPP_ASSERT_HTTP(dto->factura->descuento->descuento, Status::CODE_500, "Factura.descuento.descuento es nulo");
            OATPP_ASSERT_HTTP(dto->factura->descuento->tipo_descuento, Status::CODE_500, "Factura.descuento.tipo_descuento es nulo");

            OATPP_ASSERT_HTTP(!dto->factura->descuento->tipo_descuento.getValue("").empty(), Status::CODE_500, "Factura.descuento.tipo_descuento esta vacio");
            OATPP_ASSERT_HTTP(dto->factura->descuento->tipo_descuento.getValue("") == "monto" || dto->factura->descuento->tipo_descuento.getValue("") == "porcentaje", Status::CODE_500, "Factura.descuento.tipo_descuento invalido");
            if (dto->factura->descuento->tipo_descuento.getValue("") == "porcentaje")
            {
                OATPP_ASSERT_HTTP(dto->factura->descuento->descuento.getValue(0) < 100, Status::CODE_500, "Factura.descuento.descuento, no puede ser mayor al 100%");
            }
            else
            {
                OATPP_ASSERT_HTTP(dto->factura->descuento->descuento.getValue(0) < total, Status::CODE_500, "Factura.descuento.descuento, no puede ser mayor al total");
            }
        }

        OATPP_ASSERT_HTTP(dto->factura->pagos, Status::CODE_500, "Factura.Pagos es nulo");
        for (auto i = dto->factura->pagos->begin(); i != dto->factura->pagos->end(); i++)
        {
            OATPP_ASSERT_HTTP(i->get(), Status::CODE_500, "Factura.Pagos[n] es nulo");

            OATPP_ASSERT_HTTP(i->get()->monto, Status::CODE_500, "Factura.Pagos[n].monto es nulo");
            OATPP_ASSERT_HTTP(i->get()->metodo, Status::CODE_500, "Factura.Pagos[n].metodo es nulo");

            OATPP_ASSERT_HTTP(i->get()->monto.getValue(0) != 0, Status::CODE_500, "Factura.Pagos[n].monto es 0");
            OATPP_ASSERT_HTTP(!i->get()->metodo.getValue("").empty(), Status::CODE_500, "Factura.Pagos[n].metodo esta vacio");
            if (i->get()->impuesto)
            {
                OATPP_ASSERT_HTTP(i->get()->impuesto->igtf.get() != nullptr, Status::CODE_500, "Factura.Pagos[n].impuesto.igtf es nulo");
            }
        }

        if ("notaCredito" == cmd || "notaDebito" == cmd)
        {
            OATPP_ASSERT_HTTP(dto->factura->notaFiscal, Status::CODE_500, "Factura.NotaFiscal es nulo");
            OATPP_ASSERT_HTTP(dto->factura->notaFiscal->factura, Status::CODE_500, "Factura.NotaFiscal.Factura es nulo");
            OATPP_ASSERT_HTTP(dto->factura->notaFiscal->serialFiscal, Status::CODE_500, "Factura.NotaFiscal.SerialFiscal es nulo");
            OATPP_ASSERT_HTTP(dto->factura->notaFiscal->fecha, Status::CODE_500, "Factura.fecha es nulo");
            OATPP_ASSERT_HTTP(dto->factura->notaFiscal->hora, Status::CODE_500, "Factura.hora es nulo");

            OATPP_ASSERT_HTTP(dto->factura->notaFiscal->factura.getValue(0) > 0, Status::CODE_500, "Factura.NotaFiscal.Factura número invalido");
            OATPP_ASSERT_HTTP(dto->factura->notaFiscal->serialFiscal.getValue("").size() >= 10, Status::CODE_500, "Factura.NotaFiscal.SerialFiscal invalido");
            OATPP_ASSERT_HTTP(!dto->factura->notaFiscal->fecha.getValue("").empty(), Status::CODE_500, "Factura.fecha esta vacio");

            auto fecha = dto->factura->notaFiscal->fecha.getValue("");
            OATPP_ASSERT_HTTP(fecha.size() == 10, Status::CODE_500, "Factura.fecha formato invalido, se esperaba: YYYY/MM/DD");
            OATPP_ASSERT_HTTP(fecha[4] == '/' || fecha[4] == '\\' || fecha[4] == '-', Status::CODE_500, "Factura.fecha formato invalido, se esperaba: YYYY/MM/DD o YYYY-MM-DD");
            OATPP_ASSERT_HTTP(fecha[7] == '/' || fecha[7] == '\\' || fecha[7] == '-', Status::CODE_500, "Factura.fecha formato invalido, se esperaba: YYYY/MM/DD o YYYY-MM-DD");

            OATPP_ASSERT_HTTP(validateNumb(fecha.substr(8, 2)), Status::CODE_500, "Factura.fecha formato invalido, Dia solo puede contener números");
            OATPP_ASSERT_HTTP(validateNumb(fecha.substr(5, 2)), Status::CODE_500, "Factura.fecha formato invalido, Mes solo puede contener números");
            OATPP_ASSERT_HTTP(validateNumb(fecha.substr(0, 4)), Status::CODE_500, "Factura.fecha formato invalido, Año solo puede contener números");

            int dia, mes, anio;
            try
            {
                dia = stoi(fecha.substr(8, 2));
                mes = stoi(fecha.substr(5, 2));
                anio = stoi(fecha.substr(0, 4));
            }
            catch (...)
            {
                OATPP_ASSERT_HTTP(false, Status::CODE_500, "Factura.fecha formato invalido, se esperaba: 4YYYY/MM/DD");
            }

            OATPP_ASSERT_HTTP(dia > 0 && dia <= 31, Status::CODE_500, "Factura.fecha, formato invalido, dia fuera de rango");
            OATPP_ASSERT_HTTP(mes > 0 && mes <= 12, Status::CODE_500, "Factura.fecha formato invalido, mes fuera de rango");
            OATPP_ASSERT_HTTP(anio >= 1000, Status::CODE_500, "Factura.fecha formato invalido, año fuera de rango");
        }
    }
    else if ("textoNF" == cmd)
    {
        OATPP_ASSERT_HTTP(dto->textoNF, Status::CODE_500, "textoNF es nulo");
        OATPP_ASSERT_HTTP(dto->textoNF->size() != 0, Status::CODE_500, "textoNF vacio");

        for (auto i = dto->textoNF->begin(); i != dto->textoNF->end(); i++)
        {
            OATPP_ASSERT_HTTP(i->get(), Status::CODE_500, "textoNF[n] es nulo");
        }
    }
    else if ("textoNFF" == cmd)
    {
        OATPP_ASSERT_HTTP(dto->textoNFF, Status::CODE_500, "textoNFF es nulo");
        OATPP_ASSERT_HTTP(dto->textoNFF->size() != 0, Status::CODE_500, "textoNFF vacio");

        for (auto i = dto->textoNFF->begin(); i != dto->textoNFF->end(); i++)
        {
            OATPP_ASSERT_HTTP(i->get(), Status::CODE_500, "textoNFF[n] es nulo");
            OATPP_ASSERT_HTTP(i->get()->text, Status::CODE_500, "textoNFF[n].text es nulo");
        }
    }
    else if ("reimprimir" == cmd)
    {
        OATPP_ASSERT_HTTP(dto->reimprimir, Status::CODE_500, "reimprimir es nulo");

        OATPP_ASSERT_HTTP(dto->reimprimir->doc, Status::CODE_500, "reimprimir.doc es nulo");
        OATPP_ASSERT_HTTP(dto->reimprimir->numb, Status::CODE_500, "reimprimir.num es nulo");

        OATPP_ASSERT_HTTP(!dto->reimprimir->doc.getValue("").empty(), Status::CODE_500, "reimprimir.doc esta vacio");
        OATPP_ASSERT_HTTP(dto->reimprimir->numb.getValue(0), Status::CODE_500, "reimprimir.num es 0");
    }
}

void PrinterService::limpiarDTO(oatpp::Object<PrinterDto> &dto)
{
    OATPP_LOGI("Limpiar DTO", " v1.0")

    std::string cmd = dto->cmd.getValue("");

    if ("facturar" == cmd || "notaCredito" == cmd || "notaDebito" == cmd)
    {
        dto->factura->cliente->nombre = Tools::cleanString(dto->factura->cliente->nombre);

        for (auto i = dto->factura->items->begin(); i != dto->factura->items->end(); i++)
        {
            i->get()->codigo = Tools::cleanString(i->get()->codigo);
            i->get()->producto = Tools::cleanString(i->get()->producto);
            i->get()->texto = Tools::cleanString(i->get()->texto);
        }
        for (auto i = 0; i != dto->factura->cabecera->size(); i++)
        {
            dto->factura->cabecera[i] = Tools::cleanString(dto->factura->cabecera[i]);
        }

        for (auto i = 0; i != dto->factura->pie->size(); i++)
        {
            dto->factura->pie[i] = Tools::cleanString(dto->factura->pie[i]);
        }

        for (auto i = dto->factura->pagos->begin(); i != dto->factura->pagos->end(); i++)
        {
            i->get()->metodo = Tools::cleanString(i->get()->metodo);
        }
    }

    else if ("textoNF" == cmd)
    {
        for (auto i = 0; i < dto->textoNF->size(); i++)
        {
            dto->textoNF[i] = Tools::cleanString(dto->textoNF[i]);
        }
    }
    else if ("textoNFF" == cmd)
    {
        for (auto i = 0; i < dto->textoNFF->size(); i++)
        {
            dto->textoNFF[i]->text = Tools::cleanString(dto->textoNFF[i]->text);
        }
    }
}

bool PrinterService::validateNumb(std::string numb)
{
    bool result = true;
    std::string aux = numb;

    for (int i = 0; i < aux.size(); i++)
    {
        if (!(aux[i] >= '0' && aux[i] <= '9'))
        {
            result = false;
            break;
        }
    }
    return result;
}
