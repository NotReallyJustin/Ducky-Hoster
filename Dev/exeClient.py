# This file is for developer testing purposes only.
# It tests whether or not our HTTP server is working properly.
# When we **actually** want to run this, everything would be done via Powershell. It's just easier to write the test code with Python

import requests

# Authorization key used by auth.js
AUTH_KEY = "SI>?zOkqd5@LMsXmZ[;Q8D2Tnr?P_QzmRmz`[ZZaHi7|t_{HM5d<t2DfXdM[`=^>R0CWUv^StjaDABJGI2l`OvANPwy2KF6Q@4GPpN@XmwCa?^F4z<5<?d_Dd:?IGcnz_T4^[pX|=]uUxOq]ysxf;@nPhoCE]cM]dAY1uIs6Vt{jJMkNmtlM7xGFeh|V[LxeYPS;<sW1`g=WS5MF5DoQ7vD4HLvAGmnokXJ77[7qil95KJiXQUw1Ort4Ae{^K?hpzcCfwX6Xf2JsX2lhb>ml2_CSR8TZ57q=_jP9[Sw[Wa}3zarVe6I&qR1HHEv1LIN6vI^Tt3wUa1{oW7BzTylh@qMMeWCot]<zrBKRdF^`4KkQ}uP^lm5bev|7=:zj3M0Y5]?a[FhQiL|UCIwI1ZJy28<Aa<&aiqdDh}8lI21b=rN_=8slea}[79JyHIJxb7Izx2Rq2?mR{tJiJ8Y0wOcE`eHfgSHQonSZhz35A=tBKb^};h_VDpvchVk2zQ_~555x"
# The EXE code we want to compile and fetch. See exe_data.js for how this is handled
EXE_REQUESTED = "enumerator"

response = requests.get('http://127.0.0.1', headers={"authorization": AUTH_KEY, "exe": EXE_REQUESTED})
with open("ls_read.exe", "wb") as exe_file:         # Use b flag for bytes
    exe_file.write(response.content)