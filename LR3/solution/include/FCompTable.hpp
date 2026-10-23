#pragma once

#include "CompTable.hpp"
#include "PreCompTable.hpp"

// Фаза 2: таблица полной совместимости.
// Ячейка: 'x' — несовместимы; 'v' — совместимы. Условия остаются в ячейках,
// но после фазы 2 не используются.
class FCompTable final : public CompTable {
public:
    explicit FCompTable(const PreCompTable& preTable);

protected:
    std::string FormatCell(const CompCell& cell) const override;
};
