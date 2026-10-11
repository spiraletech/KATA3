#pragma once
#include <string>
#include <vector>
#include <optional>
namespace kata3 {
struct Diagnostic { std::size_t line; std::string message; };
struct Actor { std::string id; std::string role; std::string motive; };
struct Scene { std::string id,world,location,mood; bool preserve_canon=false,player_autonomy=false; std::vector<Actor> actors; };
struct ParseResult { std::optional<Scene> scene; std::vector<Diagnostic> errors; explicit operator bool() const { return scene.has_value(); } };
ParseResult parse(const std::string& source);
}
