#ifndef INC_MAGNETOMETRO_H
#define	INC_MAGNETOMETRO_H

#include <xc.h> // include processor files - each processor file is guarded. 

#include <stdint.h>

typedef struct {
    uint16_t x, y, z;
} coordenadas_t;

void magnetoInit(void);
void magnetoGetCord(coordenadas_t *coord);

#endif	/* INC_MAGNETOMETRO_H */

