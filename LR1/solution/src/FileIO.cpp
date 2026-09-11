#include "FileIO.hpp"

#include "DNF.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

std::string ReadScale(const std::filesystem::path& inputPath) {
    std::ifstream input(inputPath);
    if (!input.is_open()) {
        throw std::runtime_error("Cannot open input file: " + inputPath.string());
    }

    std::ostringstream contents;
    contents << input.rdbuf();

    if (input.bad()) {
        throw std::runtime_error("Cannot read input file: " + inputPath.string());
    }

    return contents.str();
}

void WriteMdnf(const DNF& dnf, const std::filesystem::path& outputPath) {
    std::ofstream output(outputPath);
    if (!output.is_open()) {
        throw std::runtime_error("Cannot open output file: " + outputPath.string());
    }

    dnf.Print(output);
    output.flush();

    if (!output) {
        throw std::runtime_error("Cannot write output file: " + outputPath.string());
    }
}
