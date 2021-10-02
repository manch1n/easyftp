## EASYFTP
This ftp program use **block-io way** to communicate.Just for studying.

### build
    1)mkdir build && cd build
    2)cmake ..& make

### run server
./server
The server port is default bind to 55555 and it will create a folder in current working directory to recv or send the files.

### run client
./client <serverIP>
The client will create a local folder to send or recv remote files.

### client command
    1)LOCALLIST: list the local files.
    2)REMOTELIST: list the remote files.
    3)STORE <local_filename>
    4)RETRIEVE <remote_filename>
    5)EXIT