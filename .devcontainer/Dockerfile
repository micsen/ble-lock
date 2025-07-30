FROM espressif/idf:release-v5.2

# Set up working directory
WORKDIR /workspace

# Copy your project (optional: mount instead)
# COPY . /workspace

# Set environment variables
ENV IDF_PATH=/opt/esp/idf \
    IDF_TOOLS_PATH=/opt/esp/.espressif

# Pre-load dependencies (optional)
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    python3-pip \
    minicom \
    screen \
    usbutils \
    && rm -rf /var/lib/apt/lists/*

RUN /usr/bin/python3 /opt/esp/idf/tools/idf_tools.py install
RUN /usr/bin/python3 /opt/esp/idf/tools/idf_tools.py install-python-env

# Activate ESP-IDF on startup
RUN echo ". $IDF_PATH/export.sh" >> ~/.bashrc


CMD ["/bin/bash"]
