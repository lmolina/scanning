#include <vector>
#include <iostream>

#include "scan.h"


int main(int argc, char ** argv) {

  if (argc != 37) {
    std::cerr << argv[0] << " <DATABASE> <EXPERIMENT> <trials> <Ch Min Max Ch Min Max ...>" << std::endl;
    // std::cerr << argv[0] << " <DATABASE> <EXPERIMENT> <chan seq> <MinCT list> <MaxCT list>" << std::endl;
    return 1;
  }

  const int kNumChannels = 11;

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
      results = scan.getAPs(channels[1], min[1], max[1]);
      discovered += results.size();
      results = scan.getAPs(channels[2], min[2], max[2]);
      discovered += results.size();
      results = scan.getAPs(channels[3], min[3], max[3]);
      discovered += results.size();
      results = scan.getAPs(channels[4], min[4], max[4]);
      discovered += results.size();
      results = scan.getAPs(channels[5], min[5], max[5]);
      discovered += results.size();
      results = scan.getAPs(channels[6], min[6], max[6]);
      discovered += results.size();
      results = scan.getAPs(channels[7], min[7], max[7]);
      discovered += results.size();
      results = scan.getAPs(channels[8], min[8], max[8]);
      discovered += results.size();
      results = scan.getAPs(channels[9], min[9], max[9]);
      discovered += results.size();
      results = scan.getAPs(channels[10], min[10], max[10]);
      discovered += results.size();

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
