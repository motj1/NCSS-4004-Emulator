#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#define INFMAX UINT16_MAX

#define GET_R0(s) ((s).regs & 0xF)
#define SET_R0(s, val) ((s).regs = ((s).regs & 0xF0) | ((val) & 0xF))

#define GET_R1(s) (((s).regs >> 4) & 0xF)
#define SET_R1(s, val) ((s).regs = ((s).regs & 0xF) | (((val) & 0xF) << 4))

#define IS_HALTED(s) ((s).flags & 1)
#define HALT(s) ((s).flags |= 1)

#define VERBOSE 0

typedef union {
    uint64_t    raw;
    uint8_t     bytes[8];
} tape_t;

typedef struct {
    uint8_t regs;
    uint8_t pc;          // In nibble units (0–15)
    uint8_t flags;          // 0 = running, 1 = halted
    tape_t tape;     // 16 nibbles of memory
    uint32_t steps;      // Instruction counter
} state_t;

void halt_fn(state_t *s) {
    HALT(*s);
}

void inc_r0(state_t *s) {
    SET_R0(*s, GET_R0(*s) + 1);
    s->pc = ((s->pc + 1) & 0xF);
}

void dec_r0(state_t *s) {
    SET_R0(*s, GET_R0(*s) - 1);
    s->pc = ((s->pc + 1) & 0xF);
}

void inc_r1(state_t *s) {
    SET_R1(*s, GET_R1(*s) + 1);
    s->pc = ((s->pc + 1) & 0xF);
}

void dec_r1(state_t *s) {
    SET_R1(*s, GET_R1(*s) - 1);
    s->pc = ((s->pc + 1) & 0xF);
}

void add_r0(state_t *s) {
    SET_R0(*s, (GET_R0(*s) + GET_R1(*s)) & 0xF);
    s->pc = ((s->pc + 1) & 0xF);
}

void sub_r0(state_t *s) {
    SET_R0(*s, (GET_R0(*s) - GET_R1(*s)) & 0xF);
    s->pc = ((s->pc + 1) & 0xF);
}

void print_r0(state_t *s) {
    // printf("%X", GET_R0(*s));
    s->pc = ((s->pc + 1) & 0xF);
}

void loop_nz(state_t *s) {
    s->pc = ((s->pc + 1) & 0xF);
    if (GET_R0(*s)) {
        if ((s->pc) & 1)
            s->pc = (s->tape.bytes[(s->pc)/2] & 0xF0) >> 4;
        else 
            s->pc = s->tape.bytes[(s->pc)/2] & 0xF;
    } else s->pc = ((s->pc + 1) & 0xF);
}

void loop_z(state_t *s) {
    s->pc = ((s->pc + 1) & 0xF);
    if (!GET_R0(*s)) {
        if ((s->pc) & 1)
            s->pc = (s->tape.bytes[(s->pc)/2] & 0xF0) >> 4;
        else 
            s->pc = s->tape.bytes[(s->pc)/2] & 0xF;
    } else s->pc = ((s->pc + 1) & 0xF);
}

void load_r0(state_t *s) {
    s->pc = ((s->pc + 1) & 0xF);
    if ((s->pc) & 1)
        SET_R0(*s, (s->tape.bytes[(s->pc)/2] & 0xF0) >> 4);
    else 
        SET_R0(*s, s->tape.bytes[(s->pc)/2] & 0xF);
    s->pc = ((s->pc + 1) & 0xF);
}

void load_r1(state_t *s) {
    s->pc = ((s->pc + 1) & 0xF);
    if ((s->pc) & 1)
        SET_R1(*s, (s->tape.bytes[(s->pc)/2] & 0xF0) >> 4);
    else 
        SET_R1(*s, s->tape.bytes[(s->pc)/2] & 0xF);
    s->pc = ((s->pc + 1) & 0xF);
}

void store_r0(state_t *s) {
    s->pc = ((s->pc + 1) & 0xF);
    uint8_t index;
    if ((s->pc) & 1)
        index = (s->tape.bytes[(s->pc)/2] & 0xF0) >> 4;
    else 
        index = s->tape.bytes[(s->pc)/2] & 0xF;
    if (index & 1) {
        s->tape.bytes[index/2] = (s->tape.bytes[index/2] & 0xF) | (GET_R0(*s) << 4);
    } else {
        s->tape.bytes[index/2] = (s->tape.bytes[index/2] & 0xF0) | GET_R0(*s);
    }
    s->pc = ((s->pc + 1) & 0xF);
}

void store_r1(state_t *s) {
    s->pc = ((s->pc + 1) & 0xF);
    uint8_t index;
    if ((s->pc) & 1)
        index = (s->tape.bytes[(s->pc)/2] & 0xF0) >> 4;
    else 
        index = s->tape.bytes[(s->pc)/2] & 0xF;
    if (index & 1) {
        s->tape.bytes[index/2] = (s->tape.bytes[index/2] & 0xF) | (GET_R1(*s) << 4);
    } else {
        s->tape.bytes[index/2] = (s->tape.bytes[index/2] & 0xF0) | GET_R1(*s);
    }
    s->pc = ((s->pc + 1) & 0xF);
}

void swap_r0(state_t *s) {
    s->pc = ((s->pc + 1) & 0xF);
    uint8_t index, tmp;
    tmp = GET_R0(*s);
    if ((s->pc) & 1)
        index = (s->tape.bytes[(s->pc)/2] & 0xF0) >> 4;
    else 
        index = s->tape.bytes[(s->pc)/2] & 0xF;
    if (index & 1) {
        SET_R0(*s, (s->tape.bytes[index/2] & 0xF0) >> 4);
        s->tape.bytes[index/2] = (s->tape.bytes[index/2] & 0xF) | ((tmp) << 4);
    } else {
        SET_R0(*s, (s->tape.bytes[index/2] & 0xF));
        s->tape.bytes[index/2] = (s->tape.bytes[index/2] & 0xF0) | (tmp);
    }
    s->pc = ((s->pc + 1) & 0xF);
}

void swap_r1(state_t *s) {
    s->pc = ((s->pc + 1) & 0xF);
    uint8_t index, tmp;
    tmp = GET_R1(*s);
    if ((s->pc) & 1)
        index = (s->tape.bytes[(s->pc)/2] & 0xF0) >> 4;
    else 
        index = s->tape.bytes[(s->pc)/2] & 0xF;
    if (index & 1) {
        SET_R1(*s, (s->tape.bytes[index/2] & 0xF0) >> 4);
        s->tape.bytes[index/2] = (s->tape.bytes[index/2] & 0xF) + (tmp << 4);
    } else {
        SET_R1(*s, (s->tape.bytes[index/2] & 0xF));
        s->tape.bytes[index/2] = (s->tape.bytes[index/2] & 0xF0) + tmp;
    }
    s->pc = ((s->pc + 1) & 0xF);
}

void (*handlers[17])(state_t *s) = {
    halt_fn,    // 0x0
    inc_r0,     // 0x1
    dec_r0,     // 0x2
    inc_r1,     // 0x3
    dec_r1,     // 0x4
    add_r0,     // 0x5
    sub_r0,     // 0x6
    print_r0,   // 0x7
    loop_nz,    // 0x8
    loop_z,     // 0x9
    load_r0,    // 0xA
    load_r1,    // 0xB
    store_r0,   // 0xC
    store_r1,   // 0xD
    swap_r0,    // 0xE
    swap_r1,    // 0xF
    halt_fn        // Fill unused or illegal opcodes with halt_fn
};

uint32_t bestLengths[16] = { 0 };
uint64_t bestTapes[16] = { 0 };

void executeInstruction(state_t *s) {
    void (*handler)(state_t *) = handlers[((s->pc) & 1)? (s->tape.bytes[s->pc/2] & 0xF0) >> 4 : s->tape.bytes[s->pc/2] & 0xF];

    if (VERBOSE) {
        printf("Iter %u:\n", s->steps);
        printf("\tr0 = %X, r1 = %X, pc = %X\n", GET_R0(*s), GET_R1(*s), s->pc);
        printf("\tRam = ");
        for (int i=0; i<16; i++) printf("%X%c", (((i) & 1)? (s->tape.bytes[i/2] & 0xF0) >> 4 : s->tape.bytes[i/2] & 0xF), (i != 15)? ' ' : '\n');
        usleep(100000);
    }
    handler(s);
}

void executeSteps(uint64_t start, uint64_t end) {
    if (start == 0) start ++;
    state_t h = {0, 0, 0, {.raw = 0}, 0};
    state_t t = {0, 0, 0, {.raw = 0}, 0};
    uint64_t SavedTape = start, BestTape = start, numInf = 0, numCaught = 0;
    uint32_t bestTime = 0;
    uint8_t size = 1;
    while ((start >> (size * 4)) > 0 && size < 8) {
        size ++;
    }

    while (SavedTape <= end && SavedTape != 0) {
        h.regs = h.pc = h.flags = h.steps = 0;
        t.regs = t.pc = t.flags = t.steps = 0;
        h.tape.raw = t.tape.raw = SavedTape;
        while (!IS_HALTED(h) && h.steps < INFMAX) {
            executeInstruction(&h);
            if (IS_HALTED(h)) {
                h.steps ++;
                break;
            }
            if (h.steps & 1) {
                executeInstruction(&t);
                t.steps ++;
            }
            h.steps ++;
            if (h.pc == t.pc && h.regs == t.regs && h.tape.raw == t.tape.raw) {
                // printf("Inf caught by hair tourtouse after %d steps in: %llX\n", h.steps, h.tape.raw);
                h.steps = INFMAX;
                numCaught ++;
                // usleep(100000);
                break;
            }
        }

        if (h.steps > bestLengths[size] && !(h.steps == INFMAX)) {
            bestLengths[size] = h.steps;
            bestTapes[size] = SavedTape;
            if (h.steps > bestTime) {
                bestTime = h.steps;
                BestTape = SavedTape;
            }
        } else if (h.steps == INFMAX) {
            numInf ++;
        }
        
        SavedTape ++;
        while (!(SavedTape & 0xF) || !(SavedTape & 0xF0)) {
            SavedTape ++;
        }

        if ((SavedTape >> (size * 4))) {
            size ++;
            printf("Best runtime for size %d was %u with:\n", size - 1, bestLengths[size - 1]);
            printf("\t%llX\n", bestTapes[size - 1]);
        }
    }
    printf("Best runtime up to size %llX was %u with:\n", end, bestTime);
    printf("\t%llX\n", BestTape);
    printf("\tAlso took %llu inf loops of which %llu were caught\n", numInf, numCaught);
}

int main(void) {
    executeSteps(0x11, 0xFFFFFFF);

    return 0;
}