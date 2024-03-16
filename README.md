# Ducky-Hoster
Malware system that uses an Arduino USB to open a reverse shell to download and execute malicious scripts from the host Node server. <br />
🚨 Server keys on Github are not up to date (for security reasons, and also because you could generate your own server keys)

# Node Dependencies
* Express 4.18.2
    * It's a bit unconventional for us to use Express without deploying a web app, but it's a good way to handle HTTP requests even if we're not serving files

# Other Dependencies
* MinGW.org GCC-6.3.0-1
* (lib)curl 8.6.0

# Compiling Instructions
Originally, we wanted Ducky Hoster to be able to use `$gcc` (via `mingw`) to compile files on the fly. However, although we could mitigate the risks by using prepared statements and forcing Node to not spawn a subshell when executing `$gcc`, I thought it'd probably be safer if we precompiled everything we needed, and just have Ducky Hoster serve as a "CDN" for exe files.
<br> <br>

All the precompiling work has already been done for you. Since I'm developing this Software on Windows OS (for obvious reasons), we're going to use powershell. <br>
```ps1
# This assumes you are in the current working directory
powershell.exe -ExecutionPolicy Bypass -File ./make.ps1
```
ExecutionPolicy is not a Windows security feature. Hence, this script simply bypasses it.