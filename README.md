# gtk_tracking_program
~~Delivery tracking program made by C. (gtk 3.0)~~

오랜만에 C로 GUI 개발 해보고 싶어서 만든 gtk 기반 택배 추적 프로그랜

## 사용한 라이브러리

__MAC__

    brew install curl
    brew install json-c
    brew install gtk+3
    brew install sqlite3
    brew install glade
    
__WIN10__
[MSYS2 Mingw64 기준](https://www.msys2.org)

    pacman -S mingw-w64-x86_64-curl
    pacman -S mingw-w64-x86_64-json-c
    pacman -S mingw-w64-x86_64-gtk3
    pacman -S mingw-w64-x86_64-sqlite3
    pacman -S mingw-w64-x86_64-glade

__UBUNTU__

    sudo apt-get install libcurl4-openssl-dev
    sudo apt-get install gtk+-3.0
    sudo apt-get install glade

* json-c 설치법

    __먼저 다운 받을 것__

        sudo apt-get install git
        sudo apt-get install cmake

    __빌드 & 설치__

        git clone https://github.com/json-c/json-c.git
        cd json-c
        mkdir build
        cd build
        cmake ..
        make
        sudo make install

* sqlite3 설치법

    __먼저 다운 받을 것__
    
        sudo apt-get install tcl

        https://www.sqlite.org/download.html 에서 Source Code 압축 파일중에 configure 스크립트가 포함된 파일을 다운받고 압축해제

    __빌드 & 설치__

        cd <위에서 해제한 디렉터리>
        ./configure
        make
        sudo make install

## 사용한 배송 조회 API

🙇‍♂️

https://github.com/shlee322/delivery-tracker
