02 60 00 1B 59 4A 75 61 6E 20 4A 6F 73 82 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 34 37 36 38 30 30 30 38 30 30 30 30 31 20 20 31 32 33 34 35 36 37 38 39 30 31 32 33 34 35 36 37 38 30 34 30 38 30 36 31 33 35 39 35 36 30 31 32 33 34 35 F1 11

    // Secuencia de bytes proporcionada (ejemplo)
    int bytes[] = converToBytes("1B 59 4A 75 61 6E 20 4A 6F 73 82 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 34 37 36 38 30 30 30 38 30 30 30 30 31 20 20 31 32 33 34 35 36 37 38 39 30 31 32 33 34 35 36 37 38 30 34 30 38 30 36 31 33 35 39 35 36 30 31 32 33 34 35");

    // Calculamos el CSL y CSH
    int csl = 0;
    int csh = 0;
    for (int i = 0; i < sizeof(bytes) / sizeof(int); ++i) {
        csl += bytes[i];
        csh += bytes[i];
    }

    // Imprimimos los resultados
    std::cout << "CSL (Byte Menos Significativo): " << csl << std::endl;
    std::cout << "CSH (Byte Más Significativo): " << csh << std::endl;