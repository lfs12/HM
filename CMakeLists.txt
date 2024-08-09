cmake_minimum_required(VERSION 3.1)

set(project_name hardware-manager)
set(CMAKE_CXX_STANDARD 17)

project(${project_name})

if(MSVC)
  add_compile_options(/bigobj)
endif()

add_library(${project_name}-lib
        src/service/SerialConnection.cpp
        
        src/service/AppService.cpp
        src/service/PrinterService.cpp
        src/service/BasculeService.cpp
        src/service/ComandaService.cpp

        src/service/Bascule/HkaBascule.cpp

        src/service/Printer/PrinterInterface.cpp
        src/service/Printer/BematechPrinter.cpp
        src/service/Printer/EcrsPrinter.cpp
        src/service/Printer/HkaPrinter.cpp
        src/service/Printer/NetsoftPrinter.cpp
        src/service/Printer/PnpPrinter.cpp
        src/service/Printer/VmaxPrinter.cpp
        src/service/Printer/SewooPrinter.cpp
        src/lib/rs232.cpp

        src/Tools.cpp
        SRC/Tools.hpp

        #START licencia
        src/cryptography/Dto.hpp
        src/cryptography/service.hpp
        src/cryptography/service.cpp
        src/licencia.cpp
        src/licencia.hpp
        lib_ibpp/lib/ibpp/all_in_one.cpp
        lib_ibpp/lib/HTTPRequest.hpp
        #END licencia
)

target_include_directories(${project_name}-lib PUBLIC src)

## link libs
include_directories("C:/Program Files/cryptopp/include/")
link_directories("C:/Program Files/cryptopp/lib")

find_package(oatpp         1.3.0 REQUIRED)
find_package(oatpp-openssl 1.3.0 REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(OpenSSL             REQUIRED)
find_package(CryptoPP REQUIRED)
find_package(CURL REQUIRED)

target_link_libraries(${project_name}-lib
        # Oat++ libraries
        PUBLIC oatpp::oatpp
        PUBLIC oatpp::oatpp-openssl

        # Extra libraries
        PUBLIC nlohmann_json::nlohmann_json
        
        # Winsock library
        PRIVATE ws2_32
        
        # Incluye la librería curl
        PUBLIC CURL::libcurl  

        #incluye Crypto
        PUBLIC OpenSSL::SSL
        PUBLIC OpenSSL::Crypto
        PUBLIC cryptopp
)

if(CMAKE_SYSTEM_NAME MATCHES Linux)
    find_package(Threads REQUIRED)
    target_link_libraries(${project_name}-lib INTERFACE Threads::Threads ${CMAKE_DL_LIBS})
endif()

#################################################################
## define certificates path
set(ACT_DATA_PATH "resources/")
set(ACT_DATA_FILE_NAME "licencia.txt")

add_definitions(
        -DCERT_PEM_PATH="cert/key.pem"
        -DCERT_CRT_PATH="cert/cert.crt"
        -DCONFIG_PATH="resources/config.json"
        -DPRINTER_CONFIG_PATH="resources/printer_config.json"

        -DACT_DATA_PATH="${ACT_DATA_PATH}"
        -DACT_DATA_FILE_NAME="${ACT_DATA_FILE_NAME}"
)
message("Ruta del archivo de licencia: ${ACT_DATA_FILE_NAME}")
IF (WIN32)
    add_definitions(-DIBPP_WINDOWS)
    set(CMAKE_CXX_FLAGS "/bigobj /EHsc" CACHE INTERNAL "")
#     add_compile_options(/bigobj)
ELSE()
    add_definitions(-DIBPP_LINUX)
    target_link_libraries(${project_name}-lib PUBLIC fbclient)
ENDIF()
#################################################################
## add executables

add_executable(${project_name}
        src/App.cpp
)
target_link_libraries(${project_name} ${project_name}-lib)
add_dependencies(${project_name} ${project_name}-lib)


#################################################################
## copy resources
add_custom_command(TARGET ${project_name} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/cert/ $<TARGET_FILE_DIR:${project_name}>/cert)
add_custom_command(TARGET ${project_name} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources/ $<TARGET_FILE_DIR:${project_name}>/resources)
