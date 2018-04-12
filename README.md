# Duplicity-auto

## Intro

Duplicity-auto is a small console application written in C++ used by myself for simplify duplicity backup to Google Drive. The application itself doesn't include **duplicity** and **pydrive** so make sure these two applications are already installed. You can check this [post](https://redplus.me/post/using-duplicity-with-google-drive-for-backup-on-macos/) for how to install the two applications.

On the first run, it will ask for input:

* targetDir (which directory on Google Drive the backup files are stored)
* sourceDir (which directory you want to backup)
* encryptKey (gpg key id for encryption)
* signKey (gpg key id for signing)
* passphrase (passphrase of encryption key)
* signPassphrase (passphrase of signing key)

The program will store above information to a **config.json** file in the current working directory for later use so you don't need to input once again. Because the information is stored in plain text without encryption, you may **NOT** want to use your current gpg keys for encryption and signing.

Whether to perform a full back or a incremental backup depends on following:

* If the total size of incremental backup files since last full backup is larger than half of the size of last full backup, a full backup will be performed.
* If the date of last full backup is 7 days before current date, a full backup will be performed.

Whenever a full backup is performed, all backup files before this full backup will be deleted.

The program is only tested on macOS 10.13.4 with Apple LLVM version 9.1.0 (clang-902.0.39.2) and depends on Boost libraries only. It should work on WINDOWS as well.

## License

```
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
```