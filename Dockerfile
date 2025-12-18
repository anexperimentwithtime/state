FROM ghcr.io/anexperimentwithtime/compiler:1.87.0-debug-static

COPY . .

RUN bash scripts/build-and-test.sh debug static