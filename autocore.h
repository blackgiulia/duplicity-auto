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
      : targetDir(_targetDir),
        sourceDir(_sourceDir),
        encryptKey(_encryptKey),
        signKey(_signKey),
        backend(_backend),
        passphrase(_passphrase),
        signPassphrase(_signPassphrase),
        p_duplicity(_p_duplicity) {}
};

#endif  // AUTOCORE_H
