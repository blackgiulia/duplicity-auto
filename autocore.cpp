#include "autocore.h"

autocore::autocore(std::string s1, std::string s2, std::string s3,
                   std::string s4, std::string s5, std::string s6,
                   std::string s7, boost::filesystem::path p1) {
  targetDir = s1;
  sourceDir = s2;
  encryptKey = s3;
  signKey = s4;
  backend = s5;
  passphrase = s6;
  signPassphrase = s7;
  p = p1;
}

bg::date autocore::lastFullDate() {
  bp::ipstream is;
  bp::child c(p, "collection-status", backend + targetDir,
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

uint64_t autocore::performBackup(bool &isFull) {
  std::string argv;
  if (isFull)
    argv = "full";
  else
    argv = "incr";
  bp::ipstream is;
  uint64_t res;
  bp::child c1(p, argv, "--encrypt-key", encryptKey, "--sign-key", signKey,
               "--gpg-options", "--cipher-algo=AES256",
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
  bp::child c2(p, "cleanup", "--force", backend + targetDir,
               bp::env["PASSPHRASE"] = passphrase,
               bp::env["SIGN_PASSPHRASE"] = signPassphrase);
  c2.wait();
  if (argv == "full") {
    bp::child c3(p, "remove-all-but-n-full", "1", "--force",
                 backend + targetDir);
    c3.wait();
  }
  return res;
}
