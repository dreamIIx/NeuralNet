make

if [%1] == [] (
    .\Debug\Test.exe
) else (
    if "%~1" == "-g"   (
        SHIFT /1
        gdb .\Debug\Test.exe %*
    ) else (
        .\Debug\Test.exe %*
        ))