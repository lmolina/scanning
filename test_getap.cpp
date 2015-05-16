#include <vector>
#include <iostream>

#include "scan.h"


int main(int argc, char ** argv) {

  if (argc != 3) {
    std::cerr << argv[0] << " <DATABASE> <EXPERIMENT>" << std::endl;
    // std::cerr << argv[0] << " <DATABASE> <EXPERIMENT> <chan seq> <MinCT list> <MaxCT list>" << std::endl;
    return 1;
  }

  ScanningCampaing scan(argv[1], argv[2]);
  scan.init();

  scan.prepareIRD();

  int ch = 9;
  int min = 6;
  int max = 87;
  int aps = 0;
  int maxaps = 0;

  for (int i = 0; i<50; i++) {
        aps = scan.getAPs(ch, min, max);
        printf("getAPs(%d, %d, %d) = %d \n", ch, min, max, aps);
        if (aps >= maxaps) {
                maxaps = aps;
        }
        aps = 0;
  }
  printf("max: %d \n", maxaps);

  return 0;
}
