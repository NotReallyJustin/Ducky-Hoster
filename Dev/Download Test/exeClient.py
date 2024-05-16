# This file is for developer testing purposes only.
# It tests whether or not our HTTP server is working properly.
# When we **actually** want to run this, everything would be done via Powershell. It's just easier to write the test code with Python

import requests

# Authorization key used by auth.js
AUTH_KEY = "tmMEQW=S0_O}<0F_1]iadHUH5MyBZqfVcRW3`vEG8v?5t[p81QQ4qh^YSix{P0`DuTvg81`D[YEaSjYOgQJu<~LRAAsF3>~@Bg~seca{OuUTBq?<^zevI}C@lTz[8UNa:C7?>]Vfd1mXs7nvV]C^v;M|^:dZ&aA3`x9jK6IQI<f2ecFcP=i?wzhKB0XQ>9Wg`V<8wINFj{LLs=1qrbDV82<t8NeX8{Lr]j}4s4zx;Jp^aP13uprR;Q49;1wIx<s[{Y&Imq9Az9p;}T5@NdSVY7Az]8`XDMon]IT==vC[FXv@at]&bz=pW=l<AS8Q4Oe6o44:OrOBiW:aAhK{4StA:_SQlOfU1^4UHmNc_<hihr&j@]]O5@23`usV&Ew^aJvtidXP<1BIY8Rn4CCblsqdHnZ;BarIC5LQ5AASL4T<O9f3dDRF]cwl`JbjOLIZ}R;`tYd9X6w|3}a99>iSQt<IcUy^k4yMHBWA__6e:3ouRp7O5Y4;U;CipxYJ}Zzje56["
# The EXE code we want to compile and fetch. See exe_data.js for how this is handled
EXE_REQUESTED = "enumerator"

response = requests.get('http://127.0.0.1', headers={"authorization": AUTH_KEY, "exe": EXE_REQUESTED})
with open("ls_read.exe", "wb") as exe_file:         # Use b flag for bytes
    exe_file.write(response.content)