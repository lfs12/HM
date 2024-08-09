# resources\printer_config.json
*Nuevo campo llamado "ip"
**Debe tener "" *SIN FALTA*

# src\dto\ConfigDto.hpp
*Nuevo campo llamado "ip" de tipo String en el objeto 'Device'

# Implementación de nueva impresora 
src\service\Printer\EcrsPrinter.cpp
src\service\Printer\EcrsPrinter.hpp

# AÑADIDO EN EL CMAKE
src\service\Printer\EcrsPrinter.hpp

# Inclusión en src\service\PrinterService.hpp
#include "Printer/EcrsPrinter.hpp"

# src\service\AppService.cpp
*Nueva condicion en las funciones CheckDevices, addDevices y checkFiscalInfo

# src\service\PrinterService.cpp && src\service\PrinterService.hpp
*Nueva funcion para validar el monto total.

# CMakeLists.txt
*Nuevo módulo añadido: CURL (PARA PETICIONES, API externas al HardwareManager)

# src\dto\PrinterConfigDto.hpp
*Nuevo campo añadido 'ecrs'

# src\dto\PrinterDto.hpp
*Nuevo campo añadido 'linea_comentario'





