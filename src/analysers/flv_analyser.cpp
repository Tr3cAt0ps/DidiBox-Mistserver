/// \file flv_analyser.cpp
/// Contains the code for the FLV Analysing tool.

#include <fcntl.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <mist/flv_tag.h> //FLV support

/// Reads FLV from stdin and outputs human-readable information to stderr.
int main() {
  FLV::Tag FLV_in; // Temporary storage for incoming FLV data.
  while (!feof(stdin)){
    if (FLV_in.FileLoader(stdin)){
      std::cout << "Tag: " << FLV_in.tagType() << "\n\tTime: " << FLV_in.tagTime() << std::endl;
    }
  }
  return 0;
}
