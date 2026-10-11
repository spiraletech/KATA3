#include <kata3/generation.hpp>
#include <cassert>
#include <string>
int main(){
 const auto scene=kata3::parse("SCENE midnight\nWORLD = HAKUI\nLOCATION = TOWER\nMOOD = NOCTURNAL\nPRESERVE_CANON = TRUE\nPLAYER_AUTONOMY = TRUE\nACTOR ether PLAYER SEEK_TRUTH\nACTOR stranger NPC HIDE_SECRET\nEND\n");
 assert(scene);
 const std::string template_text="OBJECTIVE discover | Find the key\nCHOICE ask | Ask stranger | EVENT:ask_key\nCHOICE leave | Walk away | EVENT:leave_tower\n";
 const auto contract=kata3::parse_narrative(template_text,*scene.scene);assert(contract);
 auto req=kata3::compile_generation_request(*scene.scene,*contract.narrative);
 assert(req.prompt.find("player")!=std::string::npos);
 const auto good=template_text+"DIALOGUE stranger | The key is hidden.\n";
 auto accepted=kata3::review_generation(good,*scene.scene,*contract.narrative);
 assert(accepted.accepted && accepted.narrative.dialogue.size()==1);
 assert(!kata3::review_generation(template_text+"DIALOGUE ether | I agree.\n",*scene.scene,*contract.narrative).accepted);
 auto change=template_text;change.replace(change.find("EVENT:ask_key"),13,"EVENT:kill_npc");
 assert(!kata3::review_generation(change,*scene.scene,*contract.narrative).accepted);
 auto reword=template_text;reword.replace(reword.find("Find the key"),12,"Kill the NPC");
 assert(!kata3::review_generation(reword,*scene.scene,*contract.narrative).accepted);
 auto swap=std::string("OBJECTIVE discover | Find the key\nCHOICE leave | Walk away | EVENT:leave_tower\nCHOICE ask | Ask stranger | EVENT:ask_key\n");
 assert(!kata3::review_generation(swap,*scene.scene,*contract.narrative).accepted);
 auto unknown=good+"DIALOGUE ghost | Boo.\n";
 assert(!kata3::review_generation(unknown,*scene.scene,*contract.narrative).accepted);
 auto longtext=std::string(70000,'x');assert(!kata3::review_generation(longtext,*scene.scene,*contract.narrative).accepted);

 // A generation response cannot replace a predefined choice with a new option.
 auto substituted=template_text;
 substituted.replace(substituted.find("CHOICE leave | Walk away | EVENT:leave_tower"),
                    std::string("CHOICE leave | Walk away | EVENT:leave_tower").size(),
                    "CHOICE flee | Flee | EVENT:flee_tower");
 assert(!kata3::review_generation(substituted,*scene.scene,*contract.narrative).accepted);
 // A model may supply several NPC lines, but the exact choices remain fixed.
 auto many_dialogue=template_text+"DIALOGUE stranger | First warning.\nDIALOGUE stranger | Second warning.\n";
 assert(kata3::review_generation(many_dialogue,*scene.scene,*contract.narrative).accepted);
 // Explicitly disabled scene authority must reject otherwise valid output.
 auto canon_off=*scene.scene;canon_off.preserve_canon=false;
 assert(!kata3::review_generation(good,canon_off,*contract.narrative).accepted);
 auto autonomy_off=*scene.scene;autonomy_off.player_autonomy=false;
 assert(!kata3::review_generation(good,autonomy_off,*contract.narrative).accepted);

}
