#include <vector>
#include <iostream>

#include "scan.h"


int main(int argc, char ** argv) {

  if (argc != 7) {
    std::cerr << argv[0] << " <DATABASE> <EXPERIMENT> <trials> <Ch Min Max>" << std::endl;
    return 1;
  }

  const int kNumChannels = 1;

  int channels[kNumChannels];
  double min[kNumChannels];
  double max[kNumChannels];

  int num_trials = atoi(argv[3]);
  int discovered = 0;
  int total = 0;
  int minaps = 99999999;
  int maxaps = 0;
  double avgaps = 0;

  std::vector<ProbeResponse> results;
  
  char ** args = argv + 4;

  for (int i = 0; i < kNumChannels; ++i) {
    //printf("%d - Ch: %s [%s, %s]\n", i, args[0], args[1], args[2]);
    channels[i] = atoi(args[0]);
    min[i] = atof(args[1]);
    max[i] = atof(args[2]);
    args = args + 3;
  }

  //ScanningCampaing scan(argv[1], argv[2]);
  ScanningCampaing scan(argv[1], argv[2], 1);
  scan.init();
  scan.prepareIRD();

  for (int n = 0; n < num_trials; ++ n) {
      results = scan.getAPs(channels[0], min[0], max[0]);
      discovered = results.size();

      total += discovered;
      if (discovered > maxaps) {
        maxaps = discovered;
      }

      if (discovered < minaps) {
        minaps = discovered;
      }
  }

  avgaps = total * 1.0 / num_trials;
  printf("min: %d \n", minaps);
  printf("max: %d \n", maxaps);
  printf("avg: %2.2f \n", avgaps);

  return 0;
}
