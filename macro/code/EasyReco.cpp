/*
g++ -o EasyReco ../code/EasyReco.cpp `root-config --cflags --glibs`
./EasyReco singles_out0.root
*/

#define EasyReco_cxx
#include "./src/EasyReco.cc"

int main(int argc, char const *argv[]){

  //std::cout << "First argument: input filename" << std::endl;
  std::string inputfilename = argv[1];

  EasyReco* testt = new EasyReco(inputfilename);
  testt->Loop();
  return 0;
}
