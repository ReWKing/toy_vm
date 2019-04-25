#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// 寄存器编号
enum {
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC,                   // PC 寄存器
    R_COND,                 // 标志寄存器
    R_COUNT,                // 10
};

// 每次计算完都要更新 R_COND
enum {
    FL_POS = 1 << 0,        // > 0
    FL_ZEO = 1 << 1,        // = 0
    FL_NEG = 1 << 2,        // < 0
};

// 操作码编号
enum {
    OP_BR = 0,              // branch
    OP_ADD,                 // add 
    OP_LD,                  // load
    OP_ST,                  // store
    OP_JSR,                 // jump register
    OP_AND,                 // bitwise and
    OP_LDR,                 // load register
    OP_STR,                 // store register
    OP_RTI,                 // unused 
    OP_NOT,                 // bitwise not
    OP_LDI,                 // load indirect
    OP_STI,                 // store indirect
    OP_JMP,                 // jump
    OP_RES,                 // reserved (unused)
    OP_LEA,                 // load effective address
    OP_TRAP                 // execute trap
};

// 内存映射寄存器代码地址
enum {
    MR_KBSR = 0xFE00,       // keyboard status
    MR_KBDR = 0xFE02        // keyboard data
};


// 陷入例程编号
enum {
    TRAP_GETC   = 0x20,     // get character from keyboard, not echoed onto the terminal
    TRAP_OUT    = 0x21,     // output a character
    TRAP_PUTS   = 0x22,     // output a word string
    TRAP_IN     = 0x23,     // get character from keyboard, echoed onto the terminal
    TRAP_PUTSP  = 0x24,     // output a byte string
    TRAP_HALT   = 0x25,     // halt the program
};

// 内存
uint16_t memory[UINT16_MAX];

// 寄存器
uint16_t regs[R_COUNT];

int read_image(const char *image_path);
void read_image_file(FILE *file);
uint16_t swap16(uint16_t x);

int main(int argc, const char *argv[]) {

    if(argc < 2) {
        printf("缺少文件地址");
        exit(2);
    }

    // 将文件读入内存
    for(int i = 1; i < argc; i++) {
        if(!read_image(argv[i])) {
            printf("没有找到 image 文件");
            exit(1);
        }
    }

    return 0;
}

int read_image(const char *image_path) {

    FILE *file = fopen(image_path, "rb");
    if(!file) {
        return 0;
    }
    read_image_file(file);
    fclose(file);
    return 1;
}

uint16_t swap16(uint16_t x) {
    return (x >> 8) | (x << 8);
}


void read_image_file(FILE *file) {

    // 文件开头 16 位是地址
    uint16_t origin;
    fread(&origin, sizeof(origin), 1, file);

    // 程序是大端机器写的，所以每个读出来的内容都要转换
    origin = swap16(origin);

    uint16_t max_read = UINT16_MAX - origin;
    uint16_t *p = memory + origin;

    // 读入内存
    size_t read = fread(p, sizeof(uint16_t), max_read, file);

    // 把所有的内容转换成小端
    while(read--) {
        *p = swap16(*p);
        p++;
    }
}
