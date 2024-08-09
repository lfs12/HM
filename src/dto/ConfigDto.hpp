#ifndef ConfigDto_hpp
#define ConfigDto_hpp

#include "PrinterDto.hpp"

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class Client;
class Device;

class WebService : public oatpp::DTO
{
  DTO_INIT(WebService, DTO)

  DTO_FIELD(String, host);
  DTO_FIELD(UInt16, port);
  DTO_FIELD(String, validation);
};

class Device : public oatpp::DTO
{
  DTO_INIT(Device, DTO)

  DTO_FIELD(oatpp::String, brand, "brand");
  DTO_FIELD(oatpp::String, model, "model");
  DTO_FIELD(oatpp::String, serial, "serial");
  DTO_FIELD(oatpp::String, ip, "ip");
  DTO_FIELD(oatpp::String, port, "port");
  DTO_FIELD(oatpp::String, type, "type");
  DTO_FIELD(UInt16, dispositivo, "dispositivo");
};

class DeviceStatus : public oatpp::DTO 
{
 DTO_INIT(DeviceStatus, DTO)

  DTO_FIELD(oatpp::Object<Device>, device, "device");
  DTO_FIELD(oatpp::Boolean, statusBool, "statusBool");
  DTO_FIELD(oatpp::String, status, "status");
};

class DeviceFiscalInfo : public oatpp::DTO 
{
 DTO_INIT(DeviceFiscalInfo, DTO)
  DTO_FIELD(oatpp::String, fechaYhora, "fechaYhora");
  DTO_FIELD(oatpp::String, serial, "serial");
  DTO_FIELD(oatpp::String, rif, "rif");
};



// PageDto
template <class T>
class PageDto : public oatpp::DTO
{
  DTO_INIT(PageDto, DTO)

  DTO_FIELD(UInt32, offset);
  DTO_FIELD(UInt32, limit);
  DTO_FIELD(UInt32, count);
  DTO_FIELD(Vector<T>, items);
};

class PortsPageDto : public PageDto<oatpp::String>
{
  DTO_INIT(PortsPageDto, PageDto<oatpp::String>)
};

class DevicesPageDto : public PageDto<oatpp::Object<Device>>
{
  DTO_INIT(DevicesPageDto, PageDto<oatpp::Object<Device>>)
};


#include OATPP_CODEGEN_END(DTO)

#endif /* ConfigDto_hpp */