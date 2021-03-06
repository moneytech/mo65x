#pragma once

#include "addressrange.h"
#include "commondefs.h"
#include "cpu.h"
#include "emulatorstate.h"
#include "memory.h"
#include <QObject>

class Emulator : public QObject {
  Q_OBJECT

public:
  explicit Emulator(QObject* parent = nullptr);
  const Memory& memoryView() const { return memory; }
  Memory& memoryRef() { return memory; }
  const EmulatorState state(ExecutionStatistics = {});

signals:
  void stateChanged(EmulatorState);
  void memoryContentChanged(AddressRange);
  void operationCompleted(const QString& message, bool success);

public slots:
  void execute(bool continuous, Frequency clock);
  void changeProgramCounter(Address);
  void changeStackPointer(Address);
  void changeAccumulator(uint8_t);
  void changeRegisterX(uint8_t);
  void changeRegisterY(uint8_t);
  void changeMemory(Address, uint8_t);
  void loadMemory(Address first, const Data& data);
  void loadMemoryFromFile(Address start, const QString& fname);
  void saveMemoryToFile(AddressRange range, const QString& fname);

  // to be connected as direct connections

  void triggerIrq();
  void triggerNmi();
  void triggerReset();
  void stopExecution();
  void clearStatistics();

private:
  Memory memory;
  Cpu cpu;
};
