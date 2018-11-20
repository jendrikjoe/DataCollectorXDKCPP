# DataCollectorXDKCPP
An example of a sensor data collector in C++ utilising the Bosch XDK.

# Bug resolving

The simplelink.h header has an issue which only appears under C++
remove the lines

	#ifdef    __cplusplus
	}
	#endif /*  __cplusplus */

in line 371 to 373.

# Usage 
- Create an XDK Project in the XDK-Workbench
- Copy all data in the source folder to the projects source folder.
- Replace the projects Makefile
- Make a backup of the application.mk file in the XDK/SDK/xdk110/Common folder and replace it with the one from the repository.
- Build and flash the application as usaual.
