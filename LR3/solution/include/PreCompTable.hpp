#pragma once

#include "CompTable.hpp"
#include "Table.hpp"

// Фаза 1: таблица предварительной совместимости.
// Ячейка: 'x' — несовместимы по выходам; 'v' — безусловно совместимы;
// иначе — список пар-условий.
class PreCompTable final : public CompTable {
public:
    PreCompTable(const Table& transitions, const Table& outputs);

protected:
    std::string FormatCell(const CompCell& cell) const override;
};
