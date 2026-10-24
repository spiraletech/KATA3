#pragma once
#include <kata3/scene.hpp>
#include <algorithm>
#include <optional>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>
namespace kata3::hakui {
// Copy of observed host metadata; no mutating callbacks or game-engine dependencies.
struct ActorView { std::string id; std::string role; std::string motive; };
struct Snapshot {
 std::string scene_id, world, location, mood;
 bool canon_approved=false, player_input_required=false;
 std::vector<ActorView> actors;
};
struct ImportResult {
 std::optional<kata3::Scene> scene;
 std::vector<kata3::Diagnostic> diagnostics;
 explicit operator bool() const noexcept {return scene.has_value();}
};
inline ImportResult import_read_only(const Snapshot& snapshot) {
 ImportResult out;
 auto identifier=[](const std::string& s){
  if(s.empty()||s.size()>80)return false;
  return std::all_of(s.begin(),s.end(),[](unsigned char c){
   return (c>='A'&&c<='Z')||(c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='_'||c=='-'||c=='.';
  });
 };
 auto invalid=[&](const std::string& s,const char* what){
  if(!identifier(s))out.diagnostics.push_back({0,std::string("invalid ")+what});
 };
 invalid(snapshot.scene_id,"scene id");invalid(snapshot.world,"world");
 invalid(snapshot.location,"location");invalid(snapshot.mood,"mood");
 if(snapshot.world!="HAKUI")out.diagnostics.push_back({0,"snapshot must declare HAKUI world"});
 if(!snapshot.canon_approved||!snapshot.player_input_required)
  out.diagnostics.push_back({0,"host must explicitly confirm canon and player-input authority"});
 if(snapshot.actors.empty()||snapshot.actors.size()>256)
  out.diagnostics.push_back({0,"actor count must be 1..256"});
 std::size_t player_count=0;std::unordered_set<std::string> ids;
 kata3::Scene scene;
 scene.id=snapshot.scene_id;scene.world=snapshot.world;
 scene.location=snapshot.location;scene.mood=snapshot.mood;
 scene.preserve_canon=snapshot.canon_approved;
 scene.player_autonomy=snapshot.player_input_required;
 for(const auto& a:snapshot.actors) {
  invalid(a.id,"actor id");invalid(a.motive,"actor motive");
  if(a.role!="PLAYER"&&a.role!="NPC")
   out.diagnostics.push_back({0,"actor role must be PLAYER or NPC"});
  if(!ids.insert(a.id).second)out.diagnostics.push_back({0,"duplicate actor id"});
  if(a.role=="PLAYER")++player_count;
  scene.actors.push_back({a.id,a.role,a.motive});
 }
 if(player_count==0)out.diagnostics.push_back({0,"snapshot requires a player actor"});
 if(player_count>1)out.diagnostics.push_back({0,"snapshot requires exactly one player actor"});
 if(out.diagnostics.empty())out.scene=std::move(scene);
 return out;
}
} // namespace kata3::hakui
