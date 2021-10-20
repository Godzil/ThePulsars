#ifndef __PROLO_EXEC_H__
#define __PROLO_EXEC_H__

#define    SIGNE(_x)    (((_x) > 0) ? (1) : (((_x) < 0) ? (-1) : (0)))
#define    SQR(_x)        ((_x) * (_x))
#ifndef MAXFLOAT
#define MAXFLOAT    (3.40282347e+38F)
#endif

#define CLAMP(_v, _min, _max) ((_v < _min)?_min:((_v > _max)?_max:_v))

/*
  0 - Destroy
  1 - Link
  2 - Pulse
  3 - Move
  4 - Take
 */

void exec_r4d2_move(r4d2_t *r4d2);

void exec_akx_move(akx_t *akx);

void exec_r4d2_take_akx(r4d2_t *r4d2);

void exec_r4d2_take_r4d2(r4d2_t *r4d2);

void exec_akx_pulse(akx_t *akx);

/* AJOUTE par MATHIAS */
void exec_akx_reset_energy(akx_t *akx);

void exec_akx_link(akx_t *akx);

void exec_objects();

#endif
