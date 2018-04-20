
//          Copyright blackgiulia.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "handle.hpp"

namespace bp = boost::process;
namespace bf = boost::filesystem;
namespace pt = boost::property_tree;

handle::handle(std::string _targetDir, std::string _sourceDir,
               std::string _encryptKey, std::string _signKey,
               std::string _backend, std::string _passphrase,
               std::string _signPassphrase, bf::path _p_duplicity)
    : targetDir(std::move(_targetDir)),
      sourceDir(std::move(_sourceDir)),
      encryptKey(std::move(_encryptKey)),
      signKey(std::move(_signKey)),
      backend(std::move(_backend)),
      passphrase(std::move(_passphrase)),
      signPassphrase(std::move(_signPassphrase)),
      p_duplicity(std::move(_p_duplicity)) {}

handle::handle(const pt::ptree &root)
    : targetDir(root.get<std::string>("targetDir")),
      sourceDir(root.get<std::string>("sourceDir")),
      encryptKey(root.get<std::string>("encryptKey")),
      signKey(root.get<std::string>("signKey")),
      backend(root.get<std::string>("backend")),
      passphrase(root.get<std::string>("passphrase")),
      signPassphrase(root.get<std::string>("signPassphrase")),
      p_duplicity(root.get<std::string>("duplicity_path")) {}

uint64_t handle::performBackup(const bool &isFull) const {
  std::string argv;

  if (isFull) {
    argv = "full";
  } else {
    argv = "incr";
  }

  bp::ipstream is;

  uint64_t backupSize = 0;

  bp::child c1(p_duplicity, argv, "--encrypt-key", encryptKey, "--sign-key",
               signKey, "--gpg-options", "--cipher-algo=AES256",
               "--allow-source-mismatch", sourceDir, backend + targetDir,
               bp::std_out > is, bp::env["PASSPHRASE"] = passphrase,
               bp::env["SIGN_PASSPHRASE"] = signPassphrase);
  c1.wait();

  std::string line;

  while (is && std::getline(is, line)) {
    if (line.empty()) {
      continue;
    }

    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string s;

    while (getline(ss, s, ' ')) {
      tokens.push_back(s);
    }

    if (tokens[0] == "TotalDestinationSizeChange") {
      std::stringstream sz(tokens[1]);
      sz >> backupSize;
      break;
    }
  }

  bp::child c2(p_duplicity, "cleanup", "--force", backend + targetDir,
               bp::std_out > bp::null, bp::env["PASSPHRASE"] = passphrase,
               bp::env["SIGN_PASSPHRASE"] = signPassphrase);
  c2.wait();

  if (argv == "full") {
    bp::child c3(p_duplicity, "remove-all-but-n-full", "1", "--force",
                 backend + targetDir, bp::std_out > bp::null);
    c3.wait();
  }

  return backupSize;
}

boost::property_tree::ptree handle::writeToPT() const {
  pt::ptree root;

  root.put("targetDir", targetDir);
  root.put("sourceDir", sourceDir);
  root.put("encryptKey", encryptKey);
  root.put("signKey", signKey);
  root.put("backend", backend);
  root.put("passphrase", passphrase);
  root.put("signPassphrase", signPassphrase);
  root.put("duplicity_path", p_duplicity.string());

  return root;
}

boost::property_tree::ptree readFromJson(
    const boost::filesystem::path &config_path) {
  pt::ptree root;
  bf::ifstream file;
  file.open(config_path, bf::ifstream::in);
  pt::read_json(file, root);
  file.close();
  return root;
}

void writeToJson(const boost::property_tree::ptree &root,
                 const boost::filesystem::path &config_path) {
  bf::ofstream file;
  if (!bf::exists(config_path)) {
    file.open(config_path, bf::ofstream::out);
  } else {
    file.open(config_path, bf::ofstream::out | bf::ofstream::trunc);
  }
  pt::write_json(file, root);
  file.close();
}

std::pair<std::string, std::string> get_keys() {
  auto p_gpg = bp::search_path("gpg");
  bp::ipstream is;
  bp::child c1(p_gpg, "--list-keys", bp::std_out > is);
  c1.wait();

  std::vector<std::string> keys;
  int idx = 1;
  std::string line;

  while (is && std::getline(is, line)) {
    if (line.empty()) {
      continue;
    }
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string s;
    while (getline(ss, s, ' ')) {
      tokens.push_back(s);
    }
    if (tokens.size() == 7) {
      keys.push_back(tokens[6]);
    }
    if (tokens[0] == "pub") {
      std::cout << "(" << idx++ << ") " << line << std::endl;
    } else {
      std::cout << "    " << line << std::endl;
    }
    if (tokens[0] == "sub") {
      std::cout << std::endl;
    }
  }

  std::cout << "Select keys for encryption. Type number. ";
  std::cin >> idx;

  std::string encrypt_key = keys[idx - 1];

  std::cout << "\nSelect keys for signing. Type number. ";
  std::cin >> idx;

  std::string sign_key = keys[idx - 1];

  return std::make_pair(encrypt_key, sign_key);
}

handle get_obj(const std::string &backend) {
  auto p_duplicity = bp::search_path("duplicity");
  std::string targetDir;
  std::string sourceDir;
  std::string passphrase;
  std::string signPassphrase;

  auto keys = get_keys();
  std::string encryptKey = keys.first;
  std::string signKey = keys.second;

  std::cout << "\nPassphrase for encrypt key:" << std::endl;
  std::cin >> passphrase;

  std::cout << "\nPassphrase for sign key:" << std::endl;
  std::cin >> signPassphrase;

  std::cout << "\nWhich directory to backup:" << std::endl;
  std::cin >> sourceDir;
  while (!(bf::is_directory(sourceDir))) {
    std::cout << sourceDir
              << " is not a valid directory!\nWhich directory to backup:"
              << std::endl;
    std::cin >> sourceDir;
  }

  std::cout << "\nBackup to which directory on Google Drive, default is root:"
            << std::endl;
  std::cin >> targetDir;
  if (targetDir[0] == '/') {
    targetDir = targetDir.substr(1);
  }

  handle pydriveHandle(targetDir, sourceDir, encryptKey, signKey, backend,
                       passphrase, signPassphrase, p_duplicity);

  return pydriveHandle;
}
