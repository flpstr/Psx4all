#ifndef DISASM_H
#define DISASM_H

typedef struct
{
  void *address;
  char *name;
} disasm_label;

void disasm_arm_instruction(u32 opcode, char *buffer, u32 pc,
 disasm_label *labels, u32 num_labels);
void disasm_thumb_instruction(u32 opcode, char *buffer, u32 pc,
 disasm_label *labels, u32 num_labels);
void disasm_mips_instruction(u32 opcode, char *buffer, u32 pc);

extern const char *reg_names[];

#endif
