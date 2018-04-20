
//          Copyright blackgiulia.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/date_time/gregorian/gregorian.hpp>

#include "handle.hpp"

#include <iostream>

namespace bf = boost::filesystem;
namespace pt = boost::property_tree;
namespace bg = boost::gregorian;

int main() {
  pt::ptree root;
  bool config = false;
  auto config_path = bf::current_path();
  config_path += bf::path("/handle.json");

  if (bf::exists(config_path)) {
    config = true;
    root = readFromJson(config_path);
    std::cout << "\nhandle.json is detected, reload successfully."
              << std::endl;
  }

  if (!config) {
    auto pydriveHandle = get_obj("pydrive+gdocs://developer.gserviceaccount.com/");
    bool isFull = true;

    std::cout << "\nDoing a full backup..." << std::endl;

    auto sz = pydriveHandle.performBackup(isFull);

    std::cout << "\nNew full backup files size: " << sz << std::endl;

    root = pydriveHandle.writeToPT();
    root.put("lastFullDate", bg::to_iso_string(bg::day_clock::local_day()));
    root.put("lastFullSize", sz);
    root.put("lastIncrDate", "00000000");
    root.put("totalIncrSize", 0);

    writeToJson(root, config_path);
  } else {
    root = readFromJson(config_path);
    handle pydriveHandle(root);

    bg::date dd =
        bg::from_undelimited_string(root.get<std::string>("lastFullDate"));
    bg::date cur = bg::day_clock::local_day();
    bg::date_duration backupPeriod(7);

    bool isFull = false;
    if (dd + backupPeriod < cur) {
      isFull = true;
    }

    if (root.get<uint64_t>("lastFullSize") <
        root.get<uint64_t>("totalIncrSize") * 2) {
      isFull = true;
    }

    if (isFull) {
      std::cout << "\nDoing a full backup..." << std::endl;

      uint64_t sz = pydriveHandle.performBackup(isFull);
      std::cout << "\nNew full backup files size: " << sz << std::endl;

      root.put("lastFullDate", bg::to_iso_string(cur));
      root.put("lastFullSize", sz);
      root.put("lastIncrDate", "00000000");
      root.put("totalIncrSize", 0);
    } else {
      std::cout << "\nDoing a incremental backup..." << std::endl;
      uint64_t sz = pydriveHandle.performBackup(isFull);
      std::cout << "\nNew incr backup files size: " << sz << std::endl;

      root.put("lastIncrDate", bg::to_iso_string(cur));

      auto last_incr_size = root.get<uint64_t>("totalIncrSize");
      root.put("totalIncrSize", sz + last_incr_size);
    }

    writeToJson(root, config_path);
  }

  return 0;
}