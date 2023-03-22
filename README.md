## Assignment - SW Engineering
This program collects real-time data from TCP server and relays data 
to STDOUT every 1.5 second. All data is relayed, only a color code is appended to each package.

To compile, type 'chmod +x build.sh', followed by './build.sh'.
To run, type './ProximityDecoder
By default, the program prints out the data formatted in hex. 
To print out the data in raw binary, use the "-uf" flag; note that the
byte order of the data will depend on your architecture.

To exit program, use 'Control-C'.
