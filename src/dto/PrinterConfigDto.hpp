#ifndef PrinterConfigDto_hpp
#define PrinterConfigDto_hpp

#include "ConfigDto.hpp"

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class FormasPago : public oatpp::DTO
{
  DTO_INIT(FormasPago, DTO)

  DTO_FIELD(oatpp::String, serial, "serial");
  DTO_FIELD(oatpp::String, fp_01, "01");
  DTO_FIELD(oatpp::String, fp_02, "02");
  DTO_FIELD(oatpp::String, fp_03, "03");
  DTO_FIELD(oatpp::String, fp_04, "04");
  DTO_FIELD(oatpp::String, fp_05, "05");
  DTO_FIELD(oatpp::String, fp_06, "06");
  DTO_FIELD(oatpp::String, fp_07, "07");
  DTO_FIELD(oatpp::String, fp_08, "08");
  DTO_FIELD(oatpp::String, fp_09, "09");
  DTO_FIELD(oatpp::String, fp_10, "10");
  DTO_FIELD(oatpp::String, fp_11, "11");
  DTO_FIELD(oatpp::String, fp_12, "12");
  DTO_FIELD(oatpp::String, fp_13, "13");
  DTO_FIELD(oatpp::String, fp_14, "14");
  DTO_FIELD(oatpp::String, fp_15, "15");
  DTO_FIELD(oatpp::String, fp_16, "16");
  DTO_FIELD(oatpp::String, fp_17, "17");
  DTO_FIELD(oatpp::String, fp_18, "18");
  DTO_FIELD(oatpp::String, fp_19, "19");
  DTO_FIELD(oatpp::String, fp_20, "20");
  DTO_FIELD(oatpp::String, fp_21, "21");
  DTO_FIELD(oatpp::String, fp_22, "22");
  DTO_FIELD(oatpp::String, fp_23, "23");
  DTO_FIELD(oatpp::String, fp_24, "24");
};

class Tasas : public oatpp::DTO
{
  DTO_INIT(Tasas, DTO)

  DTO_FIELD(oatpp::String, serial, "serial");
  DTO_FIELD(oatpp::Float32, G, "G");
  DTO_FIELD(oatpp::Float32, R, "R");
  DTO_FIELD(oatpp::Float32, A, "A");
};

class HkaConfig : public oatpp::DTO
{
  DTO_INIT(HkaConfig, DTO)

  DTO_FIELD(Vector<Object<FormasPago>>, formasPago, "formas-pago");
};

class PnpConfig : public oatpp::DTO
{
  DTO_INIT(PnpConfig, DTO)

  DTO_FIELD(Vector<Object<Tasas>>, tasas, "tasas");
};

class PrinterConfig : public oatpp::DTO
{
  DTO_INIT(PrinterConfig, DTO)

  DTO_FIELD(Vector<Object<Device>>, devices, "devices");
  DTO_FIELD(Object<PnpConfig>, pnpConfig, "pnp");
  DTO_FIELD(Object<HkaConfig>, hkaConfig, "hka");
  DTO_FIELD(Object<HkaConfig>, ecrs, "ecrs");
  DTO_FIELD(Object<HkaConfig>, Bematech, "Bematech");

};

#include OATPP_CODEGEN_END(DTO)

#endif /* PrinterConfigDto_hpp */