#include <Arduino.h>
#include <i2s.h>
#include <stddef.h>
#include <stdint.h>
#include "MAX98357A.h"

size_t MAX98357A::write(int16_t sample) {
    size_t ret = 0;
    if (i2s_write_lr(sample, sample)) {
        ret++;
    }
    return ret;
}

size_t MAX98357A::write(const int16_t *buffer, size_t length) {
    uint16_t written = i2s_write_buffer_mono((int16_t *)buffer, (uint16_t)length);
    return (size_t)written;
}
