#include "../headers/biblio.h"

/* Función auxiliar para detectar colisión rectangular genérica entre dos cuadrados */
int detectarColisionRectangulos(float x1, float y1, int tamaño1, float x2, float y2, int tamaño2) {
    return !(x1 + tamaño1 < x2 || 
             x1 > x2 + tamaño2 || 
             y1 + tamaño1 < y2 || 
             y1 > y2 + tamaño2);
}

/* Carga las filas del mapa desde un archivo de texto */
void cargarMapaDesdeArchivo(const char* nombreArchivo, char mapa[MAP_HEIGHT][MAP_WIDTH]) {
    FILE* archivo = fopen(nombreArchivo, "r");
    char linea[1024];
    int fila = 0;

    if (archivo == NULL) {
        fprintf(stderr, "Error: No se pudo abrir %s\n", nombreArchivo);
        return;
    }

    while (fila < MAP_HEIGHT && fgets(linea, sizeof(linea), archivo) != NULL) {
        int largo = strlen(linea);
        while (largo > 0 && (linea[largo - 1] == '\n' || linea[largo - 1] == '\r' || linea[largo - 1] == ' ')) {
            linea[--largo] = '\0';
        }

        if (largo == 0) {
            continue;
        }

        int col = 0;
        const unsigned char* p = (const unsigned char*)linea;

        while (col < MAP_WIDTH && *p != '\0') {
            if (*p == 0xE2 && *(p + 1) == 0x97 && *(p + 2) == 0x8B) {
                mapa[fila][col] = 'o';
                p += 3;
                col++;
            }
            else if (*p == 0xE2 && *(p + 1) == 0x97 && *(p + 2) == 0x8F) {
                mapa[fila][col] = 'o';
                p += 3;
                col++;
            }
            else if (*p == 'M' || *p == 'm' || *p == 'r') {
                mapa[fila][col] = 'r';
                p++;
                col++;
            }
            else if (*p == 'p' || *p == '$' || *p == 'o' || *p == 'O' || *p == 'f') {
                mapa[fila][col] = (char)*p;
                p++;
                col++;
            }
            else if (*p == ' ' || *p == '\t') {
                p++;
            }
            else {
                mapa[fila][col] = 'o';
                p++;
                col++;
            }
        }

        while (col < MAP_WIDTH) {
            mapa[fila][col++] = 'o';
        }

        fila++;
    }

    fclose(archivo);
}

/* Posiciona los enemigos según lo definido en el mapa.txt (símbolo '$') */
void cargarEnemigosDelMapa(char mapa[MAP_HEIGHT][MAP_WIDTH], Enemigo enemigos[], int maxEnemigos, int anchoTile, int altoTile, int tamanioJugador) {
    int enemigoIndex = 0;
    int tamañoEnemigo = (anchoTile < altoTile ? anchoTile : altoTile) - 4;
    if (tamañoEnemigo < 12) {
        tamañoEnemigo = 12;
    }

    for (int y = 0; y < MAP_HEIGHT && enemigoIndex < maxEnemigos; y++) {
        for (int x = 0; x < MAP_WIDTH && enemigoIndex < maxEnemigos; x++) {
            if (mapa[y][x] == '$') {
                enemigos[enemigoIndex].tamaño = tamañoEnemigo;
                enemigos[enemigoIndex].x = x * anchoTile + (anchoTile - enemigos[enemigoIndex].tamaño) / 2;
                enemigos[enemigoIndex].y = y * altoTile + (altoTile - enemigos[enemigoIndex].tamaño) / 2;
                enemigos[enemigoIndex].velocidadX = 0;
                enemigos[enemigoIndex].activo = 1;
                enemigos[enemigoIndex].vida = 1;
                enemigoIndex++;
                mapa[y][x] = 'o';  /* Cambiar '$' a fondo para que no se dibuje como plataforma */
            }
        }
    }
}

/* Dibuja todos los bloques del mapa que estén marcados como rectángulos */
void dibujarMapa(char mapa[MAP_HEIGHT][MAP_WIDTH], int anchoTile, int altoTile) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            char tile = mapa[y][x];
            if (tile == 'r') {
                Rectangulo rect;
                rect.x = x * anchoTile;
                rect.y = y * altoTile;
                rect.ancho = anchoTile;
                rect.alto = altoTile;
                dibujarRectangulo(&rect);
            }
        }
    }
}

/* Actualiza la posición del jugador aplicando gravedad y movimiento */
void actualizarJugador(Jugador* jugador, int anchoMaximo, int altoMaximo) {
    jugador->velocidadY += jugador->gravedad;
    jugador->x += jugador->velocidadX;
    jugador->y += jugador->velocidadY;
    jugador->enSuelo = 0;

    if (jugador->x < 0) {
        jugador->x = 0;
    }
    if (jugador->x + jugador->tamaño > anchoMaximo) {
        jugador->x = anchoMaximo - jugador->tamaño;
    }
    if (jugador->y < 0) {
        jugador->y = 0;
    }

    if (jugador->velocidadY > 12) {
        jugador->velocidadY = 12;
    }
}

/* Dibuja el personaje como un rectángulo verde */
void dibujarJugador(Jugador* jugador, ALLEGRO_BITMAP* imagenJugador) {
    int anchoJugador = jugador->tamaño;
    int altoJugador = jugador->tamaño;

    if (imagenJugador != NULL) {
        if (jugador->direccion == 1) {
            // mirando a la derecha
            al_draw_scaled_bitmap(
                imagenJugador,
                0, 0,
                al_get_bitmap_width(imagenJugador),
                al_get_bitmap_height(imagenJugador),
                jugador->x,
                jugador->y,
                anchoJugador,
                altoJugador,
                0
            );
        } else {
            // mirando a la izquierda (volteado horizontalmente)
            al_draw_scaled_bitmap(
                imagenJugador,
                0, 0,
                al_get_bitmap_width(imagenJugador),
                al_get_bitmap_height(imagenJugador),
                jugador->x + anchoJugador, // desplazamiento para voltear
                jugador->y,
                -anchoJugador, // escala negativa en X
                altoJugador,
                0
            );
        }
    } else {
        al_draw_filled_rectangle(
            jugador->x, 
            jugador->y, 
            jugador->x + jugador->tamaño, 
            jugador->y + jugador->tamaño,
            al_map_rgb(0, 255, 0)
        );
    }
}

/* Dibuja un bloque o plataforma del escenario */
void dibujarRectangulo(Rectangulo* rectangulo) {
    al_draw_filled_rectangle(
        rectangulo->x,
        rectangulo->y,
        rectangulo->x + rectangulo->ancho,
        rectangulo->y + rectangulo->alto,
        al_map_rgb(200,200,200)
    );
}

/* Revisa todas las plataformas del mapa y aplica colisiones */
void resolverColisionesMapa(Jugador* jugador, char mapa[MAP_HEIGHT][MAP_WIDTH], int anchoTile, int altoTile) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (mapa[y][x] != 'r') {
                continue;
            }

            Rectangulo rect;
            rect.x = x * anchoTile;
            rect.y = y * altoTile;
            rect.ancho = anchoTile;
            rect.alto = altoTile;
            resolverColision(jugador, &rect);
        }
    }
}

/* Detecta si hay colisión rectangular entre el jugador y un bloque */
int detectarColision(Jugador* jugador, Rectangulo* rectangulo) {
    return !(jugador->x + jugador->tamaño < rectangulo->x || 
             jugador->x > rectangulo->x + rectangulo->ancho || 
             jugador->y + jugador->tamaño < rectangulo->y || 
             jugador->y > rectangulo->y + rectangulo->alto);
}

/* Corrige la posición del jugador cuando choca con un rectángulo */
void resolverColision(Jugador* jugador, Rectangulo* rectangulo) {
    if (!detectarColision(jugador, rectangulo)) {
        return;
    }

    int playerBottom = jugador->y + jugador->tamaño;
    int playerTop = jugador->y;
    int playerRight = jugador->x + jugador->tamaño;
    int playerLeft = jugador->x;

    int rectTop = rectangulo->y;
    int rectBottom = rectangulo->y + rectangulo->alto;
    int rectLeft = rectangulo->x;
    int rectRight = rectangulo->x + rectangulo->ancho;

    int overlapTop = playerBottom - rectTop;
    int overlapBottom = rectBottom - playerTop;
    int overlapLeft = playerRight - rectLeft;
    int overlapRight = rectRight - playerLeft;

    int minOverlap = overlapTop;
    if (overlapBottom < minOverlap) minOverlap = overlapBottom;
    if (overlapLeft < minOverlap) minOverlap = overlapLeft;
    if (overlapRight < minOverlap) minOverlap = overlapRight;

    if (minOverlap == overlapTop && jugador->velocidadY >= 0) {
        jugador->y = rectTop - jugador->tamaño;
        jugador->velocidadY = 0;
        jugador->enSuelo = 1;
        return;
    }

    if (minOverlap == overlapBottom && jugador->velocidadY < 0) {
        jugador->y = rectBottom;
        jugador->velocidadY = 0;
        return;
    }

    if (minOverlap == overlapLeft) {
        jugador->x = rectLeft - jugador->tamaño;
    } else if (minOverlap == overlapRight) {
        jugador->x = rectRight;
    }

    jugador->velocidadX = 0;
}

/* Función principal del juego */
int main(int argc, char **argv) 
{
    ALLEGRO_DISPLAY* pantalla = NULL;
    ALLEGRO_EVENT_QUEUE* cola_eventos = NULL;
    ALLEGRO_TIMER* temporizador = NULL;
    ALLEGRO_EVENT evento;
    ALLEGRO_BITMAP* fondo = NULL;
    ALLEGRO_BITMAP* imagenJugador = NULL;
    ALLEGRO_KEYBOARD_STATE teclado;
    Jugador jugador;
    Rectangulo rectangulo;
    int necesita_redibujar = 1;
    int ejecutando = 1;
    ALLEGRO_FONT* fuente = NULL;
    Proyectil proyectiles[MAX_PROYECTILES];
    Enemigo enemigos[MAX_ENEMIGOS];

    char mapa[MAP_HEIGHT][MAP_WIDTH]={{0}}; /* Matriz para almacenar el mapa del juego */
    
    /* 1. Inicializar Allegro y sus módulos básicos */
    if (!al_init()) {
        fprintf(stderr, "Error al inicializar Allegro\n");
        return -1;
    }

    
    /* 2. Configurar entrada por teclado */
    if (!al_install_keyboard()) {
        fprintf(stderr, "Error al instalar teclado\n");
        return -1;
    }

    if (!al_init_font_addon()) {
        fprintf(stderr, "Error al inicializar addon de fuentes\n");
        return -1;
    }

    if (!al_init_ttf_addon()) {
        fprintf(stderr, "Error al inicializar addon TTF\n");
        return -1;
    }
    
    fuente = al_load_ttf_font("angel_wish.ttf", 24, 0);
    if (!fuente) {
        fprintf(stderr, "Error al cargar fuente angel_wish.ttf\n");
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
        fprintf(stderr, "Error al cargar fondo.jpg\n");
        return -1;
    }

    /* Cargar imagen del jugador */
    imagenJugador = al_load_bitmap("player.png");
    if (!imagenJugador) {
        fprintf(stderr, "Error al cargar player.png\n");
        al_destroy_bitmap(fondo);
        return -1;
    }
    
    int anchoPantalla = 1366;   // Resolución de la pantalla actual
    int altoPantalla = 768;
    
    /* Crear pantalla en modo pantalla completa */
    al_set_new_display_flags(ALLEGRO_FULLSCREEN);
    pantalla = al_create_display(anchoPantalla, altoPantalla);
    if (!pantalla) {
        fprintf(stderr, "Error al crear la pantalla\n");
        return -1;
    }
    
    al_set_window_title(pantalla, "Juego del jugador - Allegro");
    
    /* Crear temporizador */
    temporizador = al_create_timer(1.0 / 60.0);
    if (!temporizador) {
        fprintf(stderr, "Error al crear el temporizador\n");
        al_destroy_display(pantalla);
        return -1;
    }



    
    /* Crear cola de eventos */
    cola_eventos = al_create_event_queue();
    if (!cola_eventos) {
        fprintf(stderr, "Error al crear la cola de eventos\n");
        al_destroy_timer(temporizador);
        al_destroy_display(pantalla);
        return -1;
    }
    
    /* Registrar fuentes de eventos */
    al_register_event_source(cola_eventos, al_get_display_event_source(pantalla));
    al_register_event_source(cola_eventos, al_get_keyboard_event_source());
    al_register_event_source(cola_eventos, al_get_timer_event_source(temporizador));
    
    /* 3. Cargar el mapa desde el archivo de texto */
    cargarMapaDesdeArchivo("mapa.txt", mapa);

    /* 4. Calcular el tamaño de cada celda del mapa */
    int anchoTile = anchoPantalla / MAP_WIDTH;
    int altoTile = altoPantalla / MAP_HEIGHT;

    /* 5. Inicializar el jugador según la posición marcada en el mapa */
    jugador.tamaño = (anchoTile + altoTile) / 2;
    if (jugador.tamaño < 50) {
        jugador.tamaño = 50;
    }
    jugador.velocidadX = 0;
    jugador.velocidadY = 0;
    jugador.velocidadMovimiento = 9;
    jugador.gravedad = 1;
    jugador.enSuelo = 0;
    jugador.puedeSaltar = 1;
    jugador.balasMaximas = 100;
    jugador.balas = jugador.balasMaximas;
    inicializarProyectiles(proyectiles, MAX_PROYECTILES);
    inicializarEnemigos(enemigos, MAX_ENEMIGOS, jugador.tamaño, anchoPantalla, altoPantalla);

    int inicioEncontrado = 0;
    int inicioX = 0;
    int inicioY = 0;

    for (int y = 0; y < MAP_HEIGHT && !inicioEncontrado; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (mapa[y][x] == 'p') {
                inicioEncontrado = 1;
                inicioX = x;
                inicioY = y;
                break;
            }
        }
    }

    /* Cargar enemigos desde el mapa */
    cargarEnemigosDelMapa(mapa, enemigos, MAX_ENEMIGOS, anchoTile, altoTile, jugador.tamaño);

    if (inicioEncontrado) {
        jugador.x = inicioX * anchoTile + (anchoTile - jugador.tamaño) / 2;
        jugador.y = inicioY * altoTile + (altoTile - jugador.tamaño) / 2;
    } else {
        jugador.x = 0;
        jugador.y = altoPantalla - jugador.tamaño;
    }
    
    /* Inicializar el rectángulo centrado */
    rectangulo.ancho = 150;
    rectangulo.alto = 100;
    rectangulo.x = (anchoPantalla - rectangulo.ancho) / 2;
    rectangulo.y = (altoPantalla - rectangulo.alto) / 2;
    
    printf("Iniciando juego...\n");
    
    /* 6. Inicializar primitivas para dibujar rectángulos */
    if (!al_init_primitives_addon()) {
        fprintf(stderr, "Error al inicializar primitivas\n");
        al_destroy_event_queue(cola_eventos);
        al_destroy_timer(temporizador);
        al_destroy_display(pantalla);
        return -1;
    }
    
    /* 7. Iniciar temporizador del juego */
    al_start_timer(temporizador);
    
    /* 8. Bucle principal del juego */
    while (ejecutando) {
        al_wait_for_event(cola_eventos, &evento);

        /* Cerrar el juego si se cierra la ventana */
        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            ejecutando = 0;
        }
        /* Reaccionar a teclas presionadas */
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                ejecutando = 0;
            }
            else if (evento.keyboard.keycode == ALLEGRO_KEY_K) {
                dispararProyectil(proyectiles, MAX_PROYECTILES, &jugador);
                necesita_redibujar = 1;
            }
        }
        /* Actualizar lógica del juego cada frame */
        else if (evento.type == ALLEGRO_EVENT_TIMER) {
            al_get_keyboard_state(&teclado);

            if (al_key_down(&teclado, ALLEGRO_KEY_LEFT)) {
                jugador.velocidadX = -jugador.velocidadMovimiento;
                jugador.direccion = -1;//mira a la derecha
            }
            else if (al_key_down(&teclado, ALLEGRO_KEY_RIGHT)) {
                jugador.velocidadX = jugador.velocidadMovimiento;
                jugador.direccion = 1;//mira a la izquierda
            }
            else {
                jugador.velocidadX = 0;
            }

            if (al_key_down(&teclado, ALLEGRO_KEY_SPACE) && jugador.enSuelo && jugador.puedeSaltar) {
                jugador.velocidadY = -15;
                jugador.enSuelo = 0;
            }

            necesita_redibujar = 1;
            actualizarJugador(&jugador, anchoPantalla, altoPantalla);
            actualizarProyectiles(proyectiles, MAX_PROYECTILES, anchoPantalla, altoPantalla);
            actualizarEnemigos(enemigos, MAX_ENEMIGOS, anchoPantalla);
            detectarColisionesProyectilEnemigo(proyectiles, MAX_PROYECTILES, enemigos, MAX_ENEMIGOS);
            resolverColisionesMapa(&jugador, mapa, anchoTile, altoTile);
            resolverColision(&jugador, &rectangulo);

            if (jugador.y + jugador.tamaño >= altoPantalla && !jugador.enSuelo) {
                reiniciarJugador(&jugador, inicioX, inicioY, anchoTile, altoTile);
            }
        }
        
        /* Dibujar solo cuando el juego está listo para renderizar */
        if (necesita_redibujar && al_is_event_queue_empty(cola_eventos)) {
            necesita_redibujar = 0;
            
            /* Limpiar la pantalla con fondo negro */
            al_clear_to_color(al_map_rgb(0, 0, 0));

            /* Dibujar el fondo cargado */
            if (fondo != NULL) {
                al_draw_scaled_bitmap(
                    fondo,
                    0, 0,
                    al_get_bitmap_width(fondo),
                    al_get_bitmap_height(fondo),
                    0, 0,
                    anchoPantalla,
                    altoPantalla,
                    0
                );
            }

            /* Dibujar el mapa completo */
            dibujarMapa(mapa, anchoTile, altoTile);
            
            /* Dibujar jugador */
            dibujarJugador(&jugador, imagenJugador);
            dibujarEnemigos(enemigos, MAX_ENEMIGOS);
            dibujarProyectiles(proyectiles, MAX_PROYECTILES, fuente);

            char textoBalas[64];
            snprintf(textoBalas, sizeof(textoBalas), "Balas: %d", jugador.balas);
            al_draw_text(fuente, al_map_rgb(255, 255, 255), 10, 10, ALLEGRO_ALIGN_LEFT, textoBalas);
            al_draw_text(fuente, al_map_rgb(255, 255, 255), 10, 40, ALLEGRO_ALIGN_LEFT, "Usa las flechas para mover al jugador. ESC para salir. K para disparar");
            
            /* Mostrar cambios */
            al_flip_display();
        }
    }
    
    /* Liberar recursos */
    al_destroy_bitmap(fondo);
    al_destroy_event_queue(cola_eventos);
    al_destroy_timer(temporizador);
    al_destroy_font(fuente);
    al_destroy_display(pantalla);
    
    printf("Juego finalizado\n");
    
    return 0;
}

/* Inicializa todos los proyectiles como inactivos */
void inicializarProyectiles(Proyectil proyectiles[], int cantidad) {
    for (int i = 0; i < cantidad; i++) {
        proyectiles[i].activo = 0;
        proyectiles[i].x = 0;
        proyectiles[i].y = 0;
        proyectiles[i].velocidadX = 0;
        proyectiles[i].velocidadY = 0;
        proyectiles[i].tamaño = 5;
        proyectiles[i].direccion = 1;
    }
}

/* Dispara un proyectil desde la posición del jugador */
void dispararProyectil(Proyectil proyectiles[], int cantidad, Jugador* jugador) {
    if (jugador->balas <= 0) return;
    
    for (int i = 0; i < cantidad; i++) {
        if (!proyectiles[i].activo) {
            proyectiles[i].x = jugador->x + jugador->tamaño / 2;
            proyectiles[i].y = jugador->y + jugador->tamaño / 2;
            proyectiles[i].velocidadX = 12 * jugador->direccion; //dispara segun adonde apuntes
            proyectiles[i].velocidadY = 0; /* Sin gravedad */
            proyectiles[i].activo = 1;
            jugador->balas--; /* Descontar bala */
            break;
        }
    }
}

/* Actualiza la posición de los proyectiles activos */
void actualizarProyectiles(Proyectil proyectiles[], int cantidad, int anchoPantalla, int altoPantalla) {
    for (int i = 0; i < cantidad; i++) {
        if (proyectiles[i].activo) {
            proyectiles[i].x += proyectiles[i].velocidadX;
            proyectiles[i].y += proyectiles[i].velocidadY;
            
            /* Desactivar si sale de pantalla */
            if (proyectiles[i].x < 0 || proyectiles[i].x > anchoPantalla ||
                proyectiles[i].y < 0 || proyectiles[i].y > altoPantalla) {
                proyectiles[i].activo = 0;
            }
        }
    }
}

/* Dibuja todos los proyectiles activos */
void dibujarProyectiles(Proyectil proyectiles[], int cantidad, ALLEGRO_FONT* fuente) {
    for (int i = 0; i < cantidad; i++) {
        if (proyectiles[i].activo) {
            al_draw_filled_rectangle(
                proyectiles[i].x,
                proyectiles[i].y,
                proyectiles[i].x + proyectiles[i].tamaño * 2,
                proyectiles[i].y + proyectiles[i].tamaño,
                al_map_rgb(0, 255, 0)
            );
            al_draw_text(fuente, al_map_rgb(0, 0, 0), proyectiles[i].x + proyectiles[i].tamaño / 2, proyectiles[i].y - 5, ALLEGRO_ALIGN_CENTER, "~");
        }
    }
}

/* Inicializa todos los enemigos como inactivos */
void inicializarEnemigos(Enemigo enemigos[], int cantidad, int tamanioJugador, int anchoPantalla, int altoPantalla) {
    for (int i = 0; i < cantidad; i++) {
        enemigos[i].activo = 0;
        enemigos[i].x = 0;
        enemigos[i].y = 0;
        enemigos[i].tamaño = 20;
        enemigos[i].velocidadX = 3;
        enemigos[i].vida = 1;
    }
}

/* Actualiza la posición de los enemigos activos */
void actualizarEnemigos(Enemigo enemigos[], int cantidad, int anchoPantalla) {
    for (int i = 0; i < cantidad; i++) {
        if (enemigos[i].activo) {
            enemigos[i].x += enemigos[i].velocidadX;
            
            /* Cambiar dirección al llegar a los bordes */
            if (enemigos[i].x < 0 || enemigos[i].x + enemigos[i].tamaño > anchoPantalla) {
                enemigos[i].velocidadX = -enemigos[i].velocidadX;
            }
        }
    }
}

/* Dibuja todos los enemigos activos */
void dibujarEnemigos(Enemigo enemigos[], int cantidad) {
    for (int i = 0; i < cantidad; i++) {
        if (enemigos[i].activo) {
            al_draw_filled_rectangle(
                enemigos[i].x,
                enemigos[i].y,
                enemigos[i].x + enemigos[i].tamaño,
                enemigos[i].y + enemigos[i].tamaño,
                al_map_rgb(255, 0, 0)
            );
        }
    }
}

/* Detecta colisiones entre proyectiles y enemigos */
void detectarColisionesProyectilEnemigo(Proyectil proyectiles[], int cantProyectiles, Enemigo enemigos[], int cantEnemigos) {
    for (int i = 0; i < cantProyectiles; i++) {
        if (proyectiles[i].activo) {
            for (int j = 0; j < cantEnemigos; j++) {
                if (enemigos[j].activo) {
                    if (detectarColisionRectangulos(proyectiles[i].x, proyectiles[i].y, proyectiles[i].tamaño * 2,
                                                     enemigos[j].x, enemigos[j].y, enemigos[j].tamaño)) {
                        proyectiles[i].activo = 0;
                        enemigos[j].activo = 0;
                    }
                }
            }
        }
    }
}

/* Reinicia la posición del jugador a su punto de inicio */
void reiniciarJugador(Jugador* jugador, int xInicial, int yInicial, int anchoTile, int altoTile) {
    jugador->x = xInicial * anchoTile + (anchoTile - jugador->tamaño) / 2;
    jugador->y = yInicial * altoTile + (altoTile - jugador->tamaño) / 2;
    jugador->velocidadX = 0;
    jugador->velocidadY = 0;
    jugador->enSuelo = 0;
}
