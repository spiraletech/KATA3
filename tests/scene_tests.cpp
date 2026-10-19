#include <kata3/scene.hpp>
#include <cassert>
#include <string>
int main(){
 const std::string valid="SCENE midnight\nWORLD = HAKUI\nLOCATION = DEFACTO_TOWER\nMOOD = NOCTURNAL\nPRESERVE_CANON = TRUE\nPLAYER_AUTONOMY = TRUE\nACTOR ether PLAYER SEEK_TRUTH\nACTOR stranger NPC HIDE_SECRET\nEND\n";
 auto result=kata3::parse(valid);assert(result);assert(result.scene->actors.size()==2);assert(result.scene->location=="DEFACTO_TOWER");
 auto broken=valid;broken.replace(broken.find("PLAYER_AUTONOMY = TRUE"),22,"PLAYER_AUTONOMY = FALSE");assert(!kata3::parse(broken));
 assert(!kata3::parse(valid+"WORLD = OTHER\n"));
 auto duplicate=valid;duplicate.insert(duplicate.find("END"),"ACTOR ether NPC LIE\n");assert(!kata3::parse(duplicate));
 auto unknown=valid;unknown.insert(unknown.find("END"),"MAGIC = YES\n");assert(!kata3::parse(unknown));
 assert(!kata3::parse("SCENE x\nEND\n"));
 // Enforce player authority at scene compile time, not only during narrative review.
 const std::string header="SCENE dock\nWORLD = HAKUI\nLOCATION = TOWER\nMOOD = MIST\nPRESERVE_CANON = TRUE\nPLAYER_AUTONOMY = TRUE\n";
 assert(kata3::parse(header+"ACTOR ether PLAYER EXPLORE\nEND\n"));
 assert(kata3::parse(header+"ACTOR ether PLAYER EXPLORE\nACTOR guide NPC HELP\nEND\n"));
 assert(!kata3::parse(header+"ACTOR guide NPC HELP\nEND\n"));
 assert(!kata3::parse(header+"ACTOR ether SPECTATOR EXPLORE\nEND\n"));
 assert(!kata3::parse(header+"ACTOR ether PLAYER EXPLORE\nACTOR guide ADMIN CONTROL\nEND\n"));

}
