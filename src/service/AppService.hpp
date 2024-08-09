
#ifndef AppService_hpp
#define AppService_hpp

#include "PrinterService.hpp"
#include "BasculeService.hpp"
#include "../lib/rs232.hpp"

#include "dto/StatusDto.hpp"

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

// #include "../lib/HTTPRequest.hpp"

#include <thread>
#include <utility>

class AppService
{
private:
  typedef oatpp::web::protocol::http::Status Status;
  void closeAllPorts();

public:
  static std::list<std::string> portsUsed;
  static std::atomic<int> usingSerialPort;
  static std::atomic<int> configuringPort;

  static void sleep(int ms);
  static void waitConfiguration();
  static std::string DtoToString(const oatpp::data::mapping::type::Void &variant);

  std::string getDevicesInfo();
  oatpp::Vector<oatpp::Object<DeviceStatus>> checkDevices();
  oatpp::Object<DeviceFiscalInfo> checkFiscalInfo();
  void addDevices();
  //NEW ADDED
  oatpp::Object<DeviceFiscalInfo> getFiscalInfo();
  //---------
  oatpp::Object<PageDto<oatpp::String>> getAllPortsName();
  oatpp::Object<PageDto<oatpp::Object<Device>>> getAllDevices();
};

#endif // AppService_hpp
