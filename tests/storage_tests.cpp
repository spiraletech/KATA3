#include <kata3/storage.hpp>
#include <cassert>
#include <filesystem>
int main(){
 using namespace kata3;
 Scene s;s.id="scene_1";s.preserve_canon=true;s.player_autonomy=true;s.actors.push_back({"hero","PLAYER","EXPLORE"});
 Narrative n;n.objective_id="find_exit";n.choices={{"wait","Wait","wait_event"},{"go","Go","go_event"}};
 NarrativeState state;
 assert(NarrativeRuntime::select(s,n,state,"go",0));
 assert(NarrativeRuntime::select(s,n,state,"wait",1));
 auto bytes=NarrativeStorage::encode(s,n,state);assert(bytes);
 auto decoded=NarrativeStorage::decode(*bytes,s,n);assert(decoded&&decoded->history.size()==2);
 auto corrupt=*bytes;corrupt[corrupt.size()-1]^=1;assert(!NarrativeStorage::decode(corrupt,s,n));
 assert(!NarrativeStorage::decode(bytes->substr(0,bytes->size()-1),s,n));
 auto other=s;other.id="another";assert(!NarrativeStorage::decode(*bytes,other,n));
 auto changed=n;changed.choices[1].consequence="wrong";assert(!NarrativeStorage::decode(*bytes,s,changed));
 auto saved=std::filesystem::temp_directory_path()/"kata3_c42_test_ledger.bin";
 std::error_code ec;std::filesystem::remove(saved,ec);
 assert(NarrativeStorage::save(saved,s,n,state));
 NarrativeState receiver;assert(NarrativeStorage::load(saved,s,n,receiver));assert(receiver.history.size()==2);
 auto bad=receiver;bad.history[0].event_id="FORGED";assert(!NarrativeStorage::save(saved,s,n,bad));
 auto before=receiver.history.size();assert(!NarrativeStorage::load(saved,other,n,receiver));assert(receiver.history.size()==before);
 std::filesystem::remove(saved,ec);
}
