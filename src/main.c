/* Copyleft
*
*
** \brief Main example source file
 **
 ** This is a mini example of the CIAA Firmware.
 **
 **/

/** \addtogroup CIAA_Firmware CIAA Firmware
 ** @{ */
/** \addtogroup Examples CIAA Firmware Examples
 ** @{ */
/** \addtogroup Main example source file
 ** @{ */

/*
 * Initials     Name
 * ---------------------------
 * DAG          Daniel A. Gil
 *
 */

/*
 * modification history (new versions first)
 * -----------------------------------------------------------
 * 2016-07-11   v0.0.1   First version
 */

/*==================[inclusions]=============================================*/

#include "main.h"         /* <= own header */
#include "sAPI.h"         /* <= sAPI header */
#include "math_FFT.h"	  /* <= math operations header */
#include "input_signal.h"
#include "ftoa.h"

/*==================[macros and definitions]=================================*/
struct complex
{
    float r;
    float i;
};

struct complex x_signal[SAMPLES];
//struct complex result[SAMPLES * 2];

#define MAXPOW 24

/*==================[internal data declaration]==============================*/
int pow_2[MAXPOW];
int k;
int k2=0;
/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/
void radix2(struct complex *x_signal, int N){
    int    n2, k1, N1, N2;
    struct complex bfly[2];
    N1=2;
    N2=N/2;
    
    /** Do 2 Point DFT */
    for (n2=0; n2<N2; n2++){
	/** Don't hurt the butterfly */
		bfly[0].r = (x_signal[n2].r + x_signal[N2 + n2].r);
		bfly[0].i = (x_signal[n2].i + x_signal[N2 + n2].i);	
		bfly[1].r = (x_signal[n2].r - x_signal[N2 + n2].r) * MyCos(n2*2*M_PI/N) - ((x_signal[n2].i - x_signal[N2 + n2].i) * -1 * MySin(n2*2*M_PI/N)); 
		bfly[1].i = (x_signal[n2].i - x_signal[N2 + n2].i) * MyCos(n2*2*M_PI/N) + ((x_signal[n2].r - x_signal[N2 + n2].r) * -1 * MySin(n2*2*M_PI/N));
//		result[k2].r=bfly[0].r;
//		result[k2].i=bfly[0].i;
//		result[k2+1].r=bfly[1].r;
//		result[k2+1].i=bfly[1].i;	

		/** In-place results */
	for (k1=0; k1<N1; k1++){
	    x_signal[n2 + N2*k1].r = bfly[k1].r;
	    x_signal[n2 + N2*k1].i = bfly[k1].i;
		}
	}
    /** Don't recurse if we're down to one butterfly */
    if (N2!=1)
	for (k1=0; k1<N1; k1++){
	    radix2(&x_signal[N2*k1], N2);
		}
}

void bit_reverse_reorder(struct complex *vector, int N){
    int bits, i, j, k;
    float tempr, tempi;
    
    for (i=0; i<MAXPOW; i++)
	if (pow_2[i]==N) bits=i;

    for (i=0; i<N; i++){
	j=0;
	for (k=0; k<bits; k++)
	    if (i&pow_2[k]) j+=pow_2[bits-k-1];
		if (j>i)  /** Only make "up" swaps */{
			tempr=vector[i].r;
			tempi=vector[i].i;
			vector[i].r=vector[j].r;
			vector[i].i=vector[j].i;
			vector[j].r=tempr;
			vector[j].i=tempi;
		}
    }
}

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

/** \brief Main function
 *
 * This is the main entry point of the software.
 *
 * \returns 0
 *
 * \remarks This function never returns. Return value is only to avoid compiler
 *          warnings or errors.
 */

 /* FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE RESET. */

main()
{
   /* ------------- INICIALIZACIONES ------------- */

	/* Inicializar la placa */
	boardConfig();

	/* Inicializar el conteo de Ticks con resolución de 1ms */
	tickConfig(1);

	/* Inicializar DigitalIO */
	digitalConfig( 0, INITIALIZE );

	/* Configuración de pines de entrada para Teclas de la CIAA-NXP */
	digitalConfig( TEC1, INPUT );
	digitalConfig( TEC2, INPUT );
	digitalConfig( TEC3, INPUT );
	digitalConfig( TEC4, INPUT );

	/* Configuración de pines de salida para Leds de la CIAA-NXP */
	digitalConfig( LEDR, OUTPUT );
	digitalConfig( LEDG, OUTPUT );
	digitalConfig( LEDB, OUTPUT );
	digitalConfig( LED1, OUTPUT );
	digitalConfig( LED2, OUTPUT );
	digitalConfig( LED3, OUTPUT );
	
	/* seteo baudrate UART_USB */
	uartConfig( UART_USB, 115200 );

	/* Mensajes de texto salida por UART */
	uint8_t miTexto[] = "Valores se�al de entrada.\r\n";
	uint8_t miTexto2[] = "Valores FFT.\r\n";

	for (k=0;k<SAMPLES;k++)
	{
		x_signal[k].r = x_sig[k*2];
		x_signal[k].i = x_sig[(k*2)+1];
	}


	int i1=1;
	pow_2[0]=1;
	for (i1=1; i1<MAXPOW; i1++){
		pow_2[i1]=pow_2[i1-1]*2;
	}

	int n1;

	//Mostrar datos de entrada
	digitalWrite( LEDB, ON );
	uartWriteString( UART_USB, miTexto ); 
/*	char strr[20];
	char stri[20];
	float real;
	float imag;
	for(n1=0;n1<SAMPLES;n1++){
		if (x_signal[n1].r<0){
		real = -1 * x_signal[n1].r;
		uartWriteString( UART_USB, "Re:-");
		ftoa(real,strr,6);
		uartWriteString( UART_USB, strr);
		uartWriteString( UART_USB, "\r\n" );
	}
	else {
		uartWriteString( UART_USB, "Re:");
		ftoa(x_signal[n1].r,strr,6);
		uartWriteString( UART_USB, strr);
		uartWriteString( UART_USB, "\r\n" );
	}
	if (x_signal[n1].i<0){
		imag = -1 * x_signal[n1].i;
		uartWriteString( UART_USB, "Im:-");
		ftoa(imag,stri,6);
		uartWriteString( UART_USB, stri);
		uartWriteString( UART_USB, "\r\n" );
	}
	else{
		uartWriteString( UART_USB, "Im:");
		ftoa(x_signal[n1].i,stri,6);
		uartWriteString( UART_USB, stri);
		uartWriteString( UART_USB, "\r\n" );
	}
	}
*/
	// llamado a la funcion principal de la FFT
	radix2(&x_signal[0], SAMPLES);

	bit_reverse_reorder(&x_signal[0], SAMPLES);

	//Mostrar datosd de salida
	digitalWrite( LED2, ON );
	uartWriteString( UART_USB, miTexto2 ); 

/*	int	conta = 0;
	char strr2[20];
	char stri2[20];
	float real2;
	float imag2;
	while(conta<SAMPLES){
		if (x_signal[conta].r<0 ){
		real2 = -1 * x_signal[conta].r;
		ftoa(real2,strr2,6);
		uartWriteString( UART_USB, "Re:");
		uartWriteString( UART_USB, "-");
		uartWriteString( UART_USB, strr2);
		uartWriteString( UART_USB, "\r\n" );
	}
	else{
		ftoa(x_signal[conta].r,strr2,6);
		uartWriteString( UART_USB, "Re:");
		uartWriteString( UART_USB, strr2);
		uartWriteString( UART_USB, "\r\n" );
	}

	if (x_signal[conta].i<0) {
	imag2 = -1 * x_signal[conta].i;
	ftoa(imag2,stri2,6);
	uartWriteString( UART_USB, "Im:");
	uartWriteString( UART_USB, "-");
	uartWriteString( UART_USB, stri2);
	uartWriteString( UART_USB, "\r\n" );
	}
	else{
	ftoa(x_signal[conta].i,stri2,6);
	uartWriteString( UART_USB, "Im:");
	uartWriteString( UART_USB, stri2);
	uartWriteString( UART_USB, "\r\n" );
	}
	conta++;
	}
*/
	digitalWrite( LED3, ON );
//	uartWriteString( UART_USB, miTexto3 ); 

	while(1) {
		// debo encender dos leds de manera alternativa indicando la finalizacion del calculo de la FFT.
	digitalWrite( LED2, ON );
	uartWriteString( UART_USB, miTexto2 ); 

   int conta = 0;
	char strr2[20];
	char stri2[20];
	float real2=0;
	float imag2=0;
   delay(20);
	while(conta<SAMPLES){
		if (x_signal[conta].r<0 ){
		real2 = -1 * x_signal[conta].r;
		ftoa(real2,strr2,6);
		uartWriteString( UART_USB, "Re:");
		uartWriteString( UART_USB, "-");
		uartWriteString( UART_USB, strr2);
		uartWriteString( UART_USB, "\r\n" );
	}
	else{
		ftoa(x_signal[conta].r,strr2,6);
		uartWriteString( UART_USB, "Re:");
		uartWriteString( UART_USB, strr2);
		uartWriteString( UART_USB, "\r\n" );
	}

	if (x_signal[conta].i<0) {
	imag2 = -1 * x_signal[conta].i;
	ftoa(imag2,stri2,6);
	uartWriteString( UART_USB, "Im:");
	uartWriteString( UART_USB, "-");
	uartWriteString( UART_USB, stri2);
	uartWriteString( UART_USB, "\r\n" );
	}
	else{
	ftoa(x_signal[conta].i,stri2,6);
	uartWriteString( UART_USB, "Im:");
	uartWriteString( UART_USB, stri2);
	uartWriteString( UART_USB, "\r\n" );
	}
	conta++;
	}

	digitalWrite( LED3, ON );
	digitalWrite( LED2, OFF );
   delay(1500);
      }
	//digitalWrite( LED2, ON );
	return 0 ;
}

