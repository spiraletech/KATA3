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

 // Raw input controls must not become invisible token separators or bypass comments.
 auto vt=valid; vt[vt.find("ether PLAYER")+5]='\v';
 auto vt_result=kata3::parse(vt); assert(!vt_result);
 assert(vt_result.errors.front().line==7);
 assert(vt_result.errors.front().message=="invalid control character in scene line");
 auto ff=valid; ff[ff.find("ether PLAYER")+5]='\f'; assert(!kata3::parse(ff));
 auto internal_cr=valid; internal_cr[internal_cr.find("ether PLAYER")+5]='\r'; assert(!kata3::parse(internal_cr));
 auto nul_comment=valid+std::string("#comment\0hidden\n",16); assert(!kata3::parse(nul_comment));
 auto esc_comment=valid+"#comment\x1b[0m\n"; assert(!kata3::parse(esc_comment));
 auto del_comment=valid+"#comment\x7f\n"; assert(!kata3::parse(del_comment));
 // Preserve legal TAB separators, comments, and CRLF scene sources.
 auto tab=valid; tab[tab.find("ether PLAYER")+5]='\t'; assert(kata3::parse(tab));
 std::string crlf; for(char c:valid){if(c=='\n')crlf+='\r';crlf+=c;} assert(kata3::parse(crlf));
 assert(kata3::parse(valid+"\t# trailing comment\r\n"));
}
