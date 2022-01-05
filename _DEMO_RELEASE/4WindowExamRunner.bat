start GPP_TEST_RELEASE.exe 30 30

start GPP_TEST_RELEASE.exe 1000 30

start GPP_TEST_RELEASE.exe 30 540

start GPP_TEST_RELEASE.exe 1000 540
echo new ActiveXObject("WScript.Shell").AppActivate("GPP_TEST_RELEASE.exe"); > tmp.js
cscript //nologo tmp.js & del tmp.js