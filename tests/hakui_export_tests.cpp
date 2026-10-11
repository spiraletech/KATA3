#include <kata3/hakui_export.hpp>
#include <cassert>
#include <string>
#include <vector>
struct Host final : kata3::hakui::ReadOnlySceneSource {
 bool approved=true,player=true;
 std::vector<kata3::hakui::ActorView> actors{{"ether","PLAYER","EXPLORE"},{"guide","NPC","HELP"}};
 bool approved_for_narrative_read() const override {return approved;}
 bool player_controls_choices() const override {return player;}
 std::string scene_id() const override {return "tower";}
 std::string location_id() const override {return "DEFACTO";}
 std::string mood_id() const override {return "MIST";}
 std::size_t actor_count() const override {return actors.size();}
 std::optional<kata3::hakui::ActorView> actor(std::size_t i) const override {return actors[i];}
};
int main(){
 Host host;
 auto output=kata3::hakui::export_read_only(host);
 assert(output&&output.snapshot->actors.size()==2);
 auto imported=kata3::hakui::import_read_only(*output.snapshot);
 assert(imported && imported.scene->world=="HAKUI");
 output.snapshot->actors[0].id="CHANGED";
 assert(host.actors[0].id=="ether");
 host.approved=false;assert(!kata3::hakui::export_read_only(host));host.approved=true;
 host.player=false;assert(!kata3::hakui::export_read_only(host));host.player=true;
 host.actors[1].id="ether";assert(!kata3::hakui::export_read_only(host));
 host.actors[1].id="guide";host.actors.resize(257);assert(!kata3::hakui::export_read_only(host));
}
