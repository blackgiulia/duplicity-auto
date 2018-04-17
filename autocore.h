#ifndef AUTOCORE_H
#define AUTOCORE_H

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

struct autocore {
  std::string targetDir;
  std::string sourceDir;
  std::string encryptKey;
  std::string signKey;
  std::string backend;
  std::string passphrase;
  std::string signPassphrase;
  boost::filesystem::path p_duplicity;

  boost::gregorian::date lastFullDate() const;

  uint64_t performBackup(const bool &isFull) const;

  autocore()
      : targetDir(""),
        sourceDir(""),
        encryptKey(""),
        signKey(""),
        backend(""),
        passphrase(""),
        signPassphrase(""),
        p_duplicity("/") {}

  autocore(std::string _targetDir, std::string _sourceDir,
           std::string _encryptKey, std::string _signKey, std::string _backend,
           std::string _passphrase, std::string _signPassphrase,
           boost::filesystem::path _p_duplicity)
      : targetDir(std::move(_targetDir)),
        sourceDir(std::move(_sourceDir)),
        encryptKey(std::move(_encryptKey)),
        signKey(std::move(_signKey)),
        backend(std::move(_backend)),
        passphrase(std::move(_passphrase)),
        signPassphrase(std::move(_signPassphrase)),
        p_duplicity(std::move(_p_duplicity)) {}
};

#endif  // AUTOCORE_H
