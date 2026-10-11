#pragma once
#include <kata3/runtime.hpp>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
namespace kata3 {
class NarrativeStorage {
 static constexpr std::string_view magic="KATA3_LEDGER_V1\n";
 static constexpr std::size_t max_bytes=2097152;
 static void u32(std::string& s,std::uint32_t v){for(int i=0;i<4;++i)s.push_back(char((v>>(i*8))&255));}
 static void u64(std::string& s,std::uint64_t v){for(int i=0;i<8;++i)s.push_back(char((v>>(i*8))&255));}
 static bool read32(std::string_view s,std::size_t& p,std::uint32_t& v){if(p>s.size()||s.size()-p<4)return false;v=0;for(int i=0;i<4;++i)v|=std::uint32_t(static_cast<unsigned char>(s[p++]))<<(i*8);return true;}
 static bool read64(std::string_view s,std::size_t& p,std::uint64_t& v){if(p>s.size()||s.size()-p<8)return false;v=0;for(int i=0;i<8;++i)v|=std::uint64_t(static_cast<unsigned char>(s[p++]))<<(i*8);return true;}
 static std::uint64_t hash(std::string_view s){std::uint64_t h=14695981039346656037ull;for(unsigned char c:s){h^=c;h*=1099511628211ull;}return h;}
 static bool field(std::string& result,const std::string& s){if(s.empty()||s.size()>256)return false;u32(result,static_cast<std::uint32_t>(s.size()));result+=s;return true;}
 static bool unfield(std::string_view s,std::size_t& p,std::string& dest){std::uint32_t n=0;if(!read32(s,p,n)||n==0||n>256||p>s.size()||s.size()-p<n)return false;dest.assign(s.substr(p,n));p+=n;return true;}
public:
 static std::optional<std::string> encode(const Scene& scene,const Narrative& contract,const NarrativeState& state){
  if(!scene.preserve_canon||!scene.player_autonomy||!NarrativeRuntime::replay(scene,contract,state.history))return std::nullopt;
  std::string out(magic);if(!field(out,scene.id)||!field(out,contract.objective_id))return std::nullopt;
  if(state.history.size()>10000)return std::nullopt;
  u32(out,static_cast<std::uint32_t>(state.history.size()));
  for(const auto& e:state.history){u64(out,e.sequence);if(!field(out,e.choice_id)||!field(out,e.event_id))return std::nullopt;}
  if(out.size()+8>max_bytes)return std::nullopt;
  u64(out,hash(out));return out;
 }
 static std::optional<NarrativeState> decode(std::string_view data,const Scene& scene,const Narrative& contract){
  if(data.size()>max_bytes||data.size()<magic.size()+20||!data.starts_with(magic))return std::nullopt;
  std::size_t p=magic.size();std::string scene_id,objective_id;
  if(!unfield(data,p,scene_id)||!unfield(data,p,objective_id)||scene_id!=scene.id||objective_id!=contract.objective_id)return std::nullopt;
  std::uint32_t count=0;if(!read32(data,p,count)||count>10000)return std::nullopt;
  std::vector<ChoiceEvent> events;events.reserve(count);
  for(std::uint32_t i=0;i<count;++i){ChoiceEvent e;if(!read64(data,p,e.sequence)||!unfield(data,p,e.choice_id)||!unfield(data,p,e.event_id))return std::nullopt;events.push_back(std::move(e));}
  if(p>data.size()||data.size()-p!=8)return std::nullopt;
  std::uint64_t checksum=0;if(!read64(data,p,checksum)||checksum!=hash(data.substr(0,data.size()-8)))return std::nullopt;
  return NarrativeRuntime::replay(scene,contract,events);
 }
 static bool save(const std::filesystem::path& path,const Scene& scene,const Narrative& contract,const NarrativeState& state){
  auto encoded=encode(scene,contract,state);if(!encoded)return false;
  std::error_code ec;
  if(std::filesystem::is_symlink(std::filesystem::symlink_status(path,ec)))return false;
  auto staging=path;staging+=".pending";ec.clear();
  if(std::filesystem::is_symlink(std::filesystem::symlink_status(staging,ec)))return false;
  {std::ofstream f(staging,std::ios::binary|std::ios::trunc);if(!f)return false;f.write(encoded->data(),static_cast<std::streamsize>(encoded->size()));f.flush();if(!f.good()){f.close();std::filesystem::remove(staging,ec);return false;}}
  std::filesystem::rename(staging,path,ec);
  if(ec){std::filesystem::remove(staging,ec);return false;}
  return true;
 }
 static bool load(const std::filesystem::path& path,const Scene& scene,const Narrative& contract,NarrativeState& destination){
  std::error_code ec;
  if(std::filesystem::is_symlink(std::filesystem::symlink_status(path,ec)))return false;
  auto len=std::filesystem::file_size(path,ec);if(ec||len>max_bytes)return false;
  std::ifstream f(path,std::ios::binary);if(!f)return false;
  std::string bytes(static_cast<std::size_t>(len),'\0');f.read(bytes.data(),static_cast<std::streamsize>(len));if(!f)return false;
  auto result=decode(bytes,scene,contract);if(!result)return false;
  destination=std::move(*result);return true;
 }
};
}
