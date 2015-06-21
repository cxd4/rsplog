#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "mnemonics.h"
#include "rcp.h"

extern int IW_decode(char * text, u32 word);
extern size_t RSP_disassemble(FILE * stream, size_t max_PC, u8 * IMEM);

int main(int argc, char* argv[])
{
    FILE * stream;
    size_t elements_read;

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
    elements_read = fread(IMEM, sizeof(IMEM), 1, stream);
    while (fclose(stream) != 0)
        ;
    if (elements_read != 1)
    {
        fputs("IMEM file data import failure.\n", stderr);
        return 1;
    }

    stream = fopen("out.s", "w");
    if (stream == NULL)
    {
        fputs("File write permission failure.\n", stderr);
        return 1;
    }

    RSP_disassemble(stream, 0xFFF + 1, IMEM);
    while (fclose(stream) != 0)
        ;
    return 0;
}

size_t RSP_disassemble(FILE * stream, size_t max_PC, u8 * IMEM)
{
    char * line_string;
    size_t illegal_instructions;
    register size_t PC;

/*
 * 32-bit alignment forced
 * Memory map constraints imply a maximum DRAM address of 03FFFFFF_16.
 */
    max_PC &= 0x03FFFFFCul;

    line_string = malloc(80);
    illegal_instructions = 0;

    PC = 0x04001000 & 0x000;
    while (PC < max_PC)
    {
        const u32 inst = 0x00000000u
          | (IMEM[PC + 0] << 24)
          | (IMEM[PC + 1] << 16)
          | (IMEM[PC + 2] <<  8)
          | (IMEM[PC + 3] <<  0)
        ;
        illegal_instructions += IW_decode(line_string, inst);
        fputs(line_string, stream);
        fputc('\n', stream);
        PC += 0x004;
    }
    free(line_string);
    return (illegal_instructions);
}

static const char* vector_scalars[16] = {
    "", /* none */
    "",
    "[0q]", "[1q]", /* quarters */
    "[0h]", "[1h]", "[2h]", "[3h]", /* halves */
    "[0]", "[1]", "[2]", "[3]", "[4]", "[5]", "[6]", "[7]", /* wholes */
};

static const char* scalar_GPRs[32] = {
    "zero",
    "at", /* supported in RSP code */
    "v0", "v1",
    "a0", "a1", "a2", "a3",
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "t8", "t9",
    "k0", "k1",
    "gp",
    "sp", /* supported in RSP code */
    "fp", /* supported in RSP code as r30:  $s8 */
    "ra", /* supported in RSP code */
}; /* Unless marked otherwise, none of these register names are RSP-valid. */

static const char* vector_CPRs[4] = {
    "vco",
    "vcc",
    "vce",
    "***", /* seems to be VCE, according to zilmar's hardware reversing */
};

enum {
    MIPS_FORMAT_R,
    MIPS_FORMAT_R_NO_OPERANDS,
    MIPS_FORMAT_R_SHIFT,
    MIPS_FORMAT_R_SHIFT_IMM,
    MIPS_FORMAT_R_JALR,
    MIPS_FORMAT_R_JR,
    MIPS_FORMAT_R_VECTOR,
    MIPS_FORMAT_R_VECTOR_DIVIDE,

    MIPS_FORMAT_I,
    MIPS_FORMAT_I_SINGLE,
    MIPS_FORMAT_I_OFFSET,
    MIPS_FORMAT_I_OFFSET_SINGLE,
    MIPS_FORMAT_I_MEMORY_OFFSET,
    MIPS_FORMAT_I_VECTOR,
    MIPS_FORMAT_SYSTEM_CONTROL,
    MIPS_FORMAT_VECTOR_CONTROL,

    MIPS_FORMAT_J,
    MIPS_FORMAT_UNKNOWN_OPCODE
};

int IW_decode(char * text, u32 word)
{
    signed short offset_vec;
    int instruction_format, illegal;
    const unsigned long target = word & 0x03FFFFFFul;
    const unsigned short imm = word & 0x0000FFFFu;
    const signed offset = (imm & 0x8000u) ? -(short)(~imm + 1) : imm & 0x7FFF;
    const unsigned int op   = (word >> 26) % 64;
    const unsigned int func = (word >>  0) % 64;
    const unsigned int rd   = (word >> 11) % 32;
    const unsigned int rs   = (word >> 21) % 32;
    const unsigned int rt   = (word >> 16) % 32;
    const unsigned int sa   = (word >>  6) % 32;

    const unsigned int vd = sa, vs = rd, vt = rt;
    const unsigned int de = rd & 07, e = rs & 0xF;
    const unsigned int base = rs;
    const unsigned int element = (imm >> 7) & 0xF;

    if (word == 0x00000000u)
    { /* the only valid MIPS pseudo-instruction allowed in RSP assembly */
        strcpy(text, "NOP");
        return 0;
    }

    instruction_format = MIPS_FORMAT_UNKNOWN_OPCODE;
    switch (op % 64)
    {
    case 000:  /* SPECIAL */
        strcpy(text, mnemonics_SPECIAL[func]);
        if (func == 015) /* BREAK */
            instruction_format = MIPS_FORMAT_R_NO_OPERANDS;
        else if (func < 004)
            instruction_format = MIPS_FORMAT_R_SHIFT_IMM;
        else if (func < 010)
            instruction_format = MIPS_FORMAT_R_SHIFT;
        else if (func < 012)
            instruction_format = (func & 1)
              ? MIPS_FORMAT_R_JALR : MIPS_FORMAT_R_JR;
        else
            instruction_format = MIPS_FORMAT_R;
        break;
    case 001:  /* REGIMM */
        strcpy(text, mnemonics_REGIMM[rt]);
        instruction_format = MIPS_FORMAT_I_OFFSET_SINGLE;
        break;
    case 020:  /* COP0 */
        strcpy(text, mnemonics_COP0[rs]);
        instruction_format = MIPS_FORMAT_SYSTEM_CONTROL;
        break;
    case 022:  /* COP2 */
        if (rs < 16)
        {
            strcpy(text, mnemonics_COP2[rs]);
            instruction_format = MIPS_FORMAT_VECTOR_CONTROL;
            break;
        }
        strcpy(text, mnemonics_vector[func]);
        instruction_format = (func >= 060 && func <= 067)
          ? MIPS_FORMAT_R_VECTOR_DIVIDE
          : MIPS_FORMAT_R_VECTOR;
        if (func == 067) /* VNOP */
            instruction_format = MIPS_FORMAT_R_NO_OPERANDS;
        break;
    case 062:  /* LWC2 */
        strcpy(text, mnemonics_LWC2[rd]);
        instruction_format = MIPS_FORMAT_I_VECTOR;
        break;
    case 072:  /* SWC2 */
        strcpy(text, mnemonics_SWC2[rd]);
        instruction_format = MIPS_FORMAT_I_VECTOR;
        break;
    default:
        strcpy(text, mnemonics_primary[op]);
        if (op < 004)
            instruction_format = MIPS_FORMAT_J;
        else if (op < 006) /* BEQ and BNE */
            instruction_format = MIPS_FORMAT_I_OFFSET;
        else if (op < 010) /* BLEZ and BGTZ */
            instruction_format = MIPS_FORMAT_I_OFFSET_SINGLE;
        else if (op == 017) /* LUI */
            instruction_format = MIPS_FORMAT_I_SINGLE;
        else if (op >= 040) /* LB, LBU, SB and so on */
            instruction_format = MIPS_FORMAT_I_MEMORY_OFFSET;
        else 
            instruction_format = MIPS_FORMAT_I;
        break;
    }

    illegal = !strcmp(text, unused_op);
    if (instruction_format == MIPS_FORMAT_R_NO_OPERANDS)
        return (illegal); /* BREAK and VNOP */

    text = strcat(text, " ");
    text = &text[strlen(text)];
    switch (instruction_format)
    {
    case MIPS_FORMAT_R:
        sprintf(text, "$%u, $%u, $%u", rd, rs, rt);
        break;
    case MIPS_FORMAT_R_NO_OPERANDS:
        break;
    case MIPS_FORMAT_R_SHIFT:
        sprintf(text, "$%u, $%u, $%u", rd, rt, rs);
        break;
    case MIPS_FORMAT_R_SHIFT_IMM:
        sprintf(text, "$%u, $%u, %u", rd, rt, sa);
        break;
    case MIPS_FORMAT_R_JALR:
        sprintf(text, "$%u, $%u", rd, rs);
        if (rd != 31) /* $ra */
            break;
    case MIPS_FORMAT_R_JR: /* can fall through from JALR */
        sprintf(text, "$%u", rs);
        break;
    case MIPS_FORMAT_R_VECTOR:
        sprintf(text, "$v%u, $v%u, $v%u%s", vd, vs, vt, vector_scalars[e]);
        break;
    case MIPS_FORMAT_R_VECTOR_DIVIDE:
        sprintf(text, "$v%u[%u], $v%u[%u]", vd, de, vt, e & 07);
        break;

    case MIPS_FORMAT_I:
        sprintf(text, "$%u, $%u, 0x%04X", rt, rs, imm);
        break;
    case MIPS_FORMAT_I_SINGLE:
        sprintf(text, "$%u, 0x%04X", rt, imm);
        break;
    case MIPS_FORMAT_I_OFFSET:
        sprintf(text, "$%u, %u, %i", rs, rt, offset);
        break;
    case MIPS_FORMAT_I_OFFSET_SINGLE:
        sprintf(text, "$%u, %i", rs, offset);
        break;
    case MIPS_FORMAT_I_MEMORY_OFFSET:
        sprintf(text, "$%u, %i($%u)", rt, offset, base);
        break;
    case MIPS_FORMAT_I_VECTOR:
        if (imm & 0x0040)
            offset_vec = -(signed short)((~imm & 0x003F) + 1);
        else
            offset_vec = (short)(imm & 0x007F & 0x003F);
        sprintf(text, "$v%u[%u], %i($%u)", vt, element, offset_vec, base);
        break;
    case MIPS_FORMAT_SYSTEM_CONTROL:
        sprintf(text, "$%u, $c%u", rt, rd);
        break;
    case MIPS_FORMAT_VECTOR_CONTROL:
        if (rs & 2)
            sprintf(text, "$%u, $%s", rt, vector_CPRs[rd & 3]);
        else
            sprintf(text, "$%u, $v%u[%u]", rt, rd, element);
        break;

    case MIPS_FORMAT_J:
        sprintf(text, "0x%07X", target << 2);
        break;
    default:
        sprintf(
            text,
            "Unimplemented instruction format:  %i",
            instruction_format
        );
        return (illegal = 1);
    }
    return (illegal);
}
