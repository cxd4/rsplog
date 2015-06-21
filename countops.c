#include <stdlib.h>

#include <stdio.h>
#include "mnemonics.h"
#include "rcp.h"

typedef long int capacity; /* Signed types help diagnose overflow. */
capacity count_primary[077 + 1];
capacity count_SPECIAL[077 + 1];
capacity count_REGIMM [037 + 1];
capacity count_COP0   [037 + 1];
capacity count_COP2   [037 + 1];
capacity count_LWC2   [037 + 1];
capacity count_SWC2   [037 + 1];
capacity count_C2     [077 + 1];

u8 * DRAM;

extern void RSP_count(FILE * stream, size_t max_PC, u8 * IMEM);
extern long file_size(FILE * stream);

int main(int argc, char* argv[])
{
    FILE * stream;
    size_t bytes, elements_read;
    long file_size_in_bytes;

    if (argc < 2)
    {
        fputs("Command syntax missing domain.\n", stderr);
        getchar();
        return 1;
    }

    stream = fopen(argv[1], "rb");
    if (stream == NULL)
    {
        fputs("Specified file was unreadable.\n", stderr);
        return 1;
    }

    file_size_in_bytes = file_size(stream);
    if (file_size_in_bytes < 0)
    {
        fputs("The file size is immeasurable.\n", stderr);
        return 1;
    }

    bytes = (size_t)file_size_in_bytes;
    printf("Attempting to import %li-byte file...\n", file_size_in_bytes);
    rewind(stream);

    DRAM = malloc(bytes * sizeof(u8));
    if (DRAM == NULL)
    {
        fputs("Too much data to allocate RAM.\n", stderr);
        return 1;
    }

    elements_read = fread(DRAM, sizeof(u8) * bytes, 1, stream);
    while (fclose(stream) != 0)
        ;
    if (elements_read != 1)
    {
        fputs("DRAM file data import failure.\n", stderr);
        return 1;
    }

    stream = fopen("out.txt", "w");
    if (stream == NULL)
    {
        fputs("File write permission failure.\n", stderr);
        return 1;
    }

    RSP_count(stream, bytes, DRAM);
    while (fclose(stream) != 0)
        ;
    return 0;
}

void RSP_count(FILE * stream, size_t max_PC, u8 * IMEM)
{
    register size_t PC;
    register int i;

    if (max_PC & 3)
        fputs("Warning:  Truncating unaligned cache size.\n", stderr);
#if 0
    max_PC &=  0x03FFFFFCul;
#endif
    max_PC &= ~0x00000003ul;

    printf("Enumerating RSP operations...\n");
    for (PC = 0x04001000 & 0x000; PC < max_PC; PC += 0x004)
    {
        const u32 inst = 0x00000000u
          | (IMEM[PC + 0] << 24)
          | (IMEM[PC + 1] << 16)
          | (IMEM[PC + 2] <<  8)
          | (IMEM[PC + 3] <<  0)
        ;
        const unsigned int op = (inst >> 26) % 64;
        const unsigned int rs = (inst >> 21) % 32;
        const unsigned int rt = (inst >> 16) % 32;
        const unsigned short immediate = (unsigned short)(inst & 0x0000FFFFul);
        const unsigned int rd   = (immediate >> 11) % 32;
        const unsigned int func = (immediate >>  0) % 64;

        ++count_primary[op];
        switch (op)
        { /* oooooo ----- ----- ----- ----- ------ */
            case 000: /* SPECIAL */
                ++count_SPECIAL[func];
                continue; /* 000000 ----- ----- ----- ----- ffffff */
            case 001: /* REGIMM */
                ++count_REGIMM[rt];
                continue; /* 000001 ----- ttttt ----- ----- ------ */
            case 020: /* COP0 */
                ++count_COP0[rs];
                continue; /* 010000 sssss ----- ----- ----- ------ */
            case 022: /* COP2 */
                ++count_COP2[(rs >= 16) ? 16 : rs];
                count_C2[inst % 64] += !!(inst & 0x02000000ul);
                continue; /* 010010 Veeee ----- ----- ----- ffffff */
            case 062: /* LWC2 */
                ++count_LWC2[rd];
                continue;
            case 072: /* SWC2 */
                ++count_SWC2[rd];
                continue;
            default:
                continue;
        }
    }

    printf("Checking through the results...");

    fprintf(stream, "RSP Iterations Log:  %s\n", "primary R4000 op-codes");
    fputs("--------------------------------------------\n", stream);
    for (i = 0; i < 64; i++)
        if (count_primary[i] != 0)
            fprintf(stream, "%s %i\n", mnemonics_primary[i], count_primary[i]);
    fputc('\n', stream);

    fprintf(stream, "RSP Iterations Log:  %s\n", "SPECIAL operation codes");
    fputs("--------------------------------------------\n", stream);
    for (i = 0; i < 64; i++)
        if (count_SPECIAL[i] != 0)
            fprintf(stream, "%s %i\n", mnemonics_SPECIAL[i], count_SPECIAL[i]);
    fputc('\n', stream);

    fprintf(stream, "RSP Iterations Log:  %s\n", "REGIMM operation codes");
    fputs("--------------------------------------------\n", stream);
    for (i = 0; i < 32; i++)
        if (count_REGIMM[i] != 0)
            fprintf(stream, "%s %i\n", mnemonics_REGIMM[i], count_REGIMM[i]);
    fputc('\n', stream);

    fprintf(stream, "RSP Iterations Log:  %s\n", "COP0 operation codes");
    fputs("--------------------------------------------\n", stream);
    for (i = 0; i < 32; i++)
        if (count_COP0[i] != 0)
            fprintf(stream, "%s %i\n", mnemonics_COP0[i], count_COP0[i]);
    fputc('\n', stream);

    fprintf(stream, "RSP Iterations Log:  %s\n", "COP2 operation codes");
    fputs("--------------------------------------------\n", stream);
    for (i = 0; i < 32; i++)
        if (count_COP2[i] != 0)
            fprintf(stream, "%s %i\n", mnemonics_COP2[i], count_COP2[i]);
    fputc('\n', stream);

    fprintf(stream, "RSP Iterations Log:  %s\n", "LWC2 operation codes");
    fputs("--------------------------------------------\n", stream);
    for (i = 0; i < 32; i++)
        if (count_LWC2[i] != 0)
            fprintf(stream, "%s %i\n", mnemonics_LWC2[i], count_LWC2[i]);
    fputc('\n', stream);

    fprintf(stream, "RSP Iterations Log:  %s\n", "SWC2 operation codes");
    fputs("--------------------------------------------\n", stream);
    for (i = 0; i < 32; i++)
        if (count_SWC2[i] != 0)
            fprintf(stream, "%s %i\n", mnemonics_SWC2[i], count_SWC2[i]);
    fputc('\n', stream);

    fprintf(stream, "RSP Iterations Log:  %s\n", "vector operation codes");
    fputs("--------------------------------------------\n", stream);
    for (i = 0; i < 64; i++)
        if (count_C2[i] != 0)
            fprintf(stream, "%s %i\n", mnemonics_vector[i], count_C2[i]);

    printf("Results written.\n");
    return;
}

long file_size(FILE * stream)
{
    int failure;

#ifdef POSIX_REGULATIONS_ASSUMED
    failure = fseek(stream, SEEK_END, 0);
    if (failure)
        return -1L;
#else
    failure = fseek(stream, SEEK_SET, 0);
    if (failure)
        return -1L;
    while (fgetc(stream) >= 0)
        ;
#endif
    return ftell(stream);
}
