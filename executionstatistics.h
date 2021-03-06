#pragma once

#include "commondefs.h"
#include <QString>
#include <cstdint>

struct ExecutionStatistics {
  long cycles = 0;
  Duration duration = Duration::zero();

  bool valid() const { return cycles > 0 && duration != Duration::zero(); }
  double microSec() const { return std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(duration).count(); }
  double seconds() const { return std::chrono::duration_cast<std::chrono::duration<double>>(duration).count(); }
  double clockMHz() const { return cycles / microSec(); }

  ExecutionStatistics operator-(const ExecutionStatistics& e) const { return {cycles - e.cycles, duration - e.duration}; }
};

QString formatExecutionStatistics(ExecutionStatistics es);
