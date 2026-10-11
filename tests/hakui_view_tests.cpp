#include <kata3/hakui_view.hpp>
#include <kata3/narrative.hpp>
#include <kata3/runtime.hpp>
#include <cassert>
int main(){
 using namespace kata3;
 hakui::Snapshot host{"dock","HAKUI","WATERFRONT","MIST",true,true,
   {{"ether","PLAYER","EXPLORE"},{"guide","NPC","HELP"}}};
 const auto pristine=host;
 auto result=hakui::import_read_only(host);
 assert(result && result.scene->actors.size()==2 && result.scene->player_autonomy);
 assert(host.scene_id==pristine.scene_id && host.actors.size()==pristine.actors.size());
 const auto narrative=parse_narrative("OBJECTIVE find | Find the key\nCHOICE ask | Ask | EVENT:ask_key\nCHOICE leave | Leave | EVENT:leave\n",*result.scene);
 assert(narrative);
 NarrativeState ledger;
 assert(ledger.history.empty());
 auto bad=host;bad.canon_approved=false;assert(!hakui::import_read_only(bad));
 bad=host;bad.player_input_required=false;assert(!hakui::import_read_only(bad));
 bad=host;bad.actors[1].id="ether";assert(!hakui::import_read_only(bad));
 bad=host;bad.world="HOME";assert(!hakui::import_read_only(bad));
 bad=host;bad.actors[0].role="NPC";assert(!hakui::import_read_only(bad));
 bad=host;bad.location="invalid name";assert(!hakui::import_read_only(bad));
}
