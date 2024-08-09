#ifndef CryptographyService_hpp
#define CryptographyService_hpp

#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <filesystem>
#include <regex>

#include "cryptopp/base64.h"
#include "cryptopp/modes.h"
#include "cryptopp/aes.h"
#include "cryptopp/filters.h"

#include "cryptopp/files.h"
#include "cryptopp/osrng.h"
#include "cryptopp/hex.h"

#include "../Tools.hpp"
#include "dto.hpp"
#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/web/client/HttpRequestExecutor.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

using nlohmann::json;
using namespace CryptoPP;
class CryptographyService
{
private:
    typedef oatpp::web::protocol::http::Status Status;
    SecByteBlock key;

public:
    void createFolder();
    void init(DatosLicenciaDto::Wrapper dto);
    void drop();
    void update(oatpp::Object<DatosLicenciaDto> dto);
    DatosLicenciaDto::Wrapper readData();
    void writeData(DatosLicenciaDto::Wrapper data);
private:
    std::string decrypt();
    void encrypt(std::string);
public:
    std::string encryptText(std::string plain_data);
    std::string decryptText(std::string);
    void validateDatosLicenciaDto(DatosLicenciaDto::Wrapper dto);
    CryptographyService();
    bool dataExist();
    bool closefile();
};

#endif /* CryptographyService_hpp */
