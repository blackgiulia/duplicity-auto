
//          Copyright blackgiulia.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef DUPLICITYAUTO_HANDLE_HPP
#define DUPLICITYAUTO_HANDLE_HPP

#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

class handle {
 private:
  std::string targetDir;
  std::string sourceDir;
  std::string encryptKey;
  std::string signKey;
  std::string backend;
  std::string passphrase;
  std::string signPassphrase;
  boost::filesystem::path p_duplicity;

 public:
  handle(std::string _targetDir, std::string _sourceDir,
         std::string _encryptKey, std::string _signKey, std::string _backend,
         std::string _passphrase, std::string _signPassphrase,
         boost::filesystem::path _p_duplicity);

  explicit handle(const boost::property_tree::ptree &root);

  uint64_t performBackup(const bool &isFull) const;

  boost::property_tree::ptree writeToPT() const;
};

boost::property_tree::ptree readFromJson(
    const boost::filesystem::path &config_path);

void writeToJson(const boost::property_tree::ptree &root,
                 const boost::filesystem::path &config_path);

std::pair<std::string, std::string> get_keys();

handle get_obj(const std::string &backend);

#endif  // DUPLICITYAUTO_HANDLE_HPP
