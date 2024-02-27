#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// http://www.obelisk.me.uk/6502
using Byte = unsigned char;
using Word = unsigned short;
using Dword = uint32_t;

struct Mem {
  static constexpr Dword MAX_MEM = 1024 * 64;
  Byte Data[MAX_MEM];

  void Initialize() {
    for (Dword i = 0; i < MAX_MEM; i++) {
      Data[i] = 0;
    }
  }

  // read 1 byte
  Byte &operator[](Dword Address) {
    // assert here Address is < MAX_MEM;
    return Data[Address];
  }
};

struct CPU {

  Word PC; // program counter
  Byte SP; // stack pointer

  Byte A, X, Y; // registers

  Byte C : 1;       // carry
  Byte Z : 1;       // zero
  Byte I : 1;       // interrupt(IRQ) disable
  Byte D : 1;       // decimal mode
  Byte B : 1;       // break command
  Byte nothing : 1; // padding bit
  Byte V : 1;       // overflow flag
  Byte N : 1;       // negative flag

  void Reset(Mem memory) {
    PC = 0xFFFC;
    SP = 0x00FF;
    C = Z = I = D = B = V = N = 0;
    A = X = Y = 0;

    memory.Initialize();
  }

  // PC++
  Byte FetchByte(Dword &Cycles, Mem &memory) {
    Byte Data = memory[PC];
    // Byte Data = 0; // debugging
    // printf("Data = %d\n", Data);
    PC++;
    Cycles--;
    return Data;
  }

  // PC not modified
  Byte ReadByte(Dword *Cycles, Byte Address, Mem &memory) {
    Byte Data = memory[Address];
    // Byte Data = 0; // debugging
    printf("Data = %d\n", Data);
    Cycles--;
    return Data;
  }

  // opcodes
  static constexpr Byte INS_LDA_IM = 0xA9, // LDA
      INS_LDA_ZP = 0xA5,                   // LDA zero page, LDA-Z PAGE
      INS_LDA_ZPX = 0xB5;                  // LDA zero page, LDA-Z PAGE

  void Execute(Dword Cycles, Mem &memory) {
    while (Cycles > 0) {
      Byte Instruction = FetchByte(Cycles, memory);
      switch (Instruction) {
      case INS_LDA_IM: {
        Byte value = FetchByte(Cycles, memory);
        A = value;
        Z = (A == 0);
        N = (A & (1 << 7)) > 0;
        break;
      }
      case INS_LDA_ZP: {
        Byte ZeroPageAddress = FetchByte(Cycles, memory);
        A = ReadByte(&Cycles, ZeroPageAddress, memory);
        Z = (A == 0);
        N = (A & (1 << 7)) > 0;
        break;
      }
      default: {
        printf("Instruction not handled\n");
        break;
      }
      }
    }
  }
};

int main() {
  Mem mem;
  CPU cpu;
  cpu.Reset(mem);
  mem[0xFFFc] = CPU::INS_LDA_ZP; // debugging, reset
  // mem[0xFFFc] = cpu.INS_LDA_IM; // debugging, reset
  mem[0xFFFD] = 0x42; // debugging, reset
  mem[0x0042] = 42;   // debugging, zero page
  cpu.Execute(2, mem);
}

// https://www.bilibili.com/video/BV1Ro4y1272a
// 21 min
