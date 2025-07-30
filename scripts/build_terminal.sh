docker run --rm -it \
    -v $(pwd):/workspace \
    -v $(pwd)/deps:/mntDeps \
    -w /workspace \
    --device=/dev/ttyACM0 \
    --name esp32-dev \
    esp32s3-dev