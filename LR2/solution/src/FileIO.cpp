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

std::ofstream OpenOutput(const std::filesystem::path& outputPath) {
    std::ofstream output(outputPath);
    if (!output.is_open()) {
        throw std::runtime_error("Cannot open output file: " + outputPath.string());
    }

    return output;
}

void FinishOutput(std::ofstream& output, const std::filesystem::path& outputPath) {
    output.flush();

    if (!output) {
        throw std::runtime_error("Cannot write output file: " + outputPath.string());
    }
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

void ValidateCheckWord(const CheckWord& word, const std::string& source) {
    if (word.inputs.empty()) {
        throw std::invalid_argument("Check word has no inputs: " + source);
    }

    if (word.states.size() != word.inputs.size() + 1) {
        throw std::invalid_argument(
            "Check word must contain exactly one state more than inputs: " + source);
    }

    if (word.outputs.size() != word.inputs.size()) {
        throw std::invalid_argument(
            "Check word must contain as many outputs as inputs: " + source);
    }
}

std::size_t ColumnWidth(const std::vector<int>& column) {
    std::size_t width = 1;
    for (int value : column) {
        width = std::max(width, FormatValue(value).size());
    }

    return width;
}

std::string FormatCell(const std::vector<int>& column, std::size_t row, std::size_t width) {
    std::string cell = row < column.size() ? FormatValue(column[row]) : std::string();
    cell.resize(width, ' ');
    return cell;
}

}  // namespace

Table ReadTransitionTable(const std::filesystem::path& inputPath) {
    return ReadTable<ItemState>(inputPath);
}

Table ReadOutputTable(const std::filesystem::path& inputPath) {
    return ReadTable<ItemOutput>(inputPath);
}

// Формат: строки "X ...", "S ...", "Y ..."; значения через пробел или запятую.
CheckWord ReadCheckWord(const std::filesystem::path& inputPath) {
    CheckWord word;

    for (const std::string& line : ReadLines(inputPath)) {
        const std::vector<std::string> tokens = SplitTokens(line);
        if (tokens.empty()) {
            continue;
        }

        std::vector<int> values;
        for (auto token = tokens.begin() + 1; token != tokens.end(); ++token) {
            values.push_back(ParseValue(*token, inputPath));
        }

        const std::string& label = tokens.front();
        if (label == "X" || label == "x") {
            word.inputs = std::move(values);
        } else if (label == "S" || label == "s") {
            word.states = std::move(values);
        } else if (label == "Y" || label == "y") {
            word.outputs = std::move(values);
        } else {
            throw std::invalid_argument(
                "Unknown line label '" + label + "' in " + inputPath.string() +
                " (expected X, S or Y)");
        }
    }

    ValidateCheckWord(word, inputPath.string());

    const bool hasUndefinedInput =
        std::find(word.inputs.begin(), word.inputs.end(), kUndefinedValue) != word.inputs.end();
    if (hasUndefinedInput || word.states.front() == kUndefinedValue) {
        throw std::invalid_argument(
            "Inputs and the initial state of the check word must be defined: " +
            inputPath.string());
    }

    return word;
}

void WriteScale(const std::string& scale, const std::filesystem::path& outputPath) {
    std::ofstream output = OpenOutput(outputPath);
    output << scale << '\n';
    FinishOutput(output, outputPath);
}

std::vector<std::string> ReadMdnf(const std::filesystem::path& inputPath) {
    std::vector<std::string> implicants;

    for (std::string line : ReadLines(inputPath)) {
        std::erase_if(line, [](unsigned char symbol) { return std::isspace(symbol) != 0; });
        if (line.empty()) {
            continue;
        }

        if (line.find_first_not_of("01-") != std::string::npos) {
            throw std::invalid_argument(
                "Invalid implicant '" + line + "' in " + inputPath.string());
        }

        implicants.push_back(std::move(line));
    }

    return implicants;
}

void WriteOutput(const std::vector<LogicFunction>& functions, const std::filesystem::path& outputPath) {
    std::ofstream output = OpenOutput(outputPath);

    for (std::size_t index = 0; index < functions.size(); ++index) {
        if (index > 0) {
            output << '\n';
        }

        output << functions[index].name << ":\n";
        for (const std::string& implicant : functions[index].mdnf) {
            output << implicant << '\n';
        }
    }

    FinishOutput(output, outputPath);
}

// Три столбца: входы, состояния, выходы. Состояний на одно больше,
// поэтому в последней строке заполнен только средний столбец.
void WriteCheck(const CheckWord& word, const std::filesystem::path& outputPath) {
    ValidateCheckWord(word, "check result");

    const std::size_t inputWidth = ColumnWidth(word.inputs);
    const std::size_t stateWidth = ColumnWidth(word.states);
    const std::size_t outputWidth = ColumnWidth(word.outputs);

    std::ofstream output = OpenOutput(outputPath);

    for (std::size_t row = 0; row < word.states.size(); ++row) {
        std::string line =
            FormatCell(word.inputs, row, inputWidth) + ' ' +
            FormatCell(word.states, row, stateWidth) + ' ' +
            FormatCell(word.outputs, row, outputWidth);
        line.erase(line.find_last_not_of(' ') + 1);

        output << line << '\n';
    }

    FinishOutput(output, outputPath);
}

std::string FormatValue(int value) {
    return value == kUndefinedValue ? "-" : std::to_string(value);
}
