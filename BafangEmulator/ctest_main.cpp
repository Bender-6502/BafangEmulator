#include "trace.h"
#include "ctest_profile.h"
#include "ctest_packet_general.h"


int main(int argc, char** argv)
{
  TRACE_INIT(trace::flag_console | trace::flag_file);
  TRACE_FILENAME("BafangEmulator_Test.txt");
  TRACE_MESSAGE("Tests start");

  INIT_TESTS(&argc, argv);
  RUN_ALL_TESTS();

  system("Pause");
}
