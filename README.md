# SAPPOROBDD

BDD Package SAPPORO-Edition  (Shin-ichi Minato, Kyoto Univ. 2021)

# Requirement

g++ (GCC) 7.3.0 or later

# Installation

First go to ./src (cd src) and execute a script (source INSTALL) or (source INSTALL32) to compile the BDD package into a library file ../lib/BDD64.a or ../lib/BDD32.a. Then go to a directory under ../app to make an application program using the BDD package.

# Author

Shin-ichi MINATO  
at Graduate School of Informatics, Kyoto University.  
(https://www.lab2.kuis.kyoto-u.ac.jp/minato/)

# License

SAPPOROBDD is under MIT license (https://en.wikipedia.org/wiki/MIT_License)

# Files

__include/*__ :      Header files.  
__src/*__ :          Source files of BDD package.  
__src/INSTALL__ :   Script for compiling all files in src/ .  
__src/INSTALL32__ : Script for compiling all files in src/ on 32bit machine.  
__src/INSTALL_LCM__ : Script for compiling all files in src/ with LCM-library.  
__src/INSTALL32_LCM__ : Script for compiling all files in src/ on 32bit machine with LCM-library.  
__src/CLEAN__ :     Script for deleting all object files in src/ .   
__src/BDDc/*__ :     Core of package written in C.  
__src/BDDXc/*__ :    Part of package related to X11, written in C.  
__src/BDDLCMc/*__ :  Part of package related to LCM, written in C.  
__src/BDD+/*__ :     Main part of package written in C++.  
__lib/*__ :           Compiled BDD library files are stored here.  
__app/*__ :          Source programs of BDD applications.  
__man/*__ :          Manuals of BDD package. (So far Japanese only.)  


