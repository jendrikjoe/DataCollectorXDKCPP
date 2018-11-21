# This makefile triggers the targets in the application.mk

# The default value "../../.." assumes that this makefile is placed in the 
# folder xdk110/Apps/<App Folder> where the BCDS_BASE_DIR is the parent of 
# the xdk110 folder.
BCDS_BASE_DIR ?= ../../..

# Macro to define Start-up method. change this macro to "CUSTOM_STARTUP" to have custom start-up.
export BCDS_SYSTEM_STARTUP_METHOD = DEFAULT_STARTUP
export BCDS_APP_NAME = LEDsCPP
export BCDS_APP_DIR = $(CURDIR)
export BCDS_APP_SOURCE_DIR = $(BCDS_APP_DIR)/source

#Please refer BCDS_CFLAGS_COMMON variable in application.mk file
#and if any addition flags required then add that flags only in the below macro 
#export BCDS_CFLAGS_COMMON = 

#Below settings are done for optimized build.Unused common code is disabled to reduce the build time
export XDK_CONNECTIVITY_BLE=0
export XDK_CONNECTIVITY_HTTPRESTCLIENT=0
export XDK_CONNECTIVITY_LORA=0
export XDK_CONNECTIVITY_LWM2M=1
export XDK_CONNECTIVITY_MQTT=1
export XDK_CONNECTIVITY_UDP=0
export XDK_CONNECTIVITY_WLAN=1

export XDK_SENSOR_SENSOR=1
export XDK_SENSOR_VIRTUALSENSOR=1
export XDK_SENSOR_EXTERNALSENSOR=1

export XDK_UTILITY_STORAGE=1
export XDK_UTILITY_SNTP=0
export XDK_UTILITY_SERVALPAL=1
#Enabled feature macros for common code
export XDK_CONNECTIVITY_LED=1
export XDK_SENSOR_BUTTON=1
#end of settings related to optimized build

#List all the application header file under variable BCDS_XDK_INCLUDES 
#export BCDS_XDK_INCLUDES = \
	
#List all the application source file under variable BCDS_XDK_APP_SOURCE_FILES in a similar pattern as below
export BCDS_XDK_APP_SOURCE_FILES = \
	$(BCDS_APP_SOURCE_DIR)/Main.c \
	$(BCDS_APP_SOURCE_DIR)/LEDsCPP.c
	
export BCDS_XDK_APP_CPP_SOURCE_FILES = \
	$(wildcard $(BCDS_APP_SOURCE_DIR)/*.cpp) \
	$(wildcard $(BCDS_APP_SOURCE_DIR)/periph/*.cpp)
	
.PHONY: clean	debug release flash_debug_bin flash_release_bin

clean: 
	$(MAKE) -C $(BCDS_BASE_DIR)/xdk110/Common -f application.mk clean

debug: 
	$(MAKE) -C $(BCDS_BASE_DIR)/xdk110/Common -f application.mk debug

release: 
	$(MAKE) -C $(BCDS_BASE_DIR)/xdk110/Common -f application.mk release
	
clean_Libraries:
	$(MAKE) -C $(BCDS_BASE_DIR)/xdk110/Common -f application.mk clean_libraries
	
flash_debug_bin: 
	$(MAKE) -C $(BCDS_BASE_DIR)/xdk110/Common -f application.mk flash_debug_bin
	
flash_release_bin: 
	$(MAKE) -C $(BCDS_BASE_DIR)/xdk110/Common -f application.mk flash_release_bin

cleanlint: 
	$(MAKE) -C $(BCDS_BASE_DIR)/xdk110/Common -f application.mk cleanlint
	
lint: 
	$(MAKE) -C $(BCDS_BASE_DIR)/xdk110/Common -f application.mk lint
	
cdt:
	$(MAKE) -C $(BCDS_BASE_DIR)/xdk110/Common -f application.mk cdt	
