
#ifndef InitHelper_hpp
#define InitHelper_hpp

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <filesystem>

#include "Tools.hpp"
class InitHelper
{
public:
  InitHelper()
  {
    std::string resourcesPath = "resources";
    if (!std::filesystem::is_directory("resources") || !std::filesystem::exists("resources"))
    {                                                 // Check if resources folder exists
      std::filesystem::create_directory("resources"); // create resources folder
    }
    generateComandaTemplate();
    generateLineaComandaTemplate();
    generatePrecuentaTemplate();
    generateConfigJson();
    generateCuadreCajaDetalladoTemplate();
    generateCuadreCajaResumidoTemplate();
  }

private:
  static void generateComandaTemplate()
  {
    std::string title = "resources/COMANDA";
    int sequence = 11;
    std::string ext = ".txt";
    for (size_t i = 1; i <= sequence; i++)
    {
      std::string it = title + (i < 10 ? "0" : "") + (i < 10 ? std::to_string(i) : "") + ext;
      if (!Tools::fileExists(it))
      {
        std::ofstream file;
        file.open(it);
        file << "ZONA [ZONA                            ]" << std::endl;
        file << "MESA [MESA                            ]" << std::endl;
        file << "[USER                                 ]" << std::endl;
        file << "No. DOCUMENTO [CORRELA                ]" << std::endl;
        file << "FECHA [FECHA     ] HORA [HORA    ]" << std::endl;
        file << "CUENTA [NDOC                          ]" << std::endl;
        file << "" << std::endl;
        file << "CANT  PRODUCTO" << std::endl;
        file << "==== ====================================" << std::endl;
        file << "[LINEA                                   ]   " << std::endl;
        file << "==== ===================================" << std::endl;
        file << "" << std::endl;
        file.close();
      }
    }
  }
  static void generatePrecuentaTemplate()
  {
    std::string title = "resources/PRECUENTA.txt";

    std::string it = title;
    if (!Tools::fileExists(it))
    {
      std::ofstream file;
      file.open(it);

      file << "[EMPRESA                                ]" << std::endl;
      file << "CLIENTE: [CLIENTE                       ]" << std::endl;
      file << "RIF / C.I: [RIF                         ]" << std::endl;
      file << "" << std::endl;
      file << "" << std::endl;
      file << "CUENTA [NDOC                           ]" << std::endl;
      file << "NRO. DE MESA: [MESA                    ]" << std::endl;
      file << "" << std::endl;
      file << "" << std::endl;
      file << "PRODUCTO             CANT          MONTO" << std::endl;
      file << "==================== ==== ==============" << std::endl;
      file << "[PRODU               ] [%4g CANT] [%8.2 MONTL] Bs" << std::endl;
      file << "                          [%8.2 REF  ] $" << std::endl;
      file << "==================== ==== ==============" << std::endl;
      file << "" << std::endl;
      file << "       SUBTOTAL:        [%11.2 TOTBR] Bs" << std::endl;
      file << "                        [%11.2 TOTBR_REF] $" << std::endl;
      file << "       SERVICIO: [SRV]    [%11.2 TOTSRV] Bs" << std::endl;
      file << "                        [%11.2 TOTSRV_REF] $" << std::endl;
      file << "          TOTAL:        [%11.2 NETOSINIVA] Bs" << std::endl;
      file << "                        [%11.2 NETOSINIVA_REF] $" << std::endl;
      file << "            IVA: [IVA]    [%11.2 TOIVA] Bs" << std::endl;
      file << "                        [%11.2 TOIVA_REF] $" << std::endl;
      file << "  MONTO A PAGAR:        [%11.2 APAGA] Bs" << std::endl;
      file << "TOTAL CON 3 % IGTF     [%11.2 APAGA_IGTF] Bs " << std::endl;
      file << "                       [%11.2 APAGA_REF_IGTF] $" << std::endl;

      file << "                        [%11.2 APAGA_REF] $" << std::endl;
      file << "" << std::endl;

      file.close();
    }
  }

  static void generateLineaComandaTemplate()
  {
    std::string title = "resources/LINEACOMANDA.txt";

    std::string it = title;
    if (!Tools::fileExists(it))
    {
      std::ofstream file;
      file.open(it);

      file << "[%1.2n CANTD] [DESCRIPCION                     ]";

      file.close();
    }
  }
  static void generateConfigJson()
  {
    std::string title = "resources/config.json";

    std::string it = title;
    if (!Tools::fileExists(it))
    {
      std::ofstream file;
      file.open(it);

      file << R"({)" << std::endl;
      file << R"(  "dev": {)" << std::endl;
      file << R"(    "host": "0.0.0.0",)" << std::endl;
      file << R"(    "port": 5400)" << std::endl;
      file << R"(  })" << std::endl;
      file << R"(})" << std::endl;

      file.close();
    }
  }

  static void generateCuadreCajaDetalladoTemplate()
  {
    std::string title = "resources/CUADRE_CAJA_DETALLADO.txt";

    std::string it = title;
    if (!Tools::fileExists(it))
    {
      std::ofstream file;
      file.open(it);
      file << R"([EMPRESA                              ])" << std::endl;
      file << R"(RIF: [RIF        ]   TLF: [TLF        ] )" << std::endl;
      file << R"([DIRECCION                                                                                                                                                          ])" << std::endl;
      file << R"(FECHA: [FECHA     ] [HORA  ])" << std::endl;
      file << R"()" << std::endl;
      file << R"(CUADRE DE CAJA)" << std::endl;
      file << R"()" << std::endl;
      file << R"(TASA: [%5.2 TASA])" << std::endl;
      file << R"(ESTACION: [ESTACION                ])" << std::endl;
      file << R"(DESDE: [DESDE                      ] )" << std::endl;
      file << R"(HASTA: [HASTA                      ])" << std::endl;
      file << R"()" << std::endl;
      file << R"(FACTURAS: [N_FACTURAS])" << std::endl;
      file << R"(DE: [FACT_PRI     ]    A: [FACT_ULT     ])" << std::endl;
      file << R"()" << std::endl;
      file << R"(DEVOLUCIONES: [N_DEVOLUCIONES])" << std::endl;
      file << R"(DE: [DEV_PRI     ]    A: [DEV_ULT     ])" << std::endl;
      file << R"()" << std::endl;
      file << R"(INGRESO BRUTO:      [%11.2 TOTAL_VENTA_BRUTO] Bs)" << std::endl;
      file << R"(DESCUENTOS:      [%11.2 DESCUENTOS] Bs)" << std::endl;
      file << R"()" << std::endl;
      file << R"(OPERACIONES DE CONTADO)" << std::endl;
      file << R"(VENTAS([%0.0 N_VENTAS_CONTADO])       [%11.2 TOTAL_VENTA_CONTADO_VENTA] Bs)" << std::endl;
      file << R"(DEVOL.([%0.0 N_DEVOLUCIONES_CONTADO])       [%11.2 TOTAL_VENTA_CONTADO_DEVOLUCION] Bs)" << std::endl;
      file << R"(TOTAL CONTADO:   [%14.2 TOTAL_VENTA_CONTADO] Bs)" << std::endl;
      file << R"()" << std::endl;
      file << R"(OPERACIONES A CREDITO)" << std::endl;
      file << R"(VENTAS([%0.0 N_VENTAS_CREDITO])       [%11.2 TOTAL_VENTA_CREDITO_VENTA] Bs)" << std::endl;
      file << R"(DEVOL.([%0.0 N_DEVOLUCIONES_CREDITO])       [%11.2 TOTAL_VENTA_CREDITO_DEVOLUCION] Bs)" << std::endl;
      file << R"(TOTAL CREDITO:   [%14.2 TOTAL_VENTA_CREDITO] Bs)" << std::endl;
      file << R"()" << std::endl;
      file << R"(DISTRIBUCION DE LOS INGRESOS)" << std::endl;
      file << R"([METODO_PAGO   ]  [%5.2 MONTO] Bs [%5.2 MONTO_DOLAR] $)" << std::endl;
      file << R"(               ------------------)" << std::endl;
      file << R"(TOTAL:             [%5.2 TOTAL] Bs [%5.2 TOTAL_DOLAR] $)" << std::endl;

      file.close();
    }
  }
  static void generateCuadreCajaResumidoTemplate()
  {
    std::string title = "resources/CUADRE_CAJA_RESUMIDO.txt";

    std::string it = title;
    if (!Tools::fileExists(it))
    {
      std::ofstream file;
      file.open(it);
      file << R"([EMPRESA                              ])" << std::endl;
      file << R"(RIF: [RIF        ]   TLF: [TLF        ] )" << std::endl;
      file << R"([DIRECCION                                                                                                                                                          ])" << std::endl;
      file << R"(FECHA: [FECHA     ] [HORA  ])" << std::endl;
      file << R"()" << std::endl;
      file << R"(CUADRE DE CAJA)" << std::endl;
      file << R"()" << std::endl;
      file << R"(TASA: [%5.2 TASA])" << std::endl;
      file << R"(ESTACION: [ESTACION                ])" << std::endl;
      file << R"(DESDE: [DESDE                      ] )" << std::endl;
      file << R"(HASTA: [HASTA                      ])" << std::endl;
      file << R"()" << std::endl;
      file << R"(DISTRIBUCION DE LOS INGRESOS)" << std::endl;
      file << R"([METODO_PAGO   ]  [%5.2 MONTO] Bs [%5.2 MONTO_DOLAR] $)" << std::endl;
      file << R"(               ------------------)" << std::endl;
      file << R"(TOTAL:             [%5.2 TOTAL] Bs [%5.2 TOTAL_DOLAR] $)" << std::endl;

      file.close();
    }
  }
};

#endif /* InitHelper_hpp */