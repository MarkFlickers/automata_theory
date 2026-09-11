#pragma once

#include <filesystem>
#include <string>

class DNF;

std::string ReadScale(const std::filesystem::path& inputPath);
void WriteMdnf(const DNF& dnf, const std::filesystem::path& outputPath);
