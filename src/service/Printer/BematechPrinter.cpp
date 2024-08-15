#include "BematechPrinter.hpp"

//UTILS
std::vector<std::string> dividirCadenaTotalParcial(const std::string &input) {  
    std::vector<std::string> trozos;  
    std::string trozo;  

    // Omite los primeros dos caracteres y los últimos cuatro  
    size_t start = 2; // Inicia desde el tercer caracter  
    size_t end = input.length() - 4; // Termina cuatro caracteres antes del final  

    // Copiamos la subcadena que nos interesa  
    std::string subcadena = input.substr(start, end - start);  

    // Eliminamos los espacios en blanco  
    subcadena.erase(std::remove(subcadena.begin(), subcadena.end(), ' '), subcadena.end());  

    // Dividimos el string resultante en trozos de 14 caracteres  
    for (size_t i = 0; i < subcadena.length(); i += 14) {  
        trozo = subcadena.substr(i, 14);  
        if (!trozo.empty()) {  
            trozos.push_back(trozo); // Añadimos trozos no vacíos  
        }  
    }  

    return trozos;  
}
int countDigits(int number) {  
    if (number == 0) return 1; // Para el caso especial de 0  
    int count = 0;  
    while (number != 0) {  
        number /= 10;  
        count++;  
    } 
    return count;  
} 
std::vector<std::string> BematechPrinter::splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}
std::string convertirFechaYHora(const std::string& fecha, const std::string& hora) {  
    // Se espera que la fecha esté en formato "YYYY-MM-DD"  
    // y la hora en formato "HH:MM:SS".  

    // Extraer el día, mes, y año de la fecha  
    std::string anio = fecha.substr(2, 2);  // YY  
    std::string mes = fecha.substr(5, 2);   // MM  
    std::string dia = fecha.substr(8, 2);   // DD  

    // Extraer las horas, minutos y segundos de la hora  
    std::string horas = hora.substr(0, 2);   // HH  
    std::string minutos = hora.substr(3, 2);  // MM  
    std::string segundos = hora.substr(6, 2); // SS  

    // Formar el nuevo formato  
    std::string resultado = dia + mes + anio + horas + minutos + segundos; // DDMMYYHHMMSS  

    return resultado; // Resultado final  
}
std::string convertirFechaYHora2(const std::string& fecha, const std::string& hora) {  
    // Se espera que la fecha esté en formato "YYYY-MM-DD"  
    // y la hora en formato "HH:MM:SS".  

    // Extraer el día, mes, y año de la fecha  
    std::string anio = fecha.substr(2, 2);  // YY  
    std::string mes = fecha.substr(5, 2);   // MM  
    std::string dia = fecha.substr(8, 2);   // DD  

    // Extraer las horas, minutos y segundos de la hora  
    std::string horas = hora.substr(0, 2);   // HH  
    std::string minutos = hora.substr(3, 2);  // MM  
    // std::string segundos = hora.substr(6, 2); // SS  
    std::string segundos = "00"; // SS  
    // Formar el nuevo formato  
    std::string resultado = dia + mes + anio + horas + minutos + segundos; // DDMMYYHHMMSS  

    return resultado; // Resultado final  
}
std::string vectorToString(const std::vector<int> &vec) {  
    std::ostringstream oss; // Usamos ostringstream para construir la cadena  
    
    for (size_t i = 0; i < vec.size(); ++i) {  
        oss << vec[i]; // Convierte el entero a string y lo agrega al ostringstream  
        if (i != vec.size() - 1) {  
            oss << " "; // Agrega una coma y un espacio entre elementos  
        }  
    }  
    
    return oss.str(); // Devuelve la cadena construida  
}
std::vector<int> hexStringToDecimalArray(const std::string &hexString) {  
    std::vector<int> decimalValues;  
    std::stringstream ss(hexString);  
    std::string byte;  
    std::cout<<"Realizando conversión Hex->Dec: Variables asignadas."<< std::endl;
    // Leer cada par de caracteres, convertir a entero decimal y almacenarlo  
    while (ss >> byte) {  
        // Convierte el byte hexadecimal a decimal  
        int decimalValue = static_cast<int>(strtol(byte.c_str(), nullptr, 16));  
        decimalValues.push_back(decimalValue);  
        std::cout<<"Realizando conversión Hex->Dec: En el ciclo 'WHILE...'"<< std::endl;
    }  
    std::cout<<"Realizando conversión Hex->Dec: Ciclo completado."<< std::endl;
    std::cout<<"Realizando conversión Hex->Dec: Resultado: "<<vectorToString(decimalValues)<< std::endl;
    return decimalValues;  
}
void logVector(const std::vector<std::string>& vec) {  
    std::cout << "LOG VECTOR: "<< std::endl; 
    for (const auto& str : vec) {  
        std::cout << str << std::endl;  
    }  
} 
int nbl=0, nbh=0, csl=0, csh=0;
void cal_NBL_NBH_CSL_CSH(int numBytes, int& nbl_csl, int& nbh_csh){
    if(numBytes > 256){
        nbl_csl = numBytes % 256;
        nbh_csh = numBytes / 256;
    }else if(numBytes < 256){
        nbl_csl = numBytes;
        nbh_csh = 0;
    }
    std::cout<<"NBL_O_CSL: "<<nbl_csl<<std::endl;
    std::cout<<"NBH_O_CSH: "<<nbh_csh<<std::endl;
}

std::vector<uint8_t> convertStringToBytes(const std::string& text) {
    std::vector<uint8_t> bytes;
    for (char c : text) {
        bytes.push_back(static_cast<uint8_t>(c));
    }
    std::cout << "Bytes String: ";
    for (const auto& byte : bytes) {
        std::cout << std::hex << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;
    return bytes;
}

//NUM BYTES > 256  BYTES/256 RESTO = NBL VAL = NBH   NUM BYTES < 256 = 0
int convertirHexADecimal(std::string hex) {  
    int decimal = 0;  
    
    for (int i = hex.length() - 1; i >= 0; i--) {  
        int valor;  
        if (hex[i] >= '0' && hex[i] <= '9') {  
            valor = hex[i] - '0';  
        } else if (hex[i] >= 'A' && hex[i] <= 'F') {  
            valor = hex[i] - 'A' + 10;  
        } else if (hex[i] >= 'a' && hex[i] <= 'f') {  
            valor = hex[i] - 'a' + 10;  
        } else {  
            // Carácter inválido en hexadecimal  
            std::cerr << "Carácter inválido en hexadecimal: " << hex[i] << std::endl;  
            return -1;  
        }  
        
        decimal += valor * pow(16, hex.length() - 1 - i);  
    }  
    
    return decimal;  
} 
std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}
int countNumElements(const std::string &str) {
    std::stringstream ss(str);
    std::string byte;
    int count = 0;

    while (ss >> byte) {
        count++;
    }

    return count;
}
int16_t sumSequence16(const uint8_t* const blockAddress, const uint32_t sequenceOffset, const uint8_t sequenceLength) {
    const uint8_t* p = blockAddress + sequenceOffset;  // Apunta al primer byte de la secuencia
    int sum = 0;
    for (uint8_t i = 0; i < sequenceLength; i += 2) {
        int16_t t = 0;
        t = p[i];          // Lee el byte MSB (en little-endian)
        t = t << 8;        // Desplaza el byte MSB 8 bits a la izquierda
        t = t | p[i + 1];  // Agrega el byte LSB
        sum += t;          // Actualiza la suma

        // Almacena los bytes MSB y LSB (opcional)
        uint8_t msb = p[i];
        uint8_t lsb = p[i + 1];
        std::cout << "MSB: " << std::hex << static_cast<int>(msb) << ", LSB: " << static_cast<int>(lsb) << std::endl;
    }
    return sum;
}
std::vector<unsigned char> parseHexString(const std::string& hexString) {
    std::vector<unsigned char> bytes;
    std::istringstream iss(hexString);
    std::string token;
    while (std::getline(iss, token, ' ')) {
        std::stringstream ss;
        ss << std::hex << token;
        unsigned char byte;
        ss >> byte;
        bytes.push_back(byte);
    }
    return bytes;
}
void findMostAndLeastSignificantBytes(const std::vector<unsigned char>& bytes) {
    unsigned char mostSignificantByte = 0x00;
    unsigned char leastSignificantByte = 0xFF;

    for (unsigned char byte : bytes) {
        if (byte > mostSignificantByte) {
            mostSignificantByte = byte;
        }
        if (byte < leastSignificantByte) {
            leastSignificantByte = byte;
        }
    }

    std::cout << "El byte más relevante es: 0x" << std::hex << static_cast<int>(mostSignificantByte) << std::endl;
    std::cout << "El byte menos relevante es: 0x" << std::hex << static_cast<int>(leastSignificantByte) << std::endl;
}
// Función para calcular CSL y CSH
void calculateChecksum(const char* command, int& csl, int& csh) {
    int sum = 0;
     int cmdLength = sizeof(command) / sizeof(command[0]);
    for (int i = 0; i < cmdLength; ++i) {
        sum += static_cast<unsigned char>(command[i]);
    }
    csl = sum & 0xFF; // Byte menos significativo
    csh = (sum >> 8) & 0xFF; // Byte más significativo
        
    std::cout << "CSL: " << static_cast<int>(csl) << std::endl;
    std::cout << "CSH: " << static_cast<int>(csh) << std::endl;
}
// Función para convertir un entero a su representación hexadecimal
std::string intToHex(int value) {
    std::stringstream ss;
    ss << std::hex << std::setw(2) << std::setfill('0') << value;
    return ss.str();
}
unsigned int calcularNumeroBytes(const std::string& comando) {
    unsigned int sumaBytes = 0;

    // Suma los valores de los bytes en la cadena 'comando'
    for (char c : comando) {
        sumaBytes += static_cast<unsigned char>(c);
    }

    return sumaBytes;
}
std::string convertBytesToString(const std::vector<uint8_t>& bytes) {
    std::stringstream ss;
    for (const auto& byte : bytes) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::string result = ss.str();
    // Eliminar el espacio extra al final
    if (!result.empty()) {
        result.pop_back();
    }
    
    OATPP_LOGI("IN CBTS: ", "%s", result.c_str());
    return result;
}

//END UTILS


std::map<std::string, std::string> StatusBematech::StatusBematech1 = {
        {"0", "COMANDO OKEY"},  
        {"1", "COMANDO INVÁLIDO"},  
        {"2", "ERROR DESCONOCIDO"},  
        {"3", "NÚMERO DE PARÁMETRO NO VÁLIDO"},  
        {"4", "TIPO DE PARÁMETRO NO VÁLIDO"},  
        {"5", "TODAS LAS ALICUOTAS YA PROGRAMADAS"},  
        {"6", "TOTALIZADOR NO FISCAL YA PROGRAMADO"},  
        {"7", "CUPÓN FISCAL ABIERTO"},  
        {"8", "CUPÓN FISCAL CERRADO"},  
        {"9", "IMPRESORA FISCAL OCUPADA"},  
        {"10", "IMPRESORA EN ERROR"},  
        {"11", "IMPRESORA SIN PAPEL"},  
        {"12", "IMPRESORA CON CABEZAL ELEVADO"},  
        {"13", "IMPRESORA OFF LINE"},  
        {"14", "ALICUOTA NO PROGRAMADA"},  
        {"15", "FALTA TERMINADOR DE STRING"},  
        {"16", "INCREMENTO O DESCUENTO MAYOR QUE EL VALOR TOTAL DE CUPÓN FISCAL"},  
        {"17", "CUPÓN FISCAL SIN ARTÍCULO VENDIDO"},  
        {"18", "COMANDO NO EJECUTADO"},  
        {"19", "NO HAY ESPACIO PARA NUEVAS FORMAS DE PAGO"},  
        {"20", "FORMA DE PAGO NO PROGRAMADA"},  
        {"21", "ÍNDICE MAYOR AL NÚMERO DE FORMA DE PAGO"},  
        {"22", "FORMAS DE PAGO CERRADOS"},  
        {"23", "CUPÓN NO TOTALIZADO"},  
        {"24", "COMANDO MAYOR DE 7Fh (127d)"},  
        {"25", "CUPÓN FISCAL ABIERTO SIN ARTÍCULO"},  
        {"26", "ANULACIÓN NO INMEDIATAMENTE DESPUÉS"},  
        {"27", "ANULACIÓN YA EJECUTADA"},  
        {"28", "COMPROBANTE DE CRÉDITO O DÉBITO NO PERMITIDO O YA EMITIDO"},  
        {"29", "FORMA DE PAGO NO PERMITE COMPROBANTE NO FISCAL VINCULADO"},  
        {"30", "SIN COMPROBANTE NO FISCAL ABIERTO"},
        {"31", "COMPROBANTE DE CRÉDITO O DÉBITO YA ABIERTO"},  
        {"32", "REIMPRESIÓN NO PERMITIDA"},  
        {"33", "COMPROBANTE NO FISCAL YA ABIERTO"},  
        {"34", "TOTALIZADOR NO FISCAL NO PROGRAMADO"},  
        {"35", "CUPÓN NO FISCAL SIN ARTÍCULO VENDIDO"},  
        {"36", "INCREMENTO O DESCUENTO MÁS DEL TOTAL CNF"},  
        {"37", "FORMA DE PAGO NO INDICADA"},  
        {"38", "FORMA DE PAGO DIFERENTE DEL TOTAL DE RECIBO"},  
        {"39", "NO MÁS DE UNA RETIRADA DE CAJA O FONDO DE CAJA PERMITIDO"},  
        {"40", "REPORTE GERENCIAL YA PROGRAMADO"},  
        {"41", "REPORTE GERENCIAL NO PROGRAMADO"},  
        {"42", "REPORTE GERENCIAL NO PERMITIDO"},  
        {"43", "MEMORIA DE AUDITORIA NO INICIALIZADA"},  
        {"44", "FALTA MEMORIA DE AUDITORIA"},  
        {"45", "MEMORIA DE AUDITORIA SIN NÚMERO SERIAL"},  
        {"46", "MEMORIA DE AUDITORIA YA INICIALIZADA"},  
        {"47", "MEMORIA DE AUDITORIA LLENA"},  
        {"48", "CUPÓN NO FISCAL ABIERTO"},  
        {"49", "MEMORIA FISCAL DESCONECTADA"},  
        {"50", "MEMORIA FISCAL SIN NÚMERO SERIAL DE LA MEMORIA DE AUDITORIA"},  
        {"51", "MEMORIA FISCAL LLENA"},  
        {"52", "FECHA INICIAL NO VÁLIDA"},  
        {"53", "FECHA FINAL NO VÁLIDA"},  
        {"54", "CONTADOR DE REPORTE Z INICIAL NO VÁLIDO"},  
        {"55", "CONTADOR DE REPORTE Z FINAL NO VÁLIDO"},  
        {"56", "ERROR DE ASIGNACIÓN"},  
        {"57", "DATOS RTC (REAL TIME CLOCK) INCORRECTOS"},  
        {"58", "FECHA ANTES DEL ÚLTIMO DOCUMENTO EMITIDO"},  
        {"59", "FUERA DE INTERVENCIÓN TÉCNICA"},  
        {"60", "EN INTERVENCIÓN TÉCNICA"},
        {"61", "ERROR EN LA MEMORIA DE TRABAJO"},  
        {"62", "HUBO MOVIMIENTO EN EL DIA"},  
        {"63", "BLOQUEO POR RZ"},  
        {"64", "FORMA DE PAGO ABIERTA"},  
        {"65", "ESPERANDO EL PRIMER PROPIETARIO"},  
        {"66", "ESPERANDO RZ"},  
        {"67", "IMPRESORA FISCAL O TIENDA IGUAL A CERO"},  
        {"68", "CUPÓN ADICIONAL NO PERMITIDO"},  
        {"69", "DESCUENTO MAYOR QUE EL VALOR TOTAL VENDIDO EN IMPUESTOS"},  
        {"70", "RECIBO NO FISCAL NULO NO PERMITIDO"},  
        {"71", "INCREMENTO O DESCUENTO MAYOR QUE TOTAL NO FISCAL"},  
        {"72", "MEMORIA FISCAL LLENA PARA CARTUCHO NUEVO"},  
        {"73", "ERROR DE GRABACIÓN EN MF"},  
        {"74", "ERROR DE GRABACIÓN EN MEMORIA DE AUDITORIA"},  
        {"75", "DATOS RTC ANTES DEL ÚLTIMO DOCUMENTO ALMACENADO"},  
        {"76", "SIN ESPACIO DE MEMORIA FISCAL PARA REGISTRAR LAS LECTURAS DE LA MEMORIA DE AUDITORIA"},  
        {"77", "MEMORIA FISCAL SIN ESPACIO PARA GRABAR VERSIÓN DE FIRMWARE"},  
        {"78", "DESCRIPCIÓN IGUAL AL PADRÓN NO PERMITIDO"},  
        {"79", "NÚMERO EXTRAPOLADO DE REPETICIONES PERMITIDAS"},  
        {"80", "SEGUNDO COMPROBANTE DE CRÉDITO O DÉBITO NO PERMITIDO"},  
        {"81", "PLAZO FUERA DE SECUENCIA"},  
        {"82", "COMPROBANTE DE CRÉDITO O DÉBITO ABIERTO"},  
        {"83", "TEXTO CON SECUENCIA ESC NO VÁLIDO"},  
        {"84", "TEXTO CON SECUENCIA ESC INCOMPLETO"},  
        {"85", "VENTA CON VALOR NULO"},  
        {"86", "REEMBOLSO DE VALOR NULO"},  
        {"87", "FORMA DE PAGO DIFERENTE DEL VALOR TOTAL DE RETIRADA DE CAJA"},  
        {"88", "REDUCCIÓN NO PERMITIDA EN INTERVENCIÓN TÉCNICA"},  
        {"89", "ESPERANDO REPORTE Z PARA LA ENTRADA EN INTERVENCIÓN TÉCNICA"},  
        {"90", "FORMA DE PAGO CON VALOR NULO NO PERMITIDO"},
        {"91", "INCREMENTO Y DESCUENTO MAYOR QUE EL VALOR DEL ARTÍCULO"},  
        {"92", "AUTENTICACIÓN NO PERMITIDA"},  
        {"93", "TIMEOUT EN LA VALIDACIÓN"},  
        {"94", "COMANDO NO EJECUTADO EN IMPRESORA DE BILLETE DE PASAJE"},  
        {"95", "COMANDO NO EJECUTADO EN IMPRESORA DE CUPÓN FISCAL"},  
        {"96", "CUPÓN NO FISCAL CERRADO"},  
        {"97", "PARÁMETRO NO ASCII EN EL CAMPO ASCII"},  
        {"98", "PARÁMETRO NO NUMÉRICO ASCII EN CAMPO NUMÉRICO ASCII"},  
        {"99", "TIPO DE TRANSPORTE NO VÁLIDO"},  
        {"100", "FECHA Y HORA NO VÁLIDAS"},  
        {"101", "SIN REPORTE GENERAL O COMPROBANTE DE CRÉDITO O DÉBITO ABIERTO"},  
        {"102", "NÚMERO DE TOTALIZADOR NO FISCAL NO VÁLIDO"},  
        {"103", "PARÁMETRO DE INCREMENTO O DESCUENTO NO VÁLIDO"},  
        {"104", "INCREMENTO O DESCUENTO EN RETIRADA DE CAJA O FONDO DE CAJA NO PERMITIDO"},  
        {"105", "NÚMERO DE REPORTE GENERAL NO VÁLIDO"},  
        {"106", "FORMA DE PAGO ORIGEN NO PROGRAMADO"},  
        {"107", "FORMA DE PAGO DESTINO NO PROGRAMADO"},  
        {"108", "REEMBOLSO MAYOR QUE LA FORMA DE PAGO"},  
        {"109", "CARÁCTER NUMÉRICO EN CODIFICACIÓN GT NO PERMITIDO"},  
        {"110", "ERROR DE INICIALIZACIÓN MF"},  
        {"111", "DESCRIPCIÓN DEL TOTALIZADOR EN BLANCO NO PERMITIDO"},  
        {"112", "FECHA Y HORA ANTES DEL ÚLTIMO DOCUMENTO ALMACENADO"},  
        {"113", "PARÁMETRO DE INCREMENTO O DESCUENTO NO VÁLIDO"},  
        {"114", "ARTÍCULO ANTES DE LOS ÚLTIMOS TRESCIENTOS"},  
        {"115", "EL ARTÍCULO NO EXISTE O YA ESTÁ ANULADO"},  
        {"116", "CÓDIGO CON ESPACIOS NO PERMITIDOS"},  
        {"117", "DESCRIPCIÓN SIN CARÁCTER ALFABÉTICO NO PERMITIDO"},  
        {"118", "INCREMENTO MAYOR QUE EL VALOR DEL ARTÍCULO"},  
        {"119", "DESCUENTO MAYOR QUE EL VALOR DEL ARTÍCULO"},  
        {"120", "NO SE PERMITE DESCUENTO"},
        {"121", "INCREMENTO EN ARTÍCULO YA HECHO"},  
        {"122", "DESCUENTO EN ARTÍCULO YA HECHO"},  
        {"123", "ERROR DE MEMORIA FISCAL LLAMA POR ASISTENCIA TÉCNICA"},  
        {"124", "ESPERANDO GRABACIÓN DE MEMORIA FISCAL"},  
        {"125", "CARÁCTER REPETIDO EN CODIFICACIÓN GT"},  
        {"126", "VERSIÓN YA GUARDADA EN MEMORIA FISCAL"},  
        {"127", "CAPACIDAD LLENA EN CHEQUE"},  
        {"128", "TIMEOUT EN LECTURA DEL CHEQUE"},  
        {"129", "MES NO VÁLIDO"},  
        {"130", "COORDENADA NO VÁLIDA"},  
        {"131", "SUPERPOSICIÓN DE TEXTO"},  
        {"132", "SUPERPOSICIÓN DE TEXTO DE VALOR"},  
        {"133", "SUPERPOSICIÓN DE TEXTO EXTENDIDO"},  
        {"134", "SUPERPOSICIÓN DE TEXTO EN FAVORITO"},  
        {"135", "SUPERPOSICIÓN DE TEXTO DE LOCALIDAD"},  
        {"136", "SUPERPOSICIÓN DE TEXTO OPCIONAL"},  
        {"137", "SUPERPOSICIÓN DE TEXTO EN EL DÍA"},  
        {"138", "SUPERPOSICIÓN DE TEXTO EN EL MES"},  
        {"139", "SUPERPOSICIÓN DE TEXTO EN EL AÑO"},  
        {"140", "USANDO MEMORIA DE AUDITORIA DE OTRA IMPRESORA FISCAL"},  
        {"141", "PRIMER DATO DIFERENTE DE ESC O 1C"},  
        {"142", "NO SE PERMITE CAMBIAR SIN INTERVENCIÓN TÉCNICA"},  
        {"143", "ÚLTIMOS DATOS RZ DAÑADOS"},  
        {"144", "COMANDO NO PERMITIDO EN MODO DE INICIALIZACIÓN"},  
        {"145", "ESPERANDO EL AJUSTE DEL RELOJ"},  
        {"146", "MEMORIA DE AUDITORIA YA INICIALIZADA PARA OTRA MF"},  
        {"147", "ESPERANDO A QUE EL RELOJ SE PUEDA FIJAR O DESBLOQUEAR CON EL TECLADO"},  
        {"148", "VALOR DE FORMA DE PAGO SUPERIOR AL MÁXIMO PERMITIDO"},  
        {"149", "CLICHÉ EN BLANCO"},  
        {"150", "NOMBRE DE FANTASÍA EN BLANCO"},
        {"151", "DIRECCIÓN EN BLANCO"},  
        {"152", "REEMBOLSO DE CDC NO PERMITIDO"},  
        {"153", "DATOS DEL PROPIETARIO IGUALES A ACTUALES"},  
        {"154", "REEMBOLSO DE FORMA DE PAGO NO PERMITIDO"},  
        {"155", "DESCRIPCIÓN DE FORMA DE PAGO IGUAL YA PROGRAMADA"},  
        {"156", "AJUSTE DE LA HORA DE VERANO SOLO INMEDIATAMENTE DESPUÉS DE RZ"},  
        {"157", "NO PERMITIDO MF RESERVADO PARA RZ"},  
        {"158", "CONTRASEÑA RIF NO VÁLIDA"},  
        {"159", "TIMEOUT EN LA INICIALIZACIÓN DEL NUEVO MF"},  
        {"160", "NO SE ENCONTRARON DATOS EN MFD"},  
        {"161", "RETIRADA DE CAJA O FONDO DE CAJA DEBE SER ÚNICO EN CNF"},  
        {"162", "ÍNDICE DE FORMA DE PAGO NULO NO PERMITIDO"},  
        {"163", "UNIDAD FEDERAL DE DESTINO NO VÁLIDO"},  
        {"164", "TIPO DE TRANSPORTE INCOMPATIBLE CON UNIDAD FEDERAL DE DESTINO"},  
        {"165", "DESCRIPCIÓN DEL PRIMER ARTÍCULO DEL BILLETE DE PASAJE DIFERENTE DE 'TARIFA'"},  
        {"166", "ESPERA DE IMPRESIÓN O AUTENTICACIÓN DEL CHEQUE"},  
        {"167", "PROGRAMACIÓN CLICHÉ NO PERMITIDA CON ESPACIOS EN BLANCO"},  
        {"168", "PROGRAMACIÓN UNIDAD FEDERAL CON ESPACIOS EN BLANCO NO PERMITIDOS"},  
        {"169", "NÚMERO LLENO DE IMPRESIONES DE CINTA DETALLE EN ESTA INTERVENCIÓN TÉCNICA"},  
        {"170", "CUPÓN FISCAL YA SUBOTALIZADO"},  
        {"171", "CUPÓN FISCAL NO SUBTOTALIZADO"},  
        {"172", "INCREMENTO EN SUBTOTAL YA REALIZADO"},  
        {"173", "DESCUENTO EN SUBTOTAL YA REALIZADO"},  
        {"174", "INCREMENTO NULO NO PERMITIDO"},  
        {"175", "DESCUENTO NULO NO PERMITIDO"},  
        {"176", "ANULACIÓN DE INCREMENTO O DESCUENTO EN SUBTOTAL NO PERMITIDO"},  
        {"177", "FECHA INVÁLIDA"},  
        {"178", "VALOR DEL CHEQUE NULO NO PERMITIDO"},  
        {"179", "VALOR DEL CHEQUE INVÁLIDO"},  
        {"180", "CHEQUE SIN UBICACIÓN NO PERMITIDA"},
        {"181", "ANULACIÓN INCREMENTO EN ARTÍCULO NO PERMITIDO"},  
        {"182", "ANULACIÓN DESCUENTO EN ARTÍCULO NO PERMITIDO"},  
        {"183", "NÚMERO MÁXIMO DE ARTÍCULOS LOGRADOS"},  
        {"184", "NÚMERO DE ARTÍCULO NULO NO PERMITIDO"},  
        {"185", "MÁS DE DOS TARIFAS DIFERENTES EN EL BILLETE DE PASAJE NO PERMITIDAS"},  
        {"186", "INCREMENTO O DESCUENTO EN ARTÍCULO NO PERMITIDO"},  
        {"187", "ANULACIÓN DE INCREMENTO O DESCUENTO EN ARTÍCULO NO PERMITIDO"},  
        {"188", "CLICHÉ YA IMPRESO"},  
        {"189", "EL TEXTO OPCIONAL DE CHEQUE SUPERÓ EL MÁXIMO PERMITIDO"},  
        {"190", "IMPRESIÓN AUTOMÁTICA EN LA PARTE POSTERIOR NO PERMITIDA EN ESTE EQUIPO"},  
        {"191", "TIMEOUT EN PONER EL CHEQUE"},  
        {"192", "OVERFLOW EN LA CAPACIDAD DE TEXTO DEL COMPROBANTE DE CRÉDITO O DÉBITO"},  
        {"193", "PROGRAMACIÓN DE ESPACIO ENTRE CUPONES MENOS DEL MÍNIMO PERMITIDO"},  
        {"194", "EL EQUIPO NO TIENE LECTOR DE CHEQUES"},  
        {"195", "PROGRAMACIÓN DE ALICUOTA CON VALOR NULO NO PERMITIDO"},  
        {"196", "PARÁMETRO BAUD RATE NO VÁLIDO"},  
        {"197", "CONFIGURACIÓN PERMITIDA ÚNICAMENTE POR EL PUERTO FISCO"},  
        {"198", "EL VALOR TOTAL DEL ARTÍCULO SUPERA 11 DÍGITOS"},  
        {"199", "PROGRAMACIÓN DE MONEDA CON ESPACIOS EN BLANCO NO PERMITIDOS"},  
        {"200", "LAS CASAS DECIMALES SE DEBEN PROGRAMAR CON 2 O 3"},  
        {"201", "NO PERMITE REGISTRAR A DIFERENTES USUARIOS EN EL MISMO MEMORIA DE AUDITORIA"},  
        {"202", "IDENTIFICACIÓN DEL CONSUMIDOR NO PERMITIDA PARA RETIRADA DE CAJA O FONDO DE CAJA"},  
        {"203", "CASAS DECIMALES EN CANTIDAD MAS DE LO PERMITIDO"},  
        {"204", "CASAS UNITARIAS DECIMALES MAYORES DE LO PERMITIDO"},  
        {"205", "POSICIÓN RESERVADA DEL IMPUESTO"},  
        {"206", "POSICIÓN RESERVADA PARA IMPUESTO DE SERVICIO"},  
        {"207", "NO SE ADMITEN TODAS LAS ALICUOTAS CON LA MISMA VINCULACIÓN"},  
        {"208", "FECHA DE EMBARQUE ANTES DE LA FECHA DE EMISIÓN"},  
        {"209", "NO SE PERMITE IMPUESTO SIN REGISTRO MUNICIPAL"},  
        {"210", "RETORNO PAQUETE HAGA CLICHÉ FUERA DE SECUENCIA"},  
        {"211", "ESPACIO DE ALMACENAMIENTO DE CLICHÉ AGOTADO"},  
        {"212", "CLICHÉ GRÁFICO NO DISPONIBLE PARA CONFIRMACIÓN"},  
        {"213", "CRC DEL CLICHÉ GRÁFICO DIFERENTE DEL INFORMADO"},  
        {"214", "INTERVALO NO VÁLIDO"},  
        {"215", "USUARIO PROGRAMADO"},  
        {"217", "APERTURA DETECTADA DEL EQUIPO"},  
        {"218", "ANULACIÓN DE INCREMENTO/DESCUENTO NO PERMITIDO"}
    };

std::map<std::string, std::string> StatusBematech::StatusBematech2 = {
    {"1", "El comando de envío no fue ejecutado."},
    {"2", "Los datos del propietario no se registran en la impresora fiscal."},
    {"4", "No se permite anulación del cupón fiscal o factura de venta."},
    {"8", "Todas las alicuotas ya están programadas."},
    {"16", "La alicuota utilizada no se programó en la impresora fiscal."},
    {"32", "Hubo un error en la memoria de trabajo."},
    {"64", "La memoria fiscal de la impresora está llena."},
    {"128", "Algún parámetro de comando no es válido."}
    
};

std::map<std::string, std::string> StatusBematech::StatusBematech3 = {
    {"1", "Algún parámetro de comando no es válido."},
    {"2", "El cupón de venta o la factura ya está abierta."},
    {"4", "El comando de envío no existe en la impresora fiscal."},
    {"8", "El primer byte del comando enviado no es ESC (Escape)."},
    {"16", "La impresora fiscal tiene un error"},
    {"32", "La impresora fiscal tiene un error de reloj."},
    {"64", "La impresora fiscal tiene poco papel en el rollo."},
    {"128", " El rollo de papel de la impresora fiscal se ha agotado."}
};

BematechPrinter::BematechPrinter(std::string cport) : PrinterInterface::PrinterInterface(cport) {}


void BematechPrinter::validarCampos2(PrinterDto *dat) {  
    //START VALIDACIONES TAMAÑO DE CARACTERES  

    // Validaciones cliente  
    OATPP_ASSERT_HTTP(dat->factura->cliente->nombre.getValue("").size() <= 41, Status::CODE_400, "NOMBRE DE CLIENTE NO DEBE SUPERAR LOS 41 CARACTERES!");  
    OATPP_ASSERT_HTTP(dat->factura->cliente->rif.getValue("").size() <= 18, Status::CODE_400, "RIF DE CLIENTE NO DEBE SUPERAR LOS 18 CARACTERES!");  

    // Validaciones nota fiscal  
    if (dat->factura->notaFiscal != nullptr) {  
        OATPP_ASSERT_HTTP(std::to_string(dat->factura->notaFiscal->factura.getValue(0)).size() <= 6, Status::CODE_400, "NUMERO DE FACTURA EN NOTA FISCAL NO DEBE SUPERAR LOS 6 DIGITOS!");  
        OATPP_ASSERT_HTTP(dat->factura->notaFiscal->serialFiscal.getValue("").size() <= 15, Status::CODE_400, "SERIAL DE FACTURA EN NOTA FISCAL NO DEBE SUPERAR LOS 15 CARACTERES!");  
        OATPP_ASSERT_HTTP(dat->factura->notaFiscal->fecha.getValue("").size() <= 10, Status::CODE_400, "FECHA (AA-MM-DD) DE FACTURA EN NOTA FISCAL NO DEBE SUPERAR LOS 10 CARACTERES!");  
        OATPP_ASSERT_HTTP(dat->factura->notaFiscal->hora.getValue("").size() <= 10, Status::CODE_400, "HORA (HH:MM:SS) DE FACTURA EN NOTA FISCAL NO DEBE SUPERAR LOS 10 CARACTERES!");  
    }  

    // Validaciones items  
    auto items = dat->factura->items.get();  
    if (items != nullptr) {  
        for (const auto& item : *items) {  
        
            auto codigo = item->codigo.getValue("");  
            auto producto = item->producto.getValue("");  
            auto precio = item->precio.getValue(0.00);  
            auto cantidad = item->cantidad.getValue(0);  
            auto tasa = item->tasa.getValue("");  
            auto descuento = item->descuento.getValue(0);  

            OATPP_ASSERT_HTTP(codigo.size() <= 48, Status::CODE_400, "CODIGO DE ITEM NO DEBE SUPERAR LOS 48 CARACTERES!");  
            OATPP_ASSERT_HTTP(producto.size() <= 200, Status::CODE_400, "NOMBRE DE ITEM NO DEBE SUPERAR LOS 200 CARACTERES!");  
            OATPP_ASSERT_HTTP(std::to_string(precio).size() <= 15, Status::CODE_400, "PRECIO DE ITEM NO DEBE SUPERAR LOS 14 DIGITOS!");  
            OATPP_ASSERT_HTTP(std::to_string(cantidad).size() <= 8, Status::CODE_400, "CANTIDAD DE ITEM NO DEBE SUPERAR LOS 7 DIGITOS!");  
            OATPP_ASSERT_HTTP(tasa.size() <= 2, Status::CODE_400, "IMPUESTO DE ITEM NO DEBE SUPERAR LOS 2 CARACTERES!");  
            OATPP_ASSERT_HTTP(std::to_string(descuento).size() <= 11, Status::CODE_400, "DESCUENTO EN ITEM NO DEBE SUPERAR LOS 10 DIGITOS!");  
        }  
    }  

    // Validaciones pagos  
    if (!dat->factura->pagos->empty()) {  
        auto pagos = dat->factura->pagos.get();  
        for (const auto& pago : *pagos) {  
            OATPP_ASSERT_HTTP(std::to_string(pago->monto.getValue(0)).size() <= 15, Status::CODE_400, "MONTO DE PAGO NO DEBE SUPERAR LOS 14 DIGITOS!");  
            OATPP_ASSERT_HTTP(std::to_string(pago->metodo_id.getValue(0)).size() <= 2, Status::CODE_400, "METODO DE PAGO NO VALIDO!");  
        }  
    }  

    //END VALIDACIONES TAMAÑO DE CARACTERES   
}

void BematechPrinter::validarCampos(PrinterDto* dat) {  
    // Validaciones cliente  
    if (dat->factura->cliente->nombre->size() > 41) {  
        throw std::invalid_argument("NOMBRE DE CLIENTE NO DEBE SUPERAR LOS 41 CARACTERES!");  
    }  
    if (dat->factura->cliente->rif->size() > 18) {  
        throw std::invalid_argument("RIF DE CLIENTE NO DEBE SUPERAR LOS 18 CARACTERES!");  
    }  
    // Validaciones nota fiscal  
    if (dat->factura->notaFiscal != nullptr) {  
        if ( countDigits(dat->factura->notaFiscal->factura.getValue(0)) > 6 ) {  
            throw std::invalid_argument("NUMERO DE FACTURA EN NOTA FISCAL NO DEBE SUPERAR LOS 6 DIGITOS!");  
        }  
        if (dat->factura->notaFiscal->serialFiscal->size() > 15) {  
            throw std::invalid_argument("SERIAL DE FACTURA EN NOTA FISCAL NO DEBE SUPERAR LOS 15 CARACTERES!");  
        }  
        if (dat->factura->notaFiscal->fecha->size() > 10) {  
            throw std::invalid_argument("FECHA (AA-MM-DD) DE FACTURA EN NOTA FISCAL NO DEBE SUPERAR LOS 10 CARACTERES!");  
        }  
        if (dat->factura->notaFiscal->hora->size() > 10) {  
            throw std::invalid_argument("HORA (HH:MM:SS) DE FACTURA EN NOTA FISCAL NO DEBE SUPERAR LOS 10 CARACTERES!");  
        }  
    }  

    // Validaciones items  
    auto items = dat->factura->items;  
    if (items != nullptr) {  
        for (const auto& item : *items) {  
            if (item->codigo->size() > 48) {  
                throw std::invalid_argument("CODIGO DE ITEM NO DEBE SUPERAR LOS 48 CARACTERES!");  
            }  
            if (item->producto->size() > 200) {  
                throw std::invalid_argument("NOMBRE DE ITEM NO DEBE SUPERAR LOS 200 CARACTERES!");  
            }  
            if (countDigits((int)(item->precio + 0.5)) > 15) {  
                throw std::invalid_argument("PRECIO DE ITEM NO DEBE SUPERAR LOS 14 DIGITOS!");  
            }  
            if (countDigits(item->cantidad) > 8) {  
                throw std::invalid_argument("CANTIDAD DE ITEM NO DEBE SUPERAR LOS 7 DIGITOS!");  
            }  
            if (item->tasa->size() > 2) {  
                throw std::invalid_argument("IMPUESTO DE ITEM NO DEBE SUPERAR LOS 2 CARACTERES!");  
            }  
            if (countDigits((int)(item->descuento + 0.5)) > 11) {  
                throw std::invalid_argument("DESCUENTO EN ITEM NO DEBE SUPERAR LOS 10 DIGITOS!");  
            }  
        }  
    }  
    // Validaciones pagos  
    if (!dat->factura->pagos->empty()) {
        std::cout << "<<<Pagos no existe en vacio>>>" << std::endl;
        auto pagos = dat->factura->pagos;  
        for (const auto& pago : *pagos) {  
            if (countDigits((int)(pago.get()->monto.getValue(0.00) + 0.5)) > 15) {  
                throw std::invalid_argument("MONTO DE PAGO NO DEBE SUPERAR LOS 14 DIGITOS!");  
            }  
            if (countDigits(pago.get()->metodo_id) > 2) {  
                throw std::invalid_argument("METODO DE PAGO NO VALIDO!");  
            }  
        }  
    }  
} 

void BematechPrinter::refrescarConfiguracion()
{
    OATPP_LOGI("Refrescando configuracion","...");
    this->maxLength = 48;
    this->lineasTextoNF = 4;
    OATPP_LOGI("->","------------[HECHO]---------------");
}

std::string BematechPrinter::readPort()
{
    OATPP_LOGI("LEYENDO PUERTO","...");
    std::string bytes;
    int timeOut = 0;

    std::string error = this->NAK;
    std::string fin = this->END;
    std::string OK = this->ACK;
    bool buscando = true;

    while (buscando && timeOut < (25 + this->tiempoAdicional))
    {
        timeOut++;
        memset(this->buf, 0, 4095);
        int n = RS232_PollComport(this->cport_nr, this->buf, 4095);

        if (n > 0)
        {
            OATPP_LOGD("", "received %i bytes: %s", n, (char *)buf);
            bytes += std::string(buf, buf + n);
        }

        if (bytes.find(error) != std::string::npos ||
            bytes.find(fin) != std::string::npos ||
            bytes.find(OK) != std::string::npos)
        {
            buscando = false;
            OATPP_LOGD("", "\t\t\t\t (OK): %s \t\t\t (ERROR): %s \t\t\t (FIN): %s \t\t\t timeOut=%d\n",
                       (bytes.find(OK) != std::string::npos) ? "TRUE" : "false",
                       (bytes.find(error) != std::string::npos) ? "TRUE" : "false",
                       (bytes.find(fin) != std::string::npos) ? "TRUE" : "false",
                       timeOut);
        }
        else
        {
            this->sleep(150);
        }
    }
    OATPP_LOGI("->","------------[HECHO]-----------");
    return bytes;
}
std::string BematechPrinter::verSubtotal(){
    std::string cmd, resultado="ERROR", entero, decimal;
    try{
         cmd = this->hex2ascii("1b") + this->hex2ascii("1D");
         resultado = prepararYenviarComando(cmd);
         resultado = convertBytesToString(convertStringToBytes(resultado));
         OATPP_LOGI("Resultado de Ver Subtotal","%s", resultado.c_str());
         if(resultado.size() >= 14 ){
            auto result = splitString(resultado, ' ');
            entero = result[1] + result[2] + result[3] + result[4] + result[5] + result[6];
            decimal = result[7];
            resultado = entero + "." + decimal;
         }
         OATPP_LOGI("Subtotal: ", "%s", resultado.c_str());
    }catch(const std::exception &e){
            OATPP_ASSERT_HTTP(false, Status::CODE_400, e.what());
    }
    return resultado.c_str();
}

std::string BematechPrinter::verUltimaFactura(){
    std::string cmd, resultado="ERROR", num1, num2, num3;
        try{
            cmd = this->hex2ascii("1b") + this->hex2ascii("23") + this->hex2ascii("37");
            resultado = prepararYenviarComando(cmd);
            resultado = convertBytesToString(convertStringToBytes(resultado));
            OATPP_LOGI("Resultado de Ver Ultima Factura","%s", resultado.c_str());
            if(resultado.size() >= 6 ){
                auto result = splitString(resultado, ' ');
                num1=result[1];num2=result[2];num3=result[3];
                resultado = num1 + num2 + num3;
            }
            OATPP_LOGI("Ultima factura emitida: ", "%s", resultado.c_str());
        }catch(const std::exception &e){
            OATPP_ASSERT_HTTP(false, Status::CODE_400, e.what());
        }
    return resultado;
}

std::string BematechPrinter::ejecutarComando(oatpp::DTO *dto)
{
    OATPP_LOGI("EJECUTANDO COMANDO","...");
    PrinterDto *data = ((PrinterDto *)dto);
    std::string resultado = "ERROR", cmd = data->cmd.getValue("");
    OATPP_LOGI("Comando a ejecutar: %s", cmd.c_str());
    nlohmann::json jsonResponse;
    validarCampos(data);
    
    StatusBematech status = leerStatus();
    if (!(status.sts3.find("OK") != std::string::npos))
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, status.sts3);

    if ((status.sts2.find("ERROR") != std::string::npos)) // ! TRY TO RESET
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, status.sts2);

    if (!(status.sts2.find("EN ESPERA") != std::string::npos) && !(status.sts2.find("NO FISCAL") != std::string::npos))
        anularDocumentoFiscal();

    if (status.sts2.find("NO FISCAL") != std::string::npos)
        cerrarDocumentoNoFiscal();

    if ("reporteX-ver" == cmd || "reporteX-imprimir" == cmd || "reporte_x" == cmd)
    {
        resultado = reporteX(data);
    }
    else if ("reporteZ-ver" == cmd || "reporteZ-imprimir" == cmd || "reporte_z" == cmd)
    {
        resultado = reporteZ();
    }
    else if ("facturar" == cmd)
    {
        resultado = facturar(data, true);
    }
    else if ("notaCredito" == cmd)
    {
        resultado = facturar(data, false);
    }
    else if ("notaDebito" == cmd)
    {
        resultado = facturarNotaDebito(data);
    }
    else if ("textoNF" == cmd)
    {
        resultado = documentoNoFiscal(data);
    }
    else if ("status" == cmd)
    {
        status = leerStatus();
        jsonResponse["estado_fiscal"] = status.sts2;
        jsonResponse["estado_impresora"] = status.sts3;
    }
    else if ("fuente" == cmd)
    {
        configurarFuente();
    }
    else if ("fechaYhora" == cmd)
    {
        jsonResponse["fechaYhora"] = leerFechaYhora();
    }
    else if ("serial" == cmd)
    {
        jsonResponse["serial"] = leerSerial();
    }
    else if ("rif" == cmd)
    {
        jsonResponse["rif"] = leerRIF();
    }
    else if ("getSubtotales" == cmd)
    {
       resultado = "TEST"; 
    }
    else
    {
        throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "Comando desconocido.");
    }

    resultado = resultado.empty() ? jsonResponse.dump() : resultado;
    OATPP_LOGI("-COMANDO EJECUTADO","-");
    return resultado;
}

void BematechPrinter::configurarFuente()
{
    OATPP_LOGI("CONFIGURANDO FUENTE","...");
    std::string comando = ":230";
    OATPP_LOGI("->FUENTE CONFIGURADA","<-");
    prepararYenviarComando(comando);
}

std::string BematechPrinter::cerrarDocumentoFiscal()
{
    OATPP_LOGI("CERRANDO DOCUMENTO FISCAL","...");
    std::string comando = this->hex2ascii("1B") + this->hex2ascii("0A");
    prepararYenviarComando(comando);
    OATPP_LOGI("CERRANDO DOCUMENTO FISCAL","");
    return "";
}

void BematechPrinter::anularDocumentoFiscal()
{
    OATPP_LOGI("ANULANDO DOCUMENTO FISCAL","...");
    std::string comando = this->hex2ascii("1B") + this->hex2ascii("0E");
    prepararYenviarComando(comando);
    OATPP_LOGI("->DOCUMENTO FISCAL ANULADO","<-");
}

std::string BematechPrinter::cerrarDocumentoNoFiscal()
{
    OATPP_LOGI("CERRANDO DOCUMENTO FISCAL","...");
    std::string comando = "N3", respuesta;
    try
    {
        prepararYenviarComando(comando);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    OATPP_LOGI("->DOCUMENTO FISCAL CERRADO","<-");
    return "";
}


std::string BematechPrinter::documentoNoFiscal(PrinterDto *dat)
{
    OATPP_LOGI("GENERANDO DOCUMENTO NO FISCAL","...");
    auto items = dat->textoNF.get();
    std::string texto, respuesta;

    //START ABRIR DOCUMENTO NO FISCAL
    std::string comando = "N0";
    prepararYenviarComando(comando);
    //END FISCAL DOC 

    //START Printer line in Document No Fiscal
    for (auto i = items->begin(); i != items->end(); ++i)
    {
        comando = "N1" + this->SEPARADOR;
        comando += i->getValue(""); //linea de comentario
        comando += this->SEPARADOR + "0"; //Tamaño (opcional)
        comando += this->SEPARADOR + "0"; // Alineación: IZQUIERDA (“0”), CENTRADO (“1”) o DERECHA (“2”)
        comando += this->SEPARADOR + "0"; //Atributo: DEFAULT(0), FONTB (0x1), BOLD (0x2), UNDERLINE (0x4) y/o REVERSE (0x8)
        prepararYenviarComando(comando);
    }
    //END  Printer line in Document No Fiscal
    respuesta = cerrarDocumentoNoFiscal();
    OATPP_LOGI("->","DOCUMENTO NO FISCAL GENERADO<-");
    return "DOCUMENTO NO FISCAL GENERADO EXITOSAMENTE!";
}

void BematechPrinter::lineaComentario(std::string texto)
{
    OATPP_LOGI("CREANDO LINEA DE COMENTARIO","...");
    std::string comando = "F7" + this->SEPARADOR + fillString(texto.substr(0, 48), 48);

    prepararYenviarComando(comando);
    OATPP_LOGI("->","LINEA DE COMENTARIO CREADA<-");
}

std::string BematechPrinter::leerFechaYhora()
{
    OATPP_LOGI("LEYENDO FECHA Y HORA","...");
    std::string comando, respuesta, resultado, anio, mes, dia, hora, minuto, segundo;
    comando = this->hex2ascii("1b") + this->hex2ascii("23") + this->hex2ascii("17");
    respuesta = convertBytesToString(convertStringToBytes(prepararYenviarComando(comando.c_str())));
    auto respuesta_ = splitString(respuesta, ' ');
    if (respuesta_.size() == 9)
    {

        anio = "20"+respuesta_[3];
        mes = respuesta_[2];
        dia = respuesta_[1];
        hora = respuesta_[4];
        minuto = respuesta_[5];
        segundo = respuesta_[6];

        resultado = anio + "/" + mes + "/" + dia;
        hora +=":"+minuto+":"+segundo; 
        resultado += " " + hora;
    }
    OATPP_LOGI("->","LECTURA DE FECHA Y HORA TERMINADA <-");
    return resultado;
}

std::string BematechPrinter::configPrinterMP(){
        //CONFIGURAR METODOS DE PAGO
        OATPP_COMPONENT(oatpp::Object<PrinterConfig>, printerConfig);
        OATPP_ASSERT_HTTP((printerConfig && printerConfig->Bematech), Status::CODE_400, "Debe configurar los metodos de pago en printer_config.json");
        OATPP_ASSERT_HTTP(printerConfig->Bematech->formasPago->size() > 0, Status::CODE_400, "Debe configurar los metodos de pago en printer_config.json");
        FormasPago::Wrapper formasPago = nullptr;
        for (auto &&metodos : *printerConfig->Bematech->formasPago.get())
        {
            if (metodos->serial.getValue("") == this->device->serial.getValue(""))
            {
                formasPago = metodos;
                OATPP_LOGD("METODOS DE PAGO ENCONTRADOS...", "%s", metodos->serial.getValue("").c_str());
                break;
            }
        }
        if (!formasPago)
        {
            for (auto &&metodos : *printerConfig->Bematech->formasPago.get())
            {
                if (metodos->serial.getValue("").empty() || metodos->serial.getValue("") == "*")
                {
                    formasPago = metodos;
                    break;
                }
            }
        }

        for (size_t i = 0; i <= 14; i++)
        {
            if(i==0){
                auto fp1 = formasPago->fp_01.getValue("EFECTIVO");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "0";
                cmd += this->SEPARADOR + fp1;
                auto resOMP = prepararYenviarComando(cmd);
                //START: VALIDAR "CONFIG METODOS DE PAGO DISPONIBLE"
                auto resOMP_split = splitString(resOMP, '\x1c');
                OATPP_LOGI("Validation Config MP [res_split]:", "%s", resOMP_split[0].substr(1,4).c_str());
                 if(resOMP_split[0].substr(1,4) == "1542"){
                    OATPP_ASSERT_HTTP(false, Status::CODE_400, "ERROR: PRIMERO DEBE GENERAR UN 'REPORTE Z'");
                 }
                 //END: VALIDAR "CONFIG METODOS DE PAGO DISPONIBLE"
            }
            if(i==1){
                auto fp2 = formasPago->fp_02.getValue("DEBITO");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "1";
                cmd += this->SEPARADOR + fp2;
                prepararYenviarComando(cmd);
            }
            if(i==2){
                auto fp3 = formasPago->fp_03.getValue("CREDITO");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "2";
                cmd += this->SEPARADOR + fp3;
                prepararYenviarComando(cmd);
            }
            if(i==3){
                auto fp4 = formasPago->fp_04.getValue("PAGO MOVIL");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "3";
                cmd += this->SEPARADOR + fp4;
                prepararYenviarComando(cmd);
            }
            if(i==4){
                auto fp5 = formasPago->fp_05.getValue("DIVISA");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "4";
                cmd += this->SEPARADOR + fp5;
                prepararYenviarComando(cmd);
            }
            if(i==5){
                auto fp6 = formasPago->fp_06.getValue("ZELLE");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "5";
                cmd += this->SEPARADOR + fp6;
                prepararYenviarComando(cmd);
            }
            if(i==6){
                auto fp7 = formasPago->fp_07.getValue("CESTA TICKECT");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "6";
                cmd += this->SEPARADOR + fp7;
                prepararYenviarComando(cmd);
            }
            if(i==7){
                auto fp8 = formasPago->fp_08.getValue("CUPON");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "7";
                cmd += this->SEPARADOR + fp8;
                prepararYenviarComando(cmd);
            }
            if(i==8){
                auto fp9 = formasPago->fp_09.getValue("VISA");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "8";
                cmd += this->SEPARADOR + fp9;
                prepararYenviarComando(cmd);
            }
            if(i==9){
                auto fp10 = formasPago->fp_10.getValue("CIRRUS");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "9";
                cmd += this->SEPARADOR + fp10;
                prepararYenviarComando(cmd);
            }
            if(i==10){
                auto fp11 = formasPago->fp_11.getValue("AMERICAN EXPRESS");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "10";
                cmd += this->SEPARADOR + fp11;
                prepararYenviarComando(cmd);
            }
            if(i==11){
                auto fp12 = formasPago->fp_12.getValue("CHEQUE");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "11";
                cmd += this->SEPARADOR + fp12;
                prepararYenviarComando(cmd);
            }
            if(i==12){
                auto fp13 = formasPago->fp_13.getValue("DINNERS CLUB");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "12";
                cmd += this->SEPARADOR + fp13;
                prepararYenviarComando(cmd);
            }
            if(i==13){
                auto fp14 = formasPago->fp_14.getValue("TARJETA REGALO");
                std::string cmd = "C10";
                cmd += this->SEPARADOR + "13";
                cmd += this->SEPARADOR + fp14;
                prepararYenviarComando(cmd);
            }
        }
        
        
        printDTO(formasPago);

    //FINALIZAR CONFIGURACION

//    //OBTENER CONFIG METHODS PAY
//    std::string pm[13];
//    for (size_t i = 0; i < 14; i++)
//    {
//     std::string cmd = "C9" + this->SEPARADOR + std::to_string(static_cast<int>(i));
//     auto resCmd = prepararYenviarComando(cmd);
//     pm[i] = resCmd;
//    }
//    //FINALIZAR OBTENER CONFIG METHODS PAY
    return "Success!";
}

std::string BematechPrinter::reporteX(bool imprimir)
{
    OATPP_LOGI("GENERANDO REPORTE X","...");
    std::string comando, resultado;
    comando = this->hex2ascii("1b") + this->hex2ascii("06");
    try
    {
        resultado = prepararYenviarComando(comando);
    }
    catch (...)
    {
        OATPP_LOGE("ERROR: ","FALLO AL ENVIAR COMANDO DE REPORTE X");
    }

    resultado = analizarReporteXZ("reporte_x", resultado);
    OATPP_LOGI("->","REPORTE X GENERADO<-");
    return resultado;
}

std::string BematechPrinter::reporteZ()
{
    OATPP_LOGI("GENERANDO REPORTE Z","...");
    std::string comando, resultado;
    comando = this->hex2ascii("1B") + this->hex2ascii("05"); 
    resultado = prepararYenviarComando(comando);
    resultado = convertBytesToString(convertStringToBytes(resultado));
    OATPP_LOGI("Resultado de reporte_z","%s", resultado.c_str());
    return resultado;
}

std::string BematechPrinter::facturarNotaDebito(PrinterDto *dat)
{
std::string cmd, params, resultado;
    auto fechaYhora = leerFechaYhora();
    bool docFiscal = true;
    // auto fechaYhora = "2024/07/19 13:17:00";
    OATPP_LOGI("FECHA Y HORA ACTUAL", "%s", fechaYhora);
    if(docFiscal){
        // Comando para apertura de factura
        cmd = this->hex2ascii("1B") + this->hex2ascii("00");
        // Nombre del cliente
        cmd += this->fillString(dat->factura->cliente->nombre.getValue("Contado"), 41, ' ');
        //RIF del cliente
        cmd += this->fillString(dat->factura->cliente->rif.getValue("------"), 18, ' ');
        // Dirección del cliente
        cmd += this->fillString(" ", 133, ' ');
    }else if(docFiscal == false){
        // Comando para apertura de factura
        cmd = this->hex2ascii("1B") + this->hex2ascii("59");
        // Nombre del cliente
        cmd += this->fillString(dat->factura->cliente->nombre.getValue("Contado"), 41, ' ');
        //Serial de la factura
        cmd += this->fillString(dat->factura->notaFiscal->serialFiscal.getValue("--------------"), 15, ' ');
        OATPP_LOGI("NC-SERIAL ===>","%s", dat->factura->notaFiscal->serialFiscal.getValue("--------------").c_str());
        //RIF del cliente
        cmd += this->fillString(dat->factura->cliente->rif.getValue("------"), 18, ' ');
        auto fecha_hora = convertirFechaYHora(dat->factura->notaFiscal->fecha.getValue("0000-00-00"), dat->factura->notaFiscal->hora.getValue("00:00:00"));
        OATPP_LOGI("NC-FECHA_HORA ===>","%s||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           ", fecha_hora.c_str());
        //Fecha de emisión
        cmd += this->fillString(fecha_hora.substr(0,5), 6, '0');
        OATPP_LOGI("NC-FECHA ===>","%s", fecha_hora.substr(0,5).c_str());
        //Hora de emisión
        cmd += this->fillString(fecha_hora.substr(6,11), 6, '0');
        OATPP_LOGI("NC-HORA ===>","%s", fecha_hora.substr(6,11).c_str());
        //COO
        cmd += this->fillNumbToString(dat->factura->notaFiscal->factura.getValue(000000), 6, 0, false);
    }
    resultado = prepararYenviarComando(cmd);
    resultado = convertBytesToString(convertStringToBytes(resultado));
    OATPP_LOGI("Resultado de apertura","%s", resultado.c_str());

    // //START ITEMS
    auto items = dat->factura->items.get();
    for(auto i = items->begin(); i != items->end(); i++){
       cmd = this->hex2ascii("1b") + this->hex2ascii("3E") + this->hex2ascii("47") + this->hex2ascii("33");
       // Indice de la situación tributaria
       if(i->get()->tasa == "E"){
           cmd += "II";
       }else if(i->get()->tasa == "G"){
           cmd += "01";
       }else if(i->get()->tasa == "R"){
           cmd += "04";
       }else if(i->get()->tasa == "A"){
           cmd += "02";
       }else if(i->get()->tasa == "P"){
           cmd += "05";
       }else{
           OATPP_ASSERT_HTTP(false, Status::CODE_400, "Error: Tipo de impuesto no registrado");
       }
       cmd += fillNumbToString(i->get()->precio.getValue(0.00), 14, 3, false);
       cmd += fillNumbToString((i->get()->cantidad.getValue(1)*1000), 7, 0, false);
       cmd += fillNumbToString(i->get()->descuento.getValue(0.00), 10, 3, false);
       cmd += "0000000000";
       cmd += "01";
       cmd += "00000000000000000000";
       cmd += "I ";
       cmd +=  i->get()->codigo.getValue("00000000") + this->hex2ascii("00"); 
       cmd += i->get()->producto.getValue("Producto") + this->hex2ascii("00"); 
       resultado = prepararYenviarComando(cmd);
       resultado = convertBytesToString(convertStringToBytes(resultado));
       OATPP_LOGI("Resultado de venta","%s", resultado.c_str());
    }

    std::string monto = "00000000000000";
    bool isigtf = false;
    if (!dat->factura->pagos->empty())
    {
        float monto_igtf =  0.00;
        auto item = dat->factura->pagos.get();
        for (auto i = item->begin(); i != item->end(); i++)
        {
            if (i->get()->impuesto != nullptr && i->get()->impuesto->igtf.getValue(false) == true)
            {
                monto_igtf += i->get()->monto.getValue(0.00);
                isigtf = true;
            }
        }
        monto = this->fillNumbToString(monto_igtf, 14, 2, false);
    }
    OATPP_LOGI("MONTO - CIERRE","%s", monto.c_str());
    
    if(isigtf){
        //START CIERRE FACTURA
        cmd = this->hex2ascii("1b") + this->hex2ascii("20") + this->hex2ascii("4A") + this->hex2ascii("4B");
        cmd += monto;
        resultado = prepararYenviarComando(cmd);
        resultado = convertBytesToString(convertStringToBytes(resultado));
        OATPP_LOGI("Resultado de Subtotal","%s", resultado.c_str());
        //END CIERRE FACTURA
    }else{
        //START CIERRE FACTURA
        cmd = this->hex2ascii("1b") + this->hex2ascii("20");
        cmd += "i";
        cmd += "00000000000000";
        resultado = prepararYenviarComando(cmd);
        resultado = convertBytesToString(convertStringToBytes(resultado));
        OATPP_LOGI("Resultado de Subtotal","%s", resultado.c_str());
        //END CIERRE FACTURA
    }

    //START PAGOS
    pagos(dat->factura->pagos);
    //END PAGOS

    // //START CIERRE
    cmd = this->hex2ascii("1b") + this->hex2ascii("22");
    cmd += "GRACIAS POR SU COMPRA!";
    resultado = prepararYenviarComando(cmd);
    resultado = convertBytesToString(convertStringToBytes(resultado));
    OATPP_LOGI("Resultado de cierre","%s", resultado.c_str());

    //COMFORMADO DATA DE DOCUMENTO EMITIDO
    auto numDoc = docFiscal ? 1:0;
    auto comando = dat->cmd.getValue("ERROR");
    auto numFactura = verUltimaFactura();
    auto rawSubtotal = verSubtotal();
    resultado = analizarFactura(comando, std::stoi(numFactura), fechaYhora, rawSubtotal, numDoc);

    return resultado;
}

std::string BematechPrinter::facturar(PrinterDto *dat, bool docFiscal){
std::string cmd, params, resultado;
std::string total_venta_g="0";
std::string total_impuesto_g="0";
std::string total_venta_r="0";
std::string total_impuesto_r="0";
std::string total_venta_a="0";
std::string total_impuesto_a="0";
std::string total_venta_p="0";
std::string total_igtf="0";
    auto fechaYhora = leerFechaYhora();
    // auto fechaYhora = "2024/07/19 13:17:00";
    OATPP_LOGI("FECHA Y HORA ACTUAL", "%s", fechaYhora);
    if(docFiscal){
        // Comando para apertura de factura
        cmd = this->hex2ascii("1B") + this->hex2ascii("00");
        // Nombre del cliente
        cmd += this->fillString(dat->factura->cliente->nombre.getValue("Contado"), 41, ' ');
        //RIF del cliente
        cmd += this->fillString(dat->factura->cliente->rif.getValue("------"), 18, ' ');
        // Dirección del cliente
        cmd += this->fillString(" ", 133, ' ');
    }else if(docFiscal == false){
        // Comando para apertura de factura
        cmd = this->hex2ascii("1B") + this->hex2ascii("59");
        // Nombre del cliente
        cmd += this->fillString(dat->factura->cliente->nombre.getValue("Contado"), 41, ' ');
        //Serial de la factura
        cmd += this->fillString(dat->factura->notaFiscal->serialFiscal.getValue("--------------"), 15, ' ');
        OATPP_LOGI("NC-SERIAL ===>","%s", dat->factura->notaFiscal->serialFiscal.getValue("--------------").c_str());
        //RIF del cliente
        cmd += this->fillString(dat->factura->cliente->rif.getValue("------"), 18, ' ');
        auto fecha_hora = convertirFechaYHora2(dat->factura->notaFiscal->fecha.getValue("0000-00-00"), dat->factura->notaFiscal->hora.getValue("00:00:00"));
        OATPP_LOGI("NC-FECHA_HORA ===>","%s", fecha_hora.c_str());
        //Fecha de emisión
        cmd += this->fillString(fecha_hora.substr(0,5), 6, '0');
        OATPP_LOGI("NC-FECHA ===>","%s", fecha_hora.substr(0,5).c_str());
        //Hora de emisión
        cmd += this->fillString(fecha_hora.substr(6,11), 6, '0');
        OATPP_LOGI("NC-HORA ===>","%s", fecha_hora.substr(6,11).c_str());
        //COO
        cmd += this->fillNumbToString(dat->factura->notaFiscal->factura.getValue(000000), 6, 0, false);
    }
    resultado = prepararYenviarComando(cmd);
    resultado = convertBytesToString(convertStringToBytes(resultado));
    OATPP_LOGI("Resultado de apertura","%s", resultado.c_str());

    // //START ITEMS
    auto items = dat->factura->items.get();
    for(auto i = items->begin(); i != items->end(); i++){
       cmd = this->hex2ascii("1b") + this->hex2ascii("3E") + this->hex2ascii("47");
       // Indice de la situación tributaria
       if(i->get()->tasa == "E"){
           cmd += "II";
       }else if(i->get()->tasa == "G"){
           cmd += "01";
           total_impuesto_g = std::to_string(std::stof(total_impuesto_g) +  (i->get()->precio.getValue(0.00) * 0.16));
           total_venta_g = std::to_string(std::stof(total_venta_g) +  i->get()->precio.getValue(0.00));
       }else if(i->get()->tasa == "R"){
           cmd += "04";
           total_impuesto_r = std::to_string(std::stof(total_impuesto_r) +  (i->get()->precio.getValue(0.00) * 0.08));
           total_venta_r = std::to_string(std::stof(total_venta_r) +  i->get()->precio.getValue(0.00));
       }else if(i->get()->tasa == "A"){
           cmd += "02";
           total_impuesto_a = std::to_string(std::stof(total_impuesto_a) +  (i->get()->precio.getValue(0.00) * 0.32));
           total_venta_a = std::to_string(std::stof(total_venta_a) +  i->get()->precio.getValue(0.00));
       }else if(i->get()->tasa == "P"){
           cmd += "05";
       }else{
           OATPP_ASSERT_HTTP(false, Status::CODE_400, "Error: Tipo de impuesto no registrado");
       }
       cmd += fillNumbToString(i->get()->precio.getValue(0.00), 14, 3, false);
       cmd += fillNumbToString((i->get()->cantidad.getValue(1)*1000), 7, 0, false);
       cmd += fillNumbToString(i->get()->descuento.getValue(0.00), 10, 3, false);
       cmd += "0000000000";
       cmd += "01";
       cmd += "00000000000000000000";
       cmd += "I ";
       cmd +=  i->get()->codigo.getValue("00000000") + this->hex2ascii("00"); 
       cmd += i->get()->producto.getValue("Producto") + this->hex2ascii("00"); 
       resultado = prepararYenviarComando(cmd);
       resultado = convertBytesToString(convertStringToBytes(resultado));
       OATPP_LOGI("Resultado de venta","%s", resultado.c_str());
    }

    std::string monto = "00000000000000";
    bool isigtf = false;
    if (!dat->factura->pagos->empty())
    {
        float monto_igtf =  0.00;
        auto item = dat->factura->pagos.get();
        for (auto i = item->begin(); i != item->end(); i++)
        {
            if (i->get()->impuesto != nullptr && i->get()->impuesto->igtf.getValue(false) == true)
            {
                monto_igtf += i->get()->monto.getValue(0.00);
                isigtf = true;
            }
        }
        monto = this->fillNumbToString(monto_igtf, 14, 2, false);
    }
    OATPP_LOGI("MONTO - CIERRE","%s", monto.c_str());
    
    if(isigtf){
        //START CIERRE FACTURA
        cmd = this->hex2ascii("1b") + this->hex2ascii("20") + this->hex2ascii("4A") + this->hex2ascii("4B");
        cmd += monto;
        resultado = prepararYenviarComando(cmd);
        resultado = convertBytesToString(convertStringToBytes(resultado));
        OATPP_LOGI("Resultado de Subtotal","%s", resultado.c_str());
        //END CIERRE FACTURA
    }else{
        //START CIERRE FACTURA
        cmd = this->hex2ascii("1b") + this->hex2ascii("20");
        cmd += "i";
        cmd += "00000000000000";
        resultado = prepararYenviarComando(cmd);
        resultado = convertBytesToString(convertStringToBytes(resultado));
        OATPP_LOGI("Resultado de Subtotal","%s", resultado.c_str());
        //END CIERRE FACTURA
    }

    //START PAGOS
    pagos(dat->factura->pagos);
    //END PAGOS

    // //START CIERRE
    cmd = this->hex2ascii("1b") + this->hex2ascii("22");
    cmd += "GRACIAS POR SU COMPRA!";
    resultado = prepararYenviarComando(cmd);
    resultado = convertBytesToString(convertStringToBytes(resultado));
    OATPP_LOGI("Resultado de cierre","%s", resultado.c_str());

    //COMFORMADO DATA DE DOCUMENTO EMITIDO
    auto numDoc = docFiscal ? 1:0;
    auto comando = dat->cmd.getValue("ERROR");
    auto numFactura = verUltimaFactura();
    auto rawSubtotal = verSubtotal();
    auto total_monto_igtf = std::stof(monto) * 0.07;

    rawSubtotal = rawSubtotal + "|" + this->fillString(total_venta_g, 14, '0') + "|" + this->fillString(total_impuesto_g, 14, '0') + "|" + this->fillString(total_venta_r, 14, '0') +  "|" + this->fillString(total_impuesto_r, 14, '0')  + "|" + this->fillString(total_venta_a, 14, '0') + "|" + this->fillString(total_impuesto_a, 14, '0') + "|" + this->fillString(total_venta_p, 14, '0') + "|" +  this->fillString(std::to_string(total_monto_igtf), 14, 2) + "|" + "00000000000000"; 
    resultado = analizarFactura(comando, std::stoi(numFactura), fechaYhora, rawSubtotal, numDoc);

    return resultado;
}

float BematechPrinter::montosIGTF(oatpp::DTO::List<oatpp::Object<Pago>> pagos){
    float montos_igtf = 0.00;
    if(!pagos.get()->empty())
    {
        auto item = pagos.get();
        for(auto i = item->begin(); i != item->end(); i++){
            if (i->get()->impuesto != nullptr || i->get()->impuesto->igtf.getValue(false) == true)
               {
                montos_igtf += i->get()->monto.getValue(0.00);
               }
        }
    }
    OATPP_LOGI("Montos IGTF","%.2f", montos_igtf);
    return montos_igtf;
}

std::string BematechPrinter::items(oatpp::DTO::List<oatpp::Object<Item>> items) { throw; }

std::string BematechPrinter::subtotal(oatpp::DTO::List<oatpp::Object<Pago>> pagos)
{
    OATPP_LOGD("PROCESANDO SUBTOTAL", "...");
    auto device = this->getAllDevices();
    std::string comando, rawSubtotal;
    bool igtf = contieneIGTF(pagos);
    float amountIGTF = cantidadPagoIGTF(pagos);

    OATPP_LOGI("amountIGTF : "," %F",amountIGTF);


    if (igtf)
    {
        OATPP_LOGD("IGTF IN SUBTOTAL ==>", "TRUE")
        comando = "F2" + this->SEPARADOR;
        comando += "1";
        comando += this->SEPARADOR + fillNumbToString(amountIGTF, 16, 2);
        comando += this->SEPARADOR + "1";
        comando += this->SEPARADOR + "S";
    }
    else
    {
        comando = "F2" + this->SEPARADOR;
        comando += "1";
    }

    rawSubtotal = prepararYenviarComando(comando);
    OATPP_LOGD("SUBTOTTAL PROCESADO", "---[HECHO]---");
    return rawSubtotal;
}

std::string BematechPrinter::pagos(oatpp::DTO::List<oatpp::Object<Pago>> pagos)
{
    OATPP_LOGD("PROCESANDO PAGOS", "...");
    std::string comando, rawSubtotal, resultado;

    if (!pagos.get()->empty())
    {
        OATPP_LOGI("Condicion--->", "PAGOS NOT EMPTY");
        auto item = pagos.get();
        auto countI = 0;
        for (auto i = item->begin(); i != item->end(); i++)
        {
            comando = this->hex2ascii("1b") + this->hex2ascii("5A");

            comando += i->get()->metodo_id.getValue(00) <= 9 ? ("0" + std::to_string(i->get()->metodo_id.getValue(00))): std::to_string(i->get()->metodo_id.getValue(00));
            OATPP_LOGI("ID M PAGO","%s", comando.c_str());
            comando += this->fillNumbToString(i->get()->monto.getValue(0.00), 14, 2, false);
            comando += "01";
            comando += "";
            resultado = prepararYenviarComando(comando);
            resultado = convertBytesToString(convertStringToBytes(resultado));
            OATPP_LOGD("PAGO-RESULTADO", "%s", resultado.c_str());
        }
    }
    OATPP_LOGD("PAGOS PROCESADOS", "---[HECHO]--");
    return "success";
}

void BematechPrinter::cabecera(oatpp::DTO::List<oatpp::String> cabecera)
{
    OATPP_LOGI("entrando a BematechPrinter::cabecera:  ", "");
    for (auto i = cabecera->begin(); i != cabecera->end(); i++)
    {
        if (!i->getValue("").empty())
        {
            lineaComentario(i->getValue(""));
        }
    }
    lineaComentario("");
    auto device = this->getAllDevices();
    if (device->items[0]->model.getValue("") == "DTP80-i" || device->items[0]->model.getValue("") == "DTP80-i")
    {
        lineaComentario("Articulo                          Monto");
    }
    else
    {
        lineaComentario("Articulo                                   Monto");
    }

    lineaComentario(fillString("", 48, '-'));

    OATPP_LOGI("saliendo de BematechPrinter::cabecera:  ", "");
}

void BematechPrinter::pie(oatpp::DTO::List<oatpp::String> pie)
{
    OATPP_LOGI("entrando a BematechPrinter::pie: ", "");
    std::string texto;
    int caracteresPorLinea = 48;
    int lineasMax = 10;
    for (auto i = pie->begin(); i != pie->end(); i++)
        texto += i->getValue("") + ". ";

    this->trim(texto);

    for (size_t i = 0; i < texto.size() && (i / caracteresPorLinea) < lineasMax; i += caracteresPorLinea)
        lineaComentario(trim(texto.substr(i, caracteresPorLinea)));

    OATPP_LOGI("saliendo de BematechPrinter::pie: ", "");
}

std::string BematechPrinter::prepararComando(std::string comando)
{
    std::vector<uint8_t> bytes;
    auto numbytes=0;

    numbytes = calcularNumeroBytes(comando);
    cal_NBL_NBH_CSL_CSH(numbytes, csl, csh);

    comando += this->hex2ascii(intToHex(csl)) + this->hex2ascii(intToHex(csh));

    bytes = convertStringToBytes(comando);
    cal_NBL_NBH_CSL_CSH(bytes.size(), nbl, nbh);
    
    return this->STX + this->hex2ascii(intToHex(nbl)) + this->hex2ascii(intToHex(nbh)) + comando; 
}


std::string BematechPrinter::enviarComando(std::string comando)
{
    OATPP_LOGI("SERVICIO Bematech: ", "------START: enviarComando(cmd)---------");
        std::string respuesta;
        std::vector<unsigned char> bytes;
        writePort(comando);
        respuesta = this->readPort();
        this->writeLog(comando, respuesta, "Bematech");
    
        if (respuesta.empty())
            throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "SE HA PERDIDO COMUNICACIÓN CON EL DISPOSITIVO");
    
        if (respuesta.find(this->NAK) != std::string::npos)
            throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "SE HA ENCONTRADO UN ERROR AL ENVIAR EL COMANDO");
    OATPP_LOGI("SERVICIO Bematech: ", "------FINISHED: enviarComando(cmd)---------");   
        return respuesta;
}



StatusBematech BematechPrinter::leerStatus()
{
    OATPP_LOGI("START::", "LECTURA DE STATUS INICIADA");
    StatusBematech status;
    std::string comando;
    std::string respuesta;

    comando = this->hex2ascii("1B") + this->hex2ascii("13");

    respuesta = prepararYenviarComando(comando);
    respuesta = convertBytesToString(convertStringToBytes(respuesta));
    if(respuesta.size() < 7){
        respuesta.append(" 01 01");
    }
    auto splitRes = splitString(vectorToString(hexStringToDecimalArray(respuesta)), ' ');
    
    auto statusRes = splitRes[2]; 
    auto statusRes2 = splitRes[1];
    auto statusRes3 = splitRes[0];

    OATPP_LOGD("StatusRes_3 Print ==>", "%s", statusRes3.c_str());
    OATPP_LOGD("StatusRes_2 Print ==>", "%s", statusRes2.c_str());
    OATPP_LOGD("StatusRes_1 Print ==>", "%s", statusRes.c_str());

    try
    {
     //Obteniendo 'StatusBematech1'
     for(const auto& status1: status.StatusBematech1){
        if(statusRes == "0"){
            status.sts3 = "OK";
        }else if(statusRes == status1.first){
            status.sts3 = status1.second;
        }
     }
    //Obteniendo 'StatusBematech2'
    for(const auto& status2: status.StatusBematech2){
        if(statusRes2 == status2.first){
            status.sts2 = status2.second;
        }
     }
    //Obteniendo 'StatusBematech3'
    for(const auto& status3: status.StatusBematech3){
        if(statusRes3 == status3.first){
            status.sts1 = status3.second;
        }
     }

    }catch (...){
        OATPP_LOGE("[ERROR]"," FALLO AL INTENTAR OBTENER STATUS");
    }

    OATPP_LOGD("Status_3 Print ==>", "%s", status.sts1.c_str());
    OATPP_LOGD("Status_2 Print ==>", "%s", status.sts2.c_str());
    OATPP_LOGD("Status_1 Print ==>", "%s", status.sts3.c_str());

    OATPP_LOGI("END::", "LECTURA DE STATUS FINALIZADA");

    return status;
}


oatpp::Object<PageDto<oatpp::Object<Device>>> BematechPrinter::getAllDevices()
{
    OATPP_LOGI("CONSULTANDO LOS DISPOSITIVOS","...");
    auto result = oatpp::Object<PageDto<oatpp::Object<Device>>>::createShared();
    result->items = oatpp::data::mapping::type::DTO::Vector<Device::Wrapper>::createShared();
    result->count = unsigned(0);

    try
    {
        OATPP_COMPONENT(oatpp::Object<PrinterConfig>, printerConfig);
        OATPP_ASSERT_HTTP((printerConfig && printerConfig->devices), Status::CODE_400, "Debe configurar los dispositivos en printer_config.json");
        OATPP_ASSERT_HTTP(printerConfig->devices->size() > 0, Status::CODE_400, "Debe configurar los dispositivos en printer_config.json");

        // std::string url = "http://localhost:10110/info";

        // http::Request request(url);
        // const auto httpResponse = request.send("GET");
        // auto responseCode = httpResponse.status.code;
        // std::string response = std::string(httpResponse.body.begin(), httpResponse.body.end());
        // std::cout << url << " - " << httpResponse.status.code << " - " << httpResponse.status.reason << std::endl;
        // std::cout << response << std::endl;
        // auto response = "{\"app\":\"Liberty\",\"data\":{\"devices\":[{\"brand\":\"PNP\",\"model\":\"PF220\",\"serial\":\"EOG0003063\",\"type\":\"impresora\"}],\"fecha\":\"2022-06-20\",\"licencia\":\"X-Y-Z\",\"producto\":\"POS3DB\",\"razon_social\":\"PRUEBA\",\"rif\":\"J-1234567890\"},\"version\":\"1.0.0\"}";

        // nlohmann::json json = nlohmann::json::parse(response);
        // if (json["data"] != nullptr && json["data"]["devices"] != nullptr)
        // {
        result->count = printerConfig->devices->size();
        result->items = printerConfig->devices;

        // }
        // else
        // {
        //     OATPP_LOGE("getAllDevices", " LIBERTY NO HA SIDO INICIALIZADO");
        // }
    }
    catch (const std::exception &e)
    {
        OATPP_LOGE("getAllDevices", " %s", e.what());
    }
    OATPP_LOGI("->","Consulta de dispositivos finalizada <-");
    return result;
}

std::string BematechPrinter::leerSerial()
{
    OATPP_LOGI("LECTURA DE SERIAL 'Bematech':  ", "Obteniendo datos de Serializacion...");
    std::string cmd="ERROR", resultado = "ERROR", respuesta="ERROR";
    try{
        cmd = this->hex2ascii("1b") + this->hex2ascii("23") + this->hex2ascii("3A");
        resultado = prepararYenviarComando(cmd);
        respuesta = resultado.substr(1,(resultado.length()-3));
        respuesta.erase(std::remove_if(respuesta.begin(), respuesta.end(), ::isspace), respuesta.end());
        OATPP_LOGI("Resultado de Lectura de SERIAL","%s", respuesta.c_str());
        OATPP_LOGI("LECTURA DE serial 'Bematech':  ", "-[Obtenidos datos Serializacion <<serial Extraido>> ]-");
    }catch(const std::exception &e){
        OATPP_LOGE("LECTURA DE SERIAL: ", "%s", e.what());
        OATPP_ASSERT_HTTP(false, Status::CODE_400, "ERROR DE LECTURA DE SERIAL");
    }
    return respuesta;
}

std::string BematechPrinter::leerRIF()
{
    OATPP_LOGI("LECTURA DE RIF 'Bematech':  ", "Obteniendo RIF...");
    std::string cmd="ERROR", resultado = "ERROR", respuesta="ERROR";
    try{
        cmd = this->hex2ascii("1b") + this->hex2ascii("23") + this->hex2ascii("2A");
        resultado = prepararYenviarComando(cmd);
        respuesta = resultado.substr(1,(resultado.length()-3));
        respuesta.erase(std::remove_if(respuesta.begin(), respuesta.end(), ::isspace), respuesta.end());
        OATPP_LOGI("Resultado de Lectura de RIF","%s", respuesta.c_str());
        OATPP_LOGI("LECTURA DE RIF 'Bematech':  ", "-[<<RIF Extraido>> ]-");
    }catch(const std::exception &e){
        OATPP_LOGE("LECTURA DE RIF: ", "%s", e.what());
        OATPP_ASSERT_HTTP(false, Status::CODE_400, "ERROR DE LECTURA DE RIF");
    }
    return respuesta;
}

std::string BematechPrinter::analizarFactura(std::string cmd, std::string fecha, std::string rawSubtotal) { throw; }

std::string BematechPrinter::analizarFactura(std::string cmd, int nroFactura, std::string fecha, std::string rawSubtotal, int doc)
{
    OATPP_LOGI("ANALIZANDO FACTURA","...");
    nlohmann::json respuesta;
    OATPP_LOGI("FORMA IN rawSubtotal","%s", rawSubtotal.c_str());
    auto stringTotales = splitString(rawSubtotal, '|');
    auto auxRawSubtotal = stringTotales[0];
    if (!stringTotales.empty()) {
        stringTotales.erase(stringTotales.begin());
        stringTotales.erase(stringTotales.end()-1);
    }
    std::vector<std::string> camposSubtotal = {
        {"total_venta_g"},
        {"total_impuesto_g"},
        {"total_venta_r"},
        {"total_impuesto_r"},
        {"total_venta_a"},
        {"total_impuesto_a"},
        {"total_venta_p"}, 
        {"total_igtf"}
    };
    int i = 0;
    for (auto it = stringTotales.begin(); it != stringTotales.end(); ++it, i++)
    {

        try
        {   
            std::cout << "IT >> " << it->c_str() << " I >>" << i << std::endl; 
            std::cout << ">>>>>>>> SUBTOTAL >>> >>" <<camposSubtotal[i]<<"  =  "<< stof(std::string(it->c_str())) << std::endl; 
            if(camposSubtotal[i]=="total_igtf"){
                respuesta["factura"]["subtotales"][camposSubtotal[i]] = round2decimals(stof(std::string(it->c_str())) / 100.);
            }else {
                respuesta["factura"]["subtotales"][camposSubtotal[i]] = round2decimals(stof(std::string(it->c_str())));
            }
                
        }
        catch (const std::exception &e)
        {
            OATPP_LOGE("Analizar Factura", " %s: %s", it->c_str(), e.what());
        }
    }
        
        if(doc == 1){
            respuesta["factura"]["nro"] = nroFactura;
        }else if(doc == 0){
            // respuesta["Nota_de_Credito"]["nro"] = nroFactura;
            respuesta["factura"]["nro"] = nroFactura;
        }else if(doc == 2){
            // respuesta["Nota_de_Debito"]["nro"] = nroFactura;
            respuesta["factura"]["nro"] = nroFactura;
        }else{
            // respuesta["Documento"]["nro"] = nroFactura;
        }
        respuesta["cmd"] = cmd;
        respuesta["serialFiscal"] = this->device->serial;
        respuesta["fecha"] = fecha;
        respuesta["rawSubtotal"] = auxRawSubtotal;
        std::cout<<stringTotales[0] <<std::endl;

    OATPP_LOGI("->ANALISIS DE FACTURA COMPLETADO","<-");
    return respuesta.dump();
}

std::string BematechPrinter::analizarReporteXZ(std::string cmd, std::string rawReporteXZ)
{
    OATPP_LOGI("ANALIZANDO REPORTE XZ","...");
    nlohmann::json respuesta;

    respuesta["cmd"] = cmd;
    respuesta["rawResponse"] = rawReporteXZ;
    OATPP_LOGI("->","Analisis de reporte xz completado <-");
    return respuesta.dump();
}

/*
// // Función para obtener el valor de "price_old" en USD
// double getPriceOldUSD(const std::string& data) {
//     // Encuentra la posición de "usd"
//     size_t pos_usd = data.find("\"usd\"");
//     if (pos_usd == std::string::npos) {
//         std::cerr << "No se encontró la moneda USD en los datos proporcionados." << std::endl;
//         return -1; // Retorna un valor negativo para indicar un error
//     }

//     // Encuentra la posición de "price_old"
//     size_t pos_price_old = data.find("price_old", pos_usd);
//     if (pos_price_old == std::string::npos) {
//         std::cerr << "No se encontró el valor de price_old para USD en los datos proporcionados." << std::endl;
//         return -1; // Retorna un valor negativo para indicar un error
//     }

//     // Encuentra el valor de price_old en USD
//     size_t pos_colon = data.find(":", pos_price_old);
//     size_t pos_comma = data.find(",", pos_price_old);
//     std::string price_old_str = data.substr(pos_colon + 1, pos_comma - pos_colon - 1);

//     // Convierte el valor a double y devuelve
//     return std::stod(price_old_str);
// }

// // Función de escritura de datos recibidos
// size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* data) {
//     data->append((char*)contents, size * nmemb);
//     return size * nmemb;
// }

// double getCurrentRate(){

//     CURL* curl;
//     CURLcode res;
//     std::string data;
//     double price_old_usd = 0.00;

//     curl_global_init(CURL_GLOBAL_DEFAULT);
//     curl = curl_easy_init();
    
    
//     if (curl) {
//         curl_easy_setopt(curl, CURLOPT_URL, "https://pydolarvenezuela-api.vercel.app/api/v1/dollar/page?page=bcv");
//         curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
//         curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

//         res = curl_easy_perform(curl);
//         if (res != CURLE_OK) {
//             std::cerr << "Error al realizar la solicitud: " << curl_easy_strerror(res) << std::endl;
//         } else {
//             std::cout << "Datos recibidos: " << data << std::endl;
//             price_old_usd = getPriceOldUSD(data);
//             if (price_old_usd != -1) {
//                 std::cout << "El valor de price_old en USD es: " << price_old_usd << std::endl;
//             }
//         }

//         curl_easy_cleanup(curl);
//     }

//     curl_global_cleanup();

//     return price_old_usd;
// }

*/