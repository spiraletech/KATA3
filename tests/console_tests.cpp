#include <kata3/console.hpp>
#include <cassert>
#include <filesystem>
#include <fstream>
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
 fs::remove(save,ec);fs::remove(scene,ec);fs::remove(narrative,ec);fs::remove(folder,ec);
}
