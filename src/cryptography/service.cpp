#include "service.hpp"

CryptographyService::CryptographyService()
{
    byte a[] = "lbmzc2lZxFrEkpQxS6JJvyoC5mICnW3U";
    key = SecByteBlock(a, AES::MAX_KEYLENGTH);
}

void CryptographyService::createFolder()
{
    std::filesystem::create_directories(ACT_DATA_PATH);
}

void CryptographyService::init(DatosLicenciaDto::Wrapper dto)
{
    OATPP_LOGV("Init", "%s", Tools::dtoToString(dto).c_str());

    this->writeData(dto);
}

void CryptographyService::drop()
{
    remove(ACT_DATA_PATH ACT_DATA_FILE_NAME);
}

void CryptographyService::update(oatpp::Object<DatosLicenciaDto> dto)
{
    validateDatosLicenciaDto(dto);
    OATPP_ASSERT_HTTP(dataExist(), Status::CODE_500, " NO HA SIDO INICIALIZADO");

    auto data = readData();
    OATPP_ASSERT_HTTP(data, Status::CODE_500, "FALLO AL DECODIFICAR DATA");
    OATPP_ASSERT_HTTP(data->rif.getValue("") == dto->rif.getValue(""), Status::CODE_500, "RIF NO PUEDE SER ACTUALIZADO");
    this->writeData(dto);
}

bool CryptographyService::dataExist()
{
    std::ifstream file(ACT_DATA_PATH ACT_DATA_FILE_NAME);
    return file.good();
}

DatosLicenciaDto::Wrapper CryptographyService::readData()
{
    DatosLicenciaDto::Wrapper data;
    try
    {
        auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
        std::ifstream file(ACT_DATA_PATH ACT_DATA_FILE_NAME);

        data = objectMapper->readFromString<oatpp::Object<DatosLicenciaDto>>(decrypt());
    }
    catch (const std::exception &e)
    {
        OATPP_LOGE("readData", "%s", e.what());
    }
    return data;
}

void CryptographyService::writeData(DatosLicenciaDto::Wrapper data)
{
    try
    {
        std::string dataString = Tools::dtoToString(data);
        encrypt(dataString);
    }
    catch (const std::exception &e)
    {
        OATPP_LOGE("writeData", "%s", e.what());
    }
}

std::string CryptographyService::decrypt()
{
    std::string recovered;
    try
    {
        std::ifstream in{ACT_DATA_PATH ACT_DATA_FILE_NAME, std::ios::binary};
        std::string ivDecoded, base64Decoded;

        CryptoPP::FileSource base64{in, true,
                                        new Base64Decoder(
                                            new StringSink(base64Decoded))};

        CryptoPP::StringSource ivSource{base64Decoded, false,
                                            new StringSink(ivDecoded)};
        ivSource.Pump(AES::BLOCKSIZE);
        CryptoPP::byte *rawIv = (CryptoPP::byte *)ivDecoded.data();

        SecByteBlock iv(rawIv, AES::BLOCKSIZE);
  
        CBC_Mode<AES>::Decryption decryption;
        decryption.SetKeyWithIV(key, key.size(), iv);

        CryptoPP::StringSource source{base64Decoded.substr(AES::BLOCKSIZE), true,
                                          new CryptoPP::StreamTransformationFilter{
                                              decryption, new CryptoPP::StringSink{recovered}}};
    }
    catch (const Exception &e)
    {
        OATPP_LOGE("DECRYPT", "%s", e.what());
    }
    OATPP_LOGI("DECRYPT", "%s", recovered.c_str());

    return recovered;
}

std::string CryptographyService::decryptText(std::string ciphertext)
{
    std::string recovered;
    try
    {
        std::string ivDecoded, base64Decoded;

        CryptoPP::StringSource base64{ciphertext, true,
                                        new Base64Decoder(
                                            new StringSink(base64Decoded))};

        CryptoPP::StringSource ivSource{base64Decoded, false,
                                            new StringSink(ivDecoded)};
        ivSource.Pump(AES::BLOCKSIZE);
        CryptoPP::byte *rawIv = (CryptoPP::byte *)ivDecoded.data();

        SecByteBlock iv(rawIv, AES::BLOCKSIZE);
  

        CBC_Mode<AES>::Decryption decryption;
        decryption.SetKeyWithIV(key, key.size(), iv);

        CryptoPP::StringSource source{base64Decoded.substr(AES::BLOCKSIZE), true,
                                          new CryptoPP::StreamTransformationFilter{
                                              decryption, new CryptoPP::StringSink{recovered}}};
    }
    catch (const Exception &e)
    {
        OATPP_LOGE("DECRYPT", "%s", e.what());
    }
    OATPP_LOGI("DECRYPT", "%s", recovered.c_str());

    return recovered;
}

void CryptographyService::encrypt(std::string plain_data)
{
    std::ofstream out{ACT_DATA_PATH ACT_DATA_FILE_NAME, std::ios::binary};

    AutoSeededRandomPool prng;
    SecByteBlock iv = SecByteBlock(AES::BLOCKSIZE);
    prng.GenerateBlock(iv, iv.size());

    try
    {
        CBC_Mode<AES>::Encryption cipher;
        cipher.SetKeyWithIV(key, key.size(), iv);
        CryptoPP::FileSink sink{out};

        std::string ivCiphertext;
        CryptoPP::StringSink ss(ivCiphertext);

        CryptoPP::ArraySource(iv, AES::BLOCKSIZE, true,
                              new CryptoPP::Redirector(ss));

        StringSource s(plain_data,
                       true,
                       new CryptoPP::StreamTransformationFilter{
                           cipher,
                           new CryptoPP::Redirector(ss)});

        StringSource source(ivCiphertext,
                            true,
                            new CryptoPP::Base64Encoder(
                                new CryptoPP::Redirector(sink), false));
    }
    catch (const Exception &e)
    {
        OATPP_LOGE("ENCRYPT", "%s", e.what());
    }
}

std::string CryptographyService::encryptText(std::string plain_data)
{
    std::string out;

    AutoSeededRandomPool prng;
    SecByteBlock iv = SecByteBlock(AES::BLOCKSIZE);
    prng.GenerateBlock(iv, iv.size());

    try
    {
        CBC_Mode<AES>::Encryption cipher;
        cipher.SetKeyWithIV(key, key.size(), iv);
        CryptoPP::StringSink sink{out};

        std::string ivCiphertext;
        CryptoPP::StringSink ss(ivCiphertext);

        CryptoPP::ArraySource(iv, AES::BLOCKSIZE, true,
                              new CryptoPP::Redirector(ss));

        StringSource s(plain_data,
                       true,
                       new CryptoPP::StreamTransformationFilter{
                           cipher,
                           new CryptoPP::Redirector(ss)});

        StringSource source(ivCiphertext,
                            true,
                            new CryptoPP::Base64Encoder(
                                new CryptoPP::Redirector(sink), false));
    }
    catch (const Exception &e)
    {
        OATPP_LOGE("ENCRYPT TEXT", "%s", e.what());
    }
    return out;
}

void CryptographyService::validateDatosLicenciaDto(DatosLicenciaDto::Wrapper dto)
{
    OATPP_ASSERT_HTTP(dto, Status::CODE_500, "DTO :: INVALIDO");

    OATPP_ASSERT_HTTP(dto->licencia, Status::CODE_500, "LICENCIA :: INVALIDO");
    OATPP_ASSERT_HTTP(dto->razon_social, Status::CODE_500, "RAZON_SOCIAL :: INVALIDO");
    OATPP_ASSERT_HTTP(dto->rif, Status::CODE_500, "RIF :: INVALIDO");
    OATPP_ASSERT_HTTP(dto->serial, Status::CODE_500, "SERIAL :: INVALIDO");
    OATPP_ASSERT_HTTP(dto->fecha_inicio, Status::CODE_500, "FECHA DE INICIO :: INVALIDO");
    OATPP_ASSERT_HTTP(dto->fecha_vencimiento, Status::CODE_500, "FECHA DE VENCIMIENTO :: INVALIDO");
}

bool CryptographyService::closefile(){
    std::ifstream file(ACT_DATA_PATH ACT_DATA_FILE_NAME);
    file.close();
    return file.is_open();
}