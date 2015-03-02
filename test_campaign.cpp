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
  std::vector<int> results = scan.emulateScanInAllPoints(num_ch,
      chan_list, min_ch_values, max_ch_values);

  std::cout << results.size() << " " << std::endl;
  for(auto & i: results) {
    std::cout << i << std::endl;
  }

  // Copy and paste from
  // https://stackoverflow.com/questions/7616511/calculate-mean-and-standard-deviation-from-a-vector-of-samples-in-c-using-boos
  double sum = std::accumulate(results.begin(), results.end(), 0.0);
  double mean = sum / results.size();
  std::vector<double> diff(results.size());
  std::transform(results.begin(), results.end(), diff.begin(),
                     std::bind2nd(std::minus<double>(), mean));
  double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
  double stdev = std::sqrt(sq_sum / results.size());

  std::cout << "mean: " << mean << std::endl;
  std::cout << "stdev: " << stdev << std::endl;

  return 0;
}
