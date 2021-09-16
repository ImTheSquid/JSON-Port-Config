FROM ubuntu:18.04 AS dependencies

ENV DEBIAN_FRONTEND noninteractive
# https://github.com/microsoft/vcpkg/blob/master/scripts/azure-pipelines/linux/provision-image.sh
RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y git curl zip unzip tar build-essential \
    libx11-dev libxi-dev libxext-dev libgl1-mesa-dev bison python3 \
    gperf pkg-config autoconf python3-pip libglu1-mesa-dev \
    mesa-common-dev libxrandr-dev libxxf86vm-dev libxext-dev libxfixes-dev libxrender-dev \
    libxcb1-dev libx11-xcb-dev libxcb-glx0-dev libxcb-util0-dev libxkbcommon-dev libxcb-keysyms1-dev \
    libxcb-image0-dev libxcb-shm0-dev libxcb-icccm4-dev libxcb-sync0-dev \
    libxcb-xfixes0-dev libxcb-shape0-dev libxcb-randr0-dev \
    libxcb-render-util0-dev libxcb-xinerama0-dev libxcb-xkb-dev libxcb-xinput-dev libgles2-mesa-dev libwayland-dev \
    at curl unzip tar libxt-dev gperf libxaw7-dev cifs-utils \
    build-essential g++ gfortran zip libx11-dev libxkbcommon-x11-dev libxi-dev \
    libgl1-mesa-dev libglu1-mesa-dev mesa-common-dev libxinerama-dev libxxf86vm-dev \
    libxcursor-dev yasm libnuma1 libnuma-dev python-six python3-six python-yaml \
    flex libbison-dev autoconf libudev-dev libncurses5-dev libtool libxrandr-dev \
    xutils-dev dh-autoreconf autoconf-archive libgles2-mesa-dev ruby-full \
    pkg-config
RUN pip3 install meson
RUN git clone https://github.com/Microsoft/vcpkg.git
WORKDIR /vcpkg/
RUN ./bootstrap-vcpkg.sh -disableMetrics

RUN ./vcpkg install libssh2 qt5 && rm -rf buildtrees

#
# Build code
FROM ubuntu:18.04 AS compile

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y build-essential cmake

# Copy files and integrate
COPY --from=dependencies /vcpkg /vcpkg
ENV VCPKG_ROOT /vcpkg
RUN /vcpkg/vcpkg integrate install

COPY . /code

ARG buildType=Debug
WORKDIR /code
RUN cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE=${buildType} -H. -Bbuild