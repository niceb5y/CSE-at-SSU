FROM       ubuntu:16.04

RUN sed -i 's@archive.ubuntu.com@mirror.kakao.com@g' /etc/apt/sources.list

RUN        apt-get -y update

RUN apt-get -y install build-essential

COPY ./* /root/lsp/

WORKDIR /root/lsp

RUN     make
