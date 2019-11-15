#pragma once

#include "commons.h"
#include "cpu.h"
#include "cpuinfo.h"
#include "memory.h"
#include <QObject>

class System : public QObject {
  Q_OBJECT

public:
  explicit System(QObject* parent = nullptr);
  const Memory& memoryView() const { return memory_; }
  void requestIRQ();
  void requestNMI();
  void requestRESET();

signals:
  void cpuStateChanged(CpuInfo);
  void memoryContentChanged(quint16 first, quint16 last);

public slots:
  void executeSingleStep();
  void changeProgramCounter(quint16 pc);
  void propagateCurrentState();
  void uploadToMemory(quint16 first, const Data& data);
  void resetCycleCounter();

private:
  Memory memory_;
  Cpu cpu_;
};
