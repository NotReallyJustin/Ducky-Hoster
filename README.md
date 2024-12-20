# Ducky-Hoster
Malware system that uses an Arduino USB to open a reverse shell to download and execute malicious scripts from the host Node server. <br />
🚨 Server keys on Github are not up to date (for security reasons, and also because you could generate your own server keys)

# Download Instructions
All you really need is everything under the `./Server` directory. <br />
It's reccomended you download files through stable release "versions" on Github rather than directly pulling from `$git clone` since the current code may very well be buggy.

## Node Dependencies
* Express 4.21.1
    * It's a bit unconventional for us to use Express without deploying a web app, but it's a good way to handle HTTP requests even if we're not serving files
* Express Body Parser 1.20.3

To install the `node_modules`:
```ps1
cd ./Server/
npm install 
```

## Other Dependencies
* MinGW.org GCC-6.3.0-1
* Windows OS (You don't need to do anything extra here. Some executables dynamically link `wininet` - which comes preinstalled in Windows OS)

# Compiling Instructions
## Overview
Originally, we wanted Ducky Hoster to be able to use `$gcc` (via `mingw`) to compile files on the fly (due to each malware executable having different requirements for compiling). However, although we could mitigate the risks by using prepared statements and forcing Node to not spawn a subshell when executing `$gcc`, I thought it'd probably be safer if we precompiled everything we needed, and just have Ducky Hoster serve as a "CDN" for exe files.
<br>
However, given the fact that there are a few Ducky Hoster executables that require us to generate (and input) a new key for exfiltration purposes, we are going to compromise and do the following:

* Common Libraries will be precompiled into static files
* Necessary libraries will also be precompiled into static files (`.a`)
* Malware Executables will be compiled into an object file (`.o`) beforehand so the only thing left would be the linking phase of $GCC
* When the Node server recieves a Get Request to create a malware, it will run `./make.ps1` with a given write code (if necessary) to finish linking phase and add the macros

## Compiling/Assembling Static Libraries
All the precompiling work has already been done for you. Since I'm developing this Software on Windows OS (for obvious reasons), we're going to use powershell. <br>
```ps1
# This assumes you are in the current working directory
powershell.exe -ExecutionPolicy Bypass -File ./make.ps1
```
ExecutionPolicy is not a Windows security feature. Hence, this script simply bypasses it. <br>
There are some `.exe` and library files that are precompiled. However, these are for 64-bit Windows 11 machines. It's reccomended you recompile them for your own system.

## Compiling/Assembling Malware Executables
You can just use $gcc for this. Remember to use -I to link necessary header files.
```ps1
gcc -c use_rand.c -o [OUTPUT NAME].o -I[Path to header file]
```

## [Runtime] Linking Malware Executabkle
You should never be linking malware executables on your own. The Node server will automatically do that upon runtime after validating the GET request. For security reasons, the Node server do this via an already prepared `./make.ps1` file where the only parameter you could pass in is a Macro for the POST key.
```ps1
powershell.exe -ExecutionPolicy Bypass -File ./make.ps1 -SVL_ADDRESS [Server Address] -SVL_AUTHKEY [Auth Key]
```