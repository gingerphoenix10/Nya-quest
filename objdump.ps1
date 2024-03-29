if (Test-Path "./ndkpath.txt")
{
    $NDKPath = Get-Content ./ndkpath.txt
} else {
    $NDKPath = $ENV:ANDROID_NDK_HOME
}


& $NDKPath\toolchains\llvm\prebuilt\windows-x86_64\bin\llvm-objdump.exe -x -T .\build\libNya.so > objdump.txt

