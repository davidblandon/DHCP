# Use an official GCC image as a base
FROM gcc:latest

# Set the working directory
WORKDIR /usr/src/dhcp

# Copy the source code and headers into the container
COPY . .

# Install required dependencies
RUN apt-get update && apt-get install -y \
    make \
    && rm -rf /var/lib/apt/lists/*

# Clean any previous build files before building
RUN make clean -C build || true

# Build the server and client
RUN make -C build

# Create a script to run both server and client
RUN echo '#!/bin/bash\n\
./build/dhcp_server &\n\
sleep 2\n\
./build/dhcp_client' > run.sh

# Make the script executable
RUN chmod +x run.sh

# Run the script
CMD ["./run.sh"]
