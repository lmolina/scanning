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

  /*
  int i = 1;
  for (auto & scan_result: scan.scans) {
    std::cout << "Scan " << i++ << std::endl;
    for (auto & channel: scan_result.results()) {
      for (auto & presp: channel.second.responses()) {
        printf("  jiffie %.6f channel  %d (%d)\n", presp.kernel_time,
            presp.nic_channel, channel.first);
      }
    }
  }
  */

  scan.prepareIRD();

  int channel = 1;
  int response_no = 1;
  
  printf("Time between the first response and the previous one\n");
  printf("%f\n", scan.timeBetweenResponses(1, channel));

  printf("Time between the second response and the previous one\n");
  printf("%f\n", scan.timeBetweenResponses(1, channel));
 
  
  printf("Probability of getting no responses at all in a given channel\n");
  for (channel = 1; channel <= 11; ++channel)
    printf("Ch %d: %f\n", channel, scan.probabilityChannelEmpty(channel));

  printf("Probability of getting the first Probe Response before a given time in channel 1\n");
  channel = 1;
  int time = 0;
  for (time = 0; time <= 20; ++time)
    printf("Time < %d: %f\n", time, scan.probabilityResponseBefore(channel, time));

  /*
  for (int i = 0; i < 100; ++i) {
    printf("%d: %f\n", i, scan.timeBetweenResponses(response_no, 11));
  }
  */

  return 0;
}
