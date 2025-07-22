#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int strnum(char s) {
    if (s != 'A' && s != 'B' && s != 'C' && s != 'D' && s != 'E' && s != 'F') {
        return atoi(&s);
    } else {
        // printf("\n  %d : %d \n", (int)s, ((int)s - (int)'A') + 10);
        return ((int)s - (int)'A') + 10;
    }
}

char numstr(int s) {
    if (s < 10) {
        return (char)(s+(int)'0');
    } else {
        // printf("\n  %d : %c \n", s, (s - 10 + (int)'A'));
        char num = (char)(s-10+(int)'A');
        // if (num >= 'F') num = 'F';
        return num;
    }
}


int main(int argc, char *argv[]) {
    char commands[16] = { 0 };//{ 0 };
    int best = 0;
    char bCom[16];
    for (unsigned long long c = 0; c < 0xFFFFFF; c++) { // 16*16*16*16*16*16*16 (unsigned long long)(16*16*16*16*16*16*16)*16
        int size = log(c)/log(16) + 1;
        if (commands[0] != 'F') {
            commands[0] = numstr(c%16);
        } else {
            int number = 0;
            while (commands[number] == 'F') {
                commands[number] = '0';
                number ++;
                if (commands[number] >= '0' && commands[number] < 'F') {
                    commands[number] ++;
                    if (commands[number] >= ':' && commands[number] < 'A') commands[number] = 'A';
                } else if (commands[number] != 'F')
                    commands[number] = '0';

                if (commands[number] > 'F') commands[number] = 'F';
                if (commands[number] < '0') commands[number] = '0';
            }
        }
        // int size = 5;
        char tape[17] = { 0 };
        for (int i=0; i < 16; i++) {
            if (i < size)
                tape[i] = commands[i];
            else 
                tape[i] = '0';
        }

        int r0 = 0, r1 = 0;
        char is = 0;
        int i = 0;
        int stor = 0;

        for (int ip = 0; tape[ip] != '0' && tape[ip] >= '0' && tape[ip] <= 'F'; ip ++) {
            char sv = tape[ip];
            is = tape[ip];
            ip %= 16;
            // printf(" is = %c ", is);
            if (is == '0') break;
            switch (is) {
            case '1':
                r0 = (r0 >= 15)? 0 : r0 + 1;
                if (r0 < 0) r0 = 0;
                if (r0 > 15) r0 = 15;
                break;
            case '2':
                r0 = (r0 <= 0)? 15 : r0 - 1;
                if (r0 < 0) r0 = 0;
                if (r0 > 15) r0 = 15;
                break;
            case '3':
                r1 = (r1 >= 15)? 0 : r1 + 1;
                if (r1 < 0) r1 = 0;
                if (r1 > 15) r1 = 15;
                break;
            case '4':
                r1 = (r1 <= 0)? 15 : r1 - 1;
                if (r1 < 0) r1 = 0;
                if (r1 > 15) r1 = 15;
                break;
            case '5':
                r0 += r1;
                r0 %= 16;
                if (r0 < 0) r0 = 0;
                if (r0 > 15) r0 = 15;
                break;
            case '6':
                r0 -= r1;
                r0 = (r0 < 0)? (15 - ((r0+1)*-1)%16)%16 : r0;
                if (r0 < 0) r0 = 0;
                if (r0 > 15) r0 = 15;
                break;
            case '7':
                // printf("%X ", r0);
                break;
            case '8':
                if (r0 != 0) ip = (strnum(tape[(ip + 1)%16]))%16 - 1;
                else { ip ++; ip %= 16; }
                break;
            case '9':
                if (r0 == 0) ip = (strnum(tape[(ip + 1)%16]))%16 - 1;
                else { ip ++; ip %= 16; }
                break;
            case 'A':
                r0 = strnum(tape[(ip + 1)%16]);
                if (r0 < 0) r0 = 0;
                if (r0 > 15) r0 = 15;
                ip ++; ip %= 16;
                break;
            case 'B':
                r1 = strnum(tape[(ip + 1)%16]);
                if (r1 < 0) r1 = 0;
                if (r1 > 15) r1 = 15;
                ip ++; ip %= 16;
                break;
            case 'C':
                tape[(ip + 1)%16] = numstr(r0);
                ip ++; ip %= 16;
                break;
            case 'D':
                tape[(ip + 1)%16] = numstr(r1);
                ip ++; ip %= 16;
                break;
            case 'E':
                stor = r0;
                r0 = strnum(tape[strnum(tape[(ip + 1)%16])]);
                // for (int num = 0; num < 16; num ++) {
                //     if (tape[num] >= '0')
                //         printf("%c ", tape[num]);
                //     else
                //         printf("%d ", tape[num]);
                // }
                // printf("Changing\n");
                tape[strnum(tape[(ip + 1)%16])] = numstr(stor);
                // for (int num = 0; num < 16; num ++) {
                //     if (tape[num] >= '0')
                //         printf("%c ", tape[num]);
                //     else
                //         printf("%d ", tape[num]);
                // }
                // printf("Changed\n");
                if (r0 < 0) r0 = 0;
                if (r0 > 15) r0 = 15;
                ip ++; ip %= 16;
                break;
            case 'F':
                stor = r1;
                // printf("Swapping, r1 = %X to %X\n", r1, strnum(tape[strnum(tape[(ip + 1)%16])]));
                r1 = strnum(tape[strnum(tape[(ip + 1)%16])]);
                // printf("Swapping, tape position = %X to %c\n", strnum(tape[(ip + 1)%16]), numstr(stor));
                tape[strnum(tape[(ip + 1)%16])] = numstr(stor);
                if (r1 < 0) r1 = 0;
                if (r1 > 15) r1 = 15;
                ip ++; ip %= 16;
                break;
            }
            i ++;
            // printf(" i = %d is = %c \n\n", i, is);
            if (i >= 12000) break;
        }

        i ++;
        if (i > best && i != 12001) {
            for (int num = 0; num < 16; num ++) {
                if (commands[num] != 0)
                    printf("%c ", commands[num]);
                bCom[num] = commands[num];
            }
            printf("\tTook %d turn: %llu\n", i, c);
            best = i;
        }
        i = 0;
        // printf("\nDone : %s : Took %d\nr0 = %X\nr1 = %X\n", commands, i, r0, r1);
    }

    return 0;
}
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

// 2 E D 4 2 8 4 5 8 3 E D 8 0 C F

