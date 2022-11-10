#ifndef SERVER_H
#define SERVER_H

#define MSG_BUFFER_SIZE	4096

#include <iostream>
#include <vector>
#include <poll.h>
#include <string>
#include <arpa/inet.h>
#include <unistd.h>
#include <iomanip>
#include <map>
#include "../Utils/Utils.h"
#include "../User/User.h"
#include "../Channel/Channel.h"

class User;
class Channel;

class Server
{
private:
	bool							_ready_to_start;
	std::string 					_server_address;
	int								_server_port;
	int								_server_fd;
	unsigned long					_server_password_hash;
	std::vector<pollfd>				_poll_fds;
	std::map<int, User*>			_users;
	std::map<std::string, Channel*>	_channels;
	std::map<std::string, int>		_nick_to_fd;

	void			new_client_handler();
	void			old_client_handler(int client_fd, short client_event);
	void			show_info() const;
	user_commands	command_parser(std::string msg, std::vector<std::string> &args_out);
	void			close_client_connection(int client_fd);
	std::string		get_channels_info();
	std::string 	get_users_info();

public:
	Server(int port, unsigned long password_hash);
	~Server();

	void		run();
	bool		check_password(unsigned long user_password_hash) const;
	void		send_msg_to_client(int client_fd, const std::string& msg);
	void		registered_new_client(const std::string& nick, int client_fd);
	bool		is_nick_exist(const std::string& nick);
	void		new_message_for(const std::string& nick, const std::string& message);
	int 		get_user_fd(const std::string& nick);

	bool		is_channel_exist(const std::string& name);
	Channel		*join_to_channel(const std::string& channel, int client_fd);

	void 		bot_apply(int client_fd, std::vector<std::string> &args);
};


#endif
