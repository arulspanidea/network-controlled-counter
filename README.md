Network Controlled Countdown timer:
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
.
    - Shall perform basic validation on data packets received, if the data packet is valid forward commands one by one to the Countdown manager.
   
2. Countdown timer Manager:
    - Shall initialize Countdown with the default configuration
    - Shall accept the command from `Network Command Manager` and modify the Countdown timer configuration.
    - Control the Countdown UI module
3. Countdown UI:
    - Start and print the Countdown with the configuration from `Countdown Manager` in the terminal

- Commands:
    - Countdown UI refresh rate
    - Countdown format (seconds or milliseconds)
    - Set Timer
    - Reset Countdown
    - Stop Countdown
    - Restart Countdown
