# Chip_8_Emulator
> Designed for Visual Studio Standard C++ 20 ISO

## Introduction
Emulator for the interpreted programming language CHIP8, developed using C++ and [*CLove*](https://github.com/fdefelici/vs-clove-unit/tree/master) header for testing.
The application allow to emulate various Games (written in CHIP8) like TICTAC.

## SetUp
The project is divided in 3 different solution 


![devenv_uD7NNJJ3OT](https://github.com/Rene9267/Chip_8_Emulator/assets/78114332/bd8d6c3a-2b00-42e4-b77d-1e1290daa61c)  

--------------------------------------
# Application 

Contains the Main for run the application:
- Main.cpp = you have to store here the path for the C8 game
--------------------------------------
# Library

Static library linked to Application and test.
Store the actual code of the Emulator:
- CHIP8Emulator.cpp/.h
--------------------------------------
# Test

Solution responsible to the test execution.
Contains:
- CHIP_Suite.cpp = implement all the suite case for the tests
- Main.cpp = execute the test suite
- clove-unit.h
--------------------------------------

### TO BE CONTINUED
https://github.com/Rene9267/Chip_8_Emulator/assets/78114332/bda70e5d-8abb-49d6-8767-b332d0f55bb1


