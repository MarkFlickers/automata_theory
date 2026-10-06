#pragma once

#include "Table.hpp"

#include <filesystem>
#include <string>

Table ReadTransitionTable(const std::filesystem::path& inputPath);
Table ReadOutputTable(const std::filesystem::path& inputPath);

std::string FormatValue(int value);
