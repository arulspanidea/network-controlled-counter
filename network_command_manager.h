#ifndef NETWORK_COMMAND_MANAGER_H
#define NETWORK_COMMAND_MANAGER_H

#define PORT 10001

#define EXPECTED_START_FRAME 0xAAAA
#define EXPECTED_END_FRAME 0xBBBB

struct data_packet{
	int start_frame;
	int cmd_no;
	int command_value;
	int end_frame;
};

void forward_command_to_countdown_timer_manager(int pipe_fd, int command_value);

#endif
