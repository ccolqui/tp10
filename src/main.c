/* 2023 - Carmela Colqui <carmela.colqui@gmail.com>
 */

/** @file EITI_TPN° 10
 **
 ** @brief TPN° 10  - FreeRTOS
 **
 ** Ejemplo de un led parpadeando utilizando la capa de abstraccion de
 ** hardware y con sistema operativo FreeRTOS.
 **
 **
 ** @defgroup ejemplos Proyectos de ejemplo
 ** @brief Proyectos de ejemplos en Sistemas Embebidos
 ** @{
 */

/* === Inclusiones de cabeceras ============================================ */
#include <stdbool.h>

#include "FreeRTOS.h"
#include "bsp.h"
#include "event_groups.h"
#include "semphr.h"
#include "task.h"
/* === Definicion y Macros ================================================= */
#define BOTON_PROBAR_APRETADO (1 << 0)
#define BOTON_PROBAR_LIBERADO (1 << 4)

#define BOTON_PRENDER_APRETADO (1 << 1)
#define BOTON_PRENDER_LIBERADO (1 << 5)

#define BOTON_APAGAR_APRETADO (1 << 2)
#define BOTON_APAGAR_LIBERADO (1 << 6)

#define BOTON_CAMBIAR_APRETADO (1 << 3)
#define BOTON_CAMBIAR_LIBERADO (1 << 7)

/* === Declaraciones de tipos de datos internos ============================ */

typedef struct parametros_s
{
    digital_output_t led;
    uint16_t periodo;
} *parametros_t;

EventGroupHandle_t eventos_teclas;

/* === Declaraciones de funciones internas ================================= */

void Azul(void *parameters)
{
    board_t board = parameters;
    EventBits_t eventos;
    while (true)
    {
        eventos = xEventGroupWaitBits(eventos_teclas, BOTON_PROBAR_APRETADO | BOTON_PROBAR_LIBERADO, pdTRUE, pdFALSE, portMAX_DELAY);
        if (eventos & BOTON_PROBAR_APRETADO)
        {
            DigitalOutputActivate(board->led_azul);
        }

        else if (eventos & BOTON_PROBAR_LIBERADO)
        {
            DigitalOutputDeactivate(board->led_azul);
        }
    }
}

void Amarilla(void *parameters)
{
    board_t board = parameters;
    while (true)
    {
        if (xEventGroupWaitBits(eventos_teclas, BOTON_CAMBIAR_APRETADO, pdTRUE, pdFALSE, portMAX_DELAY))
        {
            DigitalOutputToggle(board->led_amarillo);
        }
    }
}

void Roja(void *parameters)
{
    board_t board = parameters;
    while (true)
    {
        if (xEventGroupWaitBits(eventos_teclas, BOTON_PRENDER_APRETADO, pdTRUE, pdFALSE, portMAX_DELAY))
        {
            DigitalOutputActivate(board->led_rojo);
        }
        else if (xEventGroupWaitBits(eventos_teclas, BOTON_APAGAR_APRETADO, pdTRUE, pdFALSE, portMAX_DELAY))
        {
            DigitalOutputDeactivate(board->led_rojo);
        }
    }
}

void Verde(void *parameters)
{
    board_t board = parameters;
    while (true)
    {
        DigitalOutputToggle(board->led_verde);
        vTaskDelay(pdMS_TO_TICKS(5 * 150));
    }
}

void Teclado(void *parameters)
{
    board_t board = parameters;

    while (true)
    {
        if (DigitalInputHasActivated(board->boton_cambiar))
        {
            xEventGroupSetBits(eventos_teclas, BOTON_CAMBIAR_APRETADO);
        }
        else if (DigitalInputHasDeactivated(board->boton_cambiar))
        {
            xEventGroupSetBits(eventos_teclas, BOTON_CAMBIAR_LIBERADO);
        }

        if (DigitalInputHasActivated(board->boton_apagar))
        {
            xEventGroupSetBits(eventos_teclas, BOTON_APAGAR_APRETADO);
        }
        else if (DigitalInputHasDeactivated(board->boton_apagar))
        {
            xEventGroupSetBits(eventos_teclas, BOTON_APAGAR_LIBERADO);
        }

        if (DigitalInputHasActivated(board->boton_prender))
        {
            xEventGroupSetBits(eventos_teclas, BOTON_PRENDER_APRETADO);
        }
        else if (DigitalInputHasDeactivated(board->boton_prender))
        {
            xEventGroupSetBits(eventos_teclas, BOTON_PRENDER_LIBERADO);
        }

        if (DigitalInputHasActivated(board->boton_prueba))
        {
            xEventGroupSetBits(eventos_teclas, BOTON_PROBAR_APRETADO);
        }
        else if (DigitalInputHasDeactivated(board->boton_prueba))
        {
            xEventGroupSetBits(eventos_teclas, BOTON_PROBAR_LIBERADO);
        }

        vTaskDelay(pdMS_TO_TICKS(150));
    }
}

/* === Definiciones de variables internas ================================== */
static board_t board;
/* === Definiciones de variables externas ================================== */

/* === Definiciones de funciones internas ================================== */

/* === Definiciones de funciones externas ================================== */

/** @brief Función principal del programa
 **
 ** @returns 0 La función nunca debería termina
 **
 ** @remarks En un sistema embebido la función main() nunca debe terminar.
 **          El valor de retorno 0 es para evitar un error en el compilador.
 */
int main(void)
{
    /* Inicializaciones y configuraciones de dispositivos */
    board = BoardCreate();
    eventos_teclas = xEventGroupCreate();

    /* Creación de las tareas */
    xTaskCreate(Teclado, "Teclado", configMINIMAL_STACK_SIZE, (void *)board, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(Azul, "Azul", configMINIMAL_STACK_SIZE, (void *)board, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(Amarilla, "Amarilla", configMINIMAL_STACK_SIZE, (void *)board, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(Roja, "Roja", configMINIMAL_STACK_SIZE, (void *)board, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(Verde, "Verde", configMINIMAL_STACK_SIZE, (void *)board, tskIDLE_PRIORITY + 1, NULL);

    /* Arranque del sistema operativo */
    vTaskStartScheduler();

    /* vTaskStartScheduler solo retorna si se detiene el sistema operativo */
    while (1)
    {
    };

    /* El valor de retorno es solo para evitar errores en el compilador*/
    return 0;
}
/* === Ciere de documentacion ============================================== */
/** @} Final de la definición del modulo para doxygen */