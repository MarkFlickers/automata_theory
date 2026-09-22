#pragma once

#include <vector>

// Проверочное слово: номера входов, состояний и выходов.
// '-' хранится как kUndefinedValue (см. TableItem.hpp).
struct CheckWord {
    std::vector<int> inputs;   // X: k входов
    std::vector<int> states;   // S: k + 1 состояние, первое — начальное
    std::vector<int> outputs;  // Y: k выходов
};
