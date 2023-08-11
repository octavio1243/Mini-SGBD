#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definimos un limite maximo de caracteres en una sentencia SQL
#define MAX_LIMIT 100 
#define MAX_VARCHAR 100
#define MAX_NOMBRE 100
#define MAX_NOMBRE_TABLA 100

/* SENTENCIAS PRIMARIA */
#define create "create"
#define drop "drop"
#define select "select"
#define insert "insert"
#define update "update"
#define deletee "delete"
#define autor "autor;"

/* SENTENCIAS SECUNDARIAS */
#define where "where"
#define table "table"
#define nonulo "not null"
#define claveprimaria "primary key"
#define into "into"
#define from "from"

/* TIPOS DE DATOS */
#define td1 "varchar"
#define td2 "date"
#define td3 "double"
#define td4 "integer"

/* DECLARACION DE TIPOS DE DATOS */

struct fecha_hora{
	int mes;
	int dia;
	int anio;
	int hora;
	int minuto;
	int segundo;
};

struct tipoDato{
	int notnull; // 0 puede se nulo, 1 no puede ser nulo
	int primarykey; // 1 es clave primaria, 0 no es clave primaria
	int tipo; // Nos permite saber cual leer (es el importante). Si el tipo es -1 entonces es porque es eliminado
	char nombre[MAX_NOMBRE]; /* Nombre de la columna*/
	char varchar[MAX_VARCHAR]; /* 0) Varchar*/
	struct fecha_hora date; /* 1) Date*/
	double doublee; /* 2) Dobule*/
	int integer; /* 3) Integer */
};

/*FUNCIONES*/

void convertitMinusculas(char * sentencia){
	for(int i=0;i<strlen(sentencia);i++){
		if ((int)'A'<=(int)sentencia[i] && (int)sentencia[i]<=(int)'Z'){
			sentencia[i]=(char)((int)sentencia[i]+32);
		}
	}
}

int verificarSintaxis(char * sentencia){	
	if (strstr(sentencia, create)){
		return 1;
	}
	if (strstr(sentencia, drop)){
		return 2;
	}
	if (strstr(sentencia, select)){
		return 3;
	}
	if (strstr(sentencia, insert)){
		return 4;
	}
	if (strstr(sentencia, update)){
		return 5;
	}
	if (strstr(sentencia, deletee)){
		return 6;
	}
	if (strstr(sentencia, autor)){
		return 7;
	}
	if (strstr(sentencia, "exit;")){
		return 8;
	}
	return 0;
}

int imprimirTipoDato(struct tipoDato * dato){
	if (dato->tipo==-1){
		return 0;
	}
	switch (dato->tipo){
		case 1:
			printf("%24s",dato->varchar);
			break;
		case 2:
			printf("%7d/%2d/%2d-%d:%d:%d",(dato->date).dia,(dato->date).mes,(dato->date).anio,(dato->date).hora,(dato->date).minuto,(dato->date).segundo);
			break;
		case 3:
			printf("%24f",dato->doublee);
			break;
		case 4:
			printf("%24d",dato->integer);
			break;
		default:
			break;
	}
	printf("|");
	return 1;
}	

void leerLineas(char * nombre, int cabeza){
	FILE * fp = fopen(nombre, "r");	
	struct tipoDato registro;
	struct tipoDato dato;
	fread(&registro, sizeof(struct tipoDato), 1, fp);
	fseek( fp, 0, SEEK_SET );
	for(int i=0;fread(&dato, sizeof(struct tipoDato), 1, fp)==1;i++) {
		if (i!=0 && strcmp(dato.nombre,registro.nombre)==0){
			printf("\n");
			if (cabeza==1){
				break;
			}
		}
		if (cabeza){
			printf("%9s	|", dato.nombre);
		}
		else{
			imprimirTipoDato(&dato);
		}
	}
	fclose(fp);
	printf("\n");
}

void escribirLineas(struct tipoDato dato, char * nombre, char * tipo){
	FILE * fp = fopen(nombre,tipo);
	fwrite(&dato,sizeof(struct tipoDato),1,fp);
	fclose (fp );
}

void analizarSubsentenciasCrearTabla(char * inicio, char * fin, char * nombreTabla, struct tipoDato * dato){
	char * aux=inicio;
	int i;
	char * pTipoDato;
	pTipoDato=strstr(inicio, td1);
	dato->tipo=1; 
	pTipoDato=pTipoDato>fin?NULL:pTipoDato;
	
	if (!pTipoDato){
		pTipoDato=strstr(inicio, td2);
		dato->tipo=2;
		pTipoDato=pTipoDato>fin?NULL:pTipoDato;
	}
	if (!pTipoDato){
		pTipoDato=strstr(inicio, td3);
		dato->tipo=3;
		pTipoDato=pTipoDato>fin?NULL:pTipoDato;
	}
	if (!pTipoDato){
		pTipoDato=strstr(inicio, td4);
		dato->tipo=4;
		pTipoDato=pTipoDato>fin?NULL:pTipoDato;
	}
	i=0;
	while((int)aux<(int)pTipoDato){
		if(*aux!=' '){
			(dato->nombre)[i]=*aux;
			i++;
		}
		aux++;
	}
	
	char * noNula = strstr(inicio,nonulo);
	noNula=noNula>fin?NULL:noNula;
	char * primaria = strstr(inicio,claveprimaria);
	primaria=primaria>fin?NULL:primaria;
	if (noNula){
		(dato->notnull)=1;
	}
	else{
		(dato->notnull)=0;
	}
	if (primaria){
		(dato->primarykey)=1;
	}
	else{
		(dato->primarykey)=0;
	}
	escribirLineas(*dato, nombreTabla,"a+");
}

void inicializarDato(struct tipoDato * dato){
	dato->notnull=0;
	dato->primarykey=0;
	dato->tipo=0;
	memset( dato->nombre, '\0', sizeof(char)*MAX_NOMBRE );
	memset( dato->varchar, '\0', sizeof(char)*MAX_VARCHAR );
	(dato->date).anio=0;
	(dato->date).mes=0;
	(dato->date).dia=0;
	(dato->date).hora=0;
	(dato->date).minuto=0;
	(dato->date).segundo=0;
	dato->doublee=0;
	dato->integer=0;
}
	
int existeFile (char * nombreTabla) {
	FILE * fp;
	if ( fp = fopen(nombreTabla, "r") ) {
		fclose(fp);
		return 1;
	} else {
		return 0;
	}   
}
	
void crearTabla(char * sentencia){
	struct tipoDato dato;
	char * inicio;
	char * fin;
	int i;
	char nombreTabla[MAX_NOMBRE_TABLA];
	memset(nombreTabla,'\0',sizeof(char)*MAX_NOMBRE_TABLA);
	inicio=strstr(sentencia, table)+5;
	fin=strstr(sentencia,"(");
	i=0;
	while((int)inicio<(int)fin){
		if (*inicio!=' '){
			nombreTabla[i]=*inicio;
			i++;
		}
		inicio++;
	}
	nombreTabla[i++]='.';
	nombreTabla[i++]='d';
	nombreTabla[i++]='a';
	nombreTabla[i++]='t';
	
	if (!existeFile(nombreTabla)){
		inicio=strstr(sentencia, "(")+1;
		fin;
		i=0;
		do{
			sentencia=inicio;		
			fin=strstr(sentencia,",");
			if (!fin){
				fin=strstr(sentencia,";")-1;
			}
			inicializarDato(&dato);
			analizarSubsentenciasCrearTabla(inicio,fin,nombreTabla,&dato);
			inicio=fin+1;
			i++;
		}while( *inicio!=';');
		printf("[+] Tabla Creada\n");	
	}
	else{
		printf("[-] Error al crear tabla. Ya existe\n");
	}

	printf("\nEl encabezado de la tabla es:\n");
	leerLineas(nombreTabla,1);
	
}
	
char* substr(char * src, int m, int n){
	int len = n - m;
	char * dest = (char*) malloc(sizeof(char) * (len + 2));
	char * pChar=dest;    
	char * aux=src;
	for (int i = m; i <=n; i++){
		*dest =aux[i];
		dest++;
	}
	*dest = '\0';
	return pChar;
	}
		
int verificarSentenciaCrear(char * sentencia){
	int estado=0;
	if (strcmp(substr(sentencia,0,11),"create table") == 0 ){
		estado=1;
	}
	for(int i=12;i<strlen(sentencia);i++){
		if (estado==1 && ' '==sentencia[i]){
			estado=1;
			continue;
		}
		if (estado==1 && (int)'a'<=(int)sentencia[i] && (int)sentencia[i]<=(int)'z'){
			estado=9;
			continue;
		}
		if (estado==9 && (int)'a'<=(int)sentencia[i] && (int)sentencia[i]<=(int)'z'){
			estado=9;
			continue;
		}
		if (estado==9 && sentencia[i]=='('){
			estado=2;
			continue;
		}
		if (estado==2 && ' '==sentencia[i]){
			estado=2;
			continue;
		}
		if (estado==2 && (int)'a'<=(int)sentencia[i] && (int)sentencia[i]<=(int)'z'){
			estado=10;
			continue;
		}
		if (estado==10 && (int)'a'<=(int)sentencia[i] && (int)sentencia[i]<=(int)'z'){
			estado=10;
			continue;
		}
		if (estado==10 && ' '==sentencia[i]){
			estado=7;
			continue;
		}
		if (estado==7 && ' '==sentencia[i]){
			estado=7;
			continue;
		}
		if (estado==7 && (strcmp(substr(sentencia,i,i+7),"varchar(") == 0)  ){
			//printf("Se cumple\n");
			estado=5;
			i=i+7;
			continue;
		}
		if (estado==7 && (strcmp(substr(sentencia,i,i+6),"integer") == 0) ){
			estado=3;
			i=i+6;
			continue;
		}
		if (estado==7 && (strcmp(substr(sentencia,i,i+3),"date") == 0)){
			estado=3;
			i=i+3;
			continue;
		}
		if (estado==7 && (strcmp(substr(sentencia,i,i+5),"double") == 0) ){
			estado=3;
			i=i+5;
			continue;
		}
		
		if (estado==5 && (int)'0'<(int)sentencia[i] && (int)sentencia[i]<=(int)'9'){
			estado=4;
			continue;
		}
		if (estado==4 && (int)'0'<=(int)sentencia[i] && (int)sentencia[i]<=(int)'9'){
			estado=4;
			continue;
		}
		if (estado==4 && ')'==sentencia[i]){
			estado=3;
			continue;
		}
		
		if (estado==3 && ','==sentencia[i]){
			estado=2;
			continue;
		}
		if (estado==3 && (strcmp(substr(sentencia,i,i+1),");") == 0) ){
			estado=6;
			i=i+1;
			continue;
		}
		///
		if (estado==3 && ' '==sentencia[i] ){
			estado=18;
			continue;
		}
		if (estado==18 && ' '==sentencia[i] ){
			estado=18;
			continue;
		}
		if (estado==18 && ','==sentencia[i] ){
			estado=2;
			continue;
		}
		if (estado==18 && (strcmp(substr(sentencia,i,i+1),");") == 0) ){
			estado=6;
			i=i+1;
			continue;
		}
		if (estado==18 && (strcmp(substr(sentencia,i,i+3),"null") == 0) ){
			estado=13;
			i=i+3;
			continue;
		}
		if (estado==18 && (strcmp(substr(sentencia,i,i+7),"not null") == 0) ){
			estado=12;
			i=i+7;
			continue;
		}
		if (estado==18 && (strcmp(substr(sentencia,i,i+10),"primary key") == 0) ){
			estado=8;
			i=i+10;
			continue;
		}
		
		if (estado==13 && ' '==sentencia[i] ){
			estado=13;
			continue;
		}
		if (estado==13 && ','==sentencia[i] ){
			estado=2;
			continue;
		}
		if (estado==13 && (strcmp(substr(sentencia,i,i+1),");") == 0) ){
			estado=6;
			i=i+1;
			continue;
		}
		
		if (estado==12 && ','==sentencia[i] ){
			estado=2;
			continue;
		}
		if (estado==12 && ' '==sentencia[i] ){
			estado=15;
			continue;
		}
		if (estado==12 && (strcmp(substr(sentencia,i,i+1),");") == 0) ){
			estado=6;
			i=i+1;
			continue;
		}
		
		if (estado==15 && ','==sentencia[i] ){
			estado=2;
			continue;
		}
		if (estado==15 && ' '==sentencia[i] ){
			estado=15;
			continue;
		}
		if (estado==15 && (strcmp(substr(sentencia,i,i+10),"primary key") == 0) ){
			estado=14;
			i=i+10;
			continue;
		}
		if (estado==14 && ','==sentencia[i] ){
			estado=2;
			continue;
		}
		if (estado==14 && ' '==sentencia[i] ){
			estado=14;
			continue;
		}
		if (estado==14 && (strcmp(substr(sentencia,i,i+1),");") == 0) ){
			estado=6;
			i=i+1;
			continue;
		}
		
		if (estado==8 && ','==sentencia[i] ){
			estado=2;
			continue;
		}
		if (estado==8 && ' '==sentencia[i] ){
			estado=17;
			continue;
		}
		if (estado==8 && (strcmp(substr(sentencia,i,i+1),");") == 0) ){
			estado=6;
			i=i+1;
			continue;
		}
		if (estado==17 && ','==sentencia[i] ){
			estado=2;
			continue;
		}
		if (estado==17 && ' '==sentencia[i] ){
			estado=17;
			continue;
		}
		if (estado==17 && (strcmp(substr(sentencia,i,i+7),"not null") == 0) ){
			estado=16;
			i=i+7;
			continue;
		}
		
		if (estado==16 && ','==sentencia[i] ){
			estado=2;
			continue;
		}
		if (estado==16 && (strcmp(substr(sentencia,i,i+1),");") == 0) ){
			estado=6;
			i=i+1;
			continue;
		}
		if (estado==9 && ' '==sentencia[i]){
			printf("Error de sintaxis en: %d\n",i);
			estado=11;
			continue;
		}
		if ( (estado==7||estado==5||estado==4||estado==3||estado==6||estado==13||estado==8||estado==17||estado==16||estado==12||estado==14) && (int)'a'<=(int)sentencia[i] && (int)sentencia[i]<=(int)'z'){
			printf("Error de sintaxis en: %d\n",i);
			estado=11;
			continue;
		}
		if (estado==6 && (int)'0'<=(int)sentencia[i] && (int)sentencia[i]<=(int)'9'){
			printf("Error de sintaxis en: %d\n",i);
			estado=11;
			continue;
		}
		
		if (estado==6 && sentencia[i]=='\n' ){
			estado=6;
			break;
		}
		estado=11;
		break;
	}
	printf("\n");
	if (estado==6){
		printf("[+] Expresion correcta\n");
		return 1;
	}
	else{
		printf("[-] Expresion incorrecta\n");
		return 0;
	}
	printf("\n");
}
	
void borrarTabla(char * sentencia){
	char * inicio;
	char * fin;
	int i;
	char nombreTabla[MAX_NOMBRE_TABLA];
	memset(nombreTabla,'\0',sizeof(char)*MAX_NOMBRE_TABLA);
	inicio=strstr(sentencia, table)+5;
	fin=strstr(sentencia,";");
	
	i=0;
	while((int)inicio<(int)fin){
		if (*inicio!=' '){
			nombreTabla[i]=*inicio;
			i++;
		}
		inicio++;
	}
	nombreTabla[i++]='.';
	nombreTabla[i++]='d';
	nombreTabla[i++]='a';
	nombreTabla[i++]='t';
	
	printf("\n");
	
	if (remove(nombreTabla) == 0) {
		printf("[+] Tabla eliminada");
	} else {
		printf("[-] Error al eliminar la tabla");
		if (!existeFile(nombreTabla)){
			printf(".La tabla no existe");
		}
	}
	printf("\n");
}

int verificarSentenciaDrop(char * sentencia){
	int estado=0;
	if (strcmp(substr(sentencia,0,9),"drop table") == 0 ){
		estado=1;
	}
	for(int i=10;i<strlen(sentencia);i++){
		if (estado==1 && ' '==sentencia[i]){
			estado=2;
			continue;
		}
		if (estado==2 && ' '==sentencia[i]){
			estado=2;
			continue;
		}
		if (estado==2 && (int)'a'<=(int)sentencia[i] && (int)sentencia[i]<=(int)'z'){
			estado=3;
			continue;
		}
		if (estado==3 && (int)'a'<=(int)sentencia[i] && (int)sentencia[i]<=(int)'z'){
			estado=3;
			continue;
		}
		if (estado==3 && ' '==sentencia[i]){
			estado=6;
			continue;
		}
		if (estado==6 && ' '==sentencia[i]){
			estado=6;
			continue;
		}
		if (estado==3 && ';'==sentencia[i]){
			estado=4;
			continue;
		}
		if (estado==6 && ';'==sentencia[i]){
			estado=4;
			continue;
		}
		if (estado==4 && sentencia[i]=='\n' ){
			estado=4;
			break;
		}
		estado=11;
		break;
	}
	printf("\n");
	
	if (estado==4){
		printf("[+] Expresion correcta\n");
		return 1;
	}
	else{
		printf("[-] Expresion incorrecta\n");
		return 0;
	}
}

int getTipoDato(char * inicio, char * fin){
	char * aux;
	
	aux=strstr(inicio,"'");
	if (aux && (int)aux<=(int)fin ){
		return 1;
	}
	aux=strstr(inicio,"\"");
	if (aux && (int)aux<=(int)fin ){
		return 2;
	}
	aux=strstr(inicio,".");
	if (aux && (int)aux<=(int)fin ){
		return 3;
	}
	else{
		return 4;
	}
}

int getTipoDatoPorString(char * s){
	char * aux=s;
	for (;aux=='\0';aux++){}
	return getTipoDato(s,aux);
}

char* substrInicioFin(char * inicio, char * fin){
	while (*inicio==' '){
		inicio++;
	}
	while(*fin==' '){
		fin--;
	}
	
	if (((int)fin-(int)inicio)>0){
		return substr(inicio,0,(int)fin-(int)inicio);
	}
	else{
		char * r = (char *) malloc(2*sizeof(char));
		r[0]=*inicio;
		r[1]='\0';
		return r;
	}
}

int nulidad(struct tipoDato dato, char * inicio, char * fin){
	int permiteNulo = !(dato.notnull);
	
	if (permiteNulo){
		return 1;
	}
	if (!permiteNulo){
		if (strcmp(substrInicioFin(inicio,fin),"null")==0){
			return 0;
		}
	}
	return 1;
}

int potencia(int base,int exp){
	int valor=0;
	if (exp==0){
		valor=1;
	}
	else{
		valor=base * potencia(base,exp-1);
	}
	return valor;
}
	
int stringToInteger(char * inicio, char * fin){
	while(*inicio=='0' && (int)inicio!=(int)fin){
		inicio++;
	}
	int numInt=0;
	char * numChar;
	int cantChar=(int)fin-(int)inicio+1;
	numChar=substrInicioFin(inicio,fin);
	for (int i=0;i<(cantChar+i);i++){
		if (numChar[i]=='\0'){
			break;
		}
		cantChar--;
		numInt=numInt+(int)(numChar[i]-'0')*potencia(10,cantChar);
	}
	return numInt;
}

double stringToDouble(char * inicio, char * fin){
	double numDouble;
	char * numChar;
	char * p;
	char * p2;
	numChar=substrInicioFin(inicio,fin);
	p=strstr(inicio,".");
	int cantChar=(int)fin-(int)p+1;
	p2=p;
	numDouble = (double)stringToInteger(inicio,p-1);
	numDouble = numDouble+((double)stringToInteger(p+1,fin))/((double)potencia(10,cantChar-1));
	return numDouble;
}
	
int setearDato(char * nombreTabla,char * inicio1, char * fin1,char * inicio2, char * fin2,char * nombre,int i,int j){
	int typeOfData;
	char * auxFecha;
	struct tipoDato dato;
	struct tipoDato registro;
	typeOfData=getTipoDato(inicio2,fin2);
	
	while(*inicio1==' '){
		inicio1++;
	}
	while(*fin1==' '){
		fin1--;
	}
	while(*inicio2==' '){
		inicio2++;
	}
	while(*fin2==' '){
		fin2--;
	}

	FILE * fp = fopen(nombre, "r");	
	for(int j=0;j<i;j++) {
		fread(&dato, sizeof(struct tipoDato), 1, fp);
	}
	fseek(fp,0,SEEK_SET);
	
	if(dato.primarykey==1){
		while(fread(&registro, sizeof(struct tipoDato), 1, fp)==1){
			if (strcmp(registro.nombre,dato.nombre)==0){
				switch (dato.tipo){
					case 1:
						if (registro.tipo>0 && strcmp(registro.varchar,substrInicioFin(inicio2+1,fin2-1))==0){
							printf("\n[-] Error. Ya existe una fila con esa clave primaria\n\n");
							goto error;
						}
						break;
					case 2:
						auxFecha=substr(substrInicioFin(inicio2,fin2),1,2);
						dato.date.dia=stringToInteger(auxFecha,auxFecha+1);
						auxFecha=substr(substrInicioFin(inicio2,fin2),4,5);
						dato.date.mes=stringToInteger(auxFecha,auxFecha+1);
						auxFecha=substr(substrInicioFin(inicio2,fin2),7,10);
						dato.date.anio=stringToInteger(auxFecha,auxFecha+3);
						auxFecha=substr(substrInicioFin(inicio2,fin2),12,13);
						dato.date.hora=stringToInteger(auxFecha,auxFecha+1);
						auxFecha=substr(substrInicioFin(inicio2,fin2),15,16);
						dato.date.minuto=stringToInteger(auxFecha,auxFecha+1);
						auxFecha=substr(substrInicioFin(inicio2,fin2),18,19);
						dato.date.segundo=stringToInteger(auxFecha,auxFecha+1);
						if (registro.tipo>0 && dato.date.dia==registro.date.dia &&
							dato.date.mes==registro.date.mes &&
							dato.date.anio==registro.date.anio &&
							dato.date.hora==registro.date.hora &&
							dato.date.minuto==registro.date.minuto &&
							dato.date.segundo==registro.date.segundo
							){
							printf("\n[-] Error. Ya existe una fila con esa clave primaria\n\n");
							goto error;
						}
						break;
					case 3:
						dato.doublee=stringToDouble(inicio2,fin2);
						if (registro.tipo>0 && dato.doublee==registro.doublee){
							printf("\n[-] Error. Ya existe una fila con esa clave primaria\n\n");
							goto error;
						}
						break;
					case 4:
						dato.integer=stringToInteger(inicio2,fin2);
						if (registro.tipo>0 && dato.integer==registro.integer){
							printf("\n[-] Error. Ya existe una fila con esa clave primaria\n\n");
							goto error;
						}
						break;
					default:
						break;
				}
			}
		}
	}
	fclose(fp);
	
	if ( strcmp(dato.nombre,substrInicioFin(inicio1,fin1))==0 
		&& dato.tipo==getTipoDato(inicio2,fin2) 
		&& nulidad(dato,inicio2,fin2)
		){
		
		switch (dato.tipo){
			case 1:
				strcpy(dato.varchar,substrInicioFin(inicio2+1,fin2-1));
				break;
			case 2:
				auxFecha=substr(substrInicioFin(inicio2,fin2),1,2);
				dato.date.dia=stringToInteger(auxFecha,auxFecha+1);
				auxFecha=substr(substrInicioFin(inicio2,fin2),4,5);
				dato.date.mes=stringToInteger(auxFecha,auxFecha+1);
				auxFecha=substr(substrInicioFin(inicio2,fin2),7,10);
				dato.date.anio=stringToInteger(auxFecha,auxFecha+3);
				auxFecha=substr(substrInicioFin(inicio2,fin2),12,13);
				dato.date.hora=stringToInteger(auxFecha,auxFecha+1);
				auxFecha=substr(substrInicioFin(inicio2,fin2),15,16);
				dato.date.minuto=stringToInteger(auxFecha,auxFecha+1);
				auxFecha=substr(substrInicioFin(inicio2,fin2),18,19);
				dato.date.segundo=stringToInteger(auxFecha,auxFecha+1);
				break;
			case 3:
				dato.doublee=stringToDouble(inicio2,fin2);
				break;
			case 4: 
				dato.integer=stringToInteger(inicio2,fin2);
				break;
			default:
				break;
		}
		if (j==1){
			escribirLineas(dato,nombreTabla,"a+");
		}
		return 1;
	}
	else{
		if( !(strcmp(dato.nombre,substrInicioFin(inicio1,fin1))==0) ){
			printf("[-] Registro invalido. La columna %s no existe.\n",dato.nombre);
		}
		if (!(dato.tipo==getTipoDato(inicio2,fin2))){
			printf("[-] Registro invalido. El valor de la columna %s no respeta el tipo.\n",dato.nombre);
		}
		if (!nulidad(dato,inicio2,fin2)){
			printf("[-] Registro invalido. El valor de la columna %s no puede ser nulo.\n",dato.nombre);
		}
		error:
		return 0;
	}
}
		
void insertarRegistro(char * sentencia){
	struct tipoDato dato;
	char * inicio;
	char * fin;
	int i;
		
	char nombreTabla[MAX_NOMBRE_TABLA];
	memset(nombreTabla,'\0',sizeof(char)*MAX_NOMBRE_TABLA);
		
	inicio=strstr(sentencia, into)+4;
	fin=strstr(sentencia,"(");
	
	i=0;
	while((int)inicio<(int)fin){
		if (*inicio!=' '){
			nombreTabla[i]=*inicio;
			i++;
		}
		inicio++;
	}
	nombreTabla[i++]='.';
	nombreTabla[i++]='d';
	nombreTabla[i++]='a';
	nombreTabla[i++]='t';
		
	if (existeFile(nombreTabla)){
		char * inicio1;
		char * fin1;
		char * sentencia1;
		char * inicio2;
		char * fin2;
		char * sentencia2;
		int typeOfData;		
		int validaciones=1;
		for (int j=0;j<2;j++){
			i=0;
			sentencia1=sentencia;
			sentencia2=sentencia;
			inicio1=strstr(sentencia, "(")+1;
			inicio2=strstr(inicio1, "(")+1;
			do{
				sentencia1=inicio1;
				fin1=strstr(sentencia1,",");
				if (!fin1 || (int)fin1>(int)strstr(sentencia1,")") ){
					fin1=strstr(sentencia1,")")-1;
				}
				sentencia2=inicio2;		
				fin2=strstr(sentencia2,",");
				if (!fin2){
					fin2=strstr(sentencia2,")")-1;
					if (!setearDato(nombreTabla,inicio1,fin1,inicio2,fin2,nombreTabla,i+1,j)){
						validaciones=0;
					}
				}
				else{
					if (!setearDato(nombreTabla,inicio1,fin1-1,inicio2,fin2-1,nombreTabla,i+1,j)){
						validaciones=0;
					}
				}
				inicio1=fin1+1;
				inicio2=fin2+1;
				i++;
			}while( *inicio2!=')');
			if (validaciones==0){
				break;
			}
		}
		if (validaciones==1){
			printf("\n[+] Fila agregada correctamente\n\n");
		}
	}
	else{
		printf("[-] Error no existe la tabla.\n");
	}
}

int verificarColumna(char * nombreColumna, char * inicio, char * fin){
	while(*inicio==' ' && (int)inicio<=(int)fin ){
		inicio++;
	}
	while(*fin==' ' && (int)inicio<=(int)fin ){
		fin--;
	}
	char * sentencia;
	char * inicioTemp=inicio;
	char * finTemp;
	do{
		sentencia=inicioTemp;		
		finTemp=strstr(sentencia,",")-1;
		if (!finTemp || finTemp>fin){
			finTemp=fin;
		}
		if (strcmp(nombreColumna,substrInicioFin(inicioTemp,finTemp))==0){
			return 1;
		}		
		inicioTemp=finTemp+2;
	}while( (int)inicioTemp<(int)fin);
	return 0;
}
	
int filtrarFila(struct tipoDato dato,char * left, char * right){
	while(*left==' '){
		left++;
	}
	struct tipoDato registro;
	char * auxFecha;
	char * inicio=right;
	char * fin=inicio;
	while(*(fin+1)!='\0'){
		fin++;
	}
	if ( strcmp(dato.nombre,left)==0 ){
		switch(dato.tipo){
		case 1:
			if (strcmp(dato.varchar,substrInicioFin(inicio+1,fin-1))==0){
				return 1;
			}
			break;
		case 2:
			auxFecha=right;
			auxFecha=substr(substrInicioFin(inicio,fin),1,2);
			registro.date.dia=stringToInteger(auxFecha,auxFecha+1);
			auxFecha=substr(substrInicioFin(inicio,fin),4,5);
			registro.date.mes=stringToInteger(auxFecha,auxFecha+1);
			auxFecha=substr(substrInicioFin(inicio,fin),7,10);
			registro.date.anio=stringToInteger(auxFecha,auxFecha+3);
			auxFecha=substr(substrInicioFin(inicio,fin),12,13);
			registro.date.hora=stringToInteger(auxFecha,auxFecha+1);
			auxFecha=substr(substrInicioFin(inicio,fin),15,16);
			registro.date.minuto=stringToInteger(auxFecha,auxFecha+1);
			auxFecha=substr(substrInicioFin(inicio,fin),18,19);
			registro.date.segundo=stringToInteger(auxFecha,auxFecha+1);
			if (dato.date.dia==registro.date.dia &&
				dato.date.mes==registro.date.mes &&
				dato.date.anio==registro.date.anio &&
				dato.date.hora==registro.date.hora &&
				dato.date.minuto==registro.date.minuto &&
				dato.date.segundo==registro.date.segundo
				){
				return 1;
			}
			break;
		case 3:
			registro.doublee=stringToDouble(inicio,fin);
			if (dato.doublee==registro.doublee){
				return 1;
			}
			break;
		case 4:
			registro.integer=stringToInteger(inicio,fin);
			if (dato.integer==registro.integer){
				return 1;
			}
			break;
		}
	}
	return 0;
	
}
	
int imprimirFila(int fila,char * nombreTabla, char * sentencia){
	struct tipoDato registro;
	struct tipoDato dato;
	char * inicio;
	char * fin;
	FILE * fp = fopen(nombreTabla, "r");
	fread(&registro, sizeof(struct tipoDato), 1, fp);
	fseek( fp, 0, SEEK_SET );
	int f=0;
	int impreso=0;
	
	for(int i=0;fread(&dato, sizeof(struct tipoDato), 1, fp)==1;i++) {
		
		
		if (i!=0 && strcmp(dato.nombre,registro.nombre)==0){
			
			f++;
		}
		if (fila==f){
			if (strstr(sentencia,"*")){
				impreso=imprimirTipoDato(&dato);
			}
			else{
				inicio=strstr(sentencia,"select")+6;
				fin=strstr(sentencia,"from");
				if (verificarColumna(dato.nombre,inicio,fin-1)){
					impreso=imprimirTipoDato(&dato);
				}
			}
		}
	}
	fclose(fp);
	return impreso;
}
	
void seleccionarRegistros(char * sentencia){
	struct tipoDato registro;
	struct tipoDato dato;
	char * inicio;
	char * fin;
	int i;
	char nombreTabla[MAX_NOMBRE_TABLA];
	memset(nombreTabla,'\0',sizeof(char)*MAX_NOMBRE_TABLA);
	inicio=strstr(sentencia, from)+4;
	fin=strstr(sentencia,where);
	if (!fin){
		fin=strstr(sentencia,";");
	}
	i=0;
	while((int)inicio<(int)fin){
		if (*inicio!=' '){
			nombreTabla[i]=*inicio;
			i++;
		}
		inicio++;
	}
	nombreTabla[i++]='.';
	nombreTabla[i++]='d';
	nombreTabla[i++]='a';
	nombreTabla[i++]='t';
	if (existeFile(nombreTabla)){
		FILE * fp = fopen(nombreTabla, "r");	
		fread(&registro, sizeof(struct tipoDato), 1, fp);
		fseek( fp, 0, SEEK_SET );
		int cabeza=1;
		int printNuevo=0;
		int printViejo=0;
		int fila=0;
		for(int i=0;fread(&dato, sizeof(struct tipoDato), 1, fp)==1;i++) {
			if (i!=0 && strcmp(dato.nombre,registro.nombre)==0 ){
				if ( (!strstr(sentencia,where) || fila==1) && dato.tipo!=-1){
					printf("\n");
				}
				fila++;
				if (cabeza==1){
					printf("\n");
					cabeza=0;
					
				}
			}
			if (cabeza){
				if (strstr(sentencia,"*")){
					printf("%24s", dato.nombre);
					printf("|", dato.nombre);
				}
				else{
					inicio=strstr(sentencia,"select")+6;
					fin=strstr(sentencia,"from");
					if (verificarColumna(dato.nombre,inicio,fin-1)){
						printf("%24s", dato.nombre);
						printf("|", dato.nombre);
					}
				}
				
			}
			else{
				if (strstr(sentencia,where)){
					char * p1=strstr(sentencia,"where")+5;
					char * p2=strstr(sentencia,"=");
					char * p3=strstr(sentencia,";");
					if (filtrarFila(dato,substrInicioFin(p1,p2-1),substrInicioFin(p2+1,p3-1))){
						if (imprimirFila(fila,nombreTabla,sentencia)){
							printf("\n");
						}						
					}
				}
				else{
					if (strstr(sentencia,"*")){
						imprimirTipoDato(&dato);
					}
					else{
						inicio=strstr(sentencia,"select")+6;
						fin=strstr(sentencia,"from");
						if (verificarColumna(dato.nombre,inicio,fin-1)){
							imprimirTipoDato(&dato);
						}
					}
				}
			}
		}
		fclose(fp);
	}
	else{
		printf("[-] Error al consultar tabla. La tabla no existe\n");
	}
}

int esFilaInteres(struct tipoDato * dato,char * nombreColumna,char * valorColumna){
	char * inicio=valorColumna;
	char * fin=inicio;
	for(;*(fin+1)!='\0';fin++){}
	char * auxFecha;
	struct tipoDato registro;
	
	if ( (strcmp(dato->nombre,nombreColumna)==0) && (dato->tipo==getTipoDatoPorString(valorColumna)) ){
		switch(dato->tipo){
		case 1:
			inicio++;
			fin--;
			if (strcmp(dato->varchar,substrInicioFin(inicio,fin))==0){
				return 1;
			}
			break;
		case 2:
			auxFecha=substr(substrInicioFin(inicio,fin),1,2);
			registro.date.dia=stringToInteger(auxFecha,auxFecha+1);
			auxFecha=substr(substrInicioFin(inicio,fin),4,5);
			registro.date.mes=stringToInteger(auxFecha,auxFecha+1);
			auxFecha=substr(substrInicioFin(inicio,fin),7,10);
			registro.date.anio=stringToInteger(auxFecha,auxFecha+3);
			auxFecha=substr(substrInicioFin(inicio,fin),12,13);
			registro.date.hora=stringToInteger(auxFecha,auxFecha+1);
			auxFecha=substr(substrInicioFin(inicio,fin),15,16);
			registro.date.minuto=stringToInteger(auxFecha,auxFecha+1);
			auxFecha=substr(substrInicioFin(inicio,fin),18,19);
			registro.date.segundo=stringToInteger(auxFecha,auxFecha+1);			
			if ((dato->date).dia==registro.date.dia &&
				(dato->date).mes==registro.date.mes &&
				(dato->date).anio==registro.date.anio &&
				(dato->date).hora==registro.date.hora &&
				(dato->date).minuto==registro.date.minuto &&
				(dato->date).segundo==registro.date.segundo
				){
				return 1;
			}
			break;
		case 3:
			registro.doublee=stringToDouble(inicio,fin);
			if (dato->doublee==registro.doublee){
				return 1;
			}
			break;
		case 4:
			registro.integer=stringToInteger(inicio,fin);
			if (dato->integer==registro.integer){
				return 1;
			}
			break;
		}
	}
	return 0;
}	

int filaExiste(char * nombreTabla,struct tipoDato registro,char * nombreColumna,char * valorColumna){
	struct tipoDato dato;
	FILE * fp = fopen(nombreTabla, "r");	
	fread(&registro, sizeof(struct tipoDato), 1, fp);
	fseek( fp, 0, SEEK_SET );
	int cabeza=1;
	int filaActual=0;
	int columnas=0;
	for(int i=0;fread(&dato, sizeof(struct tipoDato), 1, fp)==1;i++) {
		if (esFilaInteres(&dato,nombreColumna,valorColumna)){
			return 1;
		}
	}
	fclose(fp);
	return 0;
}
	
int modificarCampoFila(char * nombreTabla,int fila,int columna,char * nombreColumna,char * valorColumna){
	struct tipoDato registro;
	char * inicio=valorColumna;
	char * fin=inicio;
	for(;*(fin+1)!='\0';fin++){}
	char * auxFecha;	
	FILE * fp = fopen(nombreTabla, "r+");
	fseek( fp, sizeof(struct tipoDato)*fila*columna, SEEK_SET );
	for(int i=0;i<columna;i++){
		fread(&registro, sizeof(struct tipoDato), 1, fp);
		if ( (strcmp(registro.nombre,nombreColumna)==0) && (registro.tipo==getTipoDatoPorString(valorColumna)) ){
			if ( (registro.primarykey==1 || registro.notnull==1) && strcmp(valorColumna,"null")==0 ){
				printf("[-] No se puede hacer nulo\n");
				return 0;
			}
			if ( (registro.primarykey==1) && filaExiste(nombreTabla, registro,nombreColumna,valorColumna) ){
				printf("[-] No pueden existir más de dos filas con la misma clave primaria\n");
				return 0;
			}
			switch(registro.tipo){
			case 1:
				inicio++;
				fin--;
				strcpy(registro.varchar,substrInicioFin(inicio,fin));
				break;
			case 2:
				auxFecha=substr(substrInicioFin(inicio,fin),1,2);
				registro.date.dia=stringToInteger(auxFecha,auxFecha+1);
				auxFecha=substr(substrInicioFin(inicio,fin),4,5);
				registro.date.mes=stringToInteger(auxFecha,auxFecha+1);
				auxFecha=substr(substrInicioFin(inicio,fin),7,10);
				registro.date.anio=stringToInteger(auxFecha,auxFecha+3);
				auxFecha=substr(substrInicioFin(inicio,fin),12,13);
				registro.date.hora=stringToInteger(auxFecha,auxFecha+1);
				auxFecha=substr(substrInicioFin(inicio,fin),15,16);
				registro.date.minuto=stringToInteger(auxFecha,auxFecha+1);
				auxFecha=substr(substrInicioFin(inicio,fin),18,19);
				registro.date.segundo=stringToInteger(auxFecha,auxFecha+1);
				break;
			case 3:
				registro.doublee=stringToDouble(inicio,fin);				
				break;
			case 4:
				registro.integer=stringToInteger(inicio,fin);				
				break;
			}
			fseek(fp, ftell(fp)-sizeof(struct tipoDato), 0); //Se va al registro anterior
			fwrite(&registro,sizeof(struct tipoDato),1,fp);
			fclose(fp);
			return 1;
		}
		
	}
	fclose(fp);
	return 0;
}
	
void actualizarRegistros(char * sentencia){
	struct tipoDato registro;
	struct tipoDato dato;
	char * inicio;
	char * fin;
	char * restriccion;
	char * cambio;
	int i;
	int actualizado=0;
	char nombreTabla[MAX_NOMBRE_TABLA];
	memset(nombreTabla,'\0',sizeof(char)*MAX_NOMBRE_TABLA);
	inicio=strstr(sentencia, update)+6;
	fin=strstr(sentencia,"set");
	i=0;
	while((int)inicio<(int)fin){
		if (*inicio!=' '){
			nombreTabla[i]=*inicio;
			i++;
		}
		inicio++;
	}
	nombreTabla[i++]='.';
	nombreTabla[i++]='d';
	nombreTabla[i++]='a';
	nombreTabla[i++]='t';
	
	printf("\n");
	if (existeFile(nombreTabla)){
		inicio=strstr(sentencia, "set")+3;
		fin;
		i=0;
		do{
			sentencia=inicio;		
			fin=strstr(sentencia,"where");
			if (!fin){
				fin=strstr(sentencia,";")-1;
				restriccion=substrInicioFin(inicio+5,fin);
				
			}
			else{
				cambio=substrInicioFin(inicio,fin-1);
				
			}
			
			inicio=fin+1;
			i++;
		}while( *inicio!=';');		
		FILE * fp = fopen(nombreTabla, "r");	
		fread(&registro, sizeof(struct tipoDato), 1, fp);
		fseek( fp, 0, SEEK_SET );
		int cabeza=1;
		int filaActual=0;
		int columnas=0;
		for(int i=0;fread(&dato, sizeof(struct tipoDato), 1, fp)==1;i++) {
			if (i!=0 && strcmp(dato.nombre,registro.nombre)==0){
				filaActual++;
				if (cabeza==1){
					cabeza=0;
				}
			}
			if (!cabeza){
				if (esFilaInteres(&dato,substrInicioFin(restriccion,strstr(restriccion,"=")-1),strstr(restriccion,"=")+1)){
					if (modificarCampoFila(nombreTabla,filaActual,columnas,substrInicioFin(cambio,strstr(cambio,"=")-1),strstr(cambio,"=")+1)){
						actualizado=1;
					}
				}
			}
			else{
				columnas++;
			}
		}
		fclose(fp);
		if (actualizado){
			printf("[+] Fila Actualizada\n");
		}
		else{
			printf("[-] Error al actualizar fila. La fila no existe\n");
		}
		
	}
	else{
		printf("[-] Error al actualizar fila de tabla. La tabla no existe\n");
	}
}

int modificarCampoFila1(char * nombreTabla,int fila,int columna){
	struct tipoDato registro;	
	FILE * fp = fopen(nombreTabla, "r+");
	fseek( fp, sizeof(struct tipoDato)*fila*columna, SEEK_SET );
	for(int i=0;i<columna;i++){
		fread(&registro, sizeof(struct tipoDato), 1, fp);
		registro.tipo=-1; 
		fseek(fp, ftell(fp)-sizeof(struct tipoDato), 0);
		fwrite(&registro,sizeof(struct tipoDato),1,fp);
		fseek(fp, ftell(fp)-sizeof(struct tipoDato), 0);
		fread(&registro, sizeof(struct tipoDato), 1, fp);		
	}	
	fclose(fp);
	return 1;
}
	
void eliminarFilas(char * sentencia){
	struct tipoDato registro;
	struct tipoDato dato;
	char * inicio;
	char * fin;
	char * restriccion;
	int eliminado=0;
	int i;
	char nombreTabla[MAX_NOMBRE_TABLA];
	memset(nombreTabla,'\0',sizeof(char)*MAX_NOMBRE_TABLA);
	inicio=strstr(sentencia, from)+4;
	fin=strstr(sentencia,where);
	i=0;
	while((int)inicio<(int)fin){
		if (*inicio!=' '){
			nombreTabla[i]=*inicio;
			i++;
		}
		inicio++;
	}
	nombreTabla[i++]='.';
	nombreTabla[i++]='d';
	nombreTabla[i++]='a';
	nombreTabla[i++]='t';
	printf("\n");
	if (existeFile(nombreTabla)){
		inicio=strstr(sentencia, "where")+5;
		fin=strstr(sentencia,";")-1;
		restriccion=substrInicioFin(inicio,fin);
		FILE * fp = fopen(nombreTabla, "r");	
		fread(&registro, sizeof(struct tipoDato), 1, fp);
		fseek( fp, 0, SEEK_SET );
		int cabeza=1;
		int filaActual=0;
		int columnas=0;
		for(int i=0;fread(&dato, sizeof(struct tipoDato), 1, fp)==1;i++) {
			if (i!=0 && strcmp(dato.nombre,registro.nombre)==0){
				filaActual++;
				if (cabeza==1){
					cabeza=0;
				}
			}
			if (!cabeza){
				if (esFilaInteres(&dato,substrInicioFin(restriccion,strstr(restriccion,"=")-1),strstr(restriccion,"=")+1)){
					if ( modificarCampoFila1(nombreTabla,filaActual,columnas) ){
						eliminado=1;
					}
				}
			}
			else{
				columnas++;
			}
		}
		fclose(fp);
		if (eliminado){
			printf("[+] Fila Eliminada.\n");	
		}
		else{
			printf("[-] Error al eliminar fila. La fila no existe.\n");
		}
	}
	else{
		printf("[-] Error al eliminar fila de tabla. La tabla no existe\n");
	}
	printf("\n");
}
	
int main(int argc, char *argv[]) {
	char sentencia[MAX_LIMIT];
	struct tipoDato p3;
	while (1){
		printf(">>>");
		fgets(sentencia, MAX_LIMIT, stdin);
		convertitMinusculas((char *)&sentencia);
		switch (verificarSintaxis(sentencia))
			{
			case 1:
				if (verificarSentenciaCrear(sentencia)){
					crearTabla(sentencia);
				}
				break;
			case 2:
				if (verificarSentenciaDrop(sentencia)){
					borrarTabla(sentencia);
				}
				break;
			case 3:
				seleccionarRegistros(sentencia);
				break;
			case 4:
				insertarRegistro(sentencia);
				break;
			case 5:
				actualizarRegistros(sentencia);
				break;
			case 6:
				eliminarFilas(sentencia);
				break;
			case 7:
				printf("\n--------------INFORMACION DEL AUTOR----------------\nNombre del Autor: Octavio Alcalde\n");
				break;
			case 8:
				exit(0);
				break;
			default:
				printf("Error de sintaxis");
			}
		printf("\n");
	}
	return 0;
}

