FROM alpine:3.14

RUN apk add --no-cache curl gcc curl curl-dev libcurl g++ libc-dev make cmake git jansson jansson-dev meson ccache

# Install concord
RUN git clone https://github.com/Cogmasters/concord.git /concord
WORKDIR /concord
RUN make -j$(nproc)
RUN make install

# Install dotenv-c
RUN git clone https://github.com/Isty001/dotenv-c /dotenv-c
WORKDIR /dotenv-c
RUN mkdir build
WORKDIR /dotenv-c/build
RUN cmake ..
RUN cmake --build .
RUN make install

# Build the project
WORKDIR /app
COPY . .

RUN meson setup builddir
WORKDIR /app/builddir
RUN ninja

ENV PROD=1
CMD ["/bin/sh", "-c", "if [ ! -f /data/intents.json ]; then cp intents.json /data/intents.json; fi && ./tina"]
