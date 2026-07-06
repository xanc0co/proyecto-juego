#ifndef PROYECTIL_H
#define PROYECTIL_H

typedef struct {
    int x, y;
    int velocidadX, velocidadY;
    int tamaño;
    bool activo;
} Proyectil;

void inicializarProyectil(Proyectil* proyectil, int x, int y);
void actualizarProyectil(Proyectil* proyectil);
void dibujarProyectil(Proyectil* proyectil);

#endif