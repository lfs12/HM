#ifndef Cryptography_Dto_HPP
#define Cryptography_Dto_HPP

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)



class DatosLicenciaDto : public oatpp::DTO
{
  DTO_INIT(DatosLicenciaDto, DTO)
  
  DTO_FIELD(String, rif, "rif");
  DTO_FIELD(String, razon_social, "razon_social");
  DTO_FIELD(String, licencia, "licencia");
  DTO_FIELD(String, serial, "serial");
  DTO_FIELD(String, fecha_inicio, "fecha_inicio");
  DTO_FIELD(String, fecha_vencimiento, "fecha_vencimiento");
};

#include OATPP_CODEGEN_END(DTO)

#endif /* Cryptography_Dto_HPPP */
