#include <vector>
#include <iostream>

#include "scan.h"


int main(int argc, char ** argv) {

  if (argc != 6) {
    std::cerr << argv[0] << " <DATABASE> <EXPERIMENT> <ch> <MinCT> <MaxCT>" << std::endl;
    // std::cerr << argv[0] << " <DATABASE> <EXPERIMENT> <chan seq> <MinCT list> <MaxCT list>" << std::endl;
    return 1;
  }

  int ch = atoi(argv[3]);
  int min = atoi(argv[4]);
  int max = atoi(argv[5]);

  int maxaps = 0;

  std::vector<ProbeResponse> results;

  ScanningCampaing scan(argv[1], argv[2]);
  scan.init();

  scan.prepareIRD();

  for (int i = 0; i<50; i++) {
        results = scan.getAPs(ch, min, max);
        printf("getAPs(%d, %d, %d) = %d \n", ch, min, max, results.size());

        for (auto presp : results) {
          printf("  %s - op_ch: %d - nic_ch: %d\n",
              presp.ssid.c_str(), presp.op_channel, presp.nic_channel);
        }

        if (results.size() >= maxaps) {
                maxaps = results.size();
        }
        printf("\n");
  }
  printf("max: %d \n", maxaps);

  return 0;
}
