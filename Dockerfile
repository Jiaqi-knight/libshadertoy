FROM debian:stretch
MAINTAINER Vincent Tavernier <vince.tavernier@gmail.com>
RUN apt-get update -qq && \
	apt-get install -y build-essential dpkg-dev ca-certificates \
	libboost-all-dev libc6-dev libstdc++6 libsoil-dev \
	libglew-dev libglfw3-dev libpng-dev \
	doxygen graphviz cmake git devscripts
VOLUME /app
WORKDIR /app/libshadertoy-0.0.1
ENTRYPOINT ["/app/libshadertoy-0.0.1/entrypoint.sh"]

