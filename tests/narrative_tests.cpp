#include <kata3/narrative.hpp>
#include <cassert>
#include <string>
int main(){
 auto scene=kata3::parse("SCENE midnight\nWORLD = HAKUI\nLOCATION = TOWER\nMOOD = NOCTURNAL\nPRESERVE_CANON = TRUE\nPLAYER_AUTONOMY = TRUE\nACTOR ether PLAYER SEEK_TRUTH\nACTOR stranger NPC HIDE_SECRET\nEND\n");
 assert(scene);
 const std::string good="OBJECTIVE discover | Learn why the tower is closed\nDIALOGUE stranger | The doors will not open.\nCHOICE ask | Ask for the key | EVENT:ask_key\nCHOICE leave | Walk away | EVENT:leave_tower\n";
 auto parsed=kata3::parse_narrative(good,*scene.scene);assert(parsed);
 assert(parsed.narrative->choices.size()==2&&parsed.narrative->choices[0].consequence=="ask_key");
 assert(!kata3::parse_narrative("OBJECTIVE discover | Truth\nCHOICE ask | Ask | EVENT:ask\n",*scene.scene));
 assert(!kata3::parse_narrative(good+"CHOICE ask | Again | EVENT:again\n",*scene.scene));
 assert(!kata3::parse_narrative(good+"DIALOGUE ghost | Boo\n",*scene.scene));
 assert(!kata3::parse_narrative(good+"CHOICE x | Bypass | SET:canon\n",*scene.scene));
 assert(!kata3::parse_narrative(good+"OBJECTIVE another | Duplicate\n",*scene.scene));
 assert(!kata3::parse_narrative(good+"CHOICE x | Bad | EVENT:\n",*scene.scene));
 auto no_player=*scene.scene;for(auto& a:no_player.actors)a.role="NPC";assert(!kata3::parse_narrative(good,no_player));
}
