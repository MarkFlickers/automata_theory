#pragma once

#include <string>

// Пара состояний (si, sj). Номера состояний начинаются с 1, как в файлах.
// Порядок нормализуется: First() < Second(), поэтому SCouple(3, 2) == SCouple(2, 3).
// Пара из одинаковых состояний тривиальна и не создаётся: конструктор бросит исключение.
class SCouple {
public:
    SCouple(int first, int second);

    int First() const;
    int Second() const;

    bool operator==(const SCouple& other) const = default;

    // "(2,3)"
    std::string ToString() const;

private:
    int first_;
    int second_;
};
