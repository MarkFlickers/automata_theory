#pragma once

#include "CheckWord.hpp"
#include "LogicFunction.hpp"
#include "Table.hpp"

#include <filesystem>
#include <string>
#include <vector>

Table ReadTransitionTable(const std::filesystem::path& inputPath);
Table ReadOutputTable(const std::filesystem::path& inputPath);
CheckWord ReadCheckWord(const std::filesystem::path& inputPath);

void WriteScale(const std::string& scale, const std::filesystem::path& outputPath);
std::vector<std::string> ReadMdnf(const std::filesystem::path& inputPath);

void WriteOutput(const std::vector<LogicFunction>& functions, const std::filesystem::path& outputPath);
void WriteCheck(const CheckWord& word, const std::filesystem::path& outputPath);

std::string FormatValue(int value);
