FROM alpine:latest as build_stage

RUN apk add --no-cache make g++ cmake

WORKDIR /app

