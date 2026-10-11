#pragma once
#include <kata3/storage.hpp>
#include <filesystem>
#include <istream>
#include <ostream>
#include <string>
namespace kata3 {
// Returns 0 for a clean exit, 1 for input or storage failure, 2 for invalid assets.
// No automatic choice selection; no world-state writes.
int run_console(std::istream& input, std::ostream& output, std::ostream& errors,
                const std::filesystem::path& scene_file,
                const std::filesystem::path& narrative_file,
                const std::filesystem::path& save_file);
}
