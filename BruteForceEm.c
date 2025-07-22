#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

// #define getElement(pc) ((pc & 1)? ((tape.bytes[pc/2] & 0b11110000) >> 4) : (tape.bytes[pc/2] & 0b1111))
// #define putElement(pos, reg) (pos = (pos & 1)? ((tape.bytes[pos/2] & 0b1111) + (reg << 4)) : ((tape.bytes[pos/2] & 0b11110000) + reg))

#define VERBOSE 0

typedef struct {
    uint8_t nib1 : 4;
    uint8_t nib2 : 4;
} byte;

typedef union {
    uint64_t    raw;
    uint8_t     bytes[8];
} tape_t;

uint8_t getElement(uint8_t pos, tape_t *tape) {
    return ((pos) & 1)? (tape->bytes[(pos)/2] & 0xF0) >> 4 : tape->bytes[(pos)/2] & 0xF;
}

void insertElement(uint8_t pc, uint8_t elem, tape_t *tape) {
    uint8_t index = getElement(pc, tape);
    if (index & 1) {
        tape->bytes[index/2] = (tape->bytes[index/2] & 0xF) + (elem << 4);
    } else {
        tape->bytes[index/2] = (tape->bytes[index/2] & 0xF0) + elem;
    }
}

uint16_t getexeleng(tape_t tape) {
    uint8_t tmp, tmp2;
    uint8_t r0 = 0, r1 = 0, pc = 0;

    for (uint16_t iters = 0; iters < UINT16_MAX; iters ++) {
        if (VERBOSE) {
            printf("Iter %u:\n", iters);
            printf("\tr0 = %X, r1 = %X, pc = %X\n", r0, r1, pc);
            printf("\tRam = ");
            for (int i=0; i<16; i++) printf("%X%c", getElement(i, &tape), (i != 15)? ' ' : '\0');
            printf("\n");
            usleep(10000);
        }
        uint8_t curr = getElement(pc, &tape);
        if      (curr == 1) r0 ++;
        else if (curr == 2) r0 --;
        else if (curr == 3) r1 ++;
        else if (curr == 4) r1 --;
        else if (curr == 5) r0 = r0 + r1;
        else if (curr == 6) r0 = r0 - r1;
        // else if (curr == 7) printf("%X", r0);
        else if (curr == 8 && r0 || curr == 9 && !r0) pc = getElement(pc + 1, &tape);
        else if (curr == 8 || curr == 9) pc ++;
        else if (curr == 10) { 
            pc ++; pc &= 0xF;
            r0 = getElement(pc, &tape);
        } else if (curr == 11) { 
            pc ++; pc &= 0xF;
            r1 = getElement(pc, &tape);
        } else if (curr == 12) { 
            pc ++; pc &= 0xF;
            insertElement(pc, r0, &tape); 
        } else if (curr == 13) { 
            pc ++; pc &= 0xF;
            insertElement(pc, r1, &tape);
        } else if (curr == 14) { 
            tmp = r0;
            pc ++; pc &= 0xF;
            r0 = getElement(getElement(pc, &tape), &tape); 
            insertElement(pc, tmp, &tape); 
        } else if (curr == 15) { 
            tmp = r1; 
            pc ++; pc &= 0xF;
            r1 = getElement(getElement(pc, &tape), &tape); 
            insertElement(pc, tmp, &tape); 
        } else if (!(curr == 7)) return iters + 1;

        if (!(curr == 8 && r0 || curr == 9 && !r0))
            pc ++;
        pc &= 0xF;
        r0 &= 0xF;
        r1 &= 0xF;
    }
    return UINT16_MAX;
}

int main(void) {
    tape_t tape = {.raw = 0x11};

    uint16_t max_steps = 0;
    tape_t best_tape = {.raw = 0};

    int numinf = 0;

    do {
        // tape.raw = 0xFD08DE3854824DE2;//0x50000000002817FE;
        if (tape.raw & 0xF && tape.raw & 0xF0) {
            uint16_t steps = getexeleng(tape);
            if (steps > max_steps && steps < UINT16_MAX) {
                max_steps = steps;
                best_tape = tape;
                printf("\tNBT %llX : %u\n", best_tape.raw, max_steps);//\0 after %d inf\n", best_tape.raw, max_steps, numinf);
            } else if (steps == UINT16_MAX) {
                numinf ++;
            }
        }
        tape.raw ++;
    } while (tape.raw <= 0xFFFFF);  // loops over 2^64 combinations (or until you stop)
    printf("Best tape %llX Took %u\n", best_tape.raw, max_steps);
    printf("Also have %d inf\n", numinf);

    return 0;
}