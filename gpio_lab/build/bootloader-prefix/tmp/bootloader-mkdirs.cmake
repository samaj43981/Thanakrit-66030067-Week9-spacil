# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/samaj/esp/esp-idf/components/bootloader/subproject"
  "/home/samaj/Thanakrit-66030067-Week9-spacil/gpio_lab/build/bootloader"
  "/home/samaj/Thanakrit-66030067-Week9-spacil/gpio_lab/build/bootloader-prefix"
  "/home/samaj/Thanakrit-66030067-Week9-spacil/gpio_lab/build/bootloader-prefix/tmp"
  "/home/samaj/Thanakrit-66030067-Week9-spacil/gpio_lab/build/bootloader-prefix/src/bootloader-stamp"
  "/home/samaj/Thanakrit-66030067-Week9-spacil/gpio_lab/build/bootloader-prefix/src"
  "/home/samaj/Thanakrit-66030067-Week9-spacil/gpio_lab/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/samaj/Thanakrit-66030067-Week9-spacil/gpio_lab/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/samaj/Thanakrit-66030067-Week9-spacil/gpio_lab/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
