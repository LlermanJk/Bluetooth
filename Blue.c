//Librer�a est�ndar que definie variables, macros para utilizar las funciones de entrada/salida
#include <stdio.h>
//Librer�a que contiene funciones C para la gesti�n de memoria din�mica, control de procesos. 
#include <stdlib.h>
// Librer�a que define diversas constantes y tipos simb�licos, y declara diversas funciones.
#include <unistd.h>
//Librer�a para la creaci�n de sockets
#include <sys/socket.h>
//
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

int main(int argc, char **argv)
{
    inquiry_info *ii = NULL;
    int max_rsp, num_rsp;
    int dev_id, sock, len, flags;
    int i;
    char addr[19] = { 0 };
    char name[248] = { 0 };
    
    dev_id = hci_get_route(NULL);
    /*Se abre un socket bluetooth con el dev_id, representa una conexi�n 
    al microcontrolador en el adaptador local especificado */
    sock = hci_open_dev( dev_id );
    if (dev_id < 0 || sock < 0) {
        perror("opening socket");
        exit(1);
    }

    len  = 8;
    max_rsp = 255;
    /* si las banderas se establecen en IREQ_CACHE_FLUSH, la memoria cach� de
    los dispositivos detectados se vac�a antes de realizar la consulta actual,
    de lo contrario, si las banderas est�n establecidas en 0 , los resultados
    de las consultas pueden ser devueltas inclusi si los dispositivos 
    no est�n en el rango*/
    flags = IREQ_CACHE_FLUSH;
    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));
    
    /*hci_inquiry realiza el descubrimiento del dispositivo bluetooth y 
    devuelve una lista de dispositivos detectadosy alguna informaci�n b�sica
    en la varible ii. hci_inquiry, requiere un n�mero de recurso por lo que se
    utiliza dev_id obtenido por hci _get_route*/

    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
    if( num_rsp < 0 ) perror("hci_inquiry");

    for (i = 0; i < num_rsp; i++) {
	/*ba2str, toma un paquete de 6 Bytes y lo transforma en una cadena   
        XX:XX:XX:XX:XX:XX*/
        ba2str(&(ii+i)->bdaddr, addr);
        memset(name, 0, sizeof(name));
 	/* Una vez encontrado los dispositivos ercans y sus direcciones, el 
   	programa determina los nombres f�ciles de usar asociados con esas 
	direcciones y las presenta al usuario */

        if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), 
            name, 0) < 0)
        strcpy(name, "[unknown]");
        printf("%s  %s\n", addr, name);
    }

    free( ii );
    close( sock );
    return 0;
}