
#include "BasculeService.hpp"
#include "SerialConnection.hpp"

#include "Bascule/HkaBascule.hpp"

std::vector<SerialConnection *> BasculeService::bascules = {};

std::string BasculeService::ejecutarComando(oatpp::Object<BasculeDto> &dto)
{
  SerialConnection *sc = nullptr;
  const oatpp::Object<BasculeDto> dtoPtr = dto.getPtr();

  oatpp::DTO *data = dto.get();
  std::string result;

  sc = selectDevice(dto);

  if (sc == nullptr || sc->device == nullptr)
    throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "NO SE ENCONTRO INFORMACIÓN DE LA IMPRESORA");

  if (sc->device->brand == "HKA")
  {
    try
    {
      sc->mutex.lock();
      std::string serial;
      int intentos = 0;

      while ((serial.empty() || serial != sc->device->serial->c_str()) && intentos++ < 3)
        try
        {
          serial = sc->leerSerial();
        }
        catch (...)
        {
          OATPP_LOGE("BASCULE SERVICE: ", "ERROR AL OBTENER EL SERIAL");
        }

      if (serial.empty())
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "BALANZA NO RESPONDE");

      OATPP_LOGD("SERIAL: ", " BD: %s - DEVICE: %s", sc->device->serial.getValue("").c_str(), serial.c_str());

      if (serial != "??????????" && serial != sc->device->serial->c_str())
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "BALANZA REQUIERE ACTIVACIÓN");

      result = sc->ejecutarComando(data);
      sc->mutex.unlock();
    }
    catch (const std::exception &e)
    {
      sc->mutex.unlock();
      throw;
    }
  }
  else
  {
    throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "BALANZA :: MARCA :: NO COMPATIBLE.");
  }

  return result;
}

SerialConnection *BasculeService::selectDevice(oatpp::Object<BasculeDto> &dto)
{
  SerialConnection *sc = nullptr;

  if (BasculeService::bascules.size() == 0)
  {
    throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "NO SE DETECTARON DISPOSITIVOS");
  }
  else if (BasculeService::bascules.size() == 1)
  {
    if (BasculeService::bascules[0]->device->type.getValue("") == "balanza")
    {
      OATPP_LOGI("\n\tDispositivo", " \n\t%s: Marca: %s \n\tPuerto: %s \n\tSerial: %s", BasculeService::bascules[0]->device->type.getValue("").c_str(), BasculeService::bascules[0]->device->brand.getValue("").c_str(), BasculeService::bascules[0]->device->port.getValue("").c_str(), BasculeService::bascules[0]->device->serial.getValue("").c_str());
      sc = BasculeService::bascules[0];
    }
  }
  else
  {
    OATPP_ASSERT_HTTP(dto->serial, Status::CODE_500, "EXISTEN MULTIPLES DISPOSITIVOS, DEBE ESPECIFICAR EL SERIAL DE LA BALANZA");
    for (int i = 0; i < BasculeService::bascules.size(); i++)
    {
      if (BasculeService::bascules[i]->device->type.getValue("") == "balanza")
      {
        OATPP_LOGI("\n\tDispositivo", " \n\t%s: Marca: %s \n\tPuerto: %s \n\tSerial: %s", BasculeService::bascules[i]->device->type.getValue("").c_str(), BasculeService::bascules[i]->device->brand.getValue("").c_str(), BasculeService::bascules[i]->device->port.getValue("").c_str(), BasculeService::bascules[i]->device->serial.getValue("").c_str());
        if (BasculeService::bascules[i]->device->serial.getValue("") == dto->serial.getValue("") && !dto->serial.getValue("").empty())
        {
          sc = BasculeService::bascules[i];
          break;
        }
      }
    }
  }
  return sc;
}