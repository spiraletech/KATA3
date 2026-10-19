#include <kata3/scene.hpp>
#include <sstream>
#include <unordered_set>
#include <cctype>
namespace kata3 {
namespace {
std::string trim(std::string s) { auto p=s.find_first_not_of(" \t\r"); if(p==std::string::npos)return {};auto e=s.find_last_not_of(" \t\r");return s.substr(p,e-p+1); }
bool identifier(const std::string& s){ if(s.empty()||s.size()>80)return false;for(unsigned char c:s)if(!std::isalnum(c)&&c!='_'&&c!='-'&&c!='.')return false;return true; }
}
ParseResult parse(const std::string& source){
 ParseResult out;Scene scene;std::istringstream input(source);std::string line;std::size_t n=0;bool begun=false,ended=false,has_player=false;
 std::unordered_set<std::string> keys,actor_ids;
 auto error=[&](std::string msg){out.errors.push_back({n,std::move(msg)});};
 while(std::getline(input,line)){
  ++n;line=trim(line);if(line.empty()||line[0]=='#')continue;
  if(ended){error("content after END");continue;}
  if(!begun){if(line.rfind("SCENE ",0)!=0){error("expected SCENE identifier");continue;}scene.id=trim(line.substr(6));if(!identifier(scene.id))error("invalid scene identifier");begun=true;continue;}
  if(line=="END"){ended=true;continue;}
  if(line.rfind("ACTOR ",0)==0){
   std::istringstream tokens(line.substr(6));Actor a;std::string extra;tokens>>a.id>>a.role>>a.motive;
   if(!identifier(a.id)||!identifier(a.role)||!identifier(a.motive)||(tokens>>extra)){error("ACTOR requires id role motive");continue;}
   if(a.role!="PLAYER"&&a.role!="NPC"){error("ACTOR role must be PLAYER or NPC");continue;}
   if(!actor_ids.insert(a.id).second){error("duplicate actor");continue;}
   if(a.role=="PLAYER")has_player=true;
   scene.actors.push_back(std::move(a));continue;
  }
  auto pos=line.find('=');if(pos==std::string::npos){error("expected KEY = VALUE");continue;}
  auto key=trim(line.substr(0,pos)),value=trim(line.substr(pos+1));
  if(!keys.insert(key).second){error("duplicate field: "+key);continue;}
  if(key=="WORLD")scene.world=value;
  else if(key=="LOCATION")scene.location=value;
  else if(key=="MOOD")scene.mood=value;
  else if(key=="PRESERVE_CANON"||key=="PLAYER_AUTONOMY"){
   if(value!="TRUE"&&value!="FALSE"){error("boolean must be TRUE or FALSE");continue;}
   (key=="PRESERVE_CANON"?scene.preserve_canon:scene.player_autonomy)=value=="TRUE";
  }else error("unknown field: "+key);
 }
 if(!begun)out.errors.push_back({n+1,"missing SCENE"});
 if(!ended)out.errors.push_back({n+1,"missing END"});
 if(!identifier(scene.world)||!identifier(scene.location))out.errors.push_back({n+1,"WORLD and LOCATION required"});
 if(scene.mood.empty())out.errors.push_back({n+1,"MOOD required"});
 else if(!identifier(scene.mood))out.errors.push_back({n+1,"MOOD must be a valid identifier"});
 if(!scene.preserve_canon||!scene.player_autonomy)out.errors.push_back({n+1,"canon and player autonomy laws required"});
 if(scene.actors.empty())out.errors.push_back({n+1,"at least one ACTOR required"});
 if(!has_player)out.errors.push_back({n+1,"scene requires a PLAYER actor"});
 if(out.errors.empty())out.scene=std::move(scene);
 return out;
}
}
