#include "Printer.hpp"

#include "FileIO.hpp"

#include <algorithm>
#include <cstddef>
#include <iomanip>
#include <ostream>
#include <stdexcept>

namespace {

constexpr const char* kTableGap = "    ";

int CellWidth(const Table& table, const std::string& statePrefix) {
    std::size_t width = statePrefix.size() + std::to_string(table.StateCount()).size();
    for (int input = 0; input < table.InputCount(); ++input) {
        for (int state = 0; state < table.StateCount(); ++state) {
            width = std::max(width, FormatValue(table.Value(input, state)).size());
        }
    }

    return static_cast<int>(width);
}

void PrintHeader(std::ostream& output, const Table& table, const std::string& statePrefix, int width) {
    for (int state = 0; state < table.StateCount(); ++state) {
        output << ' ' << std::setw(width) << statePrefix + std::to_string(state + 1);
    }
}

void PrintRow(std::ostream& output, const Table& table, int input, int width) {
    for (int state = 0; state < table.StateCount(); ++state) {
        output << ' ' << std::setw(width) << FormatValue(table.Value(input, state));
    }
}

}  // namespace

void ValidateAutomaton(const Table& transitions, const Table& outputs) {
    if (transitions.InputCount() != outputs.InputCount() ||
        transitions.StateCount() != outputs.StateCount()) {
        throw std::invalid_argument("Transition and output tables must have the same size");
    }

    for (int input = 0; input < transitions.InputCount(); ++input) {
        for (int state = 0; state < transitions.StateCount(); ++state) {
            const int next = transitions.Value(input, state);
            if (next != kUndefinedValue && next > transitions.StateCount()) {
                throw std::invalid_argument(
                    "Transition f(x" + std::to_string(input + 1) + ", s" +
                    std::to_string(state + 1) + ") = " + std::to_string(next) +
                    " refers to a non-existent state");
            }
        }
    }
}

void PrintAutomaton(
    std::ostream& output,
    const Table& transitions,
    const Table& outputs,
    const std::string& statePrefix) {
    ValidateAutomaton(transitions, outputs);

    const int fWidth = CellWidth(transitions, statePrefix);
    const int gWidth = CellWidth(outputs, statePrefix);
    const std::string rowLabel(1 + std::to_string(transitions.InputCount()).size(), ' ');
    const int fTableWidth = (fWidth + 1) * transitions.StateCount();

    output << rowLabel << ' ' << std::left << std::setw(fTableWidth) << " f"
           << kTableGap << " g" << std::right << '\n';

    output << rowLabel;
    PrintHeader(output, transitions, statePrefix, fWidth);
    output << kTableGap;
    PrintHeader(output, outputs, statePrefix, gWidth);
    output << '\n';

    for (int input = 0; input < transitions.InputCount(); ++input) {
        output << std::left << std::setw(static_cast<int>(rowLabel.size()))
               << "x" + std::to_string(input + 1) << std::right;
        PrintRow(output, transitions, input, fWidth);
        output << kTableGap;
        PrintRow(output, outputs, input, gWidth);
        output << '\n';
    }
}
