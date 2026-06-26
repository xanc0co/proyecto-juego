#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#define MAP_WIDTH 40
#define MAP_HEIGHT 10

/* Estructura para representar el cubo */
typedef struct {
    int x;
    int y;
    int size ;
    int velocityX;
    int velocityY;
    int speed;
} Cube;

/* Estructura para representar el rectángulo */
typedef struct {
    int x;
    int y;
    int width;
    int height;
} Rectangle;

/* Declaraciones de funciones */
void generarMapa();
void actualizarCubo(Cube* cubo, int maxWidth, int maxHeight);
void dibujarCubo(Cube* cubo);
void dibujarRectangulo(Rectangle* rect);
int detectarColision(Cube* cubo, Rectangle* rect);
void resolverColision(Cube* cubo, Rectangle* rect);

int main(int argc, char **argv) 
{
    ALLEGRO_DISPLAY* display = NULL;
    ALLEGRO_EVENT_QUEUE* event_queue = NULL;
    ALLEGRO_TIMER* timer = NULL;
    ALLEGRO_EVENT event;
    ALLEGRO_BITMAP* fondo = NULL;
    Cube miCubo;
    Rectangle rectangulo;
    int need_redraw = 1;
    int running = 1;
    ALLEGRO_FONT* fuente = al_load_ttf_font("angel_wish.ttf",0,0);

    char map[MAP_HEIGHT][MAP_WIDTH]={{0}}; /* Matriz para almacenar el mapa del juego */
    
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

    if (!al_init_font_addon()) {
        fprintf(stderr, "Error al inicializar addon de fuentes\n");
        return -1;
    }
    
    fuente=al_load_ttf_font("arial.ttf", 24, 0);
    if (!fuente) {
        fprintf(stderr, "Error al cargar fuente\n");
        return -1;
    }

    
    /* Inicializar addon de imágenes */
    if (!al_init_image_addon()) {
        fprintf(stderr, "Error al inicializar addon de imágenes\n");
        return -1;
    }

    
    /* Cargar imagen de fondo 
    fondo = al_load_bitmap("fondo.jpg");
    if (!fondo) {
        fprintf(stderr, "Error al cargar la imagen\n");
        return -1;
    }*/
    
    int imgWidth = 800;   // Ancho por defecto
    int imgHeight = 600;  // Alto por defecto
    
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
    miCubo.size = 30; /* Tamaño inicial del cubo (px). Cambia este valor si quieres otro tamaño por defecto */
    miCubo.velocityX = 0;
    miCubo.velocityY = 0;
    miCubo.speed = 10;
    
    /* Inicializar el rectángulo centrado */
    rectangulo.width = 150;
    rectangulo.height = 100;
    rectangulo.x = (imgWidth - rectangulo.width) / 2;
    rectangulo.y = (imgHeight - rectangulo.height) / 2;
    
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
al_clear_to_color(al_map_rgb(0,0,0));

        al_draw_text(fuente,
                     al_map_rgb(255,255,255),
                     400,
                     300,
                     ALLEGRO_ALIGN_CENTER,
                     "Hola Mundo");

        al_flip_display();

        al_rest(0.01);
        


        
        
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
            resolverColision(&miCubo, &rectangulo);
        }
        
        if (need_redraw && al_is_event_queue_empty(event_queue)) {
            need_redraw = 0;
            
            /* Limpiar la pantalla con fondo blanco */
            al_clear_to_color(al_map_rgb(0, 0, 0));
            
            /* Dibujar rectángulo */
            dibujarRectangulo(&rectangulo);
            
            /* Dibujar cubo */
            dibujarCubo(&miCubo);
            
            al_draw_text(fuente, al_map_rgb(255, 255, 255), 10, 10, 0, "Use las flechas para mover el cubo. ESC para salir.");
            
            /* Mostrar cambios */
            al_flip_display();
        }
    }
    
    /* Liberar recursos */
    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);
    al_destroy_font(fuente);
    al_destroy_display(display);
    
    printf("Juego finalizado\n");
    
    return 0;
} 

/* Función para generar el mapa en mapa.txt (sin bordes) */
void generarMapa() {
    FILE* archivo;
    int i, j;
    
    archivo = fopen("mapa.txt", "w");
    
    if (archivo == NULL) {
        printf("Error: No se pudo crear el archivo mapa.txt\n");
        return;
    }
    
    for (i = 0; i < MAP_HEIGHT; i++) {
        for (j = 0; j < MAP_WIDTH; j++) {
            if (i < 4) {
                // Primeras 4 filas: piso con muros
                fprintf(archivo, "f");
            } else if (i >= 4 && i < 6) {
                // Muros horizontales con pasillos
                if ((j >= 5 && j <= 10) || (j >= 16 && j <= 21) || (j >= 27 && j <= 32)) {
                    fprintf(archivo, "r");
                } else {
                    fprintf(archivo, "f");
                }
            } else if (i >= 6 && i < 8) {
                // Siguientes filas: más piso
                fprintf(archivo, "f");
            } else {
                // Últimas filas: suelo
                fprintf(archivo, "s");
            }
        }
        fprintf(archivo, "\n");
    }
    
    fclose(archivo);
    printf("Mapa generado en mapa.txt\n");
}

/* Función para actualizar la posición del cubo utilizando de parametro la estructura del cubo y el largo con el ancho */
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

/* Función para dibujar el rectángulo */
void dibujarRectangulo(Rectangle* rect) {
    al_draw_filled_rectangle(
        rect->x,
        rect->y,
        rect->x + rect->width,
        rect->y + rect->height,
        al_map_rgb(200,200,200)
    );
}

/* Función para detectar colisión AABB (Axis-Aligned Bounding Box) */
int detectarColision(Cube* cubo, Rectangle* rect) {
    return !(cubo->x + cubo->size < rect->x || 
             cubo->x > rect->x + rect->width || 
             cubo->y + cubo->size < rect->y || 
             cubo->y > rect->y + rect->height);
}

/* Función para resolver colisión entre cubo y rectángulo */
void resolverColision(Cube* cubo, Rectangle* rect) {
    if (!detectarColision(cubo, rect)) {
        return;
    }
    
    /* Calcular la distancia desde el centro del cubo a cada lado del rectángulo */
    int cuboCenterX = cubo->x + cubo->size / 2;
    int cuboCenterY = cubo->y + cubo->size / 2;
    
    int distLeft = cuboCenterX - rect->x;
    int distRight = (rect->x + rect->width) - cuboCenterX;
    int distTop = cuboCenterY - rect->y;
    int distBottom = (rect->y + rect->height) - cuboCenterY;
    
    /* Encontrar la distancia mínima para determinar de qué lado colisionar */
    int minDist = distLeft;
    int lado = 0; /* 0: izq, 1: der, 2: arriba, 3: abajo */
    
    if (distRight < minDist) {
        minDist = distRight;
        lado = 1;
    }
    if (distTop < minDist) {
        minDist = distTop;
        lado = 2;
    }
    if (distBottom < minDist) {
        minDist = distBottom;
        lado = 3;
    }
    
    /* Resolver la colisión empujando el cubo fuera del rectángulo */
    switch(lado) {
        case 0: /* Colisión por la izquierda */
            cubo->x = rect->x - cubo->size;
            cubo->velocityX = 0;
            break;
        case 1: /* Colisión por la derecha */
            cubo->x = rect->x + rect->width;
            cubo->velocityX = 0;
            break;
        case 2: /* Colisión por arriba */
            cubo->y = rect->y - cubo->size;
            cubo->velocityY = 0;
            break;
        case 3: /* Colisión por abajo */
            cubo->y = rect->y + rect->height;
            cubo->velocityY = 0;
            break;
    }
}
