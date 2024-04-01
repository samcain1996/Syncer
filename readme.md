Syncer syncs a file to a central database.

## Requirements
1. C++ 17 or higher
2. Boost (https://www.boost.org/)
3. wxWidgets (GUI version only)

## Usage
Two computers are needed. Computer A will serve as the server and host all the content that will be synced.
Computer B will serve as the client and upload and download files to Computer A.

### Steps
1. Open res/syncer.config
2. Enter the ip of the server computer, the port it uses, and the folders you want to store information in
3. Compile the code (see Compiling The Code)
4. Start Server
5. Start Client

## Compiling The Code

### GUI
Open the terminal in the directory 'Syncer'
`clang++ -g -std=c++20 -I path_to_boost -I path_to_wxWidgets -I include `wx-config --cxxflags` -o syncer-gui src/*.cpp `wx-config --libs`

### Command Line
Open the terminal in the directory 'Syncer'
`clang++ -std=c++17 -I path_to_boost -I include src/* -o syncer`



## Running
Flags:
- s Start Server
- d *filename* Download filename from server
- u *filename* Upload filename to server

### Examples

- Starting the server
`./syncer s`

- Uploading file to server
`./syncer u filename`

- Downloading file from server
`./syncer d filename`


