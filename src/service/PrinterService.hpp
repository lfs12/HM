
#ifndef PRINTERSERVICE_HPP
#define PRINTERSERVICE_HPP

#include "dto/ConfigDto.hpp"
#include "dto/PrinterDto.hpp"
#include "dto/StatusDto.hpp"

#include "AppService.hpp"
#include "Printer/BematechPrinter.hpp"
#include "Printer/EcrsPrinter.hpp"
#include "Printer/HkaPrinter.hpp"
#include "Printer/NetsoftPrinter.hpp"
#include "Printer/PnpPrinter.hpp"
#include "Printer/VmaxPrinter.hpp"
#include "Printer/SewooPrinter.hpp"
#include "SerialConnection.hpp"
#include "../Tools.hpp"

#include <memory>
#include <thread>
#include <mutex>
#include <fstream>
#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

class PrinterService
{
private:
  typedef oatpp::web::protocol::http::Status Status;

public:
  std::string exeComandPrinter(oatpp::Object<PrinterDto> &dto);
  void validarDTO(oatpp::Object<PrinterDto> &dto);
  std::float_t monto_valido(oatpp::Object<PrinterDto> &dat);
  void limpiarDTO(oatpp::Object<PrinterDto> &dto);
  bool validateNumb(std::string numb);
  SerialConnection *selectDevice(oatpp::Object<PrinterDto> &dto);
  SerialConnection *getDevice(oatpp::Object<PrinterDto> &dto);
  static std::vector<SerialConnection *> printers;
  static std::mutex mutex;
};

#endif // PRINTERSERVICE_HPP
