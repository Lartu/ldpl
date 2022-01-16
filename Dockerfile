FROM alpine:latest as build_stage

RUN apk add --no-cache make g++

COPY . /usr/src/myapp
WORKDIR /usr/src/myapp
RUN make && make install

