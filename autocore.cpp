#include "autocore.h"

namespace bp = boost::process;
namespace bg = boost::gregorian;

boost::gregorian::date autocore::lastFullDate() const {
  bp::ipstream is;
  bp::child c(p_duplicity, "collection-status", backend + targetDir,
              boost::process::std_out > is);
  c.wait();

  std::string line;
  bg::date res(bg::min_date_time);
  while (is && std::getline(is, line)) {
    if (line.empty()) continue;
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string s;
    while (getline(ss, s, ' ')) {
      tokens.push_back(s);
    }
    if (tokens.size() == 9 && tokens[0] == "Last" && tokens[1] == "full") {
      std::string d = tokens[5] + '-' + tokens[6] + '-' + tokens[8];
      res = bg::from_us_string(d);
      break;
    }
  }

  return res;
}

uint64_t autocore::performBackup(const bool &isFull) const {
  std::string argv;
  if (isFull)
    argv = "full";
  else
    argv = "incr";

  bp::ipstream is;
  uint64_t res;
  bp::child c1(p_duplicity, argv, "--encrypt-key", encryptKey, "--sign-key",
               signKey, "--gpg-options", "--cipher-algo=AES256",
               "--allow-source-mismatch", sourceDir, backend + targetDir,
               boost::process::std_out > is, bp::env["PASSPHRASE"] = passphrase,
               bp::env["SIGN_PASSPHRASE"] = signPassphrase);
  c1.wait();

  std::string line;
  while (is && std::getline(is, line)) {
    if (line.empty()) continue;
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string s;
    while (getline(ss, s, ' ')) {
      tokens.push_back(s);
    }
    if (tokens[0] == "TotalDestinationSizeChange") {
      std::stringstream sz(tokens[1]);
      sz >> res;
      break;
    }
  }

  bp::child c2(p_duplicity, "cleanup", "--force", backend + targetDir,
               bp::env["PASSPHRASE"] = passphrase,
               bp::env["SIGN_PASSPHRASE"] = signPassphrase);
  c2.wait();

  if (argv == "full") {
    bp::child c3(p_duplicity, "remove-all-but-n-full", "1", "--force",
                 backend + targetDir);
    c3.wait();
  }

  return res;
}
