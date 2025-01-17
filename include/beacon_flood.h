#ifndef BEACON_FLOOD_H
#define BEACON_FLOOD_H

#include <string>
#include <vector>

std::vector<std::string> load_ssid_list(const std::string &filename);

void start_beacon_flood(const std::string &ifname, const std::vector<std::string> &ssids);

#endif // BEACON_FLOOD_H
