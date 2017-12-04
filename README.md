# DetectorORB
[![Windows build status](https://ci.appveyor.com/api/projects/status/bq7reismnhocrs1g/branch/master?svg=true)](https://ci.appveyor.com/project/nejcgalof/detectororb/branch/master)

## Instructions (slovenian)
ORB je detektor in deskriptor ključnih točk v sliki. Z njim poiščemo lokacije točk, njihovo velikost ter orientacijo. Hkrati pa za posamezno točko dobimo tudi vektor značilk, kateri služi za primerjavo najdenih točk med sabo. ORB je implementiran v knjižnici OpenCV in služi kot prosta alternativa detektorjem SIFT in SURF katerih uporaba je patentirana.

Za to vajo boste implementirali svoj detektor ORB točk. Kot gradivo nam bo služil [opis detektorja](http://www.willowgarage.com/sites/default/files/orb_final.pdf):

ter opisi na OpenCV straneh za [Fast](http://docs.opencv.org/3.0-beta/doc/py_tutorials/py_feature2d/py_fast/py_fast.html) in [Brief](http://docs.opencv.org/3.0-beta/doc/py_tutorials/py_feature2d/py_brief/py_brief.html):

Poleg implementacije si bomo pogledali tudi eno izmed pogostih aplikacij takšnega detektorja: gradnjo panoramske slike iz serije slik.

Za prvi del naloge se predvideva, da sami implementirate algoritme. Pri implementaciji si lahko pomagate z nizko nivojskimi funkcijami implementiranimi v OpenCV ali podobni knjižnici.

Za drugi del pa lahko pri implementaciji aplikacije uporabite poljubne komponente, ki so vam na voljo v OpenCV.

- implementacija algoritma 100 točk
  - detektor FAST 40 točk
    - osnovna detekcija 10 točk
    - hitri test 10 točk
    - odstranjevanje nemaksimalnih odzivov 10 točk
    - iskanje točk različnih velikosti 10 točk
  - značilka BRIEF 40 točk
    - izračun značilke 20 točk
    - iskanje orientacije točke in rotiranje značilke 20 točk
  - ujemanje 20 točk
    - iskanje najboljših parov točk med dvema slikama 10 točk
    - prikaz najdenih parov in podrobnosti ujemanja 10 točk
- aplikacija za gradnjo panorame 50 točk
  - prikaz ujemanja točk med dvema slikama 10 točk
  - preprosta panorama s Homografijo 10 točk
  - izbira osrednje slike za gradnjo panorame 10 točk
  - interaktivno zajemanje in sprotni prikaz poravnave 20 točk

## Prerequisites

### Windows

- Install [CMake](https://cmake.org/download/). We recommend to add CMake to path for easier console using.
- Install [opencv 2.4](https://github.com/opencv/opencv) from sources.
    - Get OpenCV [(github)](https://github.com/opencv/opencv) and put in on C:/ (It can be installed somewhere else, but it's recommended to be close to root dir to avoid too long path error). `git clone https://github.com/opencv/opencv`
    - Checkout on 2.4 branch `git checkout 2.4`.
    - Make build directory .
    - In build directory create project with cmake or cmake-gui (enable `BUILD_EXAMPLES` for later test).
    - Open project in Visual Studio.
    - Build Debug and Release versions.
    - Build `INSTALL` project.
    - Add `opencv_dir/build/bin/Release` and `opencv_dir/build/bin/Debug` to PATH variable. 
    - Test installation by running examples in `opencv/build/install/` dir.

## Installing
```
git clone https://github.com/nejcgalof/detectorORB.git
```

## Build
You can use cmake-gui or write similar like this:
```
mkdir build
cd build
cmake -G "Visual Studio 14 2015 Win64" -DOpenCV_DIR="C:/opencv/build" ..
```

## Run
`.\DetectorORB.exe`

Parameters in that order:
- Fast_threshold
- nonmax_suppresion(0|1)
- middle_image_path
- second_image_path
- third_image_path
- etc...

Without image paths you build panoramas with camera. Press `n` for taking next image.

## Results

![Alt text](results/example1.jpg?raw=true "Example 1")
![Alt text](results/example2.jpg?raw=true "Example 2")
![Alt text](results/camera_example.jpg?raw=true "Camera example 3")
