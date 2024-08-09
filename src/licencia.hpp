#ifndef APP_SERVICE_HPP
#define APP_SERVICE_HPP

#include <stdexcept>

#include "cryptography/service.hpp"
#include "service/AppService.hpp"

#include <nlohmann\json.hpp>
#include "Tools.hpp"

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/core/macro/component.hpp"

#include <iostream>
#include <fstream>
#include <chrono>
#include <cerrno>

using json = nlohmann::json;

#define CPPHTTPLIB_OPENSSL_SUPPORT

class LicenciaService
{
private:
    OATPP_COMPONENT(oatpp::Object<WebService>, config);
    CryptographyService imp_cryptography;
    AppService app_service;
public:
     
    enum LicenciaStatus
    {
        demo,
        activado,
        activado_offline,
        activado_en_cache,
        pendiente_activacion,

        verificacion_fallida,
        vencido,
        por_vencerse,
        verificacion_fallida_data
    };

    static int appStatus;
    static std::string licencia;

    void validar_licencia();
    bool onlineValidation(DatosLicenciaDto::Wrapper &datos);
    bool offlineValidation();

    bool demoValidation();

    void validar_licencia_test();
};
#endif // APP_SERVICE_HPP
