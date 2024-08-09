#include "ComandaService.hpp"

std::mutex mutex;

std::string ComandaService::replaceTags(json headerTags, std::string str)
{
    std::smatch m;
    std::regex e("\\[(.*?)\\]");

    bool ignore = false;
    std::string strIt = str;

    while (std::regex_search(strIt, m, e))
    {
        int tagLength = m.str().size() - 2;
        double _entero = 4;
        double _decimal = 2;

        std::string tag = Tools::trim(m.str().substr(1, tagLength));
        if (tag.size() > 0 && tag[0] == '%')
        {
            auto parts = Tools::explode(tag, ' ');

            tag = parts.size() == 2 ? parts[1] : tag;

            double aux;
            aux = std::stof(parts[0].substr(1, std::string::npos));

            _decimal = round(modf(aux, &_entero) * 10);
        }
        ignore = headerTags[tag] == nullptr;
        if (!tag.empty() && !ignore)
        {
            try
            {

                if (std::string(headerTags[tag].type_name()) == std::string("string"))
                {
                    std::string valor = headerTags[tag];
                    str = std::regex_replace(str, std::regex("\\[" + tag + "\\s*\\]"), Tools::fillString(valor.substr(0, tagLength), tagLength) + (valor.find("�") != std::string::npos ? "  " : ""));
                }
                else if (std::string(headerTags[tag].type_name()) == std::string("number"))
                {
                    double valor = headerTags[tag];
                    str = Tools::replace(str, m.str(), Tools::fillNumber(valor, _decimal, _entero + _decimal + (_decimal > 0 ? 1 : 0)));
                }
                else
                {
                    OATPP_LOGI("TAG TYPE:", "%s", headerTags[tag].type_name())
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << " e: " << e.what() << "\n";
            }
        }
        strIt = m.suffix().str();
    }

    return str;
}

oatpp::List<oatpp::String> ComandaService::imprimirPrecuenta(const oatpp::Object<PrecuentaDto> &dto, int length)
{
    std::stringstream imp;
    int charactersLine = length;
    int col3side = round(charactersLine * 0.375), col3center = charactersLine - round(charactersLine * 0.75);

    try
    {
        OATPP_ASSERT_HTTP(dto, Status::CODE_500, "Objeto invalido, debe ser distinto de null");
        OATPP_ASSERT_HTTP(dto->totales, Status::CODE_500, "Objeto invalido, totales debe ser distinto de null");
        OATPP_ASSERT_HTTP(dto->subtotales, Status::CODE_500, "Objeto invalido, subtotales debe ser distinto de null");
        OATPP_ASSERT_HTTP(dto->descuentosGenerales, Status::CODE_500, "Objeto invalido, descuentos debe ser distinto de null");

        std::ifstream comanda("resources/PRECUENTA.txt");

        json headerTags;
        headerTags["EMPRESA"] = dto->empresa.getValue("");
        headerTags["CLIENTE"] = dto->cliente.getValue("");
        headerTags["RIF"] = dto->rif.getValue("");
        headerTags["NDOC"] = dto->ndoc.getValue("");
        headerTags["USER"] = dto->user.getValue("");
        headerTags["CORRELA"] = dto->correlacion.getValue("");
        headerTags["FECHA"] = Tools::getDateNow();
        headerTags["HORA"] = Tools::getTimeNow();
        headerTags["ZONA"] = dto->zona.getValue("");
        headerTags["MESA"] = dto->mesa.getValue("");
        headerTags["SRV"] = dto->servicio.getValue("");
        headerTags["IVA"] = dto->iva.getValue("");

        headerTags["TOTBR"] = dto->subtotales == nullptr ? -0 : dto->subtotales[1]->monto.getValue(-0);
        headerTags["TOTSRV"] = dto->servicios == nullptr ? -0 : dto->servicios[1]->monto.getValue(-0);
        headerTags["NETOSINIVA"] = dto->totalesSinIva == nullptr ? -0 : dto->totalesSinIva[1]->monto.getValue(-0);
        headerTags["TOIVA"] = dto->totalIvas == nullptr ? -0 : dto->totalIvas[1]->monto.getValue(-0);
        headerTags["APAGA"] = dto->totales == nullptr ? -0 : dto->totales[1]->monto.getValue(-0) + dto->totalIvas[1]->monto.getValue(-0);
        headerTags["APAGA_IGTF"] = headerTags["APAGA"].get<double>() + (headerTags["APAGA"].get<double>() * 0.03);

        headerTags["TOTBR_REF"] = dto->subtotales == nullptr ? -0 : dto->subtotales[0]->monto.getValue(-0);
        headerTags["TOTSRV_REF"] = dto->servicios == nullptr ? -0 : dto->servicios[0]->monto.getValue(-0);
        headerTags["NETOSINIVA_REF"] = dto->totalesSinIva == nullptr ? -0 : dto->totalesSinIva[0]->monto.getValue(-0);
        headerTags["TOIVA_REF"] = dto->totalIvas == nullptr ? -0 : dto->totalIvas[0]->monto.getValue(-0);
        headerTags["APAGA_REF"] = dto->totales == nullptr ? -0 : dto->totales[0]->monto.getValue(-0) + dto->totalIvas[0]->monto.getValue(-0);
        headerTags["APAGA_REF_IGTF"] = headerTags["APAGA_REF"].get<double>() + (headerTags["APAGA_REF"].get<double>() * 0.03);

        std::string productsLines = "";
        std::string regex = "\\[(.*?)\\]";
        std::string linea;
        std::string lineaPosProductos = "";
        while (std::getline(comanda, linea))
        {
            if (
                (linea.find("PRODU") != std::string::npos && Tools::findRegex(regex, "PRODU", linea)) ||
                (linea.find("CANT") != std::string::npos && Tools::findRegex(regex, "CANT", linea)) ||
                (linea.find("MONTL") != std::string::npos && Tools::findRegex(regex, "MONTL", linea)) ||
                (linea.find("REF") != std::string::npos && Tools::findRegex(regex, "REF", linea)))
            {
                productsLines += (linea + "\n");
            }
            else
            {
                if (productsLines.empty())
                {
                    imp << replaceTags(headerTags, linea) << std::endl;
                }
                else
                {

                    for (auto i = dto->lineas->begin(); i != dto->lineas->end(); i++)
                    {
                        if (i->get() != nullptr)
                        {
                            json lineTag;
                            lineTag["CANT"] = i->get()->cant.getValue(-0);
                            lineTag["PRODU"] = i->get()->item.getValue("");
                            lineTag["MONTL"] = i->get()->precioTotalBs.getValue(-0);
                            lineTag["REF"] = i->get()->precioTotalD.getValue(-0);

                            imp << replaceTags(lineTag, productsLines);
                            imp << std::endl;
                        }
                    }
                    productsLines = "";
                    imp << replaceTags(headerTags, linea) << std::endl;
                }
            }
        }
    }
    catch (const std::exception &e)
    {
        OATPP_ASSERT_HTTP(false, Status::CODE_500, ("Error: " + std::string(e.what())));
    }

    OATPP_LOGI("obj", "%s", imp.str().c_str())
    return explode(imp.str(), '\n');
}

oatpp::List<oatpp::String> ComandaService::imprimirCuadreCaja(const CuadreCajaDto::Wrapper &dto, int length)
{
    std::stringstream imp;
    int charactersLine = length;

    try
    {
        std::ifstream cuadre_caja;
        if (dto->tipo == "detallado")
        {
            cuadre_caja.open("resources/CUADRE_CAJA_DETALLADO.txt");
        }
        else
        {
            cuadre_caja.open("resources/CUADRE_CAJA_RESUMIDO.txt");
        }

        json tags;
        tags["EMPRESA"] = dto->empresa.getValue("");
        tags["RIF"] = dto->rif.getValue("");
        tags["TLF"] = dto->telefono.getValue("");
        tags["TASA"] = dto->tasa.getValue(0.0);
        std::string direccion = "";
        for (size_t i = 0; i < dto->direccion.getValue("").size(); i += charactersLine)
            direccion += (dto->direccion->substr(i, charactersLine) + "\n");

        tags["DIRECCION"] = direccion;
        tags["FECHA"] = Tools::getDateNow();
        tags["HORA"] = Tools::getTimeNow();
        tags["ESTACION"] = (dto->estacion_id.getValue("") == "" ? "TODAS" : (dto->estacion_id.getValue("") + " - " + dto->estacion_nombre.getValue("")));
        tags["DESDE"] = Tools::intToDateString(dto->desde);
        tags["HASTA"] = Tools::intToDateString(dto->hasta);

        std::string fFront = "", fBack = "", dFront = "", dBack = "";
        int nFacturas = 0;
        int nDevoluciones = 0;
        double descuento = 0;
        for (auto &&factura : *dto->facturas)
        {
            if (factura->tipo == "FAC")
            {
                descuento += factura->descuento;
                nFacturas++;
                if (fFront == "")
                    fFront = factura->n_doc.getValue("00000000");
                fBack = factura->n_doc.getValue("00000000");
            }
            else
            {
                descuento -= factura->descuento;
                nDevoluciones++;
                if (dFront == "")
                    dFront = factura->n_doc.getValue("00000000");
                dBack = factura->n_doc.getValue("00000000");
            }
        }
        if (dFront.empty())
        {
            dFront = "0000000000";
            dBack = "0000000000";
        }

        tags["N_FACTURAS"] = Tools::fillNumber(nFacturas, 0, 1);
        tags["FACT_PRI"] = fFront;
        tags["FACT_ULT"] = fBack;

        tags["N_DEVOLUCIONES"] = Tools::fillNumber(nDevoluciones, 0, 1);
        tags["DEV_PRI"] = dFront;
        tags["DEV_ULT"] = dBack;

        double totalVentaContado = 0;
        double totalVentaContadoVenta = 0;
        double totalVentaContadoDevolucion = 0;

        double totalVentaCredito = 0;
        double totalVentaCreditoVenta = 0;
        double totalVentaCreditoDevolucion = 0;

        int nVentasContado = 0;
        int nDevolucionesContado = 0;
        int nVentasCredito = 0;
        int nDevolucionesCredito = 0;

        double totalVentaBruto = 0.0;
        for (auto &&factura : *dto->facturas)
        {
            bool contado = false;
            bool credito = false;
            for (auto &&pago : *factura->pagos)
            {
                if (pago->instr_id.getValue(0) == -2)
                {
                    if (pago->monto.getValue(0.0) > 0.0)
                        credito = true;
                    if (factura->tipo == "FAC")
                    {
                        totalVentaCreditoVenta += pago->monto;
                    }
                    else
                    {
                        totalVentaCreditoDevolucion += pago->monto;
                    }
                }
                else
                {
                    if (pago->monto.getValue(0.0) > 0.0)
                        contado = true;
                    if (factura->tipo == "FAC")
                    {
                        totalVentaContadoVenta += pago->monto;
                    }
                    else
                    {
                        totalVentaContadoDevolucion += pago->monto;
                    }
                }
            }
            totalVentaContado = totalVentaContadoVenta + totalVentaContadoDevolucion;
            totalVentaCredito = totalVentaCreditoVenta + totalVentaCreditoDevolucion;
            totalVentaBruto = totalVentaContado + totalVentaCredito;
            if (factura->tipo.getValue("") == "FAC")
            {
                credito &&nVentasCredito++;
                contado &&nVentasContado++;
            }
            else
            {
                credito &&nDevolucionesCredito++;
                contado &&nDevolucionesContado++;
            }
        }

        tags["TOTAL_VENTA_BRUTO"] = totalVentaBruto;
        tags["DESCUENTOS"] = descuento;

        tags["N_VENTAS_CONTADO"] = nVentasContado;
        tags["TOTAL_VENTA_CONTADO_VENTA"] = totalVentaContadoVenta;
        tags["N_DEVOLUCIONES_CONTADO"] = nDevolucionesContado;
        tags["TOTAL_VENTA_CONTADO_DEVOLUCION"] = abs(totalVentaContadoDevolucion);
        tags["TOTAL_VENTA_CONTADO"] = totalVentaContado;

        tags["N_VENTAS_CREDITO"] = nVentasCredito;
        tags["TOTAL_VENTA_CREDITO_VENTA"] = totalVentaCreditoVenta;
        tags["N_DEVOLUCIONES_CREDITO"] = nDevolucionesCredito;
        tags["TOTAL_VENTA_CREDITO_DEVOLUCION"] = abs(totalVentaCreditoDevolucion);
        tags["TOTAL_VENTA_CREDITO"] = totalVentaCredito;

        double total = 0.0;
        double total_dolar = 0.0;
        auto metodosConValor = oatpp::Vector<Metodo_pagoDto::Wrapper>::createShared();
        for (auto &&metodo : *dto->metodos)
        {
            total = total + (round(metodo->monto.getValue(0.0) * 100) / 100);
            total_dolar = total_dolar + (round(metodo->monto_dolar.getValue(0.0) * 100) / 100);
            if (metodo->monto.getValue(0.0) != 0)
            {
                metodosConValor->push_back(metodo);
            }
        }
        tags["TOTAL"] = total;
        tags["TOTAL_DOLAR"] = total_dolar;

        std::string productsLines = "";
        std::string regex = "\\[(.*?)\\]";
        std::string linea;
        while (std::getline(cuadre_caja, linea))
        {
            if (
                (linea.find("METODO_PAGO") != std::string::npos && Tools::findRegex(regex, "METODO_PAGO", linea)) ||
                (linea.find("MONTO") != std::string::npos && Tools::findRegex(regex, "MONTO", linea)) ||
                (linea.find("MONTO_DOLAR") != std::string::npos && Tools::findRegex(regex, "MONTO_DOLAR", linea)))
            {
                productsLines += (linea);
            }
            else
            {
                if (productsLines.empty())
                {
                    imp << replaceTags(tags, linea) << std::endl;
                }
                else
                {
                    for (auto &&m : *metodosConValor)
                    {
                        json lineTag;
                        lineTag["METODO_PAGO"] = m->descripcion.getValue("");
                        lineTag["MONTO"] = m->monto.getValue(0.0);
                        lineTag["MONTO_DOLAR"] = m->monto_dolar.getValue(0.0);

                        imp << replaceTags(lineTag, productsLines);
                        imp << std::endl;
                    }
                    productsLines = "";
                }
            }
        }
    }
    catch (const std::exception &e)
    {
        OATPP_ASSERT_HTTP(false, Status::CODE_500, ("Error: " + std::string(e.what())));
    }
    OATPP_LOGI("obj", "%s", imp.str().c_str())

    return explode(imp.str(), '\n');
}

oatpp::List<oatpp::String> ComandaService::imprimirComanda(oatpp::Object<ComandaDto> &dto, int length)
{
    std::stringstream imp;
    int charactersLine = length;
    int col3side = round(charactersLine * 0.375), col3center = charactersLine - round(charactersLine * 0.75);

    try
    {
        OATPP_ASSERT_HTTP(dto, Status::CODE_500, "Objeto invalido, debe ser distinto de null");
        std::string www = ("resources/COMANDA" + (dto->imp.getValue("").empty() ? "" : dto->imp) + ".txt");
        OATPP_LOGI("...", "%s", www.c_str());

        std::ifstream comanda(www);
        std::ifstream lineaComanda("resources/LINEACOMANDA.txt");
        std::stringstream _lineaComanda;
        _lineaComanda << lineaComanda.rdbuf();
        std::string lineaCmnda = _lineaComanda.str();

        json headerTags;
        headerTags["NDOC"] = dto->ndoc.getValue("");
        headerTags["USER"] = dto->user.getValue("");
        headerTags["CORRELA"] = dto->correlacion.getValue("");
        headerTags["FECHA"] = Tools::getDateNow();
        headerTags["HORA"] = Tools::getTimeNow();
        headerTags["ZONA"] = dto->zona.getValue("");
        headerTags["MESA"] = dto->mesa.getValue("");

        std::ifstream lineaesp("resources/LINEACOMANDAESPACIO.txt");
        std::stringstream _lineaesp;
        _lineaesp << lineaesp.rdbuf();
        std::string lineaCmndaesp = _lineaesp.str();

        std::string linea2;
        std::string linea;
        auto espacio = false;
        while (std::getline(comanda, linea))
        {
            std::string linea2=linea;
             if (linea2.find("[LINEA*") != std::string::npos)
            {
                espacio = true;
            }
            if (linea.find("[LINEA") != std::string::npos)
            {
                for (auto i = dto->lineas->begin(); i != dto->lineas->end(); i++)
                {
                    if (i->get() != nullptr)
                    {
                        json lineTag;
                        lineTag["CANTD"] = i->get()->cant.getValue(-0);
                        lineTag["DESCRIPCION"] = i->get()->item.getValue("");

                        linea = lineaCmnda;
                        if (i->get()->cant.getValue(0) == 0)
                        {
                            imp << ".        " + i->get()->item.getValue("");
                        }
                        else
                        {
                            imp << replaceTags(lineTag, linea);
                        }
                        imp << std::endl;
                        OATPP_ASSERT_HTTP(i->get()->contornos, Status::CODE_500, "Contornos invalido, debe ser distinto de null");
                        for (auto j = i->get()->contornos->begin(); j != i->get()->contornos->end(); j++)
                        {
                            if (j->get() != nullptr)
                            {
                                imp << ". -- " << j->getValue("") << std::endl;
                            }
                        }
                    }
                    if (espacio)
                    {
                        linea2 = lineaCmndaesp;
                        json lineTag2;
                        lineTag2["CANTD2"] = ".";
                        lineTag2["DESCRIPCION2"] = ".";
                        // linea += (linea + "\n");
                        imp << replaceTags(lineTag2, linea2);
                        imp << std::endl;
                    }
                }
            }
            else
            {

                imp << replaceTags(headerTags, linea) << std::endl;
            }
        }
    }
    catch (const std::exception &e)
    {
        OATPP_ASSERT_HTTP(false, Status::CODE_500, ("Error: " + std::string(e.what())));
    }

    OATPP_LOGI("obj", "%s", imp.str().c_str())
    return explode(imp.str(), '\n');
}

oatpp::List<oatpp::String> ComandaService::imprimirRaw(const oatpp::Object<RawDto> &dto, int length)
{
    auto result = oatpp::List<oatpp::String>::createShared();
    auto s = StatusDto::createShared();

    std::ofstream imp;
    try
    {
        OATPP_ASSERT_HTTP(dto, Status::CODE_500, "Objeto invalido, debe ser distinto de null");
        OATPP_ASSERT_HTTP(dto->lineas, Status::CODE_500, "Objeto invalido, lineas debe ser distinto de null");

        for (size_t i = 0; i < dto->lineas->size(); i++)
        {
            result->push_back(dto->lineas[i].getValue("").substr(0, length));
        }
    }
    catch (const std::exception &e)
    {
        s = StatusDto::createShared();
        s->status = "ERROR";
        s->code = 500;
        s->message = e.what();
    }

    return result;
}

void ComandaService::sleep(int sec)
{
    std::this_thread::sleep_for(std::chrono::seconds(sec));
}

std::string ComandaService::fillNumber(double n)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << n;
    std::string s = stream.str();
    std::replace(s.begin(), s.end(), '.', ',');
    return s;
}

std::string ComandaService::getDateNow()
{
    std::time_t rawtime;
    std::tm *timeinfo;
    char buffer[20];

    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);

    std::strftime(buffer, 20, "%d/%m/%Y", timeinfo);
    return buffer;
}

std::string ComandaService::getTimeNow()
{
    std::time_t rawtime;
    std::tm *timeinfo;
    char buffer[20];

    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);

    std::strftime(buffer, 20, "%I:%M %p", timeinfo);
    return buffer;
}

oatpp::List<oatpp::String> ComandaService::explode(std::string const &s, char delim)
{
    auto result = oatpp::List<oatpp::String>::createShared();
    std::istringstream iss(s);

    for (std::string token; std::getline(iss, token, delim);)
    {
        auto item = std::move(token);
        result->push_back(item);
    }

    return result;
}