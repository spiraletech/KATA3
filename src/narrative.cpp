#include <kata3/narrative.hpp>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <unordered_set>
namespace kata3 {
namespace {
std::string trim(std::string s){ auto first=s.find_first_not_of(" \t\r"); if(first==std::string::npos)return {};return s.substr(first,s.find_last_not_of(" \t\r")-first+1); }
bool ident(const std::string& s){if(s.empty()||s.size()>80)return false;return std::all_of(s.begin(),s.end(),[](unsigned char c){return std::isalnum(c)||c=='_'||c=='-'||c=='.';});}
bool prose(const std::string& s){return !s.empty()&&s.size()<=1024&&std::all_of(s.begin(),s.end(),[](unsigned char c){return c>=32 || c=='\t';});}
std::vector<std::string> split(const std::string& s){std::vector<std::string> out;std::size_t first=0;while(true){auto pos=s.find('|',first);out.push_back(trim(s.substr(first,pos==std::string::npos?pos:pos-first)));if(pos==std::string::npos)break;first=pos+1;}return out;}
}
NarrativeResult parse_narrative(std::string_view source,const Scene& scene){
 NarrativeResult out;
 // Keep direct sidecar parsing within the generation-review 64 KiB boundary.
 // Check the view length before copying or tokenizing untrusted input.
 if(source.size()>65536){out.errors.push_back({0,"narrative source exceeds 64 KiB"});return out;}
 Narrative n;std::istringstream in{std::string(source)};std::string line;std::size_t number=0;std::unordered_set<std::string> ids;bool has_player=false;
 for(const auto& actor:scene.actors)if(actor.role=="PLAYER")has_player=true;
 if(!scene.preserve_canon||!scene.player_autonomy)out.errors.push_back({0,"scene laws require canon and player autonomy"});
 if(!has_player)out.errors.push_back({0,"scene requires a PLAYER actor"});
 while(std::getline(in,line)){
  ++number;line=trim(line);if(line.empty()||line[0]=='#')continue;
  auto space=line.find(' ');auto op=line.substr(0,space);auto args=split(space==std::string::npos?"":line.substr(space+1));
  auto error=[&](std::string why){out.errors.push_back({number,std::move(why)});};
  if(op=="OBJECTIVE"){
   if(args.size()!=2||!ident(args[0])||!prose(args[1])){error("OBJECTIVE requires id | description");continue;}
   if(!n.objective_id.empty()){error("duplicate OBJECTIVE");continue;}n.objective_id=args[0];n.objective=args[1];
  }else if(op=="DIALOGUE"){
   if(args.size()!=2||!ident(args[0])||!prose(args[1])){error("DIALOGUE requires actor | text");continue;}
   const auto found=std::find_if(scene.actors.begin(),scene.actors.end(),[&](const Actor& a){return a.id==args[0];});
   if(found==scene.actors.end()){error("dialogue references unknown actor");continue;}
   n.dialogue.push_back({args[0],args[1]});
  }else if(op=="CHOICE"){
   if(args.size()!=3||!ident(args[0])||!prose(args[1])||args[2].rfind("EVENT:",0)!=0||!ident(args[2].substr(6))){error("CHOICE requires id | label | EVENT:identifier");continue;}
   if(!ids.insert(args[0]).second){error("duplicate CHOICE id");continue;}
   n.choices.push_back({args[0],args[1],args[2].substr(6)});
  }else error("unknown narrative directive");
 }
 if(n.objective_id.empty())out.errors.push_back({number+1,"missing OBJECTIVE"});
 if(n.choices.size()<2)out.errors.push_back({number+1,"at least two explicit player choices required"});
 if(out.errors.empty())out.narrative=std::move(n);
 return out;
}
}
