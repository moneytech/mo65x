#pragma once

#include <QString>
#include <map>
#include <optional>

struct SymbolTable : std::map<QString, uint16_t> {
  bool put(const QString& name, uint16_t value);
  std::optional<int> get(const QString& name) const;
};
