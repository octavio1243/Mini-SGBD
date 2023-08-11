# Mini-SGBD-C

## Notación de expresiones regulares:

--------------------------
| Símbolos | Descripción | 
|----------|-------------|
| <> | Es un símbolo terminal |
| {} | Se repite una o más veces |
| [] | Es opcional |

## Expresiones admitidas:

--------------------------
| Expresion | Ejemplos | 
|----------|-------------|
| CREATE TABLE  <> ({<NombreColumna> <TipoDato> [notnull][primary key] ,}); | create table hola( nombre varchar(10),codigo integer not null primary key, fecha date ); |
| DROP TABLE <Nombre Tabla>; | drop table hola; |
| INSERT INTO <Nombre Tabla> ({<NombreColumna> <TipoDato>,}) values (<valores>); | insert into hola(nombre, codigo, fecha) values ('Andres',123, "01/06/2000-18:55:30"); insert into hola (null,123,null); |
| SELECT * FROM <nombre tabla> WHERE <columna> = <valor>; | select * from hola; select nombre,codigo from hola; select * from hola where codigo=123; select * from hola where nombre='Andres'; select * from hola where fecha="01/06/2000-18:55:30"; select nombre,codigo from hola where codigo=123; select nombre,codigo from hola; |
| UPDATE <nombre tabla> SET <columna>=<valor> WHERE <columna>=<valor>; | update hola set  nombre='RaulVarchar' where nombre='octavio'; update hola set fecha="01/06/2012-22:55:30" where nombre='octavio'; update hola set codigo=1323 where nombre='octavio'; update hola set  nombre='RaulFecha' where fecha="01/06/2000"; update hola set  nombre='RaulEntero' where codigo=123; update hola set  nombre='RaulDouble' where flotante=32.4; |
| DELETE FROM <nombre tabla> WHERE <nombre columna> = <valor columna> | delete from hola where codigo=1; delete from hola where fecha="01/06/2012-22:55:30"; delete from hola where nombre='RaulEntero'; |
