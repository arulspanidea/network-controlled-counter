# network-controlled-counter
Network Controlled Counter:
It is a simple project that gives developers some basic ideas about application development. Software should be modular and simple. Each module will be created as a separate process, and any IPC mechanism will be used to communicate between processes.


Modules:
1. Network Command Manager:
    - Shall start TCP server and listen on port 10001 & accept incoming connection.
    - Shall read the data from the client.
        - data packet will have a starter frame (int - 4 bytes)
        - total commands in the data packet (int - 4 bytes)
        - Command list each consumes 4 bytes (int) (total commands * int)
        - end of frame (int - 4 byte)
        - sample struct for the data packet
```
struct data_packet 
{
    int start_frame;
    int cmd_no;
    int command_value;
    int end_frame;
}
```
    - Shall perform basic validation on data packets received, if the data packet is valid forward commands one by one to the Counter manager.
   
2. Counter Manager:
    - Shall initialize counter with the default configuration
    - Shall accept the command from `Network Command Manager` and modify the counter configuration.
    - Control the Counter UI module
3. Counter UI:
    - Start and print the counter with the configuration from `Counter Manager` in the terminal

- Commands:
    - Counter UI refresh rate
    - Counter Frequency
    - Reset counter
    - Stop counter
    - Restart counter
