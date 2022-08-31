# Build the library and get a debug version
pushd; 
cd ..\Nya-utils\; 
.\build.ps1; 
popd; 
rm extern\libs\libnya-utils.so;
Remove-Item -Recurse -Force "extern\includes\nya-utils";
Copy-Item -Path "..\Nya-utils\build\debug\libnya-utils.so" -Destination "extern\libs\libnya-utils.so" -Recurse
Copy-Item -Path "..\Nya-utils\shared" -Destination "extern\includes\nya-utils\shared\" -Recurse

adb push extern/libs/libnya-utils.so /sdcard/Android/data/com.beatgames.beatsaber/files/mods/libnya-utils.so

./copy.ps1
# ./profile.ps1
./start-logging.ps1