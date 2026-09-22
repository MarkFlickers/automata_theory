#include "FileIO.hpp"
#include "Lab1Runner.hpp"
#include "StructSynthesis.hpp"

#include <cstddef>
#include <exception>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>

// Путь к lab1 задаётся в CMakeLists.txt
#ifndef LAB1_EXECUTABLE_PATH
#define LAB1_EXECUTABLE_PATH "lab1.exe"
#endif

namespace {

// Ожидаемый '-' совпадает с любым полученным значением.
bool Matches(int expected, int obtained) {
    return expected == kUndefinedValue || expected == obtained;
}

bool ReportColumn(
    char label,
    const std::vector<int>& expected,
    const std::vector<int>& obtained) {
    bool passed = true;

    if (expected.size() != obtained.size()) {
        std::cout << "  " << label << ": expected " << expected.size()
                  << " values, obtained " << obtained.size() << '\n';
        return false;
    }

    for (std::size_t index = 0; index < expected.size(); ++index) {
        if (!Matches(expected[index], obtained[index])) {
            std::cout << "  " << label << '[' << index + 1 << "]: expected "
                      << FormatValue(expected[index]) << ", obtained "
                      << FormatValue(obtained[index]) << '\n';
            passed = false;
        }
    }

    return passed;
}

bool ReportCheck(const CheckWord& expected, const CheckWord& obtained) {
    if (obtained.inputs != expected.inputs) {
        std::cout << "Check FAILED: obtained inputs differ from the check word\n";
        return false;
    }

    const bool statesPassed = ReportColumn('S', expected.states, obtained.states);
    const bool outputsPassed = ReportColumn('Y', expected.outputs, obtained.outputs);
    const bool passed = statesPassed && outputsPassed;

    std::cout << (passed ? "Check passed" : "Check FAILED") << '\n';
    return passed;
}

}  // namespace

int main(int argumentCount, char* arguments[]) {
    // u8: путь из CMake записан в UTF-8
    const std::filesystem::path lab1Path =
        argumentCount > 1 ? std::filesystem::path(arguments[1])
                          : std::filesystem::path(u8"" LAB1_EXECUTABLE_PATH);

    try {
        StructSynthesis synthesis(
            ReadTransitionTable("f_table.txt"),
            ReadOutputTable("g_table.txt"));

        std::cout << "[1/4] EncodeStates\n" << std::flush;
        synthesis.EncodeStates();
        synthesis.PrintEncoding(std::cout);
        std::cout << "[2/4] BuildSdnf\n" << std::flush;
        synthesis.BuildSdnf();
        std::cout << "Functions: " << synthesis.Functions().size() << '\n';
        for (const LogicFunction& function : synthesis.Functions()) {
            std::cout << "  " << function.name << ": scale length = " << function.scale.size() << '\n';
        }
        std::cout << "[3/4] Minimize using LR1\n" << std::flush;
        synthesis.Minimize(Lab1Runner(lab1Path, "lab1_io"));
        WriteOutput(synthesis.Functions(), "output.txt");

        std::cout << "[4/4] Check using MDNF\n" << std::flush;
        const CheckWord expected = ReadCheckWord("check_word.txt");
        const CheckWord obtained = synthesis.Check(expected);
        if (obtained.inputs.empty()) {
            throw std::logic_error("Check is not implemented: StructSynthesis::Check returned no data");
        }
        WriteCheck(obtained, "check.txt");

        return ReportCheck(expected, obtained) ? 0 : 1;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
