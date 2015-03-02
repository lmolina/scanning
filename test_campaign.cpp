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

  // Ex: scan channels 1, 6, 11. Use the following timers:
  // Ch1:  MinCT = 5, Max = 15
  // Ch6:  MinCT = 2, Max = 20
  // Ch11: MinCT = 3, Max = 10

  int num_ch = 3;
  int chan_list[] = {1, 6, 11};
  int min_ch_values[] = {5, 2, 3};
  int max_ch_values[] = {15, 20, 10};
  ScanningCampaing::ScanResults results = scan.emulateScanInAllPoints(num_ch,
      chan_list, min_ch_values, max_ch_values);
  std::cout << results.size() << " results: " << std::endl;

  for (auto & r: results) {
    printf("kernel_time: %.9f\n", r.kernel_time);
    printf("jiffies: %d\n", r.jiffies);
    std::cout << "type: " << r.type << std::endl;
    std::cout << "bssid: " << r.bssid << std::endl;
    std::cout << "ssid: " << r.ssid << std::endl;
    std::cout << "delay: " << r.delay << std::endl;
    std::cout << "op_channel: " << r.op_channel << std::endl;
    std::cout << "nic_channel: " << r.nic_channel << std::endl;
    std::cout << std::endl;
  }

  return 0;
}
