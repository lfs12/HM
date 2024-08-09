#include "controller/PrinterController.hpp"
#include "controller/BasculeController.hpp"
#include "controller/AppController.hpp"
#include "service/AppService.hpp"

#include "AppComponent.hpp"
#include "InitHelper.hpp"
#include "ServiceComponent.hpp"
#include "licencia.hpp"

#include "oatpp/network/Server.hpp"

#include <chrono>
#include <thread>
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#endif
std::mutex mtx;

//START UTTILS
UINT_PTR timerID = 0; // ID del temporizador

VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    // Función de devolución de llamada del temporizador
    KillTimer(NULL, timerID); // Detener el temporizador
    EndDialog(GetActiveWindow(), 0); // Cerrar el cuadro de mensaje
}
//END UTILS

void validarLicencia(LicenciaService app)
{
    OATPP_LOGI("Iniciando validacion de licencia", "");
    while (true)
    {
        app.validar_licencia();

        if (LicenciaService::appStatus == LicenciaService::LicenciaStatus::verificacion_fallida || LicenciaService::appStatus == LicenciaService::LicenciaStatus::pendiente_activacion)
        {
            OATPP_LOGI("Task: ValidarLicencia", "Reintento en: 1 minuto...");
            std::this_thread::sleep_for(std::chrono::minutes(1));
        }
        else if (LicenciaService::appStatus == LicenciaService::LicenciaStatus::demo)
        {
            OATPP_LOGI("Task: ValidarLicencia", "Reintento en: 0 minuto...");
            std::this_thread::sleep_for(std::chrono::minutes(0));
        }
        else if (LicenciaService::appStatus == LicenciaService::LicenciaStatus::verificacion_fallida_data)
        {
            OATPP_LOGI("Task: ValidarLicencia", "Reintento en: 1 hora...");
            std::this_thread::sleep_for(std::chrono::hours(1));
        }
        else if (LicenciaService::appStatus == LicenciaService::LicenciaStatus::vencido)
        {
            OATPP_LOGI("Task: ValidarLicencia: LICENCIA VENCIDA.", "Reintento en: 1 hora...");
            MessageBox(NULL, "LICENCIA VENCIDA \n\n Reintento en 1 hora.", "Hardware-Manager", MB_OK | MB_ICONASTERISK);
            std::this_thread::sleep_for(std::chrono::hours(1));
        }
        else
        {
            if(LicenciaService::appStatus == LicenciaService::LicenciaStatus::por_vencerse){
                OATPP_LOGI("Task: ValidarLicencia: LICENCIA POR VECERSE"," Continuar normalmente.");
                MessageBox(NULL, "EL SERVICIO 'API manejador de impresora fiscal' EST\u00C1 POR VENCERSE \n\n Cont\u00E1ctese con su proveedor", "Hardware-Manager", MB_OK | MB_ICONASTERISK);
            }
            OATPP_LOGI("Task: ValidarLicencia", "Reintento en: 1 dia...");
            std::this_thread::sleep_for(std::chrono::hours(24));
        }
    }
}

void autoScanDevices()
{
    AppService *app = new AppService();
    int sleepTime;
    while (true)
    {
        app->checkDevices();
        sleepTime = app->portsUsed.size() < app->getAllDevices()->count.getValue(0) ? 5 : 30;
        sleepTime = app->getAllDevices()->count.getValue(0) == 0 ? 5 : sleepTime;

        OATPP_LOGI("Task: AutoScanDevices", "Reintento en: %d sec...", sleepTime);
        std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
    }
}

void run(const oatpp::base::CommandLineArguments &args)
{
    AppComponent appComponent(args);
    InitHelper initHelper;
    ServiceComponent serviceComponent;

    auto router = serviceComponent.httpRouter.getObject();
    oatpp::web::server::api::Endpoints docEndpoints;

    router->addController(AppController::createShared());

    docEndpoints.append(router->addController(PrinterController::createShared())->getEndpoints());
    docEndpoints.append(router->addController(BasculeController::createShared())->getEndpoints());
    docEndpoints.append(router->addController(AppController::createShared())->getEndpoints());

    LicenciaService appService;
    appService.appStatus = LicenciaService::LicenciaStatus::activado;
    // std::thread task(validarLicencia, appService);

    oatpp::network::Server server(serviceComponent.serverConnectionProvider.getObject(), serviceComponent.serverConnectionHandler.getObject());

    OATPP_LOGD("Server", "Running on port %s...", serviceComponent.serverConnectionProvider.getObject()->getProperty("port").toString()->c_str());
    // oatpp::String msgbox = "SERVICIO INICIADO \n\nAPI controlador de impresora fiscal\nPuerto: " + serviceComponent.serverConnectionProvider.getObject()->getProperty("port").toString();
    // // MessageBox(NULL, msgbox.get()->c_str(), "Hardware-Manager", MB_OK| MB_TOPMOST | MB_ICONINFORMATION);
    //     // Crear y escribir el script VBS en un archivo temporal
    // std::ofstream scriptFile("temp.vbs");
    // scriptFile << "Set objShell = CreateObject(\"WScript.Shell\")\n";
    // scriptFile << "objShell.Popup \"SERVICIO INICIADO API controlador de impresora fiscal Puerto: 5500\", 1, \"Hardware-Manager\", 64\n";
    // scriptFile << "WScript.Sleep 5000\n";
    // scriptFile << "WScript.Quit\n";
    // scriptFile.close();

    // // Ejecutar el script VBS
    // system("cscript temp.vbs");

    // // Eliminar el archivo temporal
    // remove("temp.vbs");

    // //Sonido de alerta
    // // Ruta al archivo de sonido de alerta
    // std::string soundFile = "C:\\Windows\\Media\\chimes.wav";

    // // Reproducir el sonido de alerta
    // PlaySound(soundFile.c_str(), NULL, SND_FILENAME);
    // //End Sonido de alerta
    
    
    server.run();
}

int main(int argc, const char *argv[])
{
#ifdef _WIN32
    // hide console
    if (!Tools::fileExists("consola.txt"))
        ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
#endif

    oatpp::base::Environment::init();

    run(oatpp::base::CommandLineArguments(argc, argv));

    oatpp::base::Environment::destroy();

    return 0;
}