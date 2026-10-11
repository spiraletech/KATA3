#include <kata3/runtime.hpp>
#include <cassert>
int main(){
 kata3::Scene s; s.id="a";s.preserve_canon=true;s.player_autonomy=true;s.actors.push_back({"ether","PLAYER","SEEK"});
 kata3::Narrative n;n.objective_id="discover";n.choices={{"ask","Ask","ask_key"},{"leave","Leave","leave_tower"}};
 kata3::NarrativeState state;
 assert(state.history.empty());
 auto invalid=kata3::NarrativeRuntime::select(s,n,state,"invented",0);assert(!invalid && state.history.empty());
 auto first=kata3::NarrativeRuntime::select(s,n,state,"ask",0);assert(first && first.emitted->event_id=="ask_key");
 assert(!kata3::NarrativeRuntime::select(s,n,state,"leave",0));assert(state.history.size()==1);
 auto second=kata3::NarrativeRuntime::select(s,n,state,"leave",1);assert(second && state.history.size()==2);
 auto replay=kata3::NarrativeRuntime::replay(s,n,state.history);assert(replay && replay->history.size()==2);
 auto bad=state.history;bad[0].event_id="forged";assert(!kata3::NarrativeRuntime::replay(s,n,bad));
 bad=state.history;bad[1].sequence=90;assert(!kata3::NarrativeRuntime::replay(s,n,bad));
 auto canon=s;canon.preserve_canon=false;assert(!kata3::NarrativeRuntime::select(canon,n,state,"ask",2));
 auto autonomy=s;autonomy.player_autonomy=false;assert(!kata3::NarrativeRuntime::select(autonomy,n,state,"ask",2));
 auto corrupt=state;corrupt.history[0].choice_id="unapproved";assert(!kata3::NarrativeRuntime::select(s,n,corrupt,"ask",2));
 auto empty=kata3::NarrativeRuntime::replay(s,n,{});
 assert(empty && empty->history.empty());
 auto duplicated=n;duplicated.choices.push_back(duplicated.choices.front());
 assert(!kata3::NarrativeRuntime::select(s,duplicated,state,"ask",2));
 auto reordered=state.history;std::swap(reordered[0],reordered[1]);
 assert(!kata3::NarrativeRuntime::replay(s,n,reordered));
 auto original_size=state.history.size();
 assert(!kata3::NarrativeRuntime::select(s,n,state,"not_offered",original_size));
 assert(state.history.size()==original_size);

 // Check deterministic bounded replay at the documented history limit.
 std::vector<kata3::ChoiceEvent> long_log;
 for(std::uint64_t i=1;i<=10000;++i)long_log.push_back({i,"ask","ask_key"});
 auto large=kata3::NarrativeRuntime::replay(s,n,long_log);
 assert(large && large->history.size()==10000);
 assert(!kata3::NarrativeRuntime::select(s,n,*large,"leave",10000));
 long_log.push_back({10001,"ask","ask_key"});
 assert(!kata3::NarrativeRuntime::replay(s,n,long_log));

}
