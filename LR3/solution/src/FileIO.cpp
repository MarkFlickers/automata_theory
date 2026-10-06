#include "FileIO.hpp"

#include "ItemOutput.hpp"
#include "ItemState.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

std::vector<std::string> ReadLines(const std::filesystem::path& inputPath) {
    std::ifstream input(inputPath);
    if (!input.is_open()) {
        throw std::runtime_error("Cannot open input file: " + inputPath.string());
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(input, line)) {
        // Блокнот может сохранить файл в UTF-8 с BOM
        if (lines.empty() && line.starts_with("\xEF\xBB\xBF")) {
            line.erase(0, 3);
        }
        lines.push_back(std::move(line));
    }

    if (input.bad()) {
        throw std::runtime_error("Cannot read input file: " + inputPath.string());
    }

    return lines;
}

std::vector<std::string> SplitTokens(std::string line) {
    std::replace(line.begin(), line.end(), ',', ' ');

    std::istringstream stream(line);
    std::vector<std::string> tokens;
    std::string token;
    while (stream >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

int ParseValue(const std::string& token, const std::filesystem::path& inputPath) {
    if (token == "-") {
        return kUndefinedValue;
    }

    const bool isNumber = !token.empty() && token.size() <= 9 &&
        std::all_of(token.begin(), token.end(), [](unsigned char symbol) {
            return std::isdigit(symbol) != 0;
        });
    const int value = isNumber ? std::stoi(token) : 0;

    if (value <= 0) {
        throw std::invalid_argument(
            "Invalid value '" + token + "' in " + inputPath.string() +
            " (expected a positive number or '-')");
    }

    return value;
}

template <typename Item>
Table ReadTable(const std::filesystem::path& inputPath) {
    std::vector<Table::Row> rows;

    for (const std::string& line : ReadLines(inputPath)) {
        Table::Row row;
        for (const std::string& token : SplitTokens(line)) {
            row.push_back(std::make_unique<Item>(ParseValue(token, inputPath)));
        }

        if (!row.empty()) {
            rows.push_back(std::move(row));
        }
    }

    try {
        return Table(std::move(rows));
    } catch (const std::invalid_argument& error) {
        throw std::invalid_argument(std::string(error.what()) + ": " + inputPath.string());
    }
}

}  // namespace

Table ReadTransitionTable(const std::filesystem::path& inputPath) {
    return ReadTable<ItemState>(inputPath);
}

Table ReadOutputTable(const std::filesystem::path& inputPath) {
    return ReadTable<ItemOutput>(inputPath);
}

std::string FormatValue(int value) {
    return value == kUndefinedValue ? "-" : std::to_string(value);
}
