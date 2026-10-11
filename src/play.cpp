#include <kata3/console.hpp>
#include <iostream>
int main(int argc,char** argv){
 if(argc!=4){std::cerr<<"Usage: kata3_play <scene.k3> <narrative.k3> <save.k3save>\n";return 2;}
 return kata3::run_console(std::cin,std::cout,std::cerr,argv[1],argv[2],argv[3]);
}
