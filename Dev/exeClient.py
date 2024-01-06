# This file is for developer testing purposes only.
# It tests whether or not our HTTP server is working properly.
# When we **actually** want to run this, everything would be done via Powershell. It's just easier to write the test code with Python

import requests

# Authorization key used by auth.js
AUTH_KEY = "efeewf"
# The EXE code we want to compile and fetch. See exe_data.js for how this is handled
EXE_REQUESTED = "none"

response = requests.get('http://127.0.0.1', headers={"authorization": "efeewf", "exe": EXE_REQUESTED})
print(response.text)