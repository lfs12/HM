
#ifndef ServiceComponent_hpp
#define ServiceComponent_hpp

#include "dto/ConfigDto.hpp"
#include "ErrorHandler.hpp"

#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/component.hpp"

#include "oatpp/web/server/interceptor/ResponseInterceptor.hpp"
#include "oatpp/web/server/interceptor/AllowCorsGlobal.hpp"
#include "oatpp/web/protocol/http/outgoing/ResponseFactory.hpp"
#include "oatpp/web/protocol/http/Http.hpp"

#include "oatpp-openssl/server/ConnectionProvider.hpp"
#include "oatpp-openssl/Config.hpp"

#include "oatpp/core/base/Environment.hpp"
#include "oatpp/encoding/Base64.hpp"

#include "licencia.hpp"

class CustomRequestInterceptor : public oatpp::web::server::interceptor::RequestInterceptor
{
public:
  typedef oatpp::web::protocol::http::Status Status;
  std::shared_ptr<OutgoingResponse>
  intercept(const std::shared_ptr<IncomingRequest> &request) override
  {
    auto remoteAddress = request->getConnection()->getInputStreamContext().getProperties().get("peer_address").getValue("");
    auto peticion = request->getStartingLine().path.std_str();
    auto method = request->getStartingLine().method.std_str();
    if (LicenciaService::appStatus == LicenciaService::LicenciaStatus::verificacion_fallida || LicenciaService::appStatus == LicenciaService::LicenciaStatus::pendiente_activacion)
    {
      OATPP_ASSERT_HTTP(false, Status::CODE_403, "HardwareManager: Activación pendiente");
    }
    else if (LicenciaService::appStatus == LicenciaService::LicenciaStatus::verificacion_fallida_data)
    {
      OATPP_ASSERT_HTTP(false, Status::CODE_403, "HardwareManager: Activación pendiente");
    }
    else if (LicenciaService::appStatus == LicenciaService::LicenciaStatus::vencido)
    {
      OATPP_ASSERT_HTTP(false, Status::CODE_403, "HardwareManager: Licencia Vencida (Contáctese con su proveedor)");
    }
    return nullptr;
  }
};

/**/
class ServiceComponent
{
public:
  /**
   *  Create ObjectMapper component to serialize/deserialize DTOs in Contoller's API
   */
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, apiObjectMapper)
  ([]
   {
     auto mapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
     mapper->getSerializer()->getConfig()->useBeautifier = true;
     mapper->getSerializer()->getConfig()->escapeFlags = 0;
     return mapper; }());

  /**
   *  Create ConnectionProvider component which listens on the port
   */
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)
  ([]
   {
     OATPP_COMPONENT(oatpp::Object<WebService>, config); // Get config component
     return oatpp::network::tcp::server::ConnectionProvider::createShared({"0.0.0.0", config->port, oatpp::network::Address::IP_4}, true);

    //  auto configTLS = oatpp::openssl::Config::createDefaultServerConfigShared(CERT_CRT_PATH, CERT_PEM_PATH);
    //  return oatpp::openssl::server::ConnectionProvider::createShared(configTLS, {config->host, config->port, oatpp::network::Address::IP_4}, true);
     }());

  /**
   *  Create Router component
   */
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)
  ([]
   { return oatpp::web::server::HttpRouter::createShared(); }());

  /*
   * Create Response Interceptor
   */
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, serverConnectionHandler)
  ([]
   {
     OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);           // get Router component
     OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper); // get ObjectMapper component

     auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router);

     /* Add CustomResponseInterceptor */
     connectionHandler->addRequestInterceptor(std::make_shared<CustomRequestInterceptor>());
     /* Add CORS-enabling interceptors */
     connectionHandler->addRequestInterceptor(std::make_shared<oatpp::web::server::interceptor::AllowOptionsGlobal>());
     connectionHandler->addResponseInterceptor(std::make_shared<oatpp::web::server::interceptor::AllowCorsGlobal>());
     connectionHandler->setErrorHandler(std::make_shared<ErrorHandler>(objectMapper));

     return connectionHandler; }());
};

#endif /* ServiceComponent_hpp */
