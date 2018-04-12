#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "autocore.h"

namespace bf = boost::filesystem;
namespace bg = boost::gregorian;
namespace bp = boost::process;
namespace pt = boost::property_tree;

autocore get_obj() {
  bf::path p_duplicity = bp::search_path("duplicity");
  std::string targetDir;
  std::string sourceDir;
  std::string encryptKey;
  std::string signKey;
  std::string backend = "pydrive+gdocs://developer.gserviceaccount.com/";
  std::string passphrase;
  std::string signPassphrase;

  std::cout << "Which directory to backup:" << std::endl;
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

  std::cout << "\nGPG encrypt key id:" << std::endl;
  std::cin >> encryptKey;

  std::cout << "\nGPG sign key id:" << std::endl;
  std::cin >> signKey;

  std::cout << "\nPassphrase for encrypt key:" << std::endl;
  std::cin >> passphrase;

  std::cout << "\nPassphrase for sign key:" << std::endl;
  std::cin >> signPassphrase;

  autocore d(targetDir, sourceDir, encryptKey, signKey, backend, passphrase,
             signPassphrase, p_duplicity);

  return d;
}

int main() {
  pt::ptree root;
  bool config = false;

  auto config_path = bf::current_path();
  config_path += bf::path("/config.json");

  if (bf::exists(config_path)) {
    config = true;
    bf::ifstream file;
    file.open(config_path, bf::ifstream::in);
    pt::read_json(file, root);
    file.close();
    std::cout << "config.json is detected, reload successfully..." << std::endl;
  }

  if (!config) {
    autocore dup = get_obj();
    bool isFull = true;

    std::cout << "\nDoing a full backup..." << std::endl;

    uint64_t sz = dup.performBackup(isFull);

    std::cout << "\nNew full backup files size: " << sz << std::endl;

    root.put("targetDir", dup.targetDir);
    root.put("sourceDir", dup.sourceDir);
    root.put("encryptKey", dup.encryptKey);
    root.put("signKey", dup.signKey);
    root.put("backend", dup.backend);
    root.put("passphrase", dup.passphrase);
    root.put("signPassphrase", dup.signPassphrase);
    root.put("duplicity_path", dup.p_duplicity.string());
    root.put("lastFullDate", bg::to_iso_string(bg::day_clock::local_day()));
    root.put("lastFullSize", sz);
    root.put("lastIncrDate", "00000000");
    root.put("totalIncrSize", 0);

    bf::ofstream file;
    file.open(config_path, bf::ofstream::out);
    pt::write_json(file, root);
    file.close();
  } else {
    autocore dup(
        root.get<std::string>("targetDir"), root.get<std::string>("sourceDir"),
        root.get<std::string>("encryptKey"), root.get<std::string>("signKey"),
        root.get<std::string>("backend"), root.get<std::string>("passphrase"),
        root.get<std::string>("signPassphrase"),
        root.get<std::string>("duplicity_path"));

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

      uint64_t sz = dup.performBackup(isFull);
      std::cout << "\nNew full backup files size: " << sz << std::endl;

      root.put("lastFullDate", bg::to_iso_string(cur));
      root.put("lastFullSize", sz);
      root.put("lastIncrDate", "00000000");
      root.put("totalIncrSize", 0);
    } else {
      std::cout << "\nDoing a incremental backup..." << std::endl;
      uint64_t sz = dup.performBackup(isFull);
      std::cout << "\nNew incr backup files size: " << sz << std::endl;

      root.put("lastIncrDate", bg::to_iso_string(cur));

      uint64_t last_incr_size = root.get<uint64_t>("totalIncrSize");
      root.put("totalIncrSize", sz + last_incr_size);
    }

    bf::ofstream file;
    file.open(config_path, bf::ofstream::out | bf::ofstream::trunc);
    pt::write_json(file, root);
    file.close();
  }

  return 0;
}
