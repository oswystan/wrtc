######################################################
## WebRTC build env 
## current revision: 6294a7eb71c891e9ea41273a7a94113f6802d0da  refs/branch-heads/60
## 
## use the following commands to build a specific revision
##   $ mkdir out && cd out
##   $ cmake -DWEBRTC_REVISION=be22d51 ..
##   $ cmake -DWEBRTC_BRANCH_HEAD=refs/branch-heads/57 ..
##
## docker build -t webrtc:dev .
######################################################
FROM ubuntu:16.04
RUN apt-get update && \
    apt-get install -y git cmake vim && \
    apt-get install -y build-essential libglib2.0-dev libgtk2.0-dev libxtst-dev && \
    apt-get install -y libxss-dev libpci-dev libdbus-1-dev libgconf2-dev && \
    apt-get install -y libgnome-keyring-dev libnss3-dev libasound2-dev libpulse-dev && \
    apt-get install -y libudev-dev && \
    git clone https://github.com/aisouard/libwebrtc.git


