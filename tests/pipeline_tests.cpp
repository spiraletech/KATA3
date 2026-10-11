#include <kata3/scene.hpp>
#include <kata3/narrative.hpp>
#include <kata3/generation.hpp>
#include <kata3/runtime.hpp>
#include <kata3/storage.hpp>
#include <cassert>
int main(){
 using namespace kata3;
 const auto scene=parse("SCENE bay\nWORLD = HAKUI\nLOCATION = DOCK\nMOOD = RAIN\nPRESERVE_CANON = TRUE\nPLAYER_AUTONOMY = TRUE\nACTOR hero PLAYER EXPLORE\nACTOR guide NPC HELP\nEND\n");
 assert(scene);
 const std::string approved="OBJECTIVE find | Find the dock\nCHOICE ask | Ask guide | EVENT:ask_guide\nCHOICE leave | Leave dock | EVENT:leave_dock\n";
 auto contract=parse_narrative(approved,*scene.scene);assert(contract);
 auto prompt=compile_generation_request(*scene.scene,*contract.narrative);assert(!prompt.prompt.empty());
 auto generated=review_generation(approved+"DIALOGUE guide | Go north.\n",*scene.scene,*contract.narrative);
 assert(generated.accepted);
 NarrativeState state;assert(state.history.empty());
 assert(NarrativeRuntime::select(*scene.scene,*contract.narrative,state,"ask",0));
 auto snapshot=NarrativeStorage::encode(*scene.scene,*contract.narrative,state);assert(snapshot);
 auto loaded=NarrativeStorage::decode(*snapshot,*scene.scene,*contract.narrative);
 assert(loaded && loaded->history.size()==1 && loaded->history[0].event_id=="ask_guide");
 assert(!NarrativeRuntime::select(*scene.scene,*contract.narrative,*loaded,"invented",1));
 assert(loaded->history.size()==1);
 auto bad_scene=*scene.scene;bad_scene.player_autonomy=false;
 assert(!NarrativeStorage::decode(*snapshot,bad_scene,*contract.narrative));
 auto changed=*contract.narrative;changed.choices[0].consequence="other";
 assert(!NarrativeStorage::decode(*snapshot,*scene.scene,changed));
}
