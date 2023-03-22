# Assignment - SW Engineering
This program collects real-time data from TCP server and relays data 
to STDOUT every 1.5 second. 

## To compile: 
```bash
chmod +x build.sh
./build.sh
```
## To run: 
```bash
./ProximityDecoder
```
By default, the program prints out the data formatted in hex. 
To print out the data in raw binary, use the "-uf" flag; note that the
byte order of the data will depend on your architecture.

To exit program, use 'Control-C'.

