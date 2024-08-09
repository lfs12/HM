#ifndef PrinterDto_hpp
#define PrinterDto_hpp

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class Reimprimir : public oatpp::DTO
{
    DTO_INIT(Reimprimir, DTO)

    DTO_FIELD(String, doc, "doc");
    DTO_FIELD(Int64, numb, "numb");
};

class TextoFormato : public oatpp::DTO
{
    DTO_INIT(TextoFormato, DTO);

    DTO_FIELD(String, text, "text");
    DTO_FIELD(String, formato, "formato");
};

class Impuesto : public oatpp::DTO
{
    DTO_INIT(Impuesto, DTO)
    DTO_FIELD(Boolean, igtf, "igtf");
    DTO_FIELD(Float64, tasa_cambio, "tasa_cambio");
};

class Pago : public oatpp::DTO
{
    DTO_INIT(Pago, DTO)

    DTO_FIELD(Float64, monto, "monto");
    DTO_FIELD(String, metodo, "metodo");
    DTO_FIELD(Int64, metodo_id, "metodo_id");
    DTO_FIELD(Object<Impuesto>, impuesto, "impuesto");
};

class Item : public oatpp::DTO
{
    DTO_INIT(Item, DTO)

    DTO_FIELD(String, codigo, "codigo");
    DTO_FIELD(String, producto, "producto");
    DTO_FIELD(Float64, precio, "precio");
    DTO_FIELD(Float64, cantidad, "cantidad");
    DTO_FIELD(String, tasa, "tasa");
    DTO_FIELD(Float64, descuento, "descuento");
    DTO_FIELD(String, tipo_descuento, "tipo_descuento");
    DTO_FIELD(String, texto, "texto");
};

class NotaFiscal : public oatpp::DTO
{
    DTO_INIT(NotaFiscal, DTO)

    DTO_FIELD(UInt64, factura, "factura");
    DTO_FIELD(String, serialFiscal, "serialFiscal");
    DTO_FIELD(String, fecha, "fecha");
    DTO_FIELD(String, hora, "hora");
};

class Cliente : public oatpp::DTO
{
    DTO_INIT(Cliente, DTO)

    DTO_FIELD(String, nombre, "nombre");
    DTO_FIELD(String, rif, "rif");
};
class Descuento : public oatpp::DTO
{
    DTO_INIT(Descuento, DTO)

    DTO_FIELD(String, tipo_descuento, "tipo_descuento");
    DTO_FIELD(Float64, descuento, "descuento");
};
class Factura : public oatpp::DTO
{
    DTO_INIT(Factura, DTO)

    DTO_FIELD(Object<Cliente>, cliente, "cliente");
    DTO_FIELD(List<String>, cabecera, "cabecera");
    DTO_FIELD(List<String>, pie, "pie");
    DTO_FIELD(Object<NotaFiscal>, notaFiscal, "notaFiscal");
    DTO_FIELD(List<Object<Item>>, items, "items");
    DTO_FIELD(List<Object<Pago>>, pagos, "pagos");
    DTO_FIELD(Object<Descuento>, descuento, "descuento");
};

class PrinterDto : public oatpp::DTO
{
    DTO_INIT(PrinterDto, DTO)

    DTO_FIELD(String, cmd, "cmd");
    DTO_FIELD(String, serial, "serial");
    DTO_FIELD(Object<Factura>, factura, "factura");
    DTO_FIELD(String, linea_comentario, "linea_comentario");
    DTO_FIELD(List<Object<TextoFormato>>, textoNFF, "textoNFF");
    DTO_FIELD(List<String>, textoNF, "textoNF");
    DTO_FIELD(Object<Reimprimir>, reimprimir, "reimprimir");
};

#include OATPP_CODEGEN_END(DTO)

#endif /* PrinterDto_hpp */
