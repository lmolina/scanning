#include <vector>
#include <iostream>

#include "scan.h"


int main(int argc, char ** argv) {

  if (argc != 3) {
    std::cerr << argv[0] << " <DATABASE> <EXPERIMENT>" << std::endl;
    return 1;
  }

  //Scan scan("/home/lmolina/src/wifidb/test_18.1/database.db");
  Scan scan(argv[1], argv[2]);
  scan.init();

  srand(14);
  Scan::ScanResults results = scan.execute(11, 10, 30);
  std::cout << results.size() << " results: " << std::endl;

  for (auto & r: results) {
    std::cout << "type: " << r.type << std::endl;
    std::cout << "bssid: " << r.bssid << std::endl;
    std::cout << "ssid: " << r.ssid << std::endl;
    std::cout << "delay: " << r.delay << std::endl;
    std::cout << std::endl;
  }

  return 0;
}
