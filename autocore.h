#ifndef AUTOCORE_H
#define AUTOCORE_H

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace bp = boost::process;
namespace bg = boost::gregorian;

struct autocore {
  std::string targetDir;
  std::string sourceDir;
  std::string encryptKey;
  std::string signKey;
  std::string backend;
  std::string passphrase;
  std::string signPassphrase;
  boost::filesystem::path p;

  bg::date lastFullDate();
  uint64_t performBackup(bool &isFull);
  autocore()
      : targetDir(""),
        sourceDir(""),
        encryptKey(""),
        signKey(""),
        backend(""),
        passphrase(""),
        signPassphrase(""),
        p("/") {}
  autocore(std::string, std::string, std::string, std::string, std::string,
           std::string, std::string, boost::filesystem::path);
};

#endif  // AUTOCORE_H
