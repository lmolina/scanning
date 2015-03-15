#ifndef SCAN_H
#define SCAN_H

#include <iostream>

#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

#include <cstdio>
#include <cstring>

#include <sqlite3.h>

struct ProbeResponse {
  int op_channel;
  int nic_channel;
  int delay;
  int jiffies;
  int prequest;
  float kernel_time;
  std::string ssid;
  std::string bssid;
  std::string type;
};

struct BssidCmp {
  bool operator() (const ProbeResponse & lhs,
      const ProbeResponse & rhs) const {
    return lhs.bssid < rhs.bssid;
  }
};

static bool cmp(const ProbeResponse & a, const ProbeResponse & b) {
  return a.delay < b.delay;
}

class Channel {
  public:
    Channel(int id=0) {
      this->id = id;
    }

    void addResponse(ProbeResponse & presp) {
      probe_responses.push_back(presp);
    }

    int size() {
      return probe_responses.size();
    }

    std::vector<ProbeResponse> responses() {
      return probe_responses;
    }

  private:
    int id;
    std::vector<ProbeResponse> probe_responses;
};


class ScanResult {
  public:
    //ScanResult();


    // addResponse will save the ProbeResponse in the scan_id scan and in
    // the appropriate channel, using nic_channel
    void addResponse(ProbeResponse & presp) {
      int ch = presp.nic_channel;

      // Check if the channel already exists in the map, create the channel
      // if necessary
      if (results_.count(ch) == 0) {
        // results[channel_id] = new Channel(channel_id);
        results_[ch] = Channel(ch);
      }

      results_[ch].addResponse(presp);
    }


    Channel channel(int ch) {
      Channel r;
      if (results_.count(ch) == 0) {
        return r;
      }

      return results_[ch];
    }

    std::map<int, Channel> results() {
      return results_;
    }
  private:

    // results_ groups the ProbeResponses registered at each channel. Since we
    // don't know what are the scanned channels, we use a map to holds it
    std::map<int, Channel> results_;
};


class ScanningCampaing {
  public:
    typedef std::vector<ProbeResponse> ScanResults;

    ScanningCampaing(std::string dbName, std::string experiment) {
      this->dbName = dbName;
      this->experiment = experiment;
      this->rand = NULL;
      this->gen = NULL;
    };

    ~ScanningCampaing() {
      if (rand != NULL)
        delete rand;

      if (gen != NULL)
        delete gen;
    }

    void init() {
      sqlite3* db;
      int rc = sqlite3_open(dbName.c_str(), &db);

      if (rc != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return;
      }

      fprintf(stdout, "Database opened successfully :)\n");

      std::string sql;
      sql = "SELECT algorithm, scan, op_channel, nic_channel, jiffies, kernel_time, ssid, bssid, delayj, frame_type, prequest";
      sql += " FROM frames ";
      sql += " WHERE algorithm = \"" + experiment + "\";";

      // A trick to modify the scans in the callback function (because it's
      // an static function)
      std::vector<ScanResult>* data = &scans;
      char * errMsg = 0;
      rc = sqlite3_exec(db, sql.c_str(), callback, (void*)data, &errMsg);

      if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
      }

      sqlite3_close(db);

      N = scans.size();

      //for(auto & i: scans) {
        // TODO: check the cmp operator
        //std::sort(i.begin(), i.end(), cmp);
      //}

      // Prepare the random generator
      std::random_device rd;
      gen = new std::mt19937(rd());
      rand = new std::uniform_int_distribution<>(0, scans.size() - 1);
    }


    /*
     * Emulate a channel probe using the corresponding parameters (Channel,
     * MinChannelTime and MaxChannelTime.
     *
     * The emulation works as follows:
     * 1) An scan result is randomly selected from the database
     *
     * 2) If there is at least one ProbeResponse with a response delay <=
     *    MinChannelTime, then look loop over the ProbeResponses and return
     *    all of those with a response delay <= MaxChannelTime
     *
     * 3) If there is no ProbeResponse at all, then return an empty vector
     *
     * 4) If there are several ProbeResponses but all of them contains the
     *    response delay greater than MinChannelTime, then return an empty
     *    vector
     */
    //std::vector<ProbeResponse> execute(int ch, int min_ct, int max_ct) {
    ScanResults randomScan(int ch, int min_ct, int max_ct) {
      ScanResult scan;
      Channel channel;
      std::vector<ProbeResponse> all_responses;
      ScanResults r;

      //unsigned int scanId = rand() % N;
      unsigned int scan_id = (*rand)(*gen);
      //unsigned int scan_id = 10;
      scan = scans[scan_id];

      channel = scan.channel(ch);
      if (channel.size() <= 0) {
        return r;
      }

      all_responses = channel.responses();
      std::sort(all_responses.begin(), all_responses.end(), cmp);

      if (min_ct < all_responses[0].delay)
        return r;

      for (int i = 0; i < all_responses.size(); ++i) {
        if (all_responses[i].delay <= max_ct)
          r.push_back(all_responses[i]);
        else
          return r;
      }

      return r;
    }

    /**
     * num_ch is the number of channels to scan
     * chan_list holds the channel sequence order
     * min_ch_values holds the MinCT values according to the chan_seq order
     * max_ch_values holds the MaxCT values according to the chan_seq order
     *
     * len(chan_seq) == len(min_ch_values) == len(max_ch_values) == num_ch 
     *
     * This method will loop over all the scans registered and emulate the
     * results based on the specified parameters. It will return a vector
     * containing the number of APs discovered at each location
     */
    std::vector<int> emulateScanInAllPoints(int num_ch, int * chan_list,
        int * min_ch_values, int * max_ch_values) {

      ScanResult scan;
      Channel channel;
      std::vector<ProbeResponse> all_responses;
      std::vector<int> r;

      std::set<ProbeResponse, BssidCmp> myset;

      int min_ct;
      int max_ct;
      int ch;

      // Loop over the different scan points
      for (int scan_id = 0; scan_id < N; ++scan_id) {
        myset.clear();
        scan = scans[scan_id];

        // Loop over the channels to scan
        for (int n = 0; n < num_ch; ++n) {
          ch = chan_list[n];
          min_ct = min_ch_values[n];
          max_ct = max_ch_values[n];

          channel = scan.channel(ch);
          if (channel.size() <= 0) {
            continue;
          }

          all_responses = channel.responses();
          std::sort(all_responses.begin(), all_responses.end(), cmp);
          if (min_ct < all_responses[0].delay) {
            continue;
          }

          for (int i = 0; i < all_responses.size(); ++i) {
            if (all_responses[i].delay <= max_ct)
              myset.insert(all_responses[i]);
            else
              break;
          }
        }

        // Add to the result the number of discovered APs in the current scan
        // point
        r.push_back(myset.size());
      }

      return r;
    }


    double timeBetweenResponses(double channel, double response_no) {
      long n = ird_times[channel][response_no].size();
      long index;

      if (n == 0) {
        return -1;
      }

      if (n == 1) {
        return ird_times.at(channel).at(response_no)[0];
      }
      
      std::uniform_int_distribution<> ird_rand(0, n - 1);
      
      index = ird_rand(*gen);
      return ird_times.at(channel).at(response_no)[index];
    }


    void prepareIRD() {
      std::vector<ProbeResponse> presponses;
      int ch_no;
      int response_no;
      int ird;
      int prev_delay;
      std::vector<ProbeResponse>::iterator it;

      for (auto & scan: scans) {
        for (auto & channel: scan.results()) {
          ch_no = channel.first;

          if (ird_times.count(ch_no) == 0) {
            ird_times[ch_no] = std::map<int, std::vector<int>>();
          }

          // For the first Probe Response, there is no previous delay, and
          // their IRD is equal to the delay
          prev_delay = 0;
          response_no = 1;
          presponses = channel.second.responses();
          for (it = presponses.begin(); it != presponses.end();
              ++it, ++response_no) {

            if (ird_times[ch_no].count(response_no) == 0) {
              ird_times[ch_no][response_no] = std::vector<int>();
            }

            ird = it->delay - prev_delay;
            ird_times[ch_no][response_no].push_back(ird);
            prev_delay = it->delay;
          }
        }
      }

      for (auto & ch : ird_times) {
        fprintf(stderr, "%d => \n", ch.first);
        for (auto & responses : ch.second) {
          std::sort(responses.second.begin(), responses.second.end());
          fprintf(stderr, "    %d (%d)=> [", responses.first, responses.second.size());
          for (auto & response : responses.second) {
            fprintf(stderr, "%d ", response);
          }
          fprintf(stderr, "]\n");
        }
      }
      printf("\n");
    }

  private:
    static int callback(void* data, int argc, char **argv, char **colName) {
      ProbeResponse buffer;
      
      std::vector<ScanResult>* tmp = (std::vector<ScanResult>*) data;

      unsigned int scan_id = 0;
      for (int i = 0; i < argc; ++i) {
        if (strcmp(colName[i], "scan") == 0) {
          
          // The "scan" column starts at 1, so scan - 1 to use it as 
          // the vector index in the results
          scan_id = atoi(argv[i]) - 1;
        }
        else if (strcmp(colName[i], "op_channel") == 0) {
          buffer.op_channel = atoi(argv[i]);
        }
        else if (strcmp(colName[i], "nic_channel") == 0) {
          buffer.nic_channel = atoi(argv[i]);
        }
        else if (strcmp(colName[i], "kernel_time") == 0) {
          buffer.kernel_time = atof(argv[i]);
        }
        else if (strcmp(colName[i], "jiffies") == 0) {
          buffer.jiffies = atoi(argv[i]);
        }
        else if (strcmp(colName[i], "delayj") == 0) {
          buffer.delay = atoi(argv[i]);
        }
        else if (strcmp(colName[i], "ssid") == 0) {
          buffer.ssid = std::string(argv[i]);
        }
        else if (strcmp(colName[i], "bssid") == 0) {
          buffer.bssid = std::string(argv[i]);
        }
        else if (strcmp(colName[i], "frame_type") == 0) {
          buffer.type = std::string(argv[i]);
        }
        else if (strcmp(colName[i], "prequest") == 0) {
          buffer.prequest = atoi(argv[i]);
        }
      }


      // Remember that the scan_id is the index of the scan in the vector of
      // scan results (that is a vector of ProbeResponses).
      if (scan_id >= tmp->size()) {
        tmp->resize(scan_id + 1);
      }

      (tmp->at(scan_id)).addResponse(buffer);

      return 0;
    }


    // One ScanResult contains the set of results obtained when a real
    // scanning was triggered in a real machine, this could be related to a
    // fixed location, although, this might not be true if the MS is in
    // movement. So, scan holds each one of the ScanResult
    std::vector<ScanResult> scans;

    // Keeps the distribution of the InterResponseTime, one vector position
    // per channel. Each channel contains one vector position per response
    // number, so it looks like a table
    //std::vector<IRDChannel> ird_distributions;

    unsigned int N;
    std::string dbName;
    std::string experiment;
    std::uniform_int_distribution<>* rand;
    std::mt19937* gen;
    std::map<int, std::map<int, std::vector<int>>> ird_times;
};

#endif
