# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/77021/esp/esp-idf/components/bootloader/subproject"
  "D:/VSCode/ESP32/ESP32_NVS/build/bootloader"
  "D:/VSCode/ESP32/ESP32_NVS/build/bootloader-prefix"
  "D:/VSCode/ESP32/ESP32_NVS/build/bootloader-prefix/tmp"
  "D:/VSCode/ESP32/ESP32_NVS/build/bootloader-prefix/src/bootloader-stamp"
  "D:/VSCode/ESP32/ESP32_NVS/build/bootloader-prefix/src"
  "D:/VSCode/ESP32/ESP32_NVS/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/VSCode/ESP32/ESP32_NVS/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
