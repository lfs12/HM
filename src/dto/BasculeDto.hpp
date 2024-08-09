#ifndef BasculeDto_hpp
#define BasculeDto_hpp

#include "ConfigDto.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class Peso : public oatpp::DTO
{
    DTO_INIT(Peso, DTO)

    DTO_FIELD(String, peso, "peso");
    DTO_FIELD(Boolean, estable, "estable");
    DTO_FIELD(String, precio, "precio");
    DTO_FIELD(String, total, "total");
};

class BasculeDto : public oatpp::DTO
{
    DTO_INIT(BasculeDto, DTO)

    DTO_FIELD(String, cmd, "cmd");
    DTO_FIELD(String, serial, "serial");
};

#include OATPP_CODEGEN_END(DTO)

#endif /* BasculeDto_hpp */
