#ifndef BIBLIO_H
#define BIBLIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

/* Dimensiones del mapa */
#define MAP_WIDTH 40
#define MAP_HEIGHT 10

/* Estructura que representa al jugador del juego */
typedef struct {
    int x;
    int y;
    int tamaño;
    int velocidadX;
    int velocidadY;
    int velocidadMovimiento;
    int gravedad;
    int enSuelo;
    int puedeSaltar;
} Jugador;

/* Estructura para representar un bloque o plataforma */
typedef struct {
    int x;
    int y;
    int ancho;
    int alto;
} Rectangulo;

/* Declaraciones de funciones del juego */
void cargarMapaDesdeArchivo(const char* nombreArchivo, char mapa[MAP_HEIGHT][MAP_WIDTH]);
void dibujarMapa(char mapa[MAP_HEIGHT][MAP_WIDTH], int anchoTile, int altoTile);
void actualizarJugador(Jugador* jugador, int anchoMaximo, int altoMaximo);
void dibujarJugador(Jugador* jugador);
void dibujarRectangulo(Rectangulo* rectangulo);
int detectarColision(Jugador* jugador, Rectangulo* rectangulo);
void resolverColision(Jugador* jugador, Rectangulo* rectangulo);
void resolverColisionesMapa(Jugador* jugador, char mapa[MAP_HEIGHT][MAP_WIDTH], int anchoTile, int altoTile);

#endif
