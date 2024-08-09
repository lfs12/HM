#ifndef BasculeController_hpp
#define BasculeController_hpp

#include "service/BasculeService.hpp"
#include "service/AppService.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController) //<- Begin Codegen

/**
 * Bascule REST controller.
 */
class BasculeController : public oatpp::web::server::api::ApiController
{
public:
  BasculeController(const std::shared_ptr<ObjectMapper> &objectMapper) : oatpp::web::server::api::ApiController(objectMapper)
  {
  }

private:
  BasculeService m_basculeService; // Create bascule service.
public:
  static std::shared_ptr<BasculeController> createShared(
      OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper) // Inject objectMapper component here as default parameter
  )
  {
    return std::make_shared<BasculeController>(objectMapper);
  }

  ENDPOINT("POST", "bascule", bascule, BODY_DTO(Object<BasculeDto>, basculeDto))
  {
    AppService::DtoToString(basculeDto);
    Object<BasculeDto> dto = basculeDto;
    std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> response;
    AppService::waitConfiguration();
    try
    {
      AppService::usingSerialPort++;
      response = createResponse(Status::CODE_200, m_basculeService.ejecutarComando(dto));
      AppService::usingSerialPort--;

      response->putHeader(Header::CONTENT_TYPE, "text/json");
    }
    catch (...)
    {
      AppService::usingSerialPort--;
      throw;
    }

    return response;
  }
};

#include OATPP_CODEGEN_BEGIN(ApiController) //<- End Codegen

#endif /* BasculeController_hpp */
