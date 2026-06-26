# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/ubunt/projects/calculator-app/build-tidy/_deps/calculator_math-src"
  "/home/ubunt/projects/calculator-app/build-tidy/_deps/calculator_math-build"
  "/home/ubunt/projects/calculator-app/build-tidy/_deps/calculator_math-subbuild/calculator_math-populate-prefix"
  "/home/ubunt/projects/calculator-app/build-tidy/_deps/calculator_math-subbuild/calculator_math-populate-prefix/tmp"
  "/home/ubunt/projects/calculator-app/build-tidy/_deps/calculator_math-subbuild/calculator_math-populate-prefix/src/calculator_math-populate-stamp"
  "/home/ubunt/projects/calculator-app/build-tidy/_deps/calculator_math-subbuild/calculator_math-populate-prefix/src"
  "/home/ubunt/projects/calculator-app/build-tidy/_deps/calculator_math-subbuild/calculator_math-populate-prefix/src/calculator_math-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/ubunt/projects/calculator-app/build-tidy/_deps/calculator_math-subbuild/calculator_math-populate-prefix/src/calculator_math-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/ubunt/projects/calculator-app/build-tidy/_deps/calculator_math-subbuild/calculator_math-populate-prefix/src/calculator_math-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
