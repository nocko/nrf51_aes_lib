/* Utility functions for manipulating block_t structures; 128bit
   blocks of data for AES & CMAC */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#if __has_include("config.h")
#include "config.h"
#endif
#include "block.h"

#if defined(HOST_BUILD) || defined(DEBUG_UART)
void block_print(char const *const label, void const *const b) {
    block_print_bytes(label, b, 16);
    return;
}

void block_print_bytes(char const *const label, void const *const v,
                       uint32_t num_bytes) {
    const uint8_t *b = v;
    printf("%s: ", label);
    for (int i = 0; i < num_bytes; i++) {
        printf("%.2x", b[i]);
        if (!((i + 1) % 4)) {
            printf(" ");
        }
    }
    printf("\r\n");
}
#endif

void block_xor(uint8_t *dest, uint8_t const *const a, uint8_t const *const b) {
    for (uint_fast8_t i = 0; i < 4; i++) {
        *((uint32_t *)dest + i) = *((uint32_t *)a + i) ^ *((uint32_t *)b + i);
    }
    return;
}

void block_shiftr(uint8_t *dest, uint8_t *const a, uint_fast8_t num) {
    uint_fast8_t n = (num <= 8 ? num : 8);
    for (int_fast8_t i = 15; i >= 0; i--) {
        /* Shift right from LSB to MSB */
        dest[i] = a[i] >> n;
        if (i != 0) {
            dest[i] |= (a[i - 1] << (8 - n));
        }
    }
    if (num - n != 0) {
        block_shiftr(dest, dest, num - n);
    }
    return;
}

void block_shiftl(uint8_t *dest, uint8_t *const a, uint_fast8_t num) {
    uint_fast8_t n = (num <= 8 ? num : 8);
    for (int_fast8_t i = 0; i < 16; i++) {
        dest[i] = a[i] << n;
        if (i != 15) {
            dest[i] |= (a[i + 1] >> (8 - n));
        }
    }
    if (num - n != 0) {
        block_shiftl(dest, dest, num - n);
    }
    return;
}

void block_incr(uint8_t *in) {
    for (int_fast8_t i = 15; i > 0; i--) {
        uint8_t prev = in[i];
        in[i] += 1;
        if (in[i] > prev) {
            /* No overflow, changes propagate no further */
            break;
        }
    }
    return;
}

void block_decr(uint8_t *in) {
    for (int_fast8_t i = 15; i > 0; i--) {
        uint8_t prev = in[i];
        in[i] -= 1;
        if (in[i] < prev) {
            /* No overflow, changes propagate no further */
            break;
        }
    }
    return;
}
