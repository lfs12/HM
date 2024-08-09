#ifndef AppComponent_hpp
#define AppComponent_hpp

#include "dto/ConfigDto.hpp"
#include "dto/PrinterConfigDto.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/core/base/CommandLineArguments.hpp"

#include <iostream>
#include <fstream>
class AppComponent
{
private:
  oatpp::base::CommandLineArguments m_cmdArgs;

public:
  AppComponent(const oatpp::base::CommandLineArguments &cmdArgs)
      : m_cmdArgs(cmdArgs)
  {
  }

public:

  /**
   * This should be configured through config-server ex. Consul
   */
  OATPP_CREATE_COMPONENT(oatpp::Object<WebService>, config)
  ([this]
   {
    const char *configPath = CONFIG_PATH;
    auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
    WebService::Wrapper profile;
    try
    {

      oatpp::String configText = oatpp::String::loadFromFile(configPath);
      if (configText)
      {
        oatpp::Fields<WebService::Wrapper> profiles = objectMapper->readFromString<oatpp::Fields<oatpp::Object<WebService>>>(configText);
        const char *profileArg = m_cmdArgs.getNamedArgumentValue("--profile", "dev"); // if no env varioable get from command line

        OATPP_LOGD("Server", "Loading configuration profile '%s'", profileArg);

        profile = profiles.getValueByKey(profileArg, nullptr);
      }
    }
    catch (const std::exception &e)
    {
      OATPP_LOGE("AppComponent", "%s", e.what());
    }
    if (!profile)
    {
      profile = oatpp::Object<WebService>::createShared();
      profile->port = 5500;
      profile->host = "0.0.0.0";
    }
    return profile;
    }());

 OATPP_CREATE_COMPONENT(oatpp::Object<PrinterConfig>, printerConfig)
  ([this]
   {
    const char *configPath = PRINTER_CONFIG_PATH;
    auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
    PrinterConfig::Wrapper printerConfig;
    try
    {
      oatpp::String configText = oatpp::String::loadFromFile(configPath);
      if (!configText) {
        std::ofstream configFile(configPath);

        configFile << R"({"hka":{"formas-pago":[{"10":"CHEQUE 4","11":"CHEQUE 5","12":"CHEQUE 6","13":"TARJETA 1","14":"TARJETA 2","15":"TARJETA 3","16":"TARJETA 4","17":"TARJETA 5","18":"TARJETA 6","19":"TICKET 1","20":"DIVISA 1","21":"DIVISA 2","22":"DIVISA 3","23":"DIVISA 4","24":"DIVISA 5","serial":"*","01":"EFECTIVO 1","02":"EFECTIVO 2","03":"EFECTIVO 3","04":"EFECTIVO 4","05":"EFECTIVO 5","06":"EFECTIVO 6","07":"CHEQUE 1","08":"CHEQUE 2","09":"CHEQUE 3"}]},"pnp":{"tasas":[{"serial":"*","G":16,"R":8,"A":32,"E":0,"P":0}]}})";
        configFile.close();
      }
      printerConfig = objectMapper->readFromString<oatpp::Object<PrinterConfig>>(configText);
      
      OATPP_LOGD("Server", "Loading printer configuration profiles");
    }
    catch (const std::exception &e)
    {
      OATPP_LOGE("AppComponent", "%s", e.what());
    }
    
    return printerConfig;
    }());

};

#endif /* AppComponent_hpp */