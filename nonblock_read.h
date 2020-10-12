#ifndef NONBLOCK_READ_H
#define NONBLOCK_READ_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void analogPrepareRead(uint8_t pin);
int analogReadData(void);

#ifdef __cplusplus
}
#endif

#endif // NONBLOCK_READ_H
