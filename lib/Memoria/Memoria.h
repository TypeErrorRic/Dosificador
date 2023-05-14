/**
 * @file Memoria.h.
 * @author Ricardo Pabón Serna.(ricardo.pabon@correounivalle.edu.co)
 * @brief Manejo de la memoria EEPROM.
 * @version 0.1
 * @date 2023-05-14
 *
 * @copyright Copyright (c) 2023
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <avr/io.h>
#include <inttypes.h>
#include <Arduino.h>

void Escribir_dato(uint8_t dato, unsigned int dirrecion);
uint8_t Leer_dato(unsigned int adress);

//Está estructura hace referencia a una celda de la EEPROM:
struct M_Ref
{
	//Constructor:
	M_Ref(const unsigned int index)
		: index(index)                      {}

	//Access/read members.
	uint8_t operator*() const               {return Leer_dato(index); }
    operator uint8_t() const                {return **this; }

	//ESCRIBIR DATO:
	M_Ref &operator=( uint8_t in )          { return Escribir_dato(in, index ), *this;  }
	//ACTUALIZAR DATO:
	M_Ref &update( uint8_t in )             { return  in != *this ? *this = in : *this;}
	unsigned int index;
};

//Está estrucutra es un puntero bidirrecional a las celdad representadas por M_Ref:
struct pointer
{
    //Constructor
   pointer( const int index )
        : index( index )                    {}
        
    operator int() const                    { return index; }
    pointer &operator=( int in )            { return index = in, *this; }
    
    //Iteración:
    bool operator!=( const pointer &ptr )   { return index != ptr.index; }
    M_Ref operator*()                       { return index; }
    //Incremento:
    pointer& operator++()                   { return ++index, *this; }
    pointer operator++ (int)                { return index++; }

    int index; //dirreción de la celdad actual.
};

//Esta estructura permite manipular datos desde la EEPROOM.
struct classMemoria
{
	M_Ref operator[](const int idx)	           {return idx;}
	unsigned char read( int idx )              { return M_Ref( idx ); }
	void write( int idx, unsigned char val )   { (M_Ref( idx )) = val; }
	void update( int idx, uint8_t val )        { M_Ref( idx ).update( val ); }
	 
	 //Funciones para conseguir y leer datos de la EPPROM:
	 template< typename T > T &get( int idx, T &t )
     {
		 pointer e = idx;
		 uint8_t *ptr = (uint8_t*) &t;
		 for( int count = sizeof(T) ; count ; --count, ++e)  *ptr++ = *e;
		 return t;
	 }
	 
	 template< typename T > const T &put( int idx, const T &t )
     {
		 pointer e = idx;
		 const uint8_t *ptr = (const uint8_t*) &t;
		 for( int count = sizeof(T) ; count ; --count, ++e )  (*e).update( *ptr++ );
		 return t;
	 }
};

#define NUM_ELEMENTOS_ARREGLO     10

void size_Escrito(unsigned short& dato, unsigned short aumento);
unsigned short getConserva();

//Clase para la Manipulación de los datos creados desde classMemoria:
template<typename Tipo_dato, void (*func_ptr)(Tipo_dato&, classMemoria&, unsigned short&, int)>
class Memoria
{
    private:
        Tipo_dato dato;
        unsigned short dirrecion;
        const static uint16_t length;
        static classMemoria EEPROM;
        const unsigned short size_dato;
        unsigned short sizeEscrito;
    public:
        //Constructor:
        Memoria(bool array = false) : size_dato(sizeof(Tipo_dato))
        {
            this->dirrecion = getConserva();
            EEPROM.get(dirrecion, dato);
            if(array) size_Escrito(sizeEscrito, size_dato*NUM_ELEMENTOS_ARREGLO);
            else size_Escrito(sizeEscrito, size_dato);
        }
        ~Memoria()                                      {} //Destructor.
        //Operaciones Aplicables:
        Tipo_dato operator*() const                     {return dato; } 
        Tipo_dato operator=(Tipo_dato cambio)           {return dato = cambio, dato;}
        Tipo_dato operator=(const Memoria &dato)        {return **this = *dato;}
        //Gaurdar Dato:
        void operator++()  const                        {EEPROM.put(dirrecion, dato);}
        //Regresar al Dato original:
        void operator--()                               {EEPROM.get(dirrecion, dato);}
        //Capacidad iterativa:
        void begin()                                    {for(short i = (sizeEscrito + sizeof(Tipo_dato)); i < sizeEscrito; i--) EEPROM.write(i, 0x00);}
        pointer &end()  const                           {return length; }
        //Operaciones basicas de funcionamiento:
        void operator+=(int suma)                       {if(sizeof(Tipo_dato) != sizeof(char)) dato = dato + suma;}
        void operator-=(int suma)                       {if(sizeof(Tipo_dato) != sizeof(char)) dato = dato - suma;}
        //Registros Escritos:
        const unsigned short &size() const              {return sizeEscrito;}
        static uint8_t M_DDR(int indx)                  {return EEPROM[indx];} //Se utiliza para obtener los bits del registro.
        static classMemoria &Ep()                       {return EEPROM;}
        const unsigned short &Dirr() const              {return dirrecion;}
        //Funcionalidad para arreglos:
        Memoria &operator[](int indx)                   {return func_ptr(dato, EEPROM, dirrecion, indx), *this;}
};

//Template para manejo de datos normales.
template<typename T>
void Sumar(T &valor, classMemoria &param, unsigned short &dirr, int indx)
{
    valor = valor + indx;
}

//Valores de las variables:
extern Memoria<int, Sumar<int>> NUM_ENVASES;
extern Memoria<unsigned long, Sumar<unsigned long>> FACTOR_CELDADCARGA; 
extern Memoria<float, Sumar<float>> X2;
extern Memoria<float, Sumar<float>> X;
extern Memoria<float, Sumar<float>> A;
extern Memoria<unsigned int, Sumar<unsigned int>> NUM_CICLO_FINAL;

//Template para manejo de arrays.
template<typename T>
void Array(T &valor, classMemoria &celdas, unsigned short &dirr, int indx)
{
    dirr = dirr + (sizeof(T) * (indx % NUM_ELEMENTOS_ARREGLO));
    celdas.get(dirr, valor);
}

//Array de valores para peso:
extern Memoria<float, Array<float>> PESO;

void LIMPIAR();

#endif