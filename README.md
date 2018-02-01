# mapper

Tested on Ubuntu 16.04.
To build following dependencies should be satisfied:
- libboost-program-options-dev
- libelf-dev
- elfutils

Init submodules, after repo is cloned:
`git submodule init && git submodule update`

Build libdwarf first:
* configure: `cd 3rd-party/libdwarf`
* configure: `cmake . -B_Release -DCMAKE_BUILD_TYPE=Release`
* build: `cmake --build _Release --target dd`
* (optionally install): `sudo cmake --build _Release --target install`