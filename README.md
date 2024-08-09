# oatpp-printer 

## Overview

This project is using the following oatpp modules:

- [oatpp](https://github.com/oatpp/oatpp) 
- [oatpp-libressl](https://github.com/oatpp/oatpp-libressl).


### Project layout

```
|- cert/                                 // folder with test certificates 
|- resources/config.json                 // configuration file with configuration profiles
|- src/
|    |
|    |- controller/                      // Folder containing Controller where all endpoints are declared
|    |- dto/                             // DTOs are declared here
|    |- service/                         // Service business logic classes (PrinterService)
|    |- App.cpp                          // main() is here
|    |- AppComponent.hpp                 // Service configuration is loaded here
|    |- ServiceComponent.hpp             // Service configuration (port, ObjectMapper, Database)
|
|- utility/install-oatpp-modules.sh      // utility script to install required oatpp-modules.
|- CMakeLists.txt                        // projects CMakeLists.txt
|- Dockerfile                            // Dockerfile
|- docker-compose.yaml                   // Docker-compose with this service
```

---

## Requires 

- This requires attached to user group dialout.
- IMPORTANT: After using this command it is necessary to close the session and open it again
```
$ sudo usermod -a -G dialout $USER
```

## To generate new cert and key use:
```
openssl req -newkey rsa:4096 -nodes -keyout key.pem -x509 -days 36500 -out certificate.pem
```

## Build and Run
### Using CMake
script to install required oatpp modules.   
```
$ mkdir build && cd build
$ cmake ..
$ make 
$ ./oatpp-printer-exe  # - run application.
```
