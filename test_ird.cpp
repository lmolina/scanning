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

  int channel = 1;
  int response_no = 1;
  printf("%f\n", scan.timeBetweenResponses(1, channel));
  for (int i = 0; i < 100; ++i) {
    printf("%d: %f\n", i, scan.timeBetweenResponses(response_no, 11));
  }

  return 0;
}
