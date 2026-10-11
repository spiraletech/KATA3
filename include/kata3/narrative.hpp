#pragma once
#include <kata3/scene.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
namespace kata3 {
struct Dialogue { std::string actor; std::string text; };
struct Choice { std::string id; std::string label; std::string consequence; };
struct Narrative { std::string objective_id; std::string objective; std::vector<Dialogue> dialogue; std::vector<Choice> choices; };
struct NarrativeResult { std::optional<Narrative> narrative; std::vector<Diagnostic> errors; explicit operator bool() const noexcept {return narrative.has_value();} };
// Sidecar parser: does not mutate scene or select a player choice.
NarrativeResult parse_narrative(std::string_view source, const Scene& scene);
}
