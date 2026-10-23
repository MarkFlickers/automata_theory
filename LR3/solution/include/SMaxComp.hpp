#pragma once

#include "FCompTable.hpp"

#include <iosfwd>
#include <vector>

// Фаза 3: множество классов максимальной совместимости.
// Каждый класс — номера состояний (с 1) по возрастанию: {3, 4, 5}.
class SMaxComp {
public:
    explicit SMaxComp(const FCompTable& table);

    // Порядок: 1) классы с уникальным состоянием — вперёд;
    //          2) по убыванию размера;
    //          3) лексикографически по возрастанию.
    void Sort();

    const std::vector<std::vector<int>>& Classes() const;

    // "A1 = {s3, s4, s5}" — по строке на класс в текущем порядке.
    void Print(std::ostream& output) const;

private:
    std::vector<std::vector<int>> classes_;
};
