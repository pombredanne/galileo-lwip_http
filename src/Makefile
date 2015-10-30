#
# Copyright (c) 2015, Wind River Systems, Inc. 
# 
# Licensed under the Apache License, Version 2.0 (the "License"); 
# you may not use this file except in compliance with the License. 
# You may obtain a copy of the License at 
# 
#     http://www.apache.org/licenses/LICENSE-2.0 
# 
# Unless required by applicable law or agreed to in writing, software 
# distributed under the License is distributed on an "AS IS" BASIS, 
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
# See the License for the specific language governing permissions and 
# limitations under the License.
#

LWIP := ../lwip
lwip_SRC = $(wildcard $(LWIP)/src/core/*.c $(LWIP)/src/core/ipv4/*.c $(LWIP)/src/core/ipv6/*.c \
	$(LWIP)/src/core/snmp/*.c $(LWIP)/src/netif/*.c $(LWIP)/src/api/*.c)
lwip_OBJECTS = $(patsubst %.c, %.o, $(lwip_SRC))

http_OBJECTS = ../httpserver_raw/fs.o ../httpserver_raw/httpd.o

#
# This will pull in all source files present in the src directory
#
main_SOURCES = $(wildcard ../src/*.c)
main_OBJECTS = $(patsubst %.c, %.o, $(main_SOURCES))

#
# Need to include the Light Weight IP stack and the OS prototype
# support functions that needs to be provided for it to work.
#
ccflags-y = ${PROJECTINCLUDE} -I../lwip/src/include -I../include

#
# List of objects linked into project
#
obj-y += $(main_OBJECTS)
obj-y += $(http_OBJECTS)
obj-y += $(lwip_OBJECTS)
