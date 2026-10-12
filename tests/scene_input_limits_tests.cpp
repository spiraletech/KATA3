#include <kata3/scene.hpp>
#include <cassert>
#include <iostream>
#include <string>

int main() {
 constexpr std::size_t max_bytes = 1024u * 1024u;
 const std::string valid =
  "SCENE midnight\nWORLD = HAKUI\nLOCATION = DEFACTO_TOWER\nMOOD = NOCTURNAL\n"
  "PRESERVE_CANON = TRUE\nPLAYER_AUTONOMY = TRUE\n"
  "ACTOR ether PLAYER SEEK_TRUTH\nACTOR stranger NPC HIDE_SECRET\nEND\n";
 assert(kata3::parse(valid));

 // A NUL in an otherwise ignored comment must not pass as canonical text.
 std::string before = "# preface";
 before.push_back('\0');
 before += "hidden\n" + valid;
 const auto bad_before = kata3::parse(before);
 assert(!bad_before && !bad_before.errors.empty());
 assert(bad_before.errors.front().line == 0);

 std::string after = valid + "# epilogue";
 after.push_back('\0');
 after += "hidden\n";
 const auto bad_after = kata3::parse(after);
 assert(!bad_after && !bad_after.errors.empty());
 assert(bad_after.errors.front().line == 0);

 // Exactly 1 MiB of valid scene/comment text is permitted.
 std::string at_limit = valid + "#";
 assert(at_limit.size() < max_bytes);
 at_limit.append(max_bytes - at_limit.size() - 1, 'x');
 at_limit.push_back('\n');
 assert(at_limit.size() == max_bytes);
 assert(kata3::parse(at_limit));

 // One byte beyond the bound remains syntactically valid but must be rejected.
 std::string over_limit = at_limit;
 over_limit.insert(over_limit.size() - 1, 1, 'x');
 assert(over_limit.size() == max_bytes + 1);
 const auto oversized = kata3::parse(over_limit);
 assert(!oversized && !oversized.errors.empty());
 assert(oversized.errors.front().line == 0);

 // Source-level diagnostics are deterministic and cannot grant a scene.
 const auto again = kata3::parse(over_limit);
 assert(!again && again.errors.size() == oversized.errors.size());
 assert(again.errors.front().message == oversized.errors.front().message);
 std::cout << "C6 source bounds and NUL rejection passed\n";
}
