#ifndef SCAN_H
#define SCAN_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <cstdio>
#include <cstring>

#include <sqlite3.h>


class Scan {
  public:
    struct AP {
      int channel;
      int delay;
      std::string ssid;
      std::string bssid;
      std::string type;
    };
    

    typedef std::vector<AP> ScanResults;

    Scan(std::string dbName, std::string experiment) {
      this->dbName = dbName;
      this->experiment = experiment;
      this->rand = NULL;
      this->gen = NULL;
    };

    void init() {
      sqlite3* db;
      int rc = sqlite3_open(dbName.c_str(), &db);

      if (rc != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return;
      }

      fprintf(stdout, "Database opened successfully :)\n");

      std::string sql;
      sql = "SELECT algorithm, scan, op_channel, ssid, bssid, delayj, frame_type ";
      sql += " FROM frames ";
      sql += " WHERE algorithm = \"" + experiment + "\";";

      // A trick to modify the scans in the callback function (because it's
      // an static function)
      std::vector<ScanResults>* data = &scans;
      char * errMsg = 0;
      rc = sqlite3_exec(db, sql.c_str(), callback, (void*)data, &errMsg);

      if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
      }

      sqlite3_close(db);

      N = scans.size();

      for(auto & i: scans) {
        std::sort(i.begin(), i.end(), cmp);
      }

      // Prepare the random generator
      std::random_device rd;
      gen = new std::mt19937(rd());
      rand = new std::uniform_int_distribution<>(0, scans.size() - 1);
    }

    std::vector<AP> execute(int ch, int minCT, int maxCT) {
      std::vector<AP> results;

      //unsigned int scanId = rand() % N;
      unsigned int scanId = (*rand)(*gen);

      if (scans[scanId].size() == 0)
        return results;

      if (minCT < scans[scanId][0].delay)
        return results;

      for (int i = 0; i < scans[scanId].size(); ++i) {
        if (scans[scanId][i].delay <= maxCT)
          results.push_back(scans[scanId][i]);
        else
          return results;
      }

      return results;
    }

    ~Scan() {
      if (rand != NULL)
        delete rand;

      if (gen != NULL)
        delete gen;
    }

  private:
    static int callback(void* data, int argc, char **argv, char **colName) {
      AP buffer;

      unsigned int scanId = 0;
      for (int i = 0; i < argc; ++i) {
        if (strcmp(colName[i], "scan") == 0) {
          scanId = atoi(argv[i]) - 1;
        }
        else if (strcmp(colName[i], "op_channel") == 0) {
          buffer.channel = atoi(argv[i]);
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
      }

      std::vector<ScanResults>* tmp = (std::vector<ScanResults>*) data;
      if (scanId >= tmp->size()) {
        tmp->resize(scanId + 1);
      }

      (tmp->at(scanId)).push_back(buffer);

      return 0;
    }

    static bool cmp(const AP & a, const AP & b) {
      return a.delay < b.delay;
    }

    unsigned int N;
    std::string dbName;
    std::string experiment;
    std::vector<ScanResults> scans;
    std::uniform_int_distribution<>* rand;
    std::mt19937* gen;
};

#endif
