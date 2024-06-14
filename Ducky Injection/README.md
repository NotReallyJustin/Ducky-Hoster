# Ducky Injection
This folder contains all the scripts to actually download and run our malware on their computer. <br>
Ideally, we're able to plug in our malware via a USB Keyboard, get access to admin perms by bypassing any potential UAC, and inject our PowerShell payload. <br>
Privilege escalation might be necessary down the line because I don't know how much we'll be able to do with Windows' C SDK, so we might have to make liberal use of `PowerShell` scripts <br> <br>
Which is fine - I could code in PowerShell but I prefer to not go that messy route if I don't have to

## Contents:
* `run_malware` - A Powershell script for the USB Keyboard to write in order to download and run our `.exe` payload