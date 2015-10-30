#!/bin/sh

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

#
# Checkout master branch of the lwIP repository
#
if [ ! -d "lwip" ]; then
  echo " "
  echo "******** Checking out LWIP repository ******** "
  git clone git://git.savannah.nongnu.org/lwip.git
  cd lwip && git reset --hard f278b27119c38e48437 && cd ..
  echo "****************  DONE *********************** "
  echo " "
fi

#
# Checkout the master branch of the lwip-contrib repository and keep
# only the RAW mode HTTP server project directory.
#
if [ ! -d "httpserver_raw" ]; then
  echo " "
  echo "******** Checking out httpserver_raw  ******** "
  rm -rf lwip-contrib/
  git clone git://git.savannah.nongnu.org/lwip/lwip-contrib.git
  cd lwip-contrib && git reset --hard 167c3e792e3d1c8 && cd ..
  mv lwip-contrib/apps/httpserver_raw ./
  rm -rf lwip-contrib/
  echo "****************  DONE *********************** "
  echo " "
fi
