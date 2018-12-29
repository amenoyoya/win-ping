%~d0
cd %~dp0

cl /c /O2 /Oi /EHsc getopt.cpp ipv4.cpp win-ping.cpp
cl /c /O2 /Oi /EHsc test\getopt-test.cpp test\ping-test.cpp

link /machine:x86 /out:win-ping.exe win-ping.obj getopt.obj ipv4.obj
link /machine:x86 /out:getopt-test.exe getopt-test.obj getopt.obj
link /machine:x86 /out:ping-test.exe ping-test.obj ipv4.obj

del getopt.obj ipv4.obj win-ping.obj getopt-test.obj ping-test.obj

move win-ping.exe bin\
move getopt-test.exe bin\
move ping-test.exe bin\