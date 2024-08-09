
#ifndef COMANDASERVICE_HPP
#define COMANDASERVICE_HPP

#include "dto/ComandaDto.hpp"
#include "dto/PrinterDto.hpp"
#include "dto/StatusDto.hpp"
#include "Tools.hpp"

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/core/Types.hpp"

#include <ctime>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <thread>
#include <cmath>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class ComandaService
{
private:
  typedef oatpp::web::protocol::http::Status Status;

public:
  oatpp::List<oatpp::String> imprimirComanda(oatpp::Object<ComandaDto> &dto, int length);
  oatpp::List<oatpp::String> imprimirPrecuenta(const oatpp::Object<PrecuentaDto> &dto, int length);
  oatpp::List<oatpp::String> imprimirRaw(const oatpp::Object<RawDto> &dto, int length);
  oatpp::List<oatpp::String> imprimirCuadreCaja(const CuadreCajaDto::Wrapper &dto, int length);

private:
  std::string replaceTags(json headerTags, std::string str);

  void sleep(int sec);
  std::string getDateNow();
  std::string getTimeNow();
  std::string fillNumber(double n);
  inline std::tm localtime_xp(std::time_t timer);
  oatpp::List<oatpp::String> explode(std::string const &s, char delim);

};

#endif // COMANDASERVICE_HPP
