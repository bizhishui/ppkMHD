#!/bin/bash

# compiler type : GNU, INTEL, CLANG, ...
compiler=INTEL
enable_mpi=false

verbose=true
module purge
# load the necessary modules
case $compiler in
"GNU" )
    module load userspace/all
    module load cmake/3.20.2
    module load gcc/7.2.0
    module load openmpi/gcc72
    
    if [ "$enable_mpi" = true ] ; then
	export CXX=`which mpicxx`
	export OMPI_CXX=`which g++`
    else
	export CXX=`which g++`
    fi
    ;;

"INTEL" )
    
    # intel compiler
    module load userspace/all
    module load cmake/3.20.2
    module load intel-compiler/64/2020.2.254
    
    if [ "$enable_mpi" = true ] ; then
	# intel mpicxx is already rooted to use icpc
	export CXX=`which mpicxx`
    else
	export CXX=`which icpc`
    fi
    
    ;;

# TODO
# "CLANG" )
# 	;;

* )
    printf "Wrong value for variable compiler. Available values are GNU,INTEL"
    ;;
esac

if [ "$verbose" = true ] ; then
    module list
fi


