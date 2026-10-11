#pragma once
#include <kata3/hakui_view.hpp>
#include <cstddef>
#include <optional>
#include <string>
namespace kata3::hakui {
// Host-owned, read-only API; no world mutation or actor pointers.
class ReadOnlySceneSource {
public:
 virtual ~ReadOnlySceneSource() = default;
 virtual bool approved_for_narrative_read() const = 0;
 virtual bool player_controls_choices() const = 0;
 virtual std::string scene_id() const = 0;
 virtual std::string location_id() const = 0;
 virtual std::string mood_id() const = 0;
 virtual std::size_t actor_count() const = 0;
 virtual std::optional<ActorView> actor(std::size_t index) const = 0;
};
struct ExportResult {
 std::optional<Snapshot> snapshot;
 std::vector<kata3::Diagnostic> diagnostics;
 explicit operator bool() const noexcept {return snapshot.has_value();}
};
inline ExportResult export_read_only(const ReadOnlySceneSource& host) {
 ExportResult result;
 if(!host.approved_for_narrative_read() || !host.player_controls_choices()){
  result.diagnostics.push_back({0,"host has not authorized canon and player-input boundaries"});
  return result;
 }
 const auto count=host.actor_count();
 if(count==0 || count>256){
  result.diagnostics.push_back({0,"actor count exceeds snapshot bounds"});
  return result;
 }
 Snapshot snapshot;
 snapshot.scene_id=host.scene_id();snapshot.world="HAKUI";
 snapshot.location=host.location_id();snapshot.mood=host.mood_id();
 snapshot.canon_approved=true;snapshot.player_input_required=true;
 snapshot.actors.reserve(count);
 for(std::size_t i=0;i<count;++i){
  auto observation=host.actor(i);
  if(!observation){
   result.diagnostics.push_back({0,"host failed to supply actor observation"});
   return result;
  }
  snapshot.actors.push_back(std::move(*observation));
 }
 auto imported=import_read_only(snapshot);
 if(!imported){result.diagnostics=std::move(imported.diagnostics);return result;}
 result.snapshot=std::move(snapshot);return result;
}
} // namespace kata3::hakui
