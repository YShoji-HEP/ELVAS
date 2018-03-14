ELVAS
===============================================================================
C++ Package for ELectroweak VAcuum Stability
## Introduction ##

ELVAS is a C++ package for the calculation of the decay rate of a false vacuum at the one-loop level, based on the formulae developed in [1, 2]. ELVAS is applicable to models with the following features:
+ Only one scalar boson is responsible for the vacuum decay.
+ Classical scale invariance (approximately) holds. In particular, the potential of the scalar field responsible for the vacuum decay should be well approximated by the quartic form for the calculation of the bounce solution. (Thus, the bounce is nothing but the so-called Fubini instanton.)
+ The instability of the scalar potential occurs due to RG effects; thus, the quartic coupling constant becomes negative at a high scale.

If you use ELVAS in scholarly work, please cite [1] and [2].

## Download and Install ##

*ELVAS* requires a c++ compiler that supports C++14 and the *boost* library (<http://www.boost.org/users/download/#live>).
The required version of *boost* is 1.59.0 or higher.

### Unix ###

For UNIX systems, follow the instruction below.

1. Clone the *ELVAS* git repository

``` shell
$ git clone https://github.com/YShoji-HEP/ELVAS.git
```

Equivalently, all the sources and the manual can be downloaded from <https://github.com/YShoji-HEP/ELVAS/>.

2. Generate a makefile with

``` shell
$ cmake .
```

If you do not use the default compiler, use the option `CMAKE_CXX_COMPILER`.
When the *boost* library is located at a non-standard directory, specify it with `BOOST_ROOT`, or `BOOST_INCLUDEDIR` and  `BOOST_LIBRARYDIR`.

3. Compile *ELVAS* with

``` shell
$ make
```

4. Run

``` shell
$ ./elvas sm.in sm.dat
```

If everything works well, you will see as a result

``` shell
mHiggs       mTop         log10(gamma x Gyr Gpc^3)
1.250900e+02 1.731000e+02 -5.655333e+02
1.248500e+02 1.731000e+02 -5.268473e+02
1.253300e+02 1.731000e+02 -6.086626e+02
1.250900e+02 1.725000e+02 -8.791406e+02
1.250900e+02 1.737000e+02 -3.880806e+02
```
after a header.
### Windows ###

For Windows, you may use Cygwin, MinGW, or Visual C++. In any case, you need to install cmake, which can be downloaded from the [official website](https://cmake.org/download/). For Cygwin and MinGW, the procedure is almost the same as the UNIX systems. For Visual C++, you can generate a VC++ project file with the GUI front-end of cmake. Furthermore, a pre-compiled boost library can be installed through NuGet. After installing it, you can build the project as usual.

## Basic Usage ##

The simplest way to use *ELVAS* in your model is to modify **sm.in**, which includes routines for the standard model case, and to prepare the data of the renormalization group evolution in a similar format as **sm.dat**.
You can easily add quantum corrections from extra scalars, fermions, and gauge bosons in **sm.in**.
For a quick guide, see Section 4.1 of the [manual](https://github.com/YShoji-HEP/ELVAS/blob/master/manual.pdf).

To run the program, type
``` shell
   (stdin/stdout): ./elvas
  (filein/stdout): ./elvas [INPUT1] [INPUT2] ...
  (stdin/fileout): ./elvas -o [OUTPUT]
 (filein/fileout): ./elvas -o [OUTPUT] [INPUT1] [INPUT2] ...
 ```
 You may provide multiple input files, which will be joined internally.
In such a case, provide the routine first, and the RG data after.
 If you do not provide input/output files, the standard input/output is used.
 
The allowed options are
``` shell
-o [ --output ] arg   output file
-h [ --help ]         display help message
-v [ --version ]      output version information
-n [ --no_header ]    disable header printing
```
## Citation ##

If you use *ELVAS* in your work, please cite [1] and [2].

[1] S. Chigusa, T. Moroi, and Y. Shoji, Phys. Rev.<!--garbled--> Lett. **119** (2017) no.21, 211801 [arXiv:[1707.09301](https://arxiv.org/abs/1707.09301) [hep-ph]].

[2] S. Chigusa, T. Moroi and Y. Shoji, [arXiv:[1803.03902](https://arxiv.org/abs/1803.03902) [hep-ph]].

