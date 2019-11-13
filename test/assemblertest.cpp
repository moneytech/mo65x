#include "assemblertest.h"
#include <QTest>

AssemblerTest::AssemblerTest(QObject* parent) : QObject(parent), assembler(), buffer(assembler.machineCode()) {
}

void AssemblerTest::verify(const char* str, uint8_t opcode, int lo, int hi) {
  QCOMPARE(assembler.assemble(str), Assembler::Result::Ok);
  QCOMPARE(buffer[0], opcode);
  if (lo >= 0) QCOMPARE(lo, buffer[1]);
  if (hi >= 0) QCOMPARE(hi, buffer[2]);
}

void AssemblerTest::init() {
  assembler.reset();
  assembler.setOrigin(AsmOrigin);
}

void AssemblerTest::testImpliedMode() {
  verify("SEI", 0x78);
}

void AssemblerTest::testAccumulatorMode() {
  verify("ASL", 0x0a);
}

void AssemblerTest::testImmediateMode() {
  verify("LDX #$2f", 0xa2, 0x2f);
}

void AssemblerTest::testZeroPageMode() {
  verify("LDY $8f", 0xa4, 0x8f);
}

void AssemblerTest::testZeroPageXMode() {
  verify("LDA $a0,X", 0xb5, 0xa0);
}

void AssemblerTest::testZeroPageYMode() {
  verify("STX $7a,Y", 0x96, 0x7a);
}

void AssemblerTest::testAbsoluteMode() {
  verify("ROR $3400", 0x6e, 0x00, 0x34);
}

void AssemblerTest::testAbsoluteXMode() {
  verify("LSR $35f0,X", 0x5e, 0xf0, 0x35);
}

void AssemblerTest::testAbsoluteYMode() {
  verify("EOR $f7a0,Y", 0x59, 0xa0, 0xf7);
}

void AssemblerTest::testIndirectMode() {
  verify("JMP ($ffa0)", 0x6c, 0xa0, 0xff);
}

void AssemblerTest::testIndexedIndirectXMode() {
  verify("LDA ($8c,X)", 0xa1, 0x8c);
}

void AssemblerTest::testIndirectIndexedYMode() {
  verify("ORA ($a7),Y", 0x11, 0xa7);
}

void AssemblerTest::testRelativeModeMinus() {
  verify("BCC -1", 0x90, -1);
}

void AssemblerTest::testRelativeModePlus() {
  verify("BVS +8", 0x70, 8);
}

void AssemblerTest::testOrg() {
  assembler.reset();
  QCOMPARE(assembler.assemble("  ORG $3000 ;origin"), Assembler::Result::Ok);
  QCOMPARE(assembler.locationCounter(), 0x3000);
  QCOMPARE(assembler.assemble("  ORG $4000 ;origin"), Assembler::Result::OriginDefined);
  QCOMPARE(assembler.locationCounter(), 0x3000);
}

void AssemblerTest::testComment() {
  QCOMPARE(assembler.assemble("  SEI   ; disable interrupts "), Assembler::Result::Ok);
  QCOMPARE(assembler.assemble("; disable interrupts "), Assembler::Result::Ok);
}

void AssemblerTest::testEmptyLineLabel() {
  QCOMPARE(assembler.assemble("Label_001:"), Assembler::Result::Ok);
  QCOMPARE(assembler.symbol("Label_001"), assembler.locationCounter());
  QCOMPARE(assembler.symbol("dummy", 1234), 1234);
}
