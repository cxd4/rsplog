#ifndef _MNEMONICS_H_
#define _MNEMONICS_H_

#define unused_op       "invalid"

/* R4000 primary operation codes, 32-bit subset */
static char mnemonics_primary[8 << 3][8] = {
"SPECIAL","REGIMM ","J      ","JAL    ","BEQ    ","BNE    ","BLEZ   ","BGTZ   ",
"ADDI   ","ADDIU  ","SLTI   ","SLTIU  ","ANDI   ","ORI    ","XORI   ","LUI    ",
"COP0   ",unused_op,"COP2   ",unused_op,unused_op,unused_op,unused_op,unused_op,
unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,
"LB     ","LH     ",unused_op,"LW     ","LBU    ","LHU    ",unused_op,unused_op,
"SB     ","SH     ",unused_op,"SW     ",unused_op,unused_op,unused_op,unused_op,
unused_op,unused_op,"LWC2   ",unused_op,unused_op,unused_op,unused_op,unused_op,
unused_op,unused_op,"SWC2   ",unused_op,unused_op,unused_op,unused_op,unused_op,
};
/* 000   |   001   |   010   |   011   |   100   |   101   |   110   |   111  */

/* RSP sub-operations */
static char mnemonics_SPECIAL[8 << 3][8] = {
"SLL    ",unused_op,"SRL    ","SRA    ","SLLV   ",unused_op,"SRLV   ","SRAV   ",
"JR     ","JALR   ",unused_op,unused_op,unused_op,"BREAK  ",unused_op,unused_op,
unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,
unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,
"ADD    ","ADDU   ","SUB    ","SUBU   ","AND    ","OR     ","XOR    ","NOR    ",
unused_op,unused_op,"SLT    ","SLTU   ",unused_op,unused_op,unused_op,unused_op,
unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,
unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,
};
static char mnemonics_REGIMM[8 << 2][8] = {
"BLTZ   ","BGEZ   ",unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,
unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,
"BLTZAL ","BGEZAL ",unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,
unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,
};
static char mnemonics_COP0[8 << 2][8] = {
"MFC0   ",unused_op,unused_op,unused_op,"MTC0   ",unused_op,unused_op,unused_op,
unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,
unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,
unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,
};
static char mnemonics_COP2[8 << 2][8] = {
"MFC2   ",unused_op,"CFC2   ",unused_op,"MTC2   ",unused_op,"CTC2   ",unused_op,
unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,
"C2     ","C2     ","C2     ","C2     ","C2     ","C2     ","C2     ","C2"     ,
"C2     ","C2     ","C2     ","C2     ","C2     ","C2     ","C2     ","C2"     ,
};
static char mnemonics_LWC2[8 << 2][8] = {
"LBV    ","LSV    ","LLV    ","LDV    ","LQV    ","LRV    ","LPV    ","LUV    ",
"LHV    ","LFV    ",unused_op,"LTV    ",unused_op,unused_op,unused_op,unused_op,
unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,
unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,
};
static char mnemonics_SWC2[8 << 2][8] = {
"SBV    ","SSV    ","SLV    ","SDV    ","SQV    ","SRV    ","SPV    ","SUV    ",
"SHV    ","SFV    ","SWV    ","STV    ",unused_op,unused_op,unused_op,unused_op,
unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,
unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,
};

/* RSP vector unit, sub-COP2 operation codes */
static char mnemonics_vector[8 << 3][8] = {
"VMULF  ","VMULU  ","VRNDP  ","VMULQ  ","VMUDL  ","VMUDM  ","VMUDN  ","VMUDH  ",
"VMACF  ","VMACU  ","VRNDN  ","VMACQ  ","VMADL  ","VMADM  ","VMADN  ","VMADH  ",
"VADD   ","VSUB   ",unused_op,"VABS   ","VADDC  ","VSUBC  ",unused_op,unused_op,
unused_op,unused_op,unused_op,unused_op,unused_op,"VSAR   ",unused_op,unused_op,
"VLT    ","VEQ    ","VNE    ","VGE    ","VCL    ","VCH    ","VCR    ","VMRG   ",
"VAND   ","VNAND  ","VOR    ","VNOR   ","VXOR   ","VNXOR  ",unused_op,unused_op,
"VRCP   ","VRCPL  ","VRCPH  ","VMOV   ","VRSQ   ","VRSQL  ","VRSQH  ","VNOP   ",
unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,unused_op,
};
#endif
