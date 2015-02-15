FROM ubuntu:14.04
MAINTAINER Dan Liew <daniel.liew@imperial.ac.uk>, Ioweb <contact@ioweb.fr>

RUN apt-get update && apt-get -y install curl
RUN apt-get -y install gcc g++ gdb
RUN apt-get -y --no-install-recommends install cmake mercurial git make subversion
RUN apt-get -y install python python-dev python-pip vim

WORKDIR /root

ADD amd-app-sdk.tar.bz2 /tmp

# Finally we can install it
RUN /tmp/AMD-APP-SDK-*.sh -- --acceptEULA 'yes' -s

# Remove installation files
#RUN rm /tmp/AMD-APP-SDK-*.sh && rm -rf /tmp/AMDAPPSDK-*

# Remove the samples. Keep the OpenCL ones
RUN rm -rf /opt/AMDAPPSDK-*/samples/{aparapi,bolt,opencv}

# Put the includes and library where they are expected to be
RUN ln -s /opt/AMDAPPSDK-*/include/CL /usr/include/ && ln -s /opt/AMDAPPSDK-*/lib/x86_64/libOpenCL.so.1 /usr/lib/libOpenCL.so

# Provide easy access to root if needed
RUN echo "root:root" | chpasswd

# NodeJS
RUN apt-get install -y software-properties-common python-software-properties
RUN add-apt-repository ppa:onestone/node.js-0.12 
RUN apt-get update
RUN apt-get install -y nodejs mesa-common-dev

RUN npm install -g mocha node-gyp
# Add a non root user
RUN useradd -m aasdk
USER aasdk
WORKDIR /home/aasdk


