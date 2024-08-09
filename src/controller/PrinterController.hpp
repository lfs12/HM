#ifndef PrinterController_hpp
#define PrinterController_hpp

#include "service/PrinterService.hpp"
#include "service/Printer/PrinterInterface.hpp"
#include "service/ComandaService.hpp"
#include "AppController.hpp"
#include "../dto/ComandaDto.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController) //<- Begin Codegen

/**
 * Printer controller.
 */
class PrinterController : public oatpp::web::server::api::ApiController
{
public:
  PrinterController(const std::shared_ptr<ObjectMapper> &objectMapper) : oatpp::web::server::api::ApiController(objectMapper)
  {
  }

private:
  PrinterService m_printerService; // Create printer service.
  ComandaService m_comandaService; // Create comanda service.

public:
  static std::shared_ptr<PrinterController> createShared(
      OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper) // Inject objectMapper component here as default parameter
  )
  {
    return std::make_shared<PrinterController>(objectMapper);
  }

  ENDPOINT("POST", "printer", printer, BODY_DTO(Object<PrinterDto>, printerDto))
  {
    AppService::DtoToString(printerDto);
    Object<PrinterDto> dto = printerDto;
    std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> response;
    // AppService::waitConfiguration();

    try
    {
      m_printerService.mutex.lock();
      response = createResponse(Status::CODE_200, m_printerService.exeComandPrinter(dto));
      m_printerService.mutex.unlock();

      response->putHeader(Header::CONTENT_TYPE, "text/json");
    }
    catch (...)
    {
      m_printerService.mutex.unlock();
      throw;
    }
         OATPP_LOGI("PrinterService...", "Si llega");
   // std::this_thread::sleep_for(std::chrono::seconds{10});
    return response;
  }

  ENDPOINT("POST", "cuadre_caja", cuadreCaja, BODY_DTO(Object<CuadreCajaDto>, cuadreCajaDto))
  {
    AppService::DtoToString(cuadreCajaDto);
    std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> response;
    // AppService::waitConfiguration();
    auto s = StatusDto::createShared();

    try
    {
      m_printerService.mutex.lock();

      Object<PrinterDto> dtoToPrint = PrinterDto::createShared();
      auto sc = m_printerService.selectDevice(dtoToPrint);

      OATPP_ASSERT_HTTP(sc, Status::CODE_500, "Error al obtener impresora...");

      PrinterInterface *pi = (PrinterInterface *)sc;
      OATPP_LOGD("", "%d", pi->maxLength);
      pi->closePort();      

      dtoToPrint->cmd = "textoNF";
      dtoToPrint->textoNF = m_comandaService.imprimirCuadreCaja(cuadreCajaDto, pi->maxLength);

      m_printerService.exeComandPrinter(dtoToPrint);

      s->status = "OK";
      s->code = 200;
      s->message = "OK";
      response = createDtoResponse(Status::CODE_200, s);
      m_printerService.mutex.unlock();

      response->putHeader(Header::CONTENT_TYPE, "text/json");
    }
    catch (...)
    {
      m_printerService.mutex.unlock();
      throw;
    }

    return response;
  }

  ENDPOINT("POST", "precuenta", precuenta, BODY_DTO(Object<PrecuentaDto>, precuentaDto))
  {
    AppService::DtoToString(precuentaDto);
    std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> response;
    // AppService::waitConfiguration();
    auto s = StatusDto::createShared();

    try
    {
      m_printerService.mutex.lock();

      Object<PrinterDto> dtoToPrint = PrinterDto::createShared();
      auto sc = m_printerService.selectDevice(dtoToPrint);

      OATPP_ASSERT_HTTP(sc, Status::CODE_500, "Error al obtener impresora...");

      PrinterInterface *pi = (PrinterInterface *)sc;
      OATPP_LOGD("", "%d", pi->maxLength);
      pi->closePort();      

      dtoToPrint->cmd = "textoNF";
      dtoToPrint->textoNF = m_comandaService.imprimirPrecuenta(precuentaDto, pi->maxLength);

      m_printerService.exeComandPrinter(dtoToPrint);

      s->status = "OK";
      s->code = 200;
      s->message = "OK";
      response = createDtoResponse(Status::CODE_200, s);
      m_printerService.mutex.unlock();

      response->putHeader(Header::CONTENT_TYPE, "text/json");
    }
    catch (...)
    {
      m_printerService.mutex.unlock();
      throw;
    }

    return response;
  }

  ENDPOINT("POST", "raw", raw, BODY_DTO(Object<RawDto>, rawDto))
  {
    AppService::DtoToString(rawDto);
    std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> response;
    // AppService::waitConfiguration();
    auto s = StatusDto::createShared();

    try
    {
      m_printerService.mutex.lock();

      Object<PrinterDto> dtoToPrint = PrinterDto::createShared();
      auto sc = m_printerService.selectDevice(dtoToPrint);

      OATPP_ASSERT_HTTP(sc, Status::CODE_500, "Error al obtener impresora...");

      PrinterInterface *pi = (PrinterInterface *)sc;
      OATPP_LOGD("", "%d", pi->maxLength);
      pi->closePort();      

      dtoToPrint->cmd = "textoNF";
      dtoToPrint->textoNF = m_comandaService.imprimirRaw(rawDto, pi->maxLength);

      m_printerService.exeComandPrinter(dtoToPrint);

      s->status = "OK";
      s->code = 200;
      s->message = "OK";
      response = createDtoResponse(Status::CODE_200, s);
      m_printerService.mutex.unlock();

      response->putHeader(Header::CONTENT_TYPE, "text/json");
    }
    catch (...)
    {
      m_printerService.mutex.unlock();
      throw;
    }

    return response;
  }

  ENDPOINT("POST", "comanda", comanda, BODY_DTO(Object<ComandasDto>, comandasDto))
  {
    AppService::DtoToString(comandasDto);
    std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> response;
    auto status = oatpp::Vector<oatpp::Object<StatusDto>>::createShared();
    auto s = StatusDto::createShared();

    // AppService::waitConfiguration();
    try
    {
      m_printerService.mutex.lock();
      for (int i = 0; i < comandasDto->comandas->size(); i++)
      {
        try
        {
          Object<PrinterDto> dtoToPrint = PrinterDto::createShared();
          auto comanda = comandasDto->comandas[i];
          auto sc = m_printerService.selectDevice(dtoToPrint);
          OATPP_ASSERT_HTTP(sc, Status::CODE_500, "Error al obtener impresora...");

          PrinterInterface *pi = (PrinterInterface *)sc;
          OATPP_LOGD("", "%d", pi->maxLength);
          pi->closePort();     
           
          dtoToPrint->cmd = "textoNF";
          dtoToPrint->textoNF = m_comandaService.imprimirComanda(comanda, pi->maxLength);

          m_printerService.exeComandPrinter(dtoToPrint);

          s->status = "OK";
          s->code = 200;
          s->message = "OK";
        }
        catch (const std::exception &e)
        {
          s = StatusDto::createShared();
          s->status = "ERROR";
          s->code = 500;
          s->message = e.what();
          OATPP_LOGE("Comanda[N]", "ERROR ...");
        }
        status->push_back(s);
      }
      m_printerService.mutex.unlock();

      response = createDtoResponse(Status::CODE_200, status);
      response->putHeader(Header::CONTENT_TYPE, "text/json");
    }
    catch (...)
    {
      m_printerService.mutex.unlock();
      throw;
    }

    return response;
  }
};

#include OATPP_CODEGEN_BEGIN(ApiController) //<- End Codegen

#endif /* PrinterController_hpp */
