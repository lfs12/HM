#ifndef AppController_hpp
#define AppController_hpp

#include "service/AppService.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/codegen.hpp"

#ifdef _WIN32 
#include <Windows.h>
#endif

#include OATPP_CODEGEN_BEGIN(ApiController) //<- Begin Codegen

class AppController : public oatpp::web::server::api::ApiController
{
public:
  AppController(const std::shared_ptr<ObjectMapper> &objectMapper) : oatpp::web::server::api::ApiController(objectMapper)
  {
  }

private:
  AppService m_appService; // Create bascule service.
  std::string APP = "HardwareManager";
  std::string VERSION = "1.3.0";

public:
  static std::shared_ptr<AppController> createShared(
      OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper) // Inject objectMapper component here as default parameter
  )
  {
    return std::make_shared<AppController>(objectMapper);
  }

  ENDPOINT("GET", "/", root)
  {
    nlohmann::json json;
    json["app"] = this->APP;
    json["status"] = "OK";

    auto response = createResponse(Status::CODE_200, json.dump());
    response->putHeader(Header::CONTENT_TYPE, "text/json");

    return response;
  }
  #ifdef _WIN32 
  ENDPOINT("GET", "/console", console)
  {
    auto res = ::IsWindowVisible(::GetConsoleWindow()) != FALSE;
    OATPP_LOGI("CONSOLE:", "%s", res ? "true" : "false");
    if (res){
      ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
    }
    else{
      ::ShowWindow(::GetConsoleWindow(), SW_SHOW);
    }

    auto response = createResponse(Status::CODE_200, "");

    return response;
  }
#endif

  ENDPOINT("GET", "version", version)
  {
    nlohmann::json json;
    json["app"] = this->APP;
    json["version"] = this->VERSION;

    auto response = createResponse(Status::CODE_200, json.dump());
    response->putHeader(Header::CONTENT_TYPE, "text/json");

    return response;
  }

  ENDPOINT("GET", "/info", info)
  {
    nlohmann::json json;
    json["app"] = this->APP;
    json["version"] = this->VERSION;

    json["dispositivos"] = nlohmann::json::parse(m_appService.getDevicesInfo());

    auto response = createResponse(Status::CODE_200, json.dump());
    response->putHeader(Header::CONTENT_TYPE, "text/json");

    return response;
  }

  /**
   * Check Devices
   */
  ENDPOINT("GET", "config/checkDevices", checkDevices)
  {
    return createDtoResponse(Status::CODE_200, m_appService.checkDevices());
  }
  ENDPOINT("GET", "config/fiscalInfo", checkFiscalInfo)
  {
    return createDtoResponse(Status::CODE_200, m_appService.checkFiscalInfo());
  }

  ENDPOINT("GET", "config/getAllPortsName", getAllPortsName)
  {
    return createDtoResponse(Status::CODE_200, m_appService.getAllPortsName());
  }

  /**
   * Device
   */

  ENDPOINT("GET", "config/devices", getDevices)
  {
    return createDtoResponse(Status::CODE_200, m_appService.getAllDevices());
  }
};
#include OATPP_CODEGEN_BEGIN(ApiController) //<- End Codegen

#endif /* AppController_hpp */

// static oatpp::String loadFile(const char *filename)
// {
//   auto buffer = oatpp::String::loadFromFile(filename);
//   OATPP_ASSERT_HTTP(buffer, Status::CODE_404, "File Not Found:(");
//   return buffer;
// }

// static oatpp::Response redirectPage(std::string newLocation)
// {
// auto response = createResponse(Status::CODE_302, "");
// response->putHeader("Location", newLocation.c_str());
// return response;
// }