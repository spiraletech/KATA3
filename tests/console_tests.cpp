#include <kata3/console.hpp>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <sstream>
int main(){
 namespace fs=std::filesystem;
 auto folder=fs::temp_directory_path()/"kata3_v1_play_test";
 std::error_code ec;fs::create_directories(folder,ec);
 auto scene=folder/"scene.k3",narrative=folder/"story.k3",save=folder/"history.bin";
 fs::remove(save,ec);
 {std::ofstream f(scene);f<<"SCENE bay\nWORLD = HAKUI\nLOCATION = DOCK\nMOOD = RAIN\nPRESERVE_CANON = TRUE\nPLAYER_AUTONOMY = TRUE\nACTOR hero PLAYER EXPLORE\nACTOR guide NPC HELP\nEND\n";}
 {std::ofstream f(narrative);f<<"OBJECTIVE seek | Find the dock\nDIALOGUE guide | Follow me.\nCHOICE ask | Ask guide | EVENT:ask_guide\nCHOICE leave | Leave | EVENT:leave_dock\n";}
 std::ostringstream out,errors;
 std::istringstream first("ask\n");
 assert(kata3::run_console(first,out,errors,scene,narrative,save)==0);
 assert(fs::exists(save));
 out.str("");out.clear();
 std::istringstream second("leave\n");
 assert(kata3::run_console(second,out,errors,scene,narrative,save)==0);
 assert(out.str().find("Previously recorded choices: 1")!=std::string::npos);
 out.str("");out.clear();
 std::istringstream invalid("invented\n");
 assert(kata3::run_console(invalid,out,errors,scene,narrative,save)==1);
 std::istringstream check("quit\n");
 out.str("");out.clear();
 assert(kata3::run_console(check,out,errors,scene,narrative,save)==0);
 assert(out.str().find("Previously recorded choices: 2")!=std::string::npos);

 // The console accepts exactly 64 KiB but rejects the next byte before
 // attempting to parse or persist either untrusted asset.
 std::ifstream saved_stream(save,std::ios::binary);
 const std::string saved_before((std::istreambuf_iterator<char>(saved_stream)),{});
 saved_stream.close();
 std::ifstream story_stream(narrative,std::ios::binary);
 const std::string original_story((std::istreambuf_iterator<char>(story_stream)),{});
 story_stream.close();
 constexpr std::size_t limit=65536;
 assert(original_story.size()+2<limit);
 const std::string exact_story="#"+std::string(limit-original_story.size()-2,'x')+"\n"+original_story;
 assert(exact_story.size()==limit);
 {std::ofstream f(narrative,std::ios::binary|std::ios::trunc);f<<exact_story;}
 std::istringstream boundary_quit("quit\n");
 assert(kata3::run_console(boundary_quit,out,errors,scene,narrative,save)==0);
 {std::ofstream f(narrative,std::ios::binary|std::ios::trunc);f<<exact_story<<"#";}
 std::istringstream too_large("quit\n");
 assert(kata3::run_console(too_large,out,errors,scene,narrative,save)==2);
 // A sparse multi-megabyte file must also fail without loading it wholesale.
 {std::ofstream f(narrative,std::ios::binary|std::ios::trunc);
  f.seekp(2*1024*1024-1);f.put('x');}
 std::istringstream sparse("quit\n");
 assert(kata3::run_console(sparse,out,errors,scene,narrative,save)==2);
 // Apply the same bound to the scene asset.
 {std::ofstream f(scene,std::ios::binary|std::ios::trunc);
  f.seekp(2*1024*1024-1);f.put('x');}
 std::istringstream large_scene("quit\n");
 assert(kata3::run_console(large_scene,out,errors,scene,narrative,save)==2);
 std::ifstream saved_after_stream(save,std::ios::binary);
 const std::string saved_after((std::istreambuf_iterator<char>(saved_after_stream)),{});
 assert(saved_after==saved_before);
 fs::remove(save,ec);fs::remove(scene,ec);fs::remove(narrative,ec);fs::remove(folder,ec);
}
