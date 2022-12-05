cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(Antivirus)


add_executable(Antivirus Antivirus/Analyzer/Analyzer.cpp Antivirus/Core/Core.cpp Antivirus/Core/Main.cpp Antivirus/Core/SmartKiller.cpp Antivirus/Syscalls/Daemon.cpp Antivirus/Syscalls/SyscallsHandler.cpp Antivirus/Core/Utils.cpp)

add_executable(TestVirus TestVirus/TestVirus.c)

add_executable(Commander Commander/Commander.cpp)
