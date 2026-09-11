#include "DNF.hpp"
#include "FileIO.hpp"

#include <exception>
#include <filesystem>
#include <iostream>

int main(int argumentCount, char* arguments[]) {
    const std::filesystem::path inputPath =
        argumentCount > 1 ? arguments[1] : "scale.txt";
    const std::filesystem::path outputPath =
        argumentCount > 2 ? arguments[2] : "mdnf.txt";

    try {
        DNF dnf(ReadScale(inputPath));
        dnf.Minimize();
        WriteMdnf(dnf, outputPath);
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }

    return 0;
}
