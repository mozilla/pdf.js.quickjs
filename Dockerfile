FROM emscripten/emsdk:latest

WORKDIR /tmp

ENV QUICKJS_GIT_HASH 2788d71e823b522b178db3b3660ce93689534e6d
ENV OUTPUT /js
ENV QUICKJS /tmp/quickjs
ENV INPUT /code/src

RUN git config --global user.email "you@example.com" && \
    git config --global user.name "Your Name" && \
    git clone https://github.com/bellard/quickjs.git && \
    cd quickjs && \
    git checkout -b extra ${QUICKJS_GIT_HASH}

ADD *.patch .

RUN cd quickjs && \
    git am ../0001-Extra-stuff-for-use-with-PDF.js.patch

CMD /code/compile.sh