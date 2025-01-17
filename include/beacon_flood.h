#ifndef BEACON_FLOOD_H
#define BEACON_FLOOD_H

#include <string>
#include <vector>
#include <cstdint>

using namespace std;

string mac_to_string(const uint8_t mac[6]);

vector<string> load_ssid_list(const string &filename);

void start_beacon_flood(const string &ifname, const vector<string> &ssids);

#endif // BEACON_FLOOD_H
