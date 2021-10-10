#!/bin/bash

#baisc configuration
cmake -DUSE_SDM=ON -DUSE_MPI=OFF -DKokkos_ENABLE_OPENMP=ON -DKokkos_ENABLE_HWLOC=ON ..


cmake --install . --prefix /data/code/myLibs/ppkMHD
