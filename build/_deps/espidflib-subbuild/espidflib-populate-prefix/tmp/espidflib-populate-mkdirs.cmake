# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/majorbien/eclipse-workspace/drakkar_project/build/_deps/espidflib-src"
  "/home/majorbien/eclipse-workspace/drakkar_project/build/_deps/espidflib-build"
  "/home/majorbien/eclipse-workspace/drakkar_project/build/_deps/espidflib-subbuild/espidflib-populate-prefix"
  "/home/majorbien/eclipse-workspace/drakkar_project/build/_deps/espidflib-subbuild/espidflib-populate-prefix/tmp"
  "/home/majorbien/eclipse-workspace/drakkar_project/build/_deps/espidflib-subbuild/espidflib-populate-prefix/src/espidflib-populate-stamp"
  "/home/majorbien/eclipse-workspace/drakkar_project/build/_deps/espidflib-subbuild/espidflib-populate-prefix/src"
  "/home/majorbien/eclipse-workspace/drakkar_project/build/_deps/espidflib-subbuild/espidflib-populate-prefix/src/espidflib-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/majorbien/eclipse-workspace/drakkar_project/build/_deps/espidflib-subbuild/espidflib-populate-prefix/src/espidflib-populate-stamp/${subDir}")
endforeach()
