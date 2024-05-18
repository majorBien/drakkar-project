# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/majorbien/Pobrane/esp-idf4/src/esp-idf-v4.4.7/components/bootloader/subproject"
  "/home/majorbien/eclipse-workspace/drakkar_project/build/bootloader"
  "/home/majorbien/eclipse-workspace/drakkar_project/build/bootloader-prefix"
  "/home/majorbien/eclipse-workspace/drakkar_project/build/bootloader-prefix/tmp"
  "/home/majorbien/eclipse-workspace/drakkar_project/build/bootloader-prefix/src/bootloader-stamp"
  "/home/majorbien/eclipse-workspace/drakkar_project/build/bootloader-prefix/src"
  "/home/majorbien/eclipse-workspace/drakkar_project/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/majorbien/eclipse-workspace/drakkar_project/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
