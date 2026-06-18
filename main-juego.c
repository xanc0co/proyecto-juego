#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

/* Estructura para representar el cubo */
typedef struct {
    int x;
    int y;
    int size ;
    int velocityX;
    int velocityY;
    int speed;
} Cube;

/* Función para generar el mapa en mapa.txt (sin bordes) */
void generarMapa() {
    FILE* archivo;
    int i, j;
    int mapWidth = 40; //largo del mapa
    int mapHeight = 10;//ancho del mapa
    
    archivo = fopen("mapa.txt", "w");
    
    if (archivo == NULL) {
        printf("Error: No se pudo crear el archivo mapa.txt\n");
        return;
    }
    
    fprintf(archivo, "=== MAPA DEL JUEGO ===\n");
    fprintf(archivo, "Resolucion: Ajustada a la imagen\n");
    fprintf(archivo, "Tamaño del Cubo: 20x20\n\n");
    
    fprintf(archivo, "Representacion visual del mapa:\n\n");
    
    for (i = 0; i < mapHeight; i++) {
        for (j = 0; j < mapWidth; j++) {
            if (i == 10 && j >= 15 && j <= 25) {
                fprintf(archivo, "-");
            } else if (j == 20 && i >= 5 && i <= 15) {
                fprintf(archivo, "|");
            } else {
                fprintf(archivo, " ");
            }
        }
        fprintf(archivo, "\n");
    }
    
    fprintf(archivo, "s");
    fprintf(archivo, "r");
    fprintf(archivo, "f");

    
    fclose(archivo);
    printf("Mapa generado en mapa.txt\n");
}

/* Función para actualizar la posición del cubo */
void actualizarCubo(Cube* cubo, int maxWidth, int maxHeight) {
    cubo->x += cubo->velocityX;
    cubo->y += cubo->velocityY;
    
    /* Colisiones con los bordes de la imagen */
    if (cubo->x < 0) {
        cubo->x = 0;
    }
    if (cubo->x + cubo->size > maxWidth) {
        cubo->x = maxWidth - cubo->size;
    }
    if (cubo->y < 0) {
        cubo->y = 0;
    }
    if (cubo->y + cubo->size > maxHeight) {
        cubo->y = maxHeight - cubo->size;
    }
}

/* Función para dibujar el cubo */
void dibujarCubo(Cube* cubo) {
    al_draw_filled_rectangle(
        cubo->x, 
        cubo->y, 
        cubo->x + cubo->size, 
        cubo->y + cubo->size,
        al_map_rgb(0, 255, 0)
    );
}

int main(int argc, char **argv) 
{
    ALLEGRO_DISPLAY* display = NULL;
    ALLEGRO_EVENT_QUEUE* event_queue = NULL;
    ALLEGRO_TIMER* timer = NULL;
    ALLEGRO_EVENT event;
    ALLEGRO_BITMAP* fondo = NULL;
    Cube miCubo;
    int need_redraw = 1;
    int running = 1;
    
    /* Inicializar Allegro */
    if (!al_init()) {
        fprintf(stderr, "Error al inicializar Allegro\n");
        return -1;
    }
    
    /* Inicializar input */
    if (!al_install_keyboard()) {
        fprintf(stderr, "Error al instalar teclado\n");
        return -1;
    }
    
    /* Inicializar addon de imágenes */
    if (!al_init_image_addon()) {
        fprintf(stderr, "Error al inicializar addon de imágenes\n");
        return -1;
    }
    
    /* Cargar imagen de fondo */
    fondo = al_load_bitmap("fondo.jpg");
    if (!fondo) {
        fprintf(stderr, "Error al cargar la imagen\n");
        return -1;
    }
    
    int imgWidth = al_get_bitmap_width(fondo);
    int imgHeight = al_get_bitmap_height(fondo);
    
    /* Crear pantalla con resolución de la imagen */
    display = al_create_display(imgWidth, imgHeight);
    if (!display) {
        fprintf(stderr, "Error al crear display\n");
        return -1;
    }
    
    al_set_window_title(display, "Juego del cubo - Allegro");
    
    /* Crear timer */
    timer = al_create_timer(1.0 / 60.0);
    if (!timer) {
        fprintf(stderr, "Error al crear timer\n");
        al_destroy_display(display);
        return -1;
    }
    
    /* Crear cola de eventos */
    event_queue = al_create_event_queue();
    if (!event_queue) {
        fprintf(stderr, "Error al crear cola de eventos\n");
        al_destroy_timer(timer);
        al_destroy_display(display);
        return -1;
    }
    
    /* Registrar fuentes de eventos */
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    
    /* Inicializar el cubo */
    miCubo.x = imgWidth / 2;
    miCubo.y = imgHeight / 2;
    miCubo.size = 20;
    miCubo.velocityX = 0;
    miCubo.velocityY = 0;
    miCubo.speed = 5;
    
    printf("Generando mapa...\n");
    generarMapa();
    printf("Iniciando juego...\n");
    
    /* Inicializar primitivas */
    if (!al_init_primitives_addon()) {
        fprintf(stderr, "Error al inicializar primitivas\n");
        al_destroy_event_queue(event_queue);
        al_destroy_timer(timer);
        al_destroy_display(display);
        return -1;
    }
    
    /* Iniciar timer */
    al_start_timer(timer);
    
    /* Bucle principal */
    while (running) {
        al_wait_for_event(event_queue, &event);
        
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = 0;
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (event.keyboard.keycode) {
                case ALLEGRO_KEY_UP:
                    miCubo.velocityY = -miCubo.speed;
                    break;
                case ALLEGRO_KEY_DOWN:
                    miCubo.velocityY = miCubo.speed;
                    break;
                case ALLEGRO_KEY_LEFT:
                    miCubo.velocityX = -miCubo.speed;
                    break;
                case ALLEGRO_KEY_RIGHT:
                    miCubo.velocityX = miCubo.speed;
                    break;
                case ALLEGRO_KEY_ESCAPE:
                    running = 0;
                    break;
            }
        }
        else if (event.type == ALLEGRO_EVENT_KEY_UP) {
            switch (event.keyboard.keycode) {
                case ALLEGRO_KEY_UP:
                case ALLEGRO_KEY_DOWN:
                    miCubo.velocityY = 0;
                    break;
                case ALLEGRO_KEY_LEFT:
                case ALLEGRO_KEY_RIGHT:
                    miCubo.velocityX = 0;
                    break;
            }
        }
        else if (event.type == ALLEGRO_EVENT_TIMER) {
            need_redraw = 1;
            actualizarCubo(&miCubo, imgWidth, imgHeight);
        }
        
        if (need_redraw && al_is_event_queue_empty(event_queue)) {
            need_redraw = 0;
            
            /* Dibujar fondo */
            al_draw_bitmap(fondo, 0, 0, 0);
            
            /* Dibujar cubo */
            dibujarCubo(&miCubo);
            
            /* Mostrar cambios */
            al_flip_display();
        }
    }
    
    /* Liberar recursos */
    al_destroy_bitmap(fondo);
    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);
    al_destroy_display(display);
    
    printf("Juego finalizado\n");
    
    return 0;
} 
