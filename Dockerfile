FROM gradle:jdk17-noble as builder
RUN apt-get update && apt-get install -y cmake

COPY --chown=gradle:gradle . /home/gradle/src
WORKDIR /home/gradle/src
RUN gradle build --no-daemon

FROM eclipse-temurin:17-jre-alpine
EXPOSE 8080
COPY --from=builder /home/gradle/src/build/distributions/findinpi.tar /opt/
WORKDIR /opt
RUN tar -xvf findinpi.tar
WORKDIR /opt/findinpi
RUN cp libbigfind.so /usr/lib/
CMD bin/findinpi
