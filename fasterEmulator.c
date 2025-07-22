/*
// 0: Halt
// 1: Increment R0
// 2: Decrement R0
// 3: Increment R1
// 4: Decrement R1
// 5: Compute R0 + R1 and put the result in R0
// 6: Compute R0 - R1 and put the result in R0
// 7: Print R0 (Hex)
// 8*: Jump to address <data> if R0 != 0
// 9*: Jump to address <data> if R0 == 0
// A*: Load (direct) value <data> into R0
// B*: Load (direct) value <data> into R1
// C*: Copy the contents of R0 into the memory cell at <address>
// D*: Copy the contents of R1 into the memory cell at <address>
// E*: Swap the contents of R0 and memory cell at <address>
// F*: Swap the contents of R1 and memory cell at <address>
*/

#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <unistd.h>

#define VERBOSE 1
#define MAXSIZE 0xFFFFFF

uint8_t r0, r1, pc;
uint8_t ram[16] = { 0 };

void parseFile(char *filename) {
    FILE *fp = fopen(filename, "r");
    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;

    int curr = 0;

    while ((linelen = getline(&line, &linecap, fp)) > 0) {
        char prev = ' ';
        for (char *pointer = line; *pointer; pointer ++) {
            if ((*pointer == '#' && prev != '\\') || (*pointer == '/' && prev == '/')) {
                *pointer = 0;
                break;
            } else {
                if ((*pointer) <= 'F' && (*pointer) >= 'A') {
                    ram[curr ++] = (uint8_t)(*pointer - 'A') + 10;
                } else if ((*pointer) <= '9' && (*pointer) >= '0') {
                    ram[curr ++] = (uint8_t)(*pointer - '0');
                }
                if (curr >= 16) break;
            }
            prev = *pointer;
        }
        if (curr >= 16) break;
    }
}

uint64_t execute() {
    uint8_t tmp;
    r0 = r1 = pc = 0;

    for (uint64_t iters = 0; iters < 0xFFFFFF; iters ++) {
        if (VERBOSE) {
            printf("Iter %llu:\n", iters);
            printf("\tr0 = %X, r1 = %X, pc = %X\n", r0, r1, pc);
            printf("\tRam = ");
            for (int i=0; i<16; i++) printf("%X%c", ram[i], (i != 15)? ' ' : '\0');
            printf("\n");
            usleep(10000);
        }
        uint8_t cur = ram[pc];
        if (cur == 0) return iters + 1;
        switch (cur) {
        case 1: r0 ++; break;
        case 2: r0 --; break;
        case 3: r1 ++; break;
        case 4: r1 --; break;
        case 5: r0 = r0 + r1; break;
        case 6: r0 = r0 - r1; break;
        case 7: printf("%X", r0); break;
        case 8: if (r0) pc = ram[(pc + 1) & 0xF]; else pc ++; break;
        case 9: if (!r0) pc = ram[(pc + 1) & 0xF]; else pc ++; break;
        case 10: r0 = ram[(++ pc) & 0xF]; break;
        case 11: r1 = ram[(++ pc) & 0xF]; break;
        case 12: ram[ram[(++ pc) & 0xF]] = r0; break;
        case 13: ram[ram[(++ pc) & 0xF]] = r1; break;
        case 14: 
            tmp = r0;
            r0 = ram[ram[(pc + 1) & 0xF]];
            ram[ram[(++ pc) & 0xF]] = tmp; 
            break;
        case 15: 
            tmp = r1;
            r1 = ram[ram[(pc + 1) & 0xF]];
            ram[ram[(++ pc) & 0xF]] = tmp; 
            break;
        }
        if (!((cur == 8 && r0) || (cur == 9 && !r0)))
            pc ++;
        pc &= 0xF;
        r0 &= 0xF;
        r1 &= 0xF;
    }
    return 0xFFFFFF;
}

// void executeVerbose()

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Useage: %s [filename]\n", argv[0]);
        return 1;
    }

    if (VERBOSE)
        printf("Parseing:\n");
    parseFile(argv[1]);

    printf("Executing ...\n");
    uint64_t taken = execute();
    if (taken >= 0xFFFFFF)
        printf("\nTook > 0xFFFFFF\n");
    else 
        printf("\nTook: %llu\n", taken);

    return 0;
}