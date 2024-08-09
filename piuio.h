#ifndef _PIUIO_H_
#define _PIUIO_H_

#include <stdint.h>
#include "tusb.h"

#define PIUIO_VID 0x0547
#define PIUIO_PID 0x1002

#define PIUIO_BUFF_SIZE 8
#define PIUIO_MSG_REQ 0xAE

extern uint8_t buff_to_piuio[PIUIO_BUFF_SIZE];
extern uint8_t buff_from_piuio[PIUIO_BUFF_SIZE];

extern uint8_t piuio_dev_addr;

void piuio_task();
void piuio_init();

#endif