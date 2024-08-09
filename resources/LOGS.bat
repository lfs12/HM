@echo off
setlocal enabledelayedexpansion
@echo off

set "archivo=resources\LOGS.txt"
set "tempfile=resources\temp.txt"
set "sqlite3_path=resources\sqlite3\sqlite3.exe"


echo Directorio actual: %CD%
:: Buscar la línea que contiene 'DTO' y 'RESPONSE' y guardarla en un archivo temporal
findstr /C:"DTO" /C:"RESPONSE" %archivo% > %tempfile%

:: Verificar si se encontraron las líneas en el archivo
if %errorlevel% neq 0 (
    echo No se encontraron las líneas 'DTO' y 'RESPONSE' en el archivo.
    exit /b
)

"%sqlite3_path%"  logs_hm.db "CREATE TABLE IF NOT EXISTS tabla_logs (id INTEGER PRIMARY KEY, dto TEXT, response TEXT);"
:: Leer el archivo temporal y extraer los valores de 'DTO' y 'RESPONSE'
for /f "tokens=1,* delims=:" %%a in (%tempfile%) do (
    if "%%a"=="DTO" (
        set "dto=%%b"
        set "dto=!dto:{= !"
        set "dto=!dto:}= !"
        set "dto=!dto:\",= !"
        set "dto=!dto:\\= /!"
        set "dto=!dto:\"= !"
        set "dto=!dto:"= !"
    ) else if "%%a"=="RESPONSE" (
        set "response=%%b"
        set "response=!response:{= !"
        set "response=!response:}= !"
        set "response=!response:\",= !"
        set "response=!response:\\= /!"
        set "response=!response:\"= !"
        set "response=!response:"= !"
    )
)

:: Verificar si se encontraron los valores de 'DTO' y 'RESPONSE'
if not defined dto (
    echo No se encontró el valor de 'DTO' en el archivo.
    exit /b
)
if not defined response (
    echo No se encontró el valor de 'RESPONSE' en el archivo.
    exit /b
)
:: Almacenar los valores en la base de datos SQLite
"%sqlite3_path%"  logs_hm.db "INSERT INTO tabla_logs (dto, response) VALUES ('!dto!', '!response!');"
:: Verificar si se ejecutó la inserción correctamente
if %errorlevel% neq 0 (
    echo Error al insertar los valores en la base de datos.
    exit /b
)
echo ============================DATA DE LOGS IN DB=================================
:: Realizar la consulta y formatear los resultados
"%sqlite3_path%" logs_hm.db "SELECT * FROM tabla_logs;"

:: Eliminar el archivo temporal
del %tempfile%
echo ===============================================================================
echo Proceso completado.

exit