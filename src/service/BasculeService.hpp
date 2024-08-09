
#ifndef BASCULESERVICE_HPP
#define BASCULESERVICE_HPP

#include "dto/BasculeDto.hpp"
#include "dto/StatusDto.hpp"
#include "Bascule/HkaBascule.hpp"

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/core/macro/component.hpp"

class BasculeService
{
private:
  typedef oatpp::web::protocol::http::Status Status;
  
public:
  std::string ejecutarComando(oatpp::Object<BasculeDto> &dto);
  static std::vector<SerialConnection*> bascules;
  SerialConnection* selectDevice(oatpp::Object<BasculeDto> &dto);
};

#endif // BASCULESERVICE_HPP
