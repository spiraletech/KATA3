#include <kata3/narrative.hpp>
#include <cassert>
#include <cstddef>
#include <string>
#include <string_view>

int main() {
  kata3::Scene scene;
  scene.id = "dock";
  scene.world = "HAKUI";
  scene.location = "TOWER";
  scene.mood = "NIGHT";
  scene.preserve_canon = true;
  scene.player_autonomy = true;
  scene.actors.push_back({"ether", "PLAYER", "OBSERVE"});
  scene.actors.push_back({"guide", "NPC", "ASSIST"});

  const std::string good =
      "OBJECTIVE explore | Explore the tower\n"
      "DIALOGUE guide | You can decide what happens next.\n"
      "CHOICE stay | Stay here | EVENT:stay\n"
      "CHOICE go | Walk away | EVENT:go\n";
  assert(kata3::parse_narrative(good, scene));

  constexpr std::size_t max_bytes = 64u * 1024u;
  // A long legal comment exercises the exact accepted byte boundary.
  std::string exact = "#" + std::string(max_bytes - good.size() - 2, 'x') + "\n" + good;
  assert(exact.size() == max_bytes);
  assert(kata3::parse_narrative(exact, scene));

  // Before the fix, this was accepted despite the reviewer's 64 KiB limit.
  std::string over = exact + "#";
  auto rejected = kata3::parse_narrative(over, scene);
  assert(!rejected);
  assert(!rejected.errors.empty());
  assert(rejected.errors.front().line == 0);
  assert(rejected.errors.front().message == "narrative source exceeds 64 KiB");

  // A multi-megabyte comment must be rejected before line tokenization.
  std::string huge = "#" + std::string(2u * 1024u * 1024u, 'x') + "\n" + good;
  auto large = kata3::parse_narrative(huge, scene);
  assert(!large);
  assert(large.errors.size() == 1);
  assert(large.errors.front().line == 0);

  // A non-null-terminated view must be bounded by its explicit length.
  std::string backing = good + std::string(max_bytes, 'z');
  assert(kata3::parse_narrative(std::string_view(backing.data(), good.size()), scene));
}
