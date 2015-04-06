#include <stdio.h>

#define cpuid(level, a, b, c, d) \
    asm("xchg{l}\t{%%}ebx, %1\n\t" \
        "cpuid\n\t" \
        "xchg{l}\t{%%}ebx, %1\n\t" \
        : "=a" (a), "=r" (b), "=c" (c), "=d" (d) \
        : "0" (level))

int main (int argc, char **argv) {
    unsigned int eax, ebx, ecx, edx;
    cpuid(1, eax, ebx, ecx, edx);
    if (ecx & (1<<25))
        printf("AES-NI Enabled\n");
    else
        printf("AES-NI Disabled\n");
    return 0;
}