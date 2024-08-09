#include "AppService.hpp"

std::atomic<int> AppService::usingSerialPort{0};
std::atomic<int> AppService::configuringPort{0};
std::list<std::string> AppService::portsUsed;

/**
 * Check Devices
 */
oatpp::Object<PageDto<oatpp::String>> AppService::getAllPortsName()
{
#ifdef _WIN32
    const int portNro = 33;
    const char *comports[portNro] = {"COM1", "COM2", "COM3", "COM4",
                                     "COM5", "COM6", "COM7", "COM8",
                                     "COM9", "COM10", "COM11", "COM12",
                                     "COM13", "COM14", "COM15", "COM16",
                                     "COM17", "COM18", "COM19", "COM20",
                                     "COM21", "COM22", "COM23", "COM24",
                                     "COM25", "COM26", "COM27", "COM28",
                                     "COM29", "COM30", "COM31", "COM32", "3010"};
#else
    const int portNro = 28;
    const char *comports[portNro] = {
        "ttyS0", "ttyS1", "ttyS2", "ttyS3", "ttyS4", "ttyS5",
        "ttyUSB0", "ttyUSB1", "ttyUSB2", "ttyUSB3", "ttyUSB4", "ttyUSB5",
        "ttyACM0", "ttyACM1", "ttyACM2", "ttyACM3", "ttyACM4", "ttyACM5",
        "ttyS6", "ttyS7", "ttyS8", "ttyS9", "ttyS10", "ttyS11", "ttyS12", "ttyS13", "ttyS14", "ttyS15",
        // "ttyAMA0", "ttyAMA1", "rfcomm0", "rfcomm1", "ircomm0", "ircomm1",
        // "cuau0", "cuau1", "cuau2", "cuau3", "cuaU0", "cuaU1", "cuaU2", "cuaU3"
    };
#endif

    auto items = oatpp::Vector<oatpp::String>::createShared();
    for (int i = 0; i < portNro; i++)
    {
        oatpp::String text;
        text = std::string(comports[i]);
        items->push_back(text);
    }

    auto page = PageDto<oatpp::String>::createShared();
    page->offset = unsigned(0);
    page->limit = portNro;
    page->count = portNro;
    page->items = items;

    return page;
}

/**
 * Imprimir DTO en consola
 */
std::string AppService::DtoToString(const oatpp::data::mapping::type::Void &variant)
{
    auto serializeConfig = oatpp::parser::json::mapping::Serializer::Config::createShared();
    auto deserializeConfig = oatpp::parser::json::mapping::Deserializer::Config::createShared();

    /* enable beautifier */
    serializeConfig->useBeautifier = true;

    /* create json object mapper with serializer config */
    auto jsonObjectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared(serializeConfig, deserializeConfig);
    oatpp::String json = jsonObjectMapper->writeToString(variant);
    OATPP_LOGD("", "Request Obj: \n%s", json.getValue("").c_str());
    return json.getValue("");
}

std::string AppService::getDevicesInfo()
{
    nlohmann::json json;
    nlohmann::json impresoras = nlohmann::json::array();
    nlohmann::json balanzas = nlohmann::json::array();
    auto dev = getAllDevices()->items;
    for (int i = 0; i != dev->size(); i++)
    {
        auto obj = nlohmann::json::parse(AppService::DtoToString(dev[i]));
        impresoras.push_back(obj);
    }
    // for (int i = 0; i != PrinterService::printers.size(); i++)
    // {
    //     auto device = Device::createShared();
    //     device->brand = PrinterService::printers[i]->device->brand.getValue("");
    //     device->port = PrinterService::printers[i]->device->port.getValue("");
    //     device->model = PrinterService::printers[i]->device->model.getValue("");
    //     device->serial = PrinterService::printers[i]->device->serial.getValue("");
    //     device->type = PrinterService::printers[i]->device->type.getValue("");

    //     auto obj = nlohmann::json::parse(AppService::DtoToString(device));
    //     impresoras.push_back(obj);
    // }

    for (int i = 0; i != BasculeService::bascules.size(); i++)
    {
        auto device = Device::createShared();
        device->brand = BasculeService::bascules[i]->device->brand.getValue("");
        device->port = BasculeService::bascules[i]->device->port.getValue("");
        device->model = BasculeService::bascules[i]->device->model.getValue("");
        device->serial = BasculeService::bascules[i]->device->serial.getValue("");
        device->type = BasculeService::bascules[i]->device->type.getValue("");

        auto obj = nlohmann::json::parse(AppService::DtoToString(device));
        balanzas.push_back(obj);
    }

    json["impresoras"] = impresoras;
    json["balanzas"] = balanzas;

    return json.dump();
}

void AppService::closeAllPorts()
{
    for (int i = 0; i != PrinterService::printers.size(); i++)
    {
        PrinterService::printers[i]->closePort();
        portsUsed.remove(PrinterService::printers[i]->device->port.getValue("").c_str());
    }

    for (int i = 0; i != BasculeService::bascules.size(); i++)
    {
        BasculeService::bascules[i]->closePort();
        portsUsed.remove(BasculeService::bascules[i]->device->port.getValue("").c_str());
    }
    // TODO: LLAMAR METODO DESTRUCTOR DE OBJETOS...
    PrinterService::printers.clear();
    BasculeService::bascules.clear();
}

oatpp::Vector<oatpp::Object<DeviceStatus>> AppService::checkDevices()
{
    waitConfiguration();
    auto result = oatpp::Vector<oatpp::Object<DeviceStatus>>::createShared();
    auto devices = this->getAllDevices();

    OATPP_LOGI("checkDevices", "Tarea(s) pendiente(s): %d", AppService::usingSerialPort.load());
    while (AppService::usingSerialPort != 0)
    {
        AppService::sleep(10000);
        OATPP_LOGI("checkDevices", "ESPERANDO 1000 ms, mientras se liberan los dispositivos... Tarea(s) pendiente(s): %d", AppService::usingSerialPort.load());
    }
    AppService::configuringPort++;
    closeAllPorts();

    if (devices->items->size() > 0)
    {
        SerialConnection *sc;
        auto ports = AppService::getAllPortsName();

        for (auto dbDevice = devices->items->begin(); dbDevice != devices->items->end(); dbDevice++)
        {
            OATPP_LOGD("", "------------------------- AppService:::checkDevices -------------------------");
            OATPP_LOGD("ESCANEANDO PUERTOS PARA:", " %s - %s : %s", dbDevice->get()->brand->c_str(), dbDevice->get()->serial->c_str(), dbDevice->get()->brand->c_str());
            for (auto port = ports->items->begin(); port != ports->items->end(); port++)
            {
                std::list<std::string>::iterator findPort = std::find(AppService::portsUsed.begin(), AppService::portsUsed.end(), port->get()->c_str());
                if (findPort != AppService::portsUsed.end())
                    continue;

                try
                {
                    // OATPP_LOGD("PORT", " %s", port->get()->c_str());
                    if (dbDevice->get()->type == "impresora")
                    {
                        if (dbDevice->get()->brand == "HKA")
                            sc = new HkaPrinter(port->get()->c_str());
                        else if (dbDevice->get()->brand == "NETSOFT")
                            sc = new NetsoftPrinter(port->get()->c_str());
                        else if (dbDevice->get()->brand == "PNP")
                            sc = new PnpPrinter(port->get()->c_str());
                        else if (dbDevice->get()->brand == "VMAX")
                            sc = new VmaxPrinter(port->get()->c_str());
                        else if (dbDevice->get()->brand == "SEWOO")
                            sc = new SewooPrinter(port->get()->c_str());
                        else if (dbDevice->get()->brand == "ECRS")
                            sc = new BematechPrinter(port->get()->c_str());
                        else if (dbDevice->get()->brand == "BEMATECH")
                            sc = new BematechPrinter(port->get()->c_str());
                        else
                        {
                            OATPP_LOGE("IMPRESORA : MARCA : DESCONOCIDA", ": HKA | NETSOFT | PNP | VMAX | ECRS | BEMATECH");
                        }
                        if (sc != nullptr)
                        {
                            if (dbDevice->get()->port.getValue("") == "3010")
                            {
                                OATPP_LOGD("SERIAL COINCIDE AGREGANDO", " Impresora %s %s : %s dispositivo: %d", dbDevice->get()->brand.getValue("").c_str(), dbDevice->get()->model.getValue("").c_str(), dbDevice->get()->serial.getValue("").c_str(), dbDevice->get()->dispositivo.getValue(0));
                                sc->device = new Device();
                                sc->device->brand = dbDevice->get()->brand.getValue("");
                                sc->device->model = dbDevice->get()->model.getValue("");
                                sc->device->serial = dbDevice->get()->serial.getValue("");
                                sc->device->ip = dbDevice->get()->ip.getValue("");
                                sc->device->port = dbDevice->get()->port.getValue("");
                                sc->device->type = dbDevice->get()->type.getValue("");
                                sc->device->dispositivo = dbDevice->get()->dispositivo.getValue(0);
                                sc->refrescarConfiguracion();
                                PrinterService::printers.push_back(sc);
                                portsUsed.push_back(port->get()->c_str());
                                break;
                            }
                            else if (sc->OpenPort())
                            {
                                OATPP_LOGI("PORT", " %s OPEN", port->get()->c_str());

                                auto serial = sc->leerSerial();
                                OATPP_LOGI("Serial Fiscal Impresora", "%s", serial.c_str());
                                if (serial == dbDevice->get()->serial.getValue(""))
                                {
                                    OATPP_LOGD("SERIAL COINCIDE AGREGANDO ", " Impresora %s %s : %s", dbDevice->get()->brand.getValue("").c_str(), dbDevice->get()->serial.getValue("").c_str(), dbDevice->get()->model.getValue("").c_str());
                                    sc->device = new Device();
                                    sc->device->brand = dbDevice->get()->brand;
                                    sc->device->model = dbDevice->get()->model;
                                    sc->device->serial = dbDevice->get()->serial;
                                    sc->device->ip = dbDevice->get()->ip->c_str();
                                    sc->device->port = port->get()->c_str();
                                    sc->device->type = dbDevice->get()->type;
                                    sc->refrescarConfiguracion();

                                    PrinterService::printers.push_back(sc);
                                    portsUsed.push_back(port->get()->c_str());
                                    break;
                                }
                                else
                                {
                                    OATPP_LOGE("ERROR SERIAL NO COINCIDE ", " BD: %s - DEVICE: %s", dbDevice->get()->serial.getValue("").c_str(), serial.c_str());
                                    sc->closePort();
                                }
                            }
                            else
                            {
                                sc->closePort();
                            }
                        }
                        else
                        {
                            OATPP_LOGE("IMPRESORA :: MARCA :: NO COMPATIBLE.", "");
                        }
                    }
                    else if (dbDevice->get()->type == "balanza")
                    {
                        auto balanza = new HkaBascule(port->get()->c_str());
                        if (sc->OpenPort())
                        {
                            OATPP_LOGI("PORT", " %s OPEN", port->get()->c_str());
                            auto peso = balanza->getPeso();
                            if (!peso.empty())
                            {
                                OATPP_LOGD("AGREGANDO ", " Balanza %s %s", dbDevice->get()->brand.getValue("").c_str(), dbDevice->get()->serial.getValue("").c_str());
                                sc->device = new Device();
                                sc->device->brand = dbDevice->get()->brand;
                                sc->device->model = dbDevice->get()->model;
                                sc->device->serial = dbDevice->get()->serial;
                                sc->device->port = port->get()->c_str();
                                sc->device->type = dbDevice->get()->type;
                                sc->refrescarConfiguracion();

                                PrinterService::printers.push_back(sc);
                                portsUsed.push_back(port->get()->c_str());
                                break;
                            }
                            else
                            {
                                OATPP_LOGE("ERROR DISPOSITIVO NO RESPONDE", " BD: %s", dbDevice->get()->serial.getValue("").c_str());
                                sc->closePort();
                            }
                        }
                    }
                    else
                    {
                        OATPP_LOGE("TIPO DE DISPOSITIVO INVALIDO", "");
                    }
                }
                catch (const std::exception &e)
                {
                    sc->closePort();
                    OATPP_LOGE("ERROR ESCANEANDO PUERTO SERIAL", "%s", e.what());
                }
            }
        }
    }
    AppService::configuringPort--;

    return result;
}

void AppService::addDevices()
{
    // while (AppService::usingSerialPort > 1)
    // {
    //     AppService::sleep(1 * 1000);
    //     OATPP_LOGI("addDevices", "ESPERANDO 1000 ms, mientras se liberan los dispositivos... Tarea(s) pendiente(s): %d", AppService::usingSerialPort.load());
    // }
    auto result = oatpp::Vector<oatpp::Object<DeviceStatus>>::createShared();
    auto devices = this->getAllDevices();

    PrinterService::printers.clear();
    BasculeService::bascules.clear();

    for (auto dbDevice = devices->items->begin(); dbDevice != devices->items->end(); dbDevice++)
    {
        SerialConnection *sc;
        OATPP_LOGD("", "------------------------- AppService:::AddDevices -------------------------");
        OATPP_LOGD("\nDISPOSITIVO:", "TIPO: %s \n MARCA: %s \n MODELO: %s \n SERIAL: %s \n PUERTO: %s",
                   dbDevice->get()->type.getValue("").c_str(),
                   dbDevice->get()->brand.getValue("").c_str(),
                   dbDevice->get()->model.getValue("").c_str(),
                   dbDevice->get()->serial.getValue("").c_str(),
                   dbDevice->get()->port.getValue("").c_str());

        try
        {
            if (dbDevice->get()->type == "impresora")
            {
                // sc=Serial connection
                if (dbDevice->get()->brand == "HKA" && dbDevice->get()->dispositivo.getValue(0) == 0)
                    sc = new HkaPrinter(dbDevice->get()->port.getValue(""));
                else if (dbDevice->get()->brand == "HKA" && dbDevice->get()->dispositivo.getValue(0) == 1)
                    sc = new HkaPrinter(dbDevice->get()->port.getValue(""), "8E1");
                else if (dbDevice->get()->brand == "NETSOFT")
                    sc = new NetsoftPrinter(dbDevice->get()->port.getValue(""));
                else if (dbDevice->get()->brand == "PNP")
                    sc = new PnpPrinter(dbDevice->get()->port.getValue(""));
                else if (dbDevice->get()->brand == "VMAX")
                    sc = new VmaxPrinter(dbDevice->get()->port.getValue(""));
                else if (dbDevice->get()->brand == "SEWOO")
                    sc = new SewooPrinter(dbDevice->get()->port.getValue(""));
                else if (dbDevice->get()->brand == "ECRS")
                    sc = new EcrsPrinter(dbDevice->get()->port.getValue(""));
                else if (dbDevice->get()->brand == "BEMATECH")
                    sc = new BematechPrinter(dbDevice->get()->port.getValue(""));
                else
                {
                    OATPP_LOGE("IMPRESORA : MARCA : DESCONOCIDA", ": HKA | NETSOFT | PNP | VMAX | ECRS | BEMATECH");
                }
                OATPP_LOGI("PASO EL SC ", "");
                if (sc != nullptr)
                {
                    OATPP_LOGI("PASO EL SC ", "distinto de null");
                    OATPP_LOGI("SC ", "abrir puerto");
                    if (dbDevice->get()->port.getValue("") == "3010")
                    {
                        OATPP_LOGD("SERIAL COINCIDE AGREGANDO", " Impresora %s %s : %s dispositivo: %d", dbDevice->get()->brand.getValue("").c_str(), dbDevice->get()->model.getValue("").c_str(), dbDevice->get()->serial.getValue("").c_str(), dbDevice->get()->dispositivo.getValue(0));
                        sc->device = new Device();
                        sc->device->brand = dbDevice->get()->brand.getValue("");
                        sc->device->model = dbDevice->get()->model.getValue("");
                        sc->device->serial = dbDevice->get()->serial.getValue("");
                        sc->device->ip = dbDevice->get()->ip.getValue("");
                        sc->device->port = dbDevice->get()->port.getValue("");
                        sc->device->type = dbDevice->get()->type.getValue("");
                        sc->device->dispositivo = dbDevice->get()->dispositivo.getValue(0);
                        sc->refrescarConfiguracion();
                        PrinterService::printers.push_back(sc);
                    }
                    else if (sc->OpenPort())
                    {
                        OATPP_LOGI("SC ", "puerto abierto");
                        auto serial = sc->leerSerial();
                        OATPP_LOGD("SERIAL FILE: ", "%s", dbDevice->get()->serial.getValue("").c_str());
                        OATPP_LOGD("SERIAL PRINTER: ", "%s", serial.c_str());

                        OATPP_LOGD("LEN SERIAL FILE: ", "%d", dbDevice->get()->serial.getValue("").length());
                        OATPP_LOGD("LEN SERIAL PRINTER: ", "%d", serial.length());

                        if (serial == dbDevice->get()->serial.getValue(""))
                        {
                            OATPP_LOGD("SERIAL COINCIDE AGREGANDO", " Impresora %s %s : %s dispositivo: %d", dbDevice->get()->brand.getValue("").c_str(), dbDevice->get()->model.getValue("").c_str(), dbDevice->get()->serial.getValue("").c_str(), dbDevice->get()->dispositivo.getValue(0));
                            sc->device = new Device();
                            sc->device->brand = dbDevice->get()->brand.getValue("");
                            sc->device->model = dbDevice->get()->model.getValue("");
                            sc->device->serial = dbDevice->get()->serial.getValue("");
                            sc->device->ip = dbDevice->get()->ip.getValue("");
                            sc->device->port = dbDevice->get()->port.getValue("");
                            sc->device->type = dbDevice->get()->type.getValue("");
                            sc->device->dispositivo = dbDevice->get()->dispositivo.getValue(0);
                            sc->refrescarConfiguracion();
                            PrinterService::printers.push_back(sc);
                        }
                        else
                        {
                            OATPP_LOGE("ERROR SERIAL NO COINCIDE ", " BD: %s - DEVICE: %s", dbDevice->get()->serial.getValue("").c_str(), serial.c_str());
                            sc->closePort();
                        }
                        sc->leerRIF();
                    }
                    else
                    {
                        sc->closePort();
                    }
                }
                else
                {
                    OATPP_LOGE("IMPRESORA :: MARCA :: NO COMPATIBLE.", "");
                }
                auto serial = sc->leerSerial();
                OATPP_LOGI("Serial Fiscal Impresora", "%s", serial.c_str());
            }
            else if (dbDevice->get()->type == "balanza")
            {
                auto balanza = new HkaBascule(dbDevice->get()->port.getValue(""));
                if (sc->OpenPort())
                {
                    auto peso = balanza->getPeso();
                    if (!peso.empty())
                    {
                        OATPP_LOGD("AGREGANDO ", " Balanza %s %s", dbDevice->get()->brand.getValue("").c_str(), dbDevice->get()->serial.getValue("").c_str());
                        sc->device = new Device();
                        sc->device->brand = dbDevice->get()->brand;
                        sc->device->model = dbDevice->get()->model;
                        sc->device->serial = dbDevice->get()->serial;
                        sc->device->port = dbDevice->get()->port.getValue("");
                        sc->device->type = dbDevice->get()->type;
                        sc->refrescarConfiguracion();

                        PrinterService::printers.push_back(sc);
                        portsUsed.push_back(dbDevice->get()->port.getValue(""));
                        break;
                    }
                    else
                    {
                        OATPP_LOGE("ERROR DISPOSITIVO NO RESPONDE", " BD: %s", dbDevice->get()->serial.getValue("").c_str());
                        sc->closePort();
                    }
                }
            }
            else
            {
                OATPP_LOGE("TIPO DE DISPOSITIVO INVALIDO", ": impresora | balanza");
            }
        }
        catch (const std::exception &e)
        {
            OATPP_LOGE("ERROR", " %s", e.what());
        }
    }
}

oatpp::Object<DeviceFiscalInfo> AppService::getFiscalInfo()
{
    auto result = oatpp::Object<DeviceFiscalInfo>::createShared();
    auto devices = this->getAllDevices();

    PrinterService::printers.clear();
    BasculeService::bascules.clear();

    for (auto dbDevice = devices->items->begin(); dbDevice != devices->items->end(); dbDevice++)
    {
        SerialConnection *sc;
        OATPP_LOGD("", "------------------------- AppService:::AddDevices -------------------------");
        OATPP_LOGD("\nDISPOSITIVO:", "TIPO: %s \n MARCA: %s \n MODELO: %s \n SERIAL: %s \n PUERTO: %s",
                   dbDevice->get()->type.getValue("").c_str(),
                   dbDevice->get()->brand.getValue("").c_str(),
                   dbDevice->get()->model.getValue("").c_str(),
                   dbDevice->get()->serial.getValue("").c_str(),
                   dbDevice->get()->port.getValue("").c_str());

        try
        {
            if (dbDevice->get()->type == "impresora")
            {
                // sc=Serial connection
                if (dbDevice->get()->brand == "HKA" && dbDevice->get()->dispositivo.getValue(0) == 0)
                    sc = new HkaPrinter(dbDevice->get()->port.getValue(""));
                else if (dbDevice->get()->brand == "HKA" && dbDevice->get()->dispositivo.getValue(0) == 1)
                    sc = new HkaPrinter(dbDevice->get()->port.getValue(""), "8E1");
                else if (dbDevice->get()->brand == "NETSOFT")
                    sc = new NetsoftPrinter(dbDevice->get()->port.getValue(""));
                else if (dbDevice->get()->brand == "PNP")
                    sc = new PnpPrinter(dbDevice->get()->port.getValue(""));
                else if (dbDevice->get()->brand == "VMAX")
                    sc = new VmaxPrinter(dbDevice->get()->port.getValue(""));
                else if (dbDevice->get()->brand == "SEWOO")
                    sc = new SewooPrinter(dbDevice->get()->port.getValue(""));
                else if (dbDevice->get()->brand == "ECRS")
                    sc = new EcrsPrinter(dbDevice->get()->port.getValue(""));
                else if (dbDevice->get()->brand == "BEMATECH")
                    sc = new BematechPrinter(dbDevice->get()->port.getValue(""));
                else
                {
                    OATPP_LOGE("IMPRESORA : MARCA : DESCONOCIDA", ": HKA | NETSOFT | PNP | VMAX | ECRS | BEMATECH");
                }
                OATPP_LOGI("PASO EL SC ", "");
                if (sc != nullptr)
                {
                    OATPP_LOGI("PASO EL SC ", "distinto de null");
                    OATPP_LOGI("SC ", "abrir puerto");
                    if (dbDevice->get()->port.getValue("") == "3010")
                    {
                        OATPP_LOGD("SERIAL COINCIDE AGREGANDO", " Impresora %s %s : %s dispositivo: %d", dbDevice->get()->brand.getValue("").c_str(), dbDevice->get()->model.getValue("").c_str(), dbDevice->get()->serial.getValue("").c_str(), dbDevice->get()->dispositivo.getValue(0));
                        sc->device = new Device();
                        sc->device->brand = dbDevice->get()->brand.getValue("");
                        sc->device->model = dbDevice->get()->model.getValue("");
                        sc->device->serial = dbDevice->get()->serial.getValue("");
                        sc->device->ip = dbDevice->get()->ip.getValue("");
                        sc->device->port = dbDevice->get()->port.getValue("");
                        sc->device->type = dbDevice->get()->type.getValue("");
                        sc->device->dispositivo = dbDevice->get()->dispositivo.getValue(0);
                        sc->refrescarConfiguracion();
                        PrinterService::printers.push_back(sc);
                    }
                    else if (sc->OpenPort())
                    {
                        OATPP_LOGI("SC ", "puerto abierto");
                        auto serial = sc->leerSerial();
                        // auto serial="TIY2300013";
                        OATPP_LOGD("EL SERIAL: ", "%s", serial.c_str());
                        if (serial == dbDevice->get()->serial.getValue(""))
                        {
                            OATPP_LOGD("SERIAL COINCIDE AGREGANDO", " Impresora %s %s : %s dispositivo: %d", dbDevice->get()->brand.getValue("").c_str(), dbDevice->get()->model.getValue("").c_str(), dbDevice->get()->serial.getValue("").c_str(), dbDevice->get()->dispositivo.getValue(0));
                            sc->device = new Device();
                            sc->device->brand = dbDevice->get()->brand.getValue("");
                            sc->device->model = dbDevice->get()->model.getValue("");
                            sc->device->serial = dbDevice->get()->serial.getValue("");
                            sc->device->ip = dbDevice->get()->ip.getValue("");
                            sc->device->port = dbDevice->get()->port.getValue("");
                            sc->device->type = dbDevice->get()->type.getValue("");
                            sc->device->dispositivo = dbDevice->get()->dispositivo.getValue(0);
                            sc->refrescarConfiguracion();
                            PrinterService::printers.push_back(sc);
                        }
                        else
                        {
                            OATPP_LOGE("ERROR SERIAL NO COINCIDE ", " BD: %s - DEVICE: %s", dbDevice->get()->serial.getValue("").c_str(), serial);
                            sc->closePort();
                        }
                        sc->leerRIF();
                    }
                    else
                    {
                        sc->closePort();
                    }
                }
                else
                {
                    OATPP_LOGE("IMPRESORA :: MARCA :: NO COMPATIBLE.", "");
                }
                result->fechaYhora = sc->leerFechaYhora();
                result->rif = sc->leerRIF();
                result->serial = sc->leerSerial();
                OATPP_LOGI("Serial Fiscal Impresora", "%s", result->serial->c_str());
            }
            else if (dbDevice->get()->type == "balanza")
            {
                auto balanza = new HkaBascule(dbDevice->get()->port.getValue(""));
                if (sc->OpenPort())
                {
                    auto peso = balanza->getPeso();
                    if (!peso.empty())
                    {
                        OATPP_LOGD("AGREGANDO ", " Balanza %s %s", dbDevice->get()->brand.getValue("").c_str(), dbDevice->get()->serial.getValue("").c_str());
                        sc->device = new Device();
                        sc->device->brand = dbDevice->get()->brand;
                        sc->device->model = dbDevice->get()->model;
                        sc->device->serial = dbDevice->get()->serial;
                        sc->device->port = dbDevice->get()->port.getValue("");
                        sc->device->type = dbDevice->get()->type;
                        sc->refrescarConfiguracion();

                        PrinterService::printers.push_back(sc);
                        portsUsed.push_back(dbDevice->get()->port.getValue(""));
                        break;
                    }
                    else
                    {
                        OATPP_LOGE("ERROR DISPOSITIVO NO RESPONDE", " BD: %s", dbDevice->get()->serial.getValue("").c_str());
                        sc->closePort();
                    }
                }
            }
            else
            {
                OATPP_LOGE("TIPO DE DISPOSITIVO INVALIDO", ": impresora | balanza");
            }
            return result;
        }
        catch (const std::exception &e)
        {
            OATPP_LOGE("ERROR", " %s", e.what());
            return result;
        }
    }
}

void AppService::sleep(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void AppService::waitConfiguration()
{
    while (AppService::usingSerialPort != 0)
    {
        AppService::sleep(200);
        OATPP_LOGI("Wait Configuration", "...");
    }
}

/**
 * Device
 */

oatpp::Object<PageDto<oatpp::Object<Device>>> AppService::getAllDevices()
{
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
    return result;
}

oatpp::Object<DeviceFiscalInfo> AppService::checkFiscalInfo()
{
    OATPP_LOGI("ENTRANDO A: Object<DeviceFiscalInfo> AppService::checkFiscalInfo() ", " ");
    waitConfiguration();
    auto result = oatpp::Object<DeviceFiscalInfo>::createShared();
    auto devices = this->getAllDevices();

    OATPP_LOGI("checkDevices", "Tarea(s) pendiente(s): %d", AppService::usingSerialPort.load());
    while (AppService::usingSerialPort != 0)
    {
        AppService::sleep(10000);
        OATPP_LOGI("checkDevices", "ESPERANDO 1000 ms, mientras se liberan los dispositivos... Tarea(s) pendiente(s): %d", AppService::usingSerialPort.load());
    }
    AppService::configuringPort++;
    closeAllPorts();

    if (devices->items->size() > 0)
    {
        OATPP_COMPONENT(oatpp::Object<PrinterConfig>, printerConfig);
        OATPP_ASSERT_HTTP((printerConfig && printerConfig->devices), Status::CODE_400, "Debe configurar los dispositivos en printer_config.json");
        OATPP_ASSERT_HTTP(printerConfig->devices->size() > 0, Status::CODE_400, "Debe configurar los dispositivos en printer_config.json");

        SerialConnection *sc;
        // auto ports = AppService::getAllPortsName();
        //     Tools::printDTO(ports);
        auto items = oatpp::Vector<oatpp::String>::createShared();
        oatpp::String text;
        text = std::string(printerConfig->devices[0]->port);
        items->push_back(text);
        auto page = PageDto<oatpp::String>::createShared();
        page->offset = unsigned(0);
        page->limit = 1;
        page->count = 1;
        page->items = items;
        auto ports = page;

        Tools::printDTO(ports);
        //  return result;

        for (auto dbDevice = devices->items->begin(); dbDevice != devices->items->end(); dbDevice++)
        {
            OATPP_LOGD("", "------------------------- AppService:::checkFiscalInfo -------------------------");
            OATPP_LOGD("ESCANEANDO PUERTOS PARA:", " %s - %s : %s", dbDevice->get()->brand->c_str(), dbDevice->get()->serial->c_str(), dbDevice->get()->brand->c_str());
            for (auto port = ports->items->begin(); port != ports->items->end(); port++)
            {
                if (devices->items[0]->port != "3010")
                {
                    OATPP_LOGD("", "-----------------22222222222222222222222-------------");
                    std::list<std::string>::iterator findPort = std::find(AppService::portsUsed.begin(), AppService::portsUsed.end(), port->get()->c_str());
                    if (findPort != AppService::portsUsed.end())
                        continue;
                }
                try
                {
                    OATPP_LOGD("", "----------------333333333333333-------------");
                    if (dbDevice->get()->type == "impresora")
                    {
                        //  dbDevice->get()->port->c_str();
                        if (dbDevice->get()->brand == "HKA")
                            sc = new HkaPrinter(port->get()->c_str());
                        else if (dbDevice->get()->brand == "NETSOFT")
                            sc = new NetsoftPrinter(port->get()->c_str());
                        else if (dbDevice->get()->brand == "PNP")
                            sc = new PnpPrinter(port->get()->c_str());
                        else if (dbDevice->get()->brand == "VMAX")
                            sc = new VmaxPrinter(port->get()->c_str());
                        else if (dbDevice->get()->brand == "SEWOO")
                            sc = new SewooPrinter(port->get()->c_str());
                        else if (dbDevice->get()->brand == "ECRS")
                            sc = new EcrsPrinter(port->get()->c_str());
                        else if (dbDevice->get()->brand == "BEMATECH")
                            sc = new BematechPrinter(port->get()->c_str());
                        else
                        {
                            OATPP_LOGE("IMPRESORA : MARCA : DESCONOCIDA", ": HKA | NETSOFT | PNP | VMAX | ECRS | BEMATECH");
                        }
                        if (sc != nullptr)
                        {
                            if (dbDevice->get()->port.getValue("") == "3010")
                            {
                                OATPP_LOGD("SERIAL COINCIDE AGREGANDO", " Impresora %s %s : %s dispositivo: %d", dbDevice->get()->brand.getValue("").c_str(), dbDevice->get()->model.getValue("").c_str(), dbDevice->get()->serial.getValue("").c_str(), dbDevice->get()->dispositivo.getValue(0));
                                sc->device = new Device();
                                sc->device->brand = dbDevice->get()->brand.getValue("");
                                sc->device->model = dbDevice->get()->model.getValue("");
                                sc->device->serial = dbDevice->get()->serial.getValue("");
                                sc->device->ip = dbDevice->get()->ip.getValue("");
                                sc->device->port = dbDevice->get()->port.getValue("");
                                sc->device->type = dbDevice->get()->type.getValue("");
                                sc->device->dispositivo = dbDevice->get()->dispositivo.getValue(0);
                                sc->refrescarConfiguracion();
                                PrinterService::printers.push_back(sc);
                                result->fechaYhora = sc->leerFechaYhora();
                                result->rif = sc->leerRIF();
                                result->serial = sc->leerSerial();
                            }
                            else if (sc->OpenPort())
                            {
                                OATPP_LOGI("PORT", " %s OPEN", dbDevice->get()->port->c_str());

                                auto serial = sc->leerSerial();
                                // auto serial = "TIY2300013";
                                if (serial == dbDevice->get()->serial.getValue(""))
                                {
                                    OATPP_LOGD("SERIAL COINCIDE AGREGANDO ", " Impresora %s %s : %s", dbDevice->get()->brand.getValue("").c_str(), dbDevice->get()->serial.getValue("").c_str(), dbDevice->get()->model.getValue("").c_str());
                                    result->serial = serial;
                                    result->rif = sc->leerRIF();
                                    sc->closePort();
                                }
                                else
                                {
                                    OATPP_LOGE("ERROR SERIAL NO COINCIDE ", " BD: %s - DEVICE: %s", dbDevice->get()->serial.getValue("").c_str(), serial.c_str());
                                    sc->closePort();
                                }
                            }
                            else
                            {
                                sc->closePort();
                            }
                        }
                        else
                        {
                            OATPP_LOGE("IMPRESORA :: MARCA :: NO COMPATIBLE.", "");
                        }
                    }
                    else if (dbDevice->get()->type == "balanza")
                    {
                    }
                    else
                    {
                        OATPP_LOGE("TIPO DE DISPOSITIVO INVALIDO", "");
                    }
                }
                catch (const std::exception &e)
                {
                    sc->closePort();
                    OATPP_LOGE("ERROR ESCANEANDO PUERTO SERIAL", "%s", e.what());
                }
            }
        }
    }
    AppService::configuringPort--;
    OATPP_LOGI("saliendo de : Object<DeviceFiscalInfo> AppService::checkFiscalInfo() ", " ");

    return result;
}

// //TEST WEBSOCKET ONE
// void AppService::testConnection(){
//         // Dirección IP de la impresora fiscal
//     const char* printerIpAddress = "0.0.0.0";
//     // Puerto de la impresora fiscal
//     v_int32 printerPort = 3010;
//     OATPP_LOGI("Crear un cliente ws","");
//     // Crear un cliente websocket
//     auto connectionProvider = oatpp::network::tcp::client::ConnectionProvider::createShared({"demo.piesocket.com", 80});
//     auto connector = oatpp::websocket::Connector::createShared(connectionProvider);
//     OATPP_LOGI("Conectar al servidor de la impresora fiscal ws","");
//     // Conectar al servidor de la impresora fiscal
//     auto connection = connector->connect("ws://" + oatpp::String(printerIpAddress) + ":" + oatpp::utils::conversion::int32ToStr(printerPort));
//     // Verificar si la conexión fue exitosa
//     OATPP_LOGI("Verificar si la conexión fue exitosa","");
//     if(connection) {
//         auto connection = connector->connect("v3/channel_1?api_key=VCXCEuvhGcBDP7XhiJJUDvR1e1D3eiVjgZ9VRiaV&notify_self");
//         auto socket = oatpp::websocket::WebSocket::createShared(connection, true /* maskOutgoingMessages must be true for clients */);
//         OATPP_LOGD("Client", "Connected to printer successfully");

//         // Enviar el comando para obtener el estado
//         socket->sendOneFrameText("<STX>C0<ETX>");

//         // Recibir la respuesta
//         auto response = socket->getListener();
//         // Procesar la respuesta
//         if(response) {
//             OATPP_LOGD("Client", "Response from printer: %s", response);
//             // Procesar la respuesta recibida...
//         } else {
//             OATPP_LOGD("Client", "No response from printer");
//         }
//     } else {
//         OATPP_LOGD("Client", "Failed to connect to printer");
//     }

// }
// //END TESTWEBSOCKET
