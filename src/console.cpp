#include <kata3/console.hpp>
#include <kata3/scene.hpp>
#include <kata3/narrative.hpp>
#include <fstream>
#include <iterator>
#include <string>
namespace kata3 {
namespace {
bool read_text(const std::filesystem::path& p,std::string& value) {
 std::ifstream in(p,std::ios::binary);
 if(!in)return false;
 value.assign(std::istreambuf_iterator<char>(in),{});
 return !in.bad() && value.size()<=65536;
}
}
int run_console(std::istream& input,std::ostream& output,std::ostream& errors,
 const std::filesystem::path& scene_file,const std::filesystem::path& narrative_file,
 const std::filesystem::path& save_file) {
 std::string scene_text,narrative_text;
 if(!read_text(scene_file,scene_text)||!read_text(narrative_file,narrative_text)) {
  errors<<"Unable to read scene or narrative assets.\n";return 2;
 }
 auto scene=parse(scene_text);
 if(!scene) {for(const auto& d:scene.errors)errors<<"Scene line "<<d.line<<": "<<d.message<<"\n";return 2;}
 auto narrative=parse_narrative(narrative_text,*scene.scene);
 if(!narrative) {for(const auto& d:narrative.errors)errors<<"Narrative line "<<d.line<<": "<<d.message<<"\n";return 2;}
 NarrativeState state;
 std::error_code ec;
 if(std::filesystem::exists(save_file,ec)) {
  if(!NarrativeStorage::load(save_file,*scene.scene,*narrative.narrative,state)){
   errors<<"Existing save failed validation; refusing overwrite.\n";return 1;
  }
 }
 output<<"KATA3: "<<scene.scene->id<<" at "<<scene.scene->location<<"\n";
 output<<"Objective: "<<narrative.narrative->objective<<"\n";
 output<<"Previously recorded choices: "<<state.history.size()<<"\n";
 for(const auto& d:narrative.narrative->dialogue)output<<d.actor<<": "<<d.text<<"\n";
 output<<"Available player choices:\n";
 for(const auto& choice:narrative.narrative->choices)output<<choice.id<<" - "<<choice.label<<"\n";
 output<<"Enter a choice ID, or quit: ";
 std::string selected;
 if(!std::getline(input,selected)){errors<<"No player input received.\n";return 1;}
 if(selected=="quit"){output<<"No choice made.\n";return 0;}
 auto next=state;
 auto result=NarrativeRuntime::select(*scene.scene,*narrative.narrative,next,selected,next.history.size());
 if(!result){errors<<"Choice rejected. No changes saved.\n";return 1;}
 if(!NarrativeStorage::save(save_file,*scene.scene,*narrative.narrative,next)){
  errors<<"Save failed; choice was not committed.\n";return 1;
 }
 output<<"Saved choice "<<result.emitted->choice_id<<" => EVENT:"<<result.emitted->event_id<<"\n";
 return 0;
}
}
