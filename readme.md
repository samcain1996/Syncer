Syncer syncs a file to a central database.

## Requirements
1. C++17 or higher
2. Boost
3. wxWidgets

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

### macos
Open the terminal in the directory 'Syncer'
`chmod +x setup/mac/setup.sh`
`setup/mac/setup.sh`

## Running the Executable

### macos
Open Syncer.app in the Syncer directory


