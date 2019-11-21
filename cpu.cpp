#include "cpu.h"
#include "decodetable.h"
#include <chrono>

Cpu::Cpu(Memory& memory) : memory(memory) {
}

void Cpu::prepImpliedMode() {
  // nothing to do
}

void Cpu::prepAccumulatorMode() {
  effectiveOperandPtr.lo = &regs.a;
}

void Cpu::prepRelativeMode() {
  // all handled by branch instructions
}

void Cpu::prepIndirectMode() {
  effectiveAddress = memory.word(operandPtr.word());
}

void Cpu::prepImmediateMode() {
  effectiveOperandPtr.lo = operandPtr.lo;
}

void Cpu::prepZeroPageMode() {
  effectiveAddress = *operandPtr.lo;
  setEffectiveOperandPtrToAddress();
}

void Cpu::prepZeroPageXMode() {
  calculateZeroPageEffectiveAddress(*operandPtr.lo, regs.x);
  setEffectiveOperandPtrToAddress();
}

void Cpu::prepZeroPageYMode() {
  calculateZeroPageEffectiveAddress(*operandPtr.lo, regs.y);
  setEffectiveOperandPtrToAddress();
}

void Cpu::prepIndexedIndirectXMode() {
  effectiveAddress = memory.word(static_cast<uint8_t>((*operandPtr.lo + regs.x)));
  setEffectiveOperandPtrToAddress();
}

void Cpu::prepIndirectIndexedYMode() {
  calculateEffectiveAddress(memory.word(*operandPtr.lo), regs.y);
  setEffectiveOperandPtrToAddress();
}

void Cpu::prepAbsoluteMode() {
  effectiveAddress = operandPtr.word();
  setEffectiveOperandPtrToAddress();
}

void Cpu::prepAbsoluteXMode() {
  calculateEffectiveAddress(operandPtr.word(), regs.x);
  setEffectiveOperandPtrToAddress();
}

void Cpu::prepAbsoluteYMode() {
  calculateEffectiveAddress(operandPtr.word(), regs.y);
  setEffectiveOperandPtrToAddress();
}

void Cpu::execLDA() {
  regs.p.computeNZ(regs.a = *effectiveOperandPtr.lo);
  applyExtraCycleOnPageBoundaryCrossing();
}

void Cpu::execLDX() {
  regs.p.computeNZ(regs.x = *effectiveOperandPtr.lo);
  applyExtraCycleOnPageBoundaryCrossing();
}

void Cpu::execLDY() {
  regs.p.computeNZ(regs.y = *effectiveOperandPtr.lo);
  applyExtraCycleOnPageBoundaryCrossing();
}

void Cpu::execSTA() {
  *effectiveOperandPtr.lo = regs.a;
}

void Cpu::execSTX() {
  *effectiveOperandPtr.lo = regs.x;
}

void Cpu::execSTY() {
  *effectiveOperandPtr.lo = regs.y;
}

void Cpu::execADC() {
  execAddWithCarry(*effectiveOperandPtr.lo);
  applyExtraCycleOnPageBoundaryCrossing();
}

void Cpu::execSBC() {
  execAddWithCarry(*effectiveOperandPtr.lo ^ 0xff);
  applyExtraCycleOnPageBoundaryCrossing();
}

void Cpu::execINC() {
  regs.p.computeNZ(++(*effectiveOperandPtr.lo));
}

void Cpu::execINX() {
  regs.p.computeNZ(++regs.x);
}

void Cpu::execINY() {
  regs.p.computeNZ(++regs.y);
}

void Cpu::execDEC() {
  regs.p.computeNZ(--(*effectiveOperandPtr.lo));
}

void Cpu::execDEX() {
  regs.p.computeNZ(--regs.x);
}

void Cpu::execDEY() {
  regs.p.computeNZ(--regs.y);
}

void Cpu::execASL() {
  auto val = *effectiveOperandPtr.lo;
  regs.p.carry = val & 0x80;
  regs.p.computeNZ(*effectiveOperandPtr.lo = static_cast<uint8_t>(val << 1));
}

void Cpu::execLSR() {
  auto val = *effectiveOperandPtr.lo;
  regs.p.carry = val & 0x01;
  regs.p.computeNZ(*effectiveOperandPtr.lo = static_cast<uint8_t>(val >> 1));
}

void Cpu::execROL() {
  const uint16_t res = static_cast<uint16_t>(*effectiveOperandPtr.lo << 1) | regs.p.carry;
  regs.p.carry = res & 0x100;
  regs.p.computeNZ(*effectiveOperandPtr.lo = static_cast<uint8_t>(res));
}

void Cpu::execROR() {
  const uint16_t tmp = *effectiveOperandPtr.lo | (regs.p.carry ? 0x100 : 0x00);
  regs.p.carry = tmp & 0x01;
  regs.p.computeNZ(*effectiveOperandPtr.lo = static_cast<uint8_t>(tmp >> 1));
}

void Cpu::execAND() {
  regs.p.computeNZ(regs.a &= *effectiveOperandPtr.lo);
  applyExtraCycleOnPageBoundaryCrossing();
}

void Cpu::execORA() {
  regs.p.computeNZ(regs.a |= *effectiveOperandPtr.lo);
  applyExtraCycleOnPageBoundaryCrossing();
}

void Cpu::execEOR() {
  regs.p.computeNZ(regs.a ^= *effectiveOperandPtr.lo);
  applyExtraCycleOnPageBoundaryCrossing();
}

void Cpu::execCompare() {
  execCompare(regs.a);
  applyExtraCycleOnPageBoundaryCrossing();
}

void Cpu::execCPX() {
  execCompare(regs.x);
}

void Cpu::execCPY() {
  execCompare(regs.y);
}

void Cpu::execBIT() {
  const auto operand = *effectiveOperandPtr.lo;
  regs.p.zero = !(regs.a & operand);
  regs.p.negative = operand & 0x80;
  regs.p.overflow = operand & 0x40;
}

void Cpu::execSED() {
  regs.p.decimal = true;
}

void Cpu::execSEI() {
  regs.p.interrupt = true;
}

void Cpu::execSEC() {
  regs.p.carry = true;
}

void Cpu::execCLC() {
  regs.p.carry = false;
}

void Cpu::execCLD() {
  regs.p.decimal = false;
}

void Cpu::execCLI() {
  regs.p.interrupt = false;
}

void Cpu::execCLV() {
  regs.p.overflow = false;
}

void Cpu::execTAX() {
  regs.p.computeNZ(regs.x = regs.a);
}

void Cpu::execTXA() {
  regs.p.computeNZ(regs.a = regs.x);
}

void Cpu::execTAY() {
  regs.p.computeNZ(regs.y = regs.a);
}

void Cpu::execTYA() {
  regs.p.computeNZ(regs.a = regs.y);
}

void Cpu::execTSX() {
  regs.p.computeNZ(regs.x = regs.sp.offset);
}

void Cpu::execTXS() {
  regs.sp.offset = regs.x;
}

void Cpu::execPHA() {
  push(regs.a);
}

void Cpu::execPLA() {
  regs.p.computeNZ(regs.a = pull());
}

void Cpu::execPHP() {
  push(regs.p);
}

void Cpu::execPLP() {
  regs.p = pull();
}

void Cpu::execNOP() {
  // literally do nothing :-)
}

void Cpu::execBCC() {
  if (!regs.p.carry) execBranch();
}

void Cpu::execBCS() {
  if (regs.p.carry) execBranch();
}

void Cpu::execBEQ() {
  if (regs.p.zero) execBranch();
}

void Cpu::execBMI() {
  if (regs.p.negative) execBranch();
}

void Cpu::execBNE() {
  if (!regs.p.zero) execBranch();
}

void Cpu::execBPL() {
  if (!regs.p.negative) execBranch();
}

void Cpu::execBVC() {
  if (!regs.p.overflow) execBranch();
}

void Cpu::execBVS() {
  if (regs.p.overflow) execBranch();
}

void Cpu::execJMP() {
  regs.pc = effectiveAddress;
}

void Cpu::execJSR() {
  pushWord(regs.pc - 1);
  regs.pc = effectiveAddress;
}

void Cpu::execRTS() {
  regs.pc = pullWord() + 1;
}

void Cpu::execRTI() {
  regs.p = pull();
  regs.pc = pullWord();
  regs.p.interrupt = false;
}

void Cpu::execBRK() {
  pushWord(regs.pc + 1);
  push(regs.p | ProcessorStatus::BreakBitMask);
  regs.p.interrupt = true;
  regs.pc = memory.word(CpuAddress::IrqVector);
}

void Cpu::irq() {
  pushWord(regs.pc);
  push(regs.p);
  regs.p.interrupt = true;
  regs.pc = memory.word(CpuAddress::IrqVector);
  runLevel = CpuRunLevel::Program;
}

void Cpu::nmi() {
  pushWord(regs.pc);
  push(regs.p);
  regs.p.interrupt = true;
  regs.pc = memory.word(CpuAddress::NmiVector);
  runLevel = CpuRunLevel::Program;
}

void Cpu::reset() {
  regs.pc = memory.word(CpuAddress::ResetVector);
  regs.a = 0;
  regs.x = 0;
  regs.y = 0;
  regs.sp.offset = 0xfd;
  regs.p.negative = false;
  regs.p.overflow = false;
  regs.p.decimal = false;
  regs.p.interrupt = true;
  regs.p.zero = false;
  regs.p.carry = false;
  resetStatistics();
  runLevel = CpuRunLevel::Program;
}

void Cpu::resetExecutionState() {
  if (state == CpuState::Halted || state == CpuState::Stopped) state = CpuState::Idle;
}

void Cpu::resetStatistics() {
  cycles = 0;
  duration = Duration::zero();
}

void Cpu::stopExecution() {
  if (state == CpuState::Running) state = CpuState::Stopping;
}

void Cpu::execHalt() {
  regs.pc--;
  state = CpuState::Halted;
}

void Cpu::execute(bool continuous) {
  state = CpuState::Running;
  runLevel = continuous ? CpuRunLevel::Program : CpuRunLevel::SingleStep;
  while (state == CpuState::Running) {
    const auto t0 = PreciseClock::now();
    pageBoundaryCrossed = false;
    const auto pcPtr = &memory[regs.pc];
    operandPtr.lo = &memory[regs.pc + 1];
    operandPtr.hi = &memory[regs.pc + 2];
    const auto& entry = DecodeTable[*pcPtr];
    const auto ins = entry.instruction;

    regs.pc += ins->size;
    cycles += ins->cycles;

    (this->*entry.prepareOperands)();
    (this->*entry.executeInstruction)();

    duration += std::chrono::duration_cast<Duration>(PreciseClock::now() - t0);

    switch (runLevel) {
    case CpuRunLevel::Program: break;
    case CpuRunLevel::SingleStep: goto ExitLoop;
    case CpuRunLevel::Reset: reset(); break;
    case CpuRunLevel::Nmi: nmi(); break;
    case CpuRunLevel::Irq: irq(); break;
    }
  }

ExitLoop:
  switch (state) {
  case CpuState::Running: state = CpuState::Idle; break;
  case CpuState::Stopping: state = CpuState::Stopped; break;
  case CpuState::Halting: state = CpuState::Halted; break;
  default: break;
  }
}

void Cpu::triggerReset() {
  if (runLevel < CpuRunLevel::Reset) {
    if (running()) {
      runLevel = CpuRunLevel::Reset;
    } else
      reset();
  }
}

void Cpu::triggerNmi() {
  if (runLevel < CpuRunLevel::Nmi) {
    if (running()) {
      runLevel = CpuRunLevel::Nmi;
    } else {
      nmi();
    }
  }
}

void Cpu::triggerIrq() {
  if (runLevel < CpuRunLevel::Irq && !regs.p.interrupt) {
    if (!running()) {
      runLevel = CpuRunLevel::Irq;
    } else {
      irq();
    }
  }
}

CpuInfo Cpu::info() const {
  return {runLevel, state};
}
