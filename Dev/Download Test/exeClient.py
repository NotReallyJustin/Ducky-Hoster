# This file is for developer testing purposes only.
# It tests whether or not our HTTP server is working properly.
# When we **actually** want to run this, everything would be done via Powershell. It's just easier to write the test code with Python

import requests

# Authorization key used by auth.js
AUTH_KEY = "zNBc:[wKWEwComu<bQ[e>rTzPTpcJII<JDP1RZQ1H3W1XyOf8d=CjH]F}S;jU7wM>xC9eDsN>9aUz;T&&1=oqji{0zRnrYN9liaw<Db[fRGr;i4WpAP9F7g4&5f;tSX`O7H0gn>QA9scgdx}X5[eKcV[nRPc:dQ[99w&h{M{_c1AZZnyYe267c{{rUgwICBGeE:?Ev9dPAngKUgLSM1eTf&@Ok_dD7hWdp21v97J@{P:A:|R2Jq=Lc^cPH6[pFaQjLnOaY;&h3QzkXeF58mVjqyHrDC[<BOiHT1MAb8Tg5jSC]lGz`l<h9pHC9?@;pP[h9l<KtBlfM=@NTyWLgq2{{<rs7^7ikT;6Hc6HwVPybne@TfR7|&6<V@vpID57=G7ADkPJ{;7576VNPT_GlaWWOv[;ZC?Q8Gze>[Pu6G84BOGupUIYxrkTPQ9Zvx=fpAnK|tDooy&7h|VLBg445N&F|=mDHv}54W`I5wX_m}Qa55dkJM=u[=}nJzFQdcitG7r"
# The EXE code we want to compile and fetch. See exe_data.js for how this is handled
EXE_REQUESTED = "enumerator"

response = requests.get('http://127.0.0.1', headers={"authorization": AUTH_KEY, "exe": EXE_REQUESTED})
with open("ls_read.exe", "wb") as exe_file:         # Use b flag for bytes
    exe_file.write(response.content)