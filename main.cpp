#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <fstream>
#include "autocore.h"

namespace pt = boost::property_tree;
namespace bp = boost::process;
namespace bg = boost::gregorian;

autocore get_obj() {
  boost::filesystem::path p_duplicity =
      boost::process::search_path("duplicity");
  std::string targetDir;
  std::string sourceDir;
  std::string encryptKey;
  std::string signKey;
  std::string backend = "pydrive+gdocs://developer.gserviceaccount.com/";
  std::string passphrase;
  std::string signPassphrase;

  std::cout << "Which directory to backup:" << std::endl;
  std::cin >> sourceDir;
  while (!(boost::filesystem::is_directory(sourceDir))) {
    std::cout << sourceDir
              << " is not a valid directory!\nWhich directory to backup:"
              << std::endl;
    std::cin >> sourceDir;
  }

  std::cout << "Backup to which directory on Google Drive, default is root:"
            << std::endl;
  std::cin >> targetDir;
  if (targetDir[0] == '/') {
    targetDir = targetDir.substr(1);
  }

  std::cout << "GPG encrypt key id:" << std::endl;
  std::cin >> encryptKey;

  std::cout << "GPG sign key id:" << std::endl;
  std::cin >> signKey;

  std::cout << "Passphrase for encrypt key:" << std::endl;
  std::cin >> passphrase;

  std::cout << "Passphrase for sign key:" << std::endl;
  std::cin >> signPassphrase;

  autocore d(targetDir, sourceDir, encryptKey, signKey, backend, passphrase,
             signPassphrase, p_duplicity);

  return d;
}

int main() {
  pt::ptree root;
  bool config = false;

  if (boost::filesystem::exists("config.json")) {
    config = true;
    pt::read_json("config.json", root);
    std::cout << "config.json is detected, reload it..." << std::endl;
  }

  if (!config) {
    autocore dup = get_obj();
    bool isFull = true;
    uint64_t sz = dup.performBackup(isFull);

    std::cout << "New full backup files size: " << sz << std::endl;

    root.put("targetDir", dup.targetDir);
    root.put("sourceDir", dup.sourceDir);
    root.put("encryptKey", dup.encryptKey);
    root.put("signKey", dup.signKey);
    root.put("backend", dup.backend);
    root.put("passphrase", dup.passphrase);
    root.put("signPassphrase", dup.signPassphrase);
    root.put("duplicity_path", dup.p_duplicity.string());
    root.put("lastFullDate",
             boost::gregorian::to_iso_string(bg::day_clock::local_day()));
    root.put("lastFullSize", sz);
    root.put("lastIncrDate", "00000000");
    root.put("totalIncrSize", 0);

    std::ofstream file;
    file.open("config.json", std::fstream::out);
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
      uint64_t sz = dup.performBackup(isFull);
      std::cout << "New full backup files size: " << sz << std::endl;

      root.put("lastFullDate", bg::to_iso_string(cur));
      root.put("lastFullSize", sz);
      root.put("lastIncrDate", "00000000");
      root.put("totalIncrSize", 0);
    } else {
      uint64_t sz = dup.performBackup(isFull);
      std::cout << "New incr backup files size: " << sz << std::endl;

      root.put("lastIncrDate", bg::to_iso_string(cur));

      uint64_t last_incr_size = root.get<uint64_t>("totalIncrSize");
      root.put("totalIncrSize", sz + last_incr_size);
    }

    std::ofstream file;
    file.open("config.json", std::fstream::out | std::fstream::trunc);
    pt::write_json(file, root);
    file.close();
  }

  return 0;
}
