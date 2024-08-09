#include "licencia.hpp"
int LicenciaService::appStatus = 3;
std::string LicenciaService::licencia;
using json = nlohmann::json;

//UTILS
long long dtomil(std::string fecha){
    OATPP_LOGI("DTOMIL","FECHA DADA %s", fecha.c_str());
    std::tm time = {};
    time.tm_year = std::stoi(fecha.substr(0,4)) - 1900; // Ajustar el año
    time.tm_mon = std::stoi(fecha.substr(5,2)) - 1; // Ajustar el mes
    time.tm_mday = std::stoi(fecha.substr(8,2));
    std::time_t timeSinceEpoch = std::mktime(&time);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::from_time_t(timeSinceEpoch).time_since_epoch());
    return milliseconds.count();
}

size_t write_data(void *ptr, size_t size, size_t nmemb, std::string *data) {
    data->append((char *)ptr, size * nmemb);
    return size * nmemb;
}
//END UTILS

void LicenciaService::validar_licencia(){
    OATPP_LOGI("VALIDACION DE LICENCIA", "[Validando licencia...]");
    try{
        if(imp_cryptography.dataExist()){
            auto data = (imp_cryptography.readData() != nullptr) ?  imp_cryptography.readData():nullptr;
            if(onlineValidation(data)) OATPP_LOGI("", "LICENCIA ACTIVADA");
        }else{
            OATPP_LOGE("", "Archivo de licencia no existe!");
            if(demoValidation()){
                appStatus = LicenciaService::demo;
            }else{
                appStatus = LicenciaService::verificacion_fallida;
            }
        }
    }catch(const std::exception &e){
        if(appStatus == LicenciaStatus::vencido){ //F.V DE LICENCIA ONLINE ES IGUAL A LA DEL ARCHIVO
            OATPP_LOGE("", "LICENCIA VENCIDA");
        }else if(appStatus == LicenciaStatus::verificacion_fallida){ //DATA LICENCIA ONLINE NO COINCIDE CON LA DEL ARCHIVO
            OATPP_LOGE("", "VERIFICACION FALLIDA");
        }else if(appStatus == LicenciaService::pendiente_activacion){ //SIN CONEXION A INTERNET
            if(!offlineValidation()) OATPP_LOGE("", "VERIFICACION FALLIDA");
        }
    }

    OATPP_LOGI("VALIDACION DE LICENCIA", "[COMPLETADA]"); 
}

bool LicenciaService::onlineValidation(DatosLicenciaDto::Wrapper &datos){ 
    OATPP_LOGI("Validation ONLINE"," [STARTING... ]");
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl)
    {
        std::string body = imp_cryptography.encryptText(Tools::dtoToString(datos));
        std::string response;
        std::cout << "------------------------------------\n"
                  << body << "\n------------------------------------\n";           
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: text/plain");
        curl_easy_setopt(curl, CURLOPT_URL, imp_cryptography.decryptText(config->validation->c_str()).c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);

        if (res == CURLE_OK)
        {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            if (response_code == 200)
            {
                    // Comprobar errores
                    if (res != CURLE_OK) {
                        std::cerr << "Error al realizar la solicitud: " << curl_easy_strerror(res) << std::endl;
                    } else {
                        // Imprimir el cuerpo de la respuesta
                        std::cout << "\n++++++++++++++++++ INFO RESPONSE DATA ++++++++++++++++++\n"<< response << "\n++++++++++++++++++ INFO RESPONSE DATA ++++++++++++++++++\n";
                        if(response == "") return "";
                        auto objText = imp_cryptography.decryptText(response);
                        auto jsonObjectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
                        auto obj = jsonObjectMapper->readFromString<DatosLicenciaDto::Wrapper>(objText);
                        //Start Validate
                        if(imp_cryptography.dataExist()){
                          //Start data file
                          auto data = (imp_cryptography.readData() != nullptr) ?  imp_cryptography.readData():nullptr;
                          if(data!=nullptr){
                                  auto rif_f = data->rif.getValue("");
                                  auto razon_social_f = data->razon_social.getValue("");
                                  auto serial_f = data->serial.getValue("");
                                  auto fecha_f = data->fecha_vencimiento.getValue("");
                                  //End data file
                                  if(obj->rif.getValue("")!="" && rif_f!="" && 
                                     obj->razon_social.getValue("")!="" && razon_social_f!="" &&
                                     obj->serial.getValue("")!=""&&serial_f!="" && 
                                     obj->fecha_vencimiento.getValue("")!="" && fecha_f!=""){
                                    
                                    if( dtomil(obj->fecha_vencimiento->substr(0,10)) == dtomil(fecha_f) ){ 
                                        if(obj->rif == rif_f && obj->serial == serial_f && obj->razon_social == razon_social_f){
                                            OATPP_LOGI("LICENCIA"," [Cumplio con las condiciones legales]");
                                            appStatus = LicenciaStatus::activado;
                                            return true;
                                        }else{
                                            OATPP_LOGI("DATAFILE"," [Data de archivo invalida]");
                                            appStatus = LicenciaStatus::verificacion_fallida;
                                            OATPP_LOGI("DATAFILE","Modificando licencia...");       
                                            auto licencia = DatosLicenciaDto::createShared();
                                            licencia->rif = obj->rif;
                                            licencia->razon_social = obj->razon_social;
                                            licencia->serial = obj->serial;
                                            licencia->licencia = obj->licencia;
                                            licencia->fecha_inicio = obj->fecha_inicio;
                                            licencia->fecha_vencimiento = obj->fecha_vencimiento;
                                            imp_cryptography.update(licencia);
                                            OATPP_LOGI("DATAFILE","Licencia Modificada [AHORA ES LEGAL]"); 
                                            return false; 
                                        } 
                                    }else if(dtomil(obj->fecha_vencimiento->substr(0,10)) > dtomil(fecha_f)){
                                        OATPP_LOGI("LICENCIA RENOVADA", "[Iniciando operacion con archivo de licencia...]");
                                            OATPP_LOGI("DATAFILE", "Eliminando archivo [Vieja licencia]...");
                                            imp_cryptography.drop();
                                            OATPP_LOGI("DATAFILE", "Archivo [Vieja licencia] eliminado");
                                            auto licencia = DatosLicenciaDto::createShared();
                                            licencia->rif = obj->rif;
                                            licencia->razon_social = obj->razon_social;
                                            licencia->serial = obj->serial;
                                            licencia->licencia = obj->licencia;
                                            licencia->fecha_inicio = obj->fecha_inicio;
                                            licencia->fecha_vencimiento = obj->fecha_vencimiento;
                                            OATPP_LOGI("DATAFILE", "Creando archivo [Nueva licencia]...");
                                            imp_cryptography.createFolder();
                                            imp_cryptography.init(licencia);
                                            OATPP_LOGI("DATAFILE", "Archivo creado [Nueva licencia]");
                                        OATPP_LOGI("LICENCIA RENOVADA", "[Operacion completada]");
                                        appStatus = LicenciaStatus::verificacion_fallida;
                                        return false;
                                    }else{
                                        OATPP_LOGE("ERROR", "[LA DATA HA SIDO CAMBIADA - LIC ILEGAL]");
                                        appStatus = LicenciaStatus::verificacion_fallida_data;
                                        return false;
                                    }
                                  }else{
                                    appStatus = LicenciaStatus::verificacion_fallida;
                                    return false;
                                  }
                            }else{
                                OATPP_LOGE("", "Archivo de licencia no tiene data!");
                                if(demoValidation()){
                                    appStatus = LicenciaService::demo;
                                    return true;
                                }else{
                                    appStatus = LicenciaService::verificacion_fallida;
                                    return false;
                                }           
                            }
                        }
                        //End Validate
                    }
                    // Limpiar
                    curl_easy_cleanup(curl);
            }else if(response_code == 404){
                OATPP_LOGE("ERROR", " Licencia no encontrada");
                appStatus = LicenciaService::verificacion_fallida;
                throw std::invalid_argument("ERROR VALIDATE LICENCIA ONLINE");
            }else if(response_code == 409){
                OATPP_LOGE("ERROR", " LICENCIA VENCIDA");
                appStatus = LicenciaService::vencido;
                throw std::invalid_argument("ERROR VALIDATE LICENCIA ONLINE");
            }
            else
            {
                OATPP_LOGE("ERROR", " HTTP CODE %ld", response_code);
                appStatus = LicenciaService::pendiente_activacion;
                throw std::invalid_argument("OFFLINE");
            }
        }
        else
        {
            OATPP_LOGE("ERROR", "La solicitud online no se realizó correctamente");
            appStatus = LicenciaService::pendiente_activacion;
            throw std::invalid_argument("OFFLINE");
        }

        curl_easy_cleanup(curl);
        OATPP_LOGI("Validation ONLINE"," [FINISHED]","");
    }
    else
    {
        OATPP_LOGE("ERROR", "Inicialización de curl fallo");
        appStatus = LicenciaService::pendiente_activacion;
        throw std::invalid_argument("OFFLINE");
    }
}

bool LicenciaService::offlineValidation()
{
    appStatus = -1;
    OATPP_LOGI("Validation OFFLINE"," [STARTING... ]");
    bool status = true;
    DeviceFiscalInfo::Wrapper res;

    //Start data print
    res = app_service.getFiscalInfo();
    auto rif = res->rif.getValue("");
    auto serial = res->serial.getValue("");
    auto fecha_hora = res->fechaYhora.getValue("");
    auto fecha = (fecha_hora != "") ? fecha_hora.substr(0, 10) : "";
    for (char& c : fecha) {
        if (c == '/') {
            c = '-';
        }
    }
    //End data print
    
    //Start Validate
    if(imp_cryptography.dataExist()){
      //Start data file
      auto data = (imp_cryptography.readData() != nullptr) ?  imp_cryptography.readData():nullptr;
      if(data!=nullptr){
              auto rif_f = data->rif.getValue("");
              auto serial_f = data->serial.getValue("");
        
              auto fecha_f = data->fecha_vencimiento.getValue("").substr(0,10);
              //End data file
              if(rif!=""&&rif_f!=""&&serial!=""&&serial_f!=""&&fecha!=""&&fecha_f!=""){

                if( dtomil(fecha) < dtomil(fecha_f) ){ 

                    if(rif == rif_f && serial == serial_f){
                        OATPP_LOGI("LICENCIA"," [DATOS LEGALES]");
                        appStatus = LicenciaStatus::activado;
                        if(
                          ((dtomil(fecha) + 2629800000) == dtomil(fecha_f)) || ((dtomil(fecha) + 2160000000) == dtomil(fecha_f)) || 
                          ((dtomil(fecha) + 1728000000) == dtomil(fecha_f)) || ((dtomil(fecha) + 1296000000) == dtomil(fecha_f)) ||
                          ((dtomil(fecha) + 864000000) == dtomil(fecha_f)) ||  ((dtomil(fecha) + 432000000) == dtomil(fecha_f))  ||
                          ((dtomil(fecha) + 345600000) == dtomil(fecha_f)) ||  ((dtomil(fecha) + 259200000) == dtomil(fecha_f)) ||
                          ((dtomil(fecha) + 172800000) == dtomil(fecha_f)) ||  ((dtomil(fecha) + 86400000) == dtomil(fecha_f))
                          ){
                                appStatus = LicenciaStatus::por_vencerse;
                           }
                    }else{
                        OATPP_LOGE("LICENCIA"," [Data de archivo invalida]");
                        appStatus = LicenciaStatus::verificacion_fallida;
                        status = false;
                    } 

                }else if( dtomil(fecha) >= dtomil(fecha_f) ){

                    OATPP_LOGE("LICENCIA"," [ VENCIDA ]");
                    appStatus = LicenciaStatus::vencido;

                }else{

                    OATPP_LOGE("LICENCIA"," [ERROR AL PROCESAR FECHAS]");
                    appStatus = LicenciaStatus::verificacion_fallida;
                    status = false;
                }
              }else{
                appStatus = LicenciaStatus::verificacion_fallida;
                status = false;
              }
        }else{
            appStatus = LicenciaStatus::verificacion_fallida;
            status = false;           
        }
    }
    //End Validate


    OATPP_LOGI("Validation OFFLINE"," [FINISHED]","");
    return status;
}

bool LicenciaService::demoValidation(){
    OATPP_LOGI("Validation DEMO"," [STARTING... ]");
    auto active = false;
    //GET DATA PRINTER
    DeviceFiscalInfo::Wrapper resPrinter;
    resPrinter = app_service.getFiscalInfo();
    auto rif = resPrinter->rif.getValue("");
    auto serial = resPrinter->serial.getValue("");
    //END GET_DATA_PRINTER

    if(rif!="" && serial!=""){
        //CREATE OBJ LICENCE DEMO
        auto licencia = DatosLicenciaDto::createShared();
        licencia->rif = rif;
        licencia->razon_social = "DEMO";
        licencia->serial = serial;
        licencia->licencia = "DEMO";
        licencia->fecha_inicio = "DEMO";
        licencia->fecha_vencimiento = "DEMO";
        //END _CREATE_OBJ_LICENCE_DEMO
    
        //CONNET DB CLOUD
        CURL *curl;
        CURLcode res;
        curl = curl_easy_init();
        
        if (curl)
        {
            std::string body = imp_cryptography.encryptText(Tools::dtoToString(licencia));
            std::cout << "------------------------------------\n"<< body << "\n------------------------------------\n";
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Content-Type: text/plain");
            std::string response;
            curl_easy_setopt(curl, CURLOPT_URL, imp_cryptography.decryptText(config->validation->c_str()).c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    
            res = curl_easy_perform(curl);
    
            std::cout << "------------------------------------DATA ENCRIPTADA. INIT load cloud------------------------------------\n";
            if (res == CURLE_OK)
            {
                long response_code;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
                if (response_code == 200)
                {
                    // Comprobar errores
                    if (res != CURLE_OK) {
                        std::cerr << "Error al realizar la solicitud: " << curl_easy_strerror(res) << std::endl;
                    } else {
                        // Imprimir el cuerpo de la respuesta
                        std::cout << "\n++++++++++++++++++ INFO RESPONSE DATA ++++++++++++++++++\n"<< response << "\n++++++++++++++++++ INFO RESPONSE DATA ++++++++++++++++++\n";
                        if(response == "") return "";
                        auto objText = imp_cryptography.decryptText(response);
                        auto jsonObjectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
                        auto obj = jsonObjectMapper->readFromString<DatosLicenciaDto::Wrapper>(objText);
                        
                        OATPP_LOGI("DEMO", "Creando Archivo para una DEMO...");
                        auto licencia = DatosLicenciaDto::createShared();
                        licencia->rif = obj->rif;
                        licencia->razon_social = obj->razon_social;
                        licencia->serial = obj->serial;
                        licencia->licencia = obj->licencia;
                        licencia->fecha_inicio = obj->fecha_inicio;
                        licencia->fecha_vencimiento = obj->fecha_vencimiento;
                        imp_cryptography.createFolder();
                        imp_cryptography.init(licencia);
                        OATPP_LOGI("DATAFILE", "Archivo creado [DEMO]");
                       appStatus = LicenciaStatus::demo; 
                       active = true;
                    }
                }
                else{
                    std::cerr << "Error al realizar la solicitud 'response_code != 200': " << curl_easy_strerror(res) << std::endl;
                    appStatus = LicenciaStatus::verificacion_fallida;
                }
            }else{
                std::cerr << "Error al realizar la solicitud 'res != CURLE_OK': " << curl_easy_strerror(res) << std::endl;
                appStatus = LicenciaStatus::verificacion_fallida;
            }
        }else{
           appStatus = LicenciaStatus::verificacion_fallida; 
        }
    }else{
        appStatus = LicenciaStatus::verificacion_fallida; 
    }
    return active;
}


// //THIS IS TEST SECTION

// std::string leerArchivoJSON(const std::string& nombreArchivo) {
//     // Obtener la ruta del directorio actual del archivo fuente
//     std::string directorioActual = __FILE__;
//     size_t found = directorioActual.find_last_of("/\\");
//     std::string rutaDirectorio = directorioActual.substr(0, found);

//     // Construir la ruta completa al archivo JSON en el directorio "resources"
//     std::string rutaCompleta = rutaDirectorio + "/../resources/" + nombreArchivo;

//     std::ifstream archivo(rutaCompleta);
//     if (!archivo.is_open()) {
//         std::cerr << "Error al abrir el archivo: " << rutaCompleta << std::endl;
//         return ""; // Devolver una cadena vacía en caso de error
//     }

//     std::stringstream contenidoStream;
//     contenidoStream << archivo.rdbuf();
//     std::string contenido = contenidoStream.str();

//     if (contenido.empty()) {
//         std::cerr << "El archivo " << rutaCompleta << " está vacío." << std::endl;
//     }

//     return contenido;
// }

// void LicenciaService::validar_licencia_test(){
//   OATPP_LOGI("VALIDAR LICENCIA MOD"," [Validando licencia... ]");
//     auto res = app_service.getFiscalInfo();
//     auto rif = res->rif.getValue("");
//     auto serial = res->serial.getValue("");
//     auto date = res->fechaYhora.getValue("");
//     OATPP_LOGI("RIF TESTING", "======================HERE===%s===HERE=====================", rif.c_str());
//     OATPP_LOGI("SERIAL TESTING", "======================HERE===%s===HERE=====================", serial.c_str());
//     OATPP_LOGI("FECHA Y HORA TESTING", "======================HERE===%s===HERE=====================", date.c_str());
//     // Check if the license file exists
//     if (imp_cryptography.dataExist()) {
//         OATPP_LOGD("ARCHIVO 'licencia.txt'", "-[EXISTE]-");
//         auto data = imp_cryptography.readData();
//         auto rif_data_file = data->rif.getValue("IS NULL");
//         OATPP_LOGI("RIF DATA FILE TESTING", "======================HERE===%s===HERE=====================", rif_data_file.c_str()); 
//     }
   
// }

// void test(){
      //       //DATA FILE
      //     std::string rif = nullptr;
      //     std::string razon_social = nullptr;
      //     std::string licencia = nullptr;
       
       
      //    // Leer el archivo JSON
      //    std::string contenidoJSON = leerArchivoJSON("licencia.json");
      //    OATPP_LOGI("Lectura de archivo 'licencia.json'","---[completado]---");
       
      //    // Analizar el contenido JSON
      //    json j = json::parse(contenidoJSON);
      //    OATPP_LOGI("Analisis de archivo 'licencia.json'","---[completado]---");
       
      //  // Iterar sobre los objetos del array
      //  for (const auto& obj : j) {
      //      std::string rif = obj["rif"];
      //      std::string razon_social = obj["razon_social"];
      //      std::string licencia = obj["licencia"];
       
      //  }
      //   OATPP_LOGI("Iteracion sobre los objetos del archivo 'licencia.json'","---[Completada]---"); 
    //       std::tm time = {};
    // time.tm_year = std::stoi(fecha.substr(0,4));
    // time.tm_mon = std::stoi(fecha.substr(4,2));
    // time.tm_mday = std::stoi(fecha.substr(6));
    // std::time_t timeSinceEpoch = std::mktime(&time);
    // auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::from_time_t(timeSinceEpoch).time_since_epoch());

                // std::string response;
                // //TESTING DATA RESPONSE
                //     curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8000/api/licencias/hardware_manager/validar");

                //     // Configurar la función de escritura de datos
                //     curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
                //     curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            
                //     // Realizar la solicitud
                //     res = curl_easy_perform(curl);
            
                //     // Comprobar errores
                //     if (res != CURLE_OK) {
                //         std::cerr << "Error al realizar la solicitud: " << curl_easy_strerror(res) << std::endl;
                //     } else {
                //         // Imprimir el cuerpo de la respuesta
                //         std::cout << "\n++++++++++++++++++ INFO RESPONSE DATA ++++++++++++++++++\n"<< response << "\n++++++++++++++++++ INFO RESPONSE DATA ++++++++++++++++++\n";
                //     }

                //     // Limpiar
                //     curl_easy_cleanup(curl);
                // //END TEST DATA RESPONSE

                // // std::string response_data;
                // // auto res_body =curl_easy_getinfo(curl,  CURLINFO_RESPONSE_CODE, &response_data);
                // // std::cout << "\n++++++++++++++++++ INFO RESPONSE DATA ++++++++++++++++++\n"<< response_data << "\n++++++++++++++++++ INFO RESPONSE DATA ++++++++++++++++++\n";
                // // auto objText = imp_cryptography.decryptText(response_data);
                // // auto jsonObjectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
                // // auto obj = jsonObjectMapper->readFromString<DatosLicenciaDto::Wrapper>(objText);
                // // OATPP_LOGI("TESTING ONLINE VALIDATION", "SUCCESS EJ. %s", obj->serial);
                // std::string directorioActual = __FILE__;
                // size_t found = directorioActual.find_last_of("/\\");
                //          std::string rutaDirectorio = directorioActual.substr(0, found);
                // std::string rutaCompleta = rutaDirectorio + "/../resources/licencia.txt";
                // std::string file =  rutaCompleta;
                // if (remove(file.c_str()) != 0) {
                //     perror("Error al eliminar el archivo");
                //     printf("Error number: %d\n", errno);
                // } else {
                //     printf("El archivo fue eliminado correctamente");
                //                             }
// }