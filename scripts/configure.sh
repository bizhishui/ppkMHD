#!/bin/bash

#baisc configuration
cmake -DUSE_SDM=ON -DUSE_MPI=OFF -DKokkos_ENABLE_OPENMP=ON -DKokkos_ENABLE_HWLOC=ON BUILD_DOC=ON -DUSE_VTK=ON ..
make -j 16
cmake --install . --prefix /data/code/myLibs/ppkMHD


cmake -DCMAKE_INSTALL_PREFIX=/data/code/myLibs/ppkMHD -DUSE_SDM=ON -DUSE_MPI=OFF -DKokkos_ENABLE_OPENMP=ON -DKokkos_ENABLE_HWLOC=ON BUILD_DOC=ON -DUSE_VTK=ON ..
make -j 16
make install
