#pragma once
#include <kata3/narrative.hpp>
#include <algorithm>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>
namespace kata3 {
struct ChoiceEvent { std::uint64_t sequence{}; std::string choice_id; std::string event_id; };
struct NarrativeState { std::vector<ChoiceEvent> history; };
enum class ResolutionError { none, invalid_scene, invalid_contract, unknown_choice, stale_sequence, invalid_history };
struct Resolution { ResolutionError error{ResolutionError::none}; std::optional<ChoiceEvent> emitted; explicit operator bool() const noexcept {return emitted.has_value();} };
class NarrativeRuntime {
 static bool legal(const Scene& scene, const Narrative& contract) {
  if(!scene.preserve_canon || !scene.player_autonomy || contract.objective_id.empty() || contract.choices.size()<2)return false;
  if(std::none_of(scene.actors.begin(),scene.actors.end(),[](const Actor& a){return a.role=="PLAYER";}))return false;
  std::unordered_set<std::string> ids;
  for(const auto& c:contract.choices)if(c.id.empty()||c.consequence.empty()||!ids.insert(c.id).second)return false;
  return true;
 }
 static const Choice* lookup(const Narrative& contract,std::string_view id) {
  const auto it=std::find_if(contract.choices.begin(),contract.choices.end(),[&](const Choice& c){return c.id==id;});
  return it==contract.choices.end()?nullptr:&*it;
 }
public:
 // Caller supplies a concrete player-selected choice and expected history length.
 static Resolution select(const Scene& scene,const Narrative& contract,NarrativeState& state,std::string_view selected,std::uint64_t expected_sequence) {
  if(!legal(scene,contract))return {ResolutionError::invalid_contract,{}};
  if(expected_sequence!=state.history.size())return {ResolutionError::stale_sequence,{}};
  if(!valid_history(contract,state))return {ResolutionError::invalid_history,{}};
  const auto* choice=lookup(contract,selected);
  if(!choice)return {ResolutionError::unknown_choice,{}};
  ChoiceEvent event{expected_sequence+1,choice->id,choice->consequence};
  state.history.push_back(event);
  return {ResolutionError::none,event};
 }
 static bool valid_history(const Narrative& contract,const NarrativeState& state) {
  for(std::size_t i=0;i<state.history.size();++i){const auto& e=state.history[i];const auto* c=lookup(contract,e.choice_id);
   if(e.sequence!=i+1||!c||c->consequence!=e.event_id)return false;}
  return true;
 }
 static std::optional<NarrativeState> replay(const Scene& scene,const Narrative& contract,const std::vector<ChoiceEvent>& events) {
  if(!legal(scene,contract))return std::nullopt;
  NarrativeState result;
  for(const auto& event:events) {
   auto r=select(scene,contract,result,event.choice_id,result.history.size());
   if(!r||r.emitted->sequence!=event.sequence||r.emitted->event_id!=event.event_id)return std::nullopt;
  }
  return result;
 }
};
}
