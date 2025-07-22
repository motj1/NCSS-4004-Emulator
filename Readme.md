# NCSS 4004 emulator

This project is the culmination of a few years of learning and shows how from 2023 (OldestEmulator.c) to now (ThirdBFE.c) I have evolved as a programmer. It is by no means perfect and in the coming weeks I will improve it however, it conveys the importance of optomisation and in depth learning of languages.

## Inspiration

In year 10 I went on a 10 day coding camp with NCSS where we stayed at New College and were taught by Richard Buckland and an amazing team of tutors about the fundamentals of computer programming. This introduced me to the world of embedded systems and low latency programming which has inspired my projects and interests ever since. Within this program we worked with a simplified version of the IBM 4004 processor in which there was 16 bytes of ram and the instruction set,  
0: Halt  
1: Increment R0  
2: Decrement R0  
3: Increment R1  
4: Decrement R1  
5: Compute R0 + R1 and put the result in R0  
6: Compute R0 - R1 and put the result in R0  
7: Print R0 (Hex)  
8*: Jump to address \<data\> if R0 != 0  
9*: Jump to address \<data\> if R0 == 0  
A*: Load (direct) value \<data\> into R0  
B*: Load (direct) value \<data\> into R1  
C*: Copy the contents of R0 into the memory cell at \<address\>  
D*: Copy the contents of R1 into the memory cell at \<address\>  
E*: Swap the contents of R0 and memory cell at \<address\>  
F*: Swap the contents of R1 and memory cell at \<address\>

Which posed a interesting challenge that I have thought about on and off ever since.  
What it is actually doing within this challenge is attempting to solve a challenge they set during the course of the camp which is the "busy beaver challenge" which was to attempt to make the longest running program out of the 16 byte ram and the instructions given which halts.

## How I made it:

In 2023 a few months after the camp I implemented my first implementation, the file "OldestEmulator.c" which was built as well as I could at the time, looking back the inefficiency is clear to me however this was the beggining of my C journey so I am still quite pleased with it. After this, I just recently became interested in the challenge once again so I first attempted to solve the challenge again utilising my new skillset. The first challenge was creating an Emulator for the chip as shown in "BaseEmulator.c" which was made to validate my solutions made in the brute forcer programs. After this I created this base emulator I implemented my first brute force implementation in "FirstBruteForceEmulator.c" then went on to revise it two more times.

## Optimisations:

Every subsequent implementation of my four builds upon the other and attempts to optomise it further, in my seccond brute force emulator I was targeting making the program faster which I used an array of function pointers to optomise away the large amount of branch statements which accompany large switch statements which make CPU optomisation difficult. Despite this complex optimisation I found that the main optomisations I can create is by attempting to prune infinite loops instead of just making it run faster as I was initially doing. I utilise the rabit and hair algorithm in my final brute force emulator "ThirdBFEm.c" to do this which improved the programs speed significantly and from my oldest emulator I was able to improve speed by a significant factor.
