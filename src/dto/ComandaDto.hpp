#ifndef ComandasDto_hpp
#define ComandasDto_hpp

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class LineaPrecuentaDto : public oatpp::DTO
{
  DTO_INIT(LineaPrecuentaDto, DTO)

  DTO_FIELD(String, item, "item");
  DTO_FIELD(Float64, cant, "cant");
  DTO_FIELD(Float64, precioUnitarioBs, "precioUBs");
  DTO_FIELD(Float64, precioTotalBs, "precioTBs");
  DTO_FIELD(Float64, precioUnitarioD, "precioUD");
  DTO_FIELD(Float64, precioTotalD, "precioTD");
  DTO_FIELD(Float64, descuentoBs, "descuentoBs");
  DTO_FIELD(Float64, descuentoD, "descuentoD");
  DTO_FIELD(String, porcentajeDescuento, "porcentajeDescuento");
};
class LineaComandaDto : public oatpp::DTO
{
  DTO_INIT(LineaComandaDto, DTO)

  DTO_FIELD(String, item, "item");
  DTO_FIELD(Float64, cant, "cant");

  DTO_FIELD(Vector<String>, contornos, "contornos");
};

class ComandaDto : public oatpp::DTO
{
  DTO_INIT(ComandaDto, DTO)

  DTO_FIELD(String, imp, "imp");
  DTO_FIELD(String, impresora, "impresora");
  DTO_FIELD(String, zona, "zona");
  DTO_FIELD(String, mesa, "mesa");
  DTO_FIELD(String, ndoc, "cuenta");
  DTO_FIELD(String, user, "user");
  DTO_FIELD(String, correlacion, "ndoc");
  DTO_FIELD(Vector<Object<LineaComandaDto>>, lineas, "lineas");
};

class MonedaDto : public oatpp::DTO
{
  DTO_INIT(MonedaDto, DTO)

  DTO_FIELD(String, desc, "desc");
  DTO_FIELD(Float64, monto, "monto");
};

class DescuentoDto : public oatpp::DTO
{
  DTO_INIT(DescuentoDto, DTO)

  DTO_FIELD(String, desc, "desc");
  DTO_FIELD(Float64, monto, "monto");
  DTO_FIELD(String, tipo, "tipo");
};

class PrecuentaDto : public oatpp::DTO
{
  DTO_INIT(PrecuentaDto, DTO)

  DTO_FIELD(String, impresora, "impresora");
  DTO_FIELD(String, ndoc, "cuenta");
  DTO_FIELD(String, empresa, "empresa");
  DTO_FIELD(String, cliente, "cliente");
  DTO_FIELD(String, rif, "rif");
  DTO_FIELD(String, mesa, "mesa");
  DTO_FIELD(String, zona, "zona");
  DTO_FIELD(String, user, "user");
  DTO_FIELD(String, correlacion, "ndoc");
  DTO_FIELD(String, iva, "iva");
  DTO_FIELD(String, servicio, "servicio");
  DTO_FIELD(Vector<Object<LineaPrecuentaDto>>, lineas, "lineas");
  DTO_FIELD(Vector<Object<MonedaDto>>, subtotales, "subtotales");
  DTO_FIELD(Vector<Object<MonedaDto>>, servicios, "servicios");
  DTO_FIELD(Vector<Object<MonedaDto>>, totalesSinIva, "totalesSinIva");
  DTO_FIELD(Vector<Object<MonedaDto>>, totalIvas, "totalIvas");
  DTO_FIELD(Vector<Object<MonedaDto>>, totales, "totales");
  DTO_FIELD(Vector<Object<DescuentoDto>>, descuentosGenerales, "descuentosGenerales");
};

class ComandasDto : public oatpp::DTO
{
  DTO_INIT(ComandasDto, DTO)

  DTO_FIELD(Vector<Object<ComandaDto>>, comandas, "comandas");
};

class RawDto : public oatpp::DTO
{
  DTO_INIT(RawDto, DTO)

  DTO_FIELD(String, impresora, "impresora");
  DTO_FIELD(Vector<String>, lineas, "lineas");
};

class PagoCCDto : public oatpp::DTO
{
  DTO_INIT(PagoCCDto, DTO)

  DTO_FIELD(String, nombre, "nombre");
  DTO_FIELD(Int64, instr_id, "instr_id");
  DTO_FIELD(Float64, monto, "monto");
  DTO_FIELD(Float64, monto_dolar, "monto_dolar");
};
class Metodo_pagoDto : public oatpp::DTO
{

  DTO_INIT(Metodo_pagoDto, DTO)

  DTO_FIELD(Int64, metodo_pago_id, "metodo_pago_id");
  DTO_FIELD(String, tipo, "tipo");
  DTO_FIELD(String, descripcion, "descripcion");
  DTO_FIELD(Int64, oculto, "oculto");
  DTO_FIELD(String, displayord, "displayord");
  DTO_FIELD(Int64, grp_id, "grp_id");
  DTO_FIELD(Float64, monto, "monto");
  DTO_FIELD(Float64, monto_dolar, "monto_dolar");
  DTO_FIELD(String, simbolo, "simbolo");
};
class FacturaCCDto : public oatpp::DTO
{
  DTO_INIT(FacturaCCDto, DTO)

  DTO_FIELD(String, n_doc, "n_doc");
  DTO_FIELD(String, nombre, "nombre");
  DTO_FIELD(String, tipo, "tipo");
  DTO_FIELD(Int64, fecha, "fecha");
  DTO_FIELD(Float64, descuento, "descuento");
  DTO_FIELD(Vector<Object<PagoCCDto>>, pagos, "pagos");
  DTO_FIELD(Vector<Object<PagoCCDto>>, pagosAux, "pagosAux");
};
class CuadreCajaDto : public oatpp::DTO
{
  DTO_INIT(CuadreCajaDto, DTO)
  DTO_FIELD(String, empresa, "empresa");
  DTO_FIELD(String, rif, "rif");
  DTO_FIELD(String, telefono, "telefono");
  DTO_FIELD(String, direccion, "direccion");
  DTO_FIELD(Float64, tasa, "tasa");
  DTO_FIELD(Int64, desde, "desde");
  DTO_FIELD(Int64, hasta, "hasta");
  DTO_FIELD(String, estacion_id, "estacion_id");
  DTO_FIELD(String, estacion_nombre, "estacion_nombre");
  DTO_FIELD(String, tipo, "tipo");

  DTO_FIELD(Vector<Object<FacturaCCDto>>, facturas, "facturas");
  DTO_FIELD(Vector<Object<Metodo_pagoDto>>, metodos, "metodos");
};

#include OATPP_CODEGEN_END(DTO)

#endif /* ComandasDto_hpp */
