#pragma once
#include <kata3/narrative.hpp>
#include <string>
#include <string_view>
#include <vector>
namespace kata3 {
struct GenerationRequest { std::string prompt; };
struct GenerationReview { bool accepted{}; std::vector<Diagnostic> diagnostics; Narrative narrative; };
// Pure transformation, no provider calls or scene mutations.
GenerationRequest compile_generation_request(const Scene& scene, const Narrative& contract);
// Strictly validate model output against the supplied contract.
GenerationReview review_generation(std::string_view proposed, const Scene& scene, const Narrative& contract);
}
