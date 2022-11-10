#include "Server.h"

Server::Server(int port, unsigned long password_hash)
		: _ready_to_start(false), _server_port(port), _server_password_hash(password_hash)
{
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd == -1)
		Utils::error_exit(ERROR_NEW_SOCKET);

	int enable = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		Utils::error_exit(ERROR_SOCK_OPT);

	sockaddr_in hint = {};
	hint.sin_family = AF_INET;
	hint.sin_port = htons(_server_port);
	inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
	if (bind(_server_fd, (sockaddr*)&hint, sizeof(hint)) == -1)
		Utils::error_exit(ERROR_BIND_PORT);
	_server_address = inet_ntoa(hint.sin_addr);

	if (listen(_server_fd, SOMAXCONN) == -1)
		Utils::error_exit(ERROR_PORT_LISTENING);

	pollfd pfd = {};
	pfd.fd = _server_fd;
	pfd.events = POLLIN;
	_poll_fds.push_back(pfd);

	_ready_to_start = true;
}

Server::~Server()
{
	close(_server_fd);
}

void Server::run()
{
	if (!_ready_to_start)
		Utils::error_exit(ERROR_SERVER_READY);
	show_info();
	while (true)
	{
		if (poll(&(_poll_fds[0]), _poll_fds.size(), 1000) == -1)
		{
			Utils::error_print(ERROR_POLL);
			usleep(POLL_SLEEP_TIME_MS);
			continue;
		}
		if (_poll_fds[0].revents == POLLIN)
			new_client_handler();
		else
		{
			std::vector<int> need_to_del;
			for (std::vector<pollfd>::iterator it = _poll_fds.begin() + 1; it != _poll_fds.end(); ++it)
			{
				if (_users[(*it).fd]->is_need_close())
				{
					need_to_del.push_back((*it).fd);
					continue;
				}
				old_client_handler((*it).fd, (*it).revents);
			}
			if (!need_to_del.empty())
			{
				for (std::vector<int>::iterator it = need_to_del.begin(); it != need_to_del.end(); ++it)
					close_client_connection((*it));
			}
			if (!_channels.empty())
			{
				for (std::map<std::string, Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
				{
					if ((*it).second->is_empty_channel())
					{
						delete (*it).second;
						_channels.erase(it);
						if (_channels.empty())
							break;
						it = _channels.begin();
					}
					else
						(*it).second->send_messages_to_members();
				}
			}
		}
	}
}

void Server::new_client_handler()
{
	sockaddr_in address = {};
	socklen_t csin_len = sizeof(address);
	int new_client_fd = accept(_server_fd, (struct sockaddr *)&address, &csin_len);
	if (new_client_fd == -1)
	{
		Utils::error_print(ERROR_CLIENT_NEW_SOCKET);
		return;
	}

	std::cout << "[" << new_client_fd  << "]: new connection" << std::endl;
	pollfd client_pfd = {};
	client_pfd.fd = new_client_fd;
	client_pfd.events = POLLIN;
	_poll_fds.push_back(client_pfd);
	User *user = new User(*this, new_client_fd);
	_users[new_client_fd] = user;
}

void Server::old_client_handler(int client_fd, short client_event)
{
	_users[client_fd]->send_messages_to_client();
	if (client_event == (POLLIN | POLLHUP))
	{
		_users[client_fd]->set_need_close();
		return;
	}
	if (client_event == POLLIN)
	{
		char buffer[MSG_BUFFER_SIZE + 1];
		memset(buffer, 0, MSG_BUFFER_SIZE);
		ssize_t size = recv(client_fd, &buffer, MSG_BUFFER_SIZE, 0);
		if (size == -1)
		{
			std::cerr << "[" << client_fd << "]: break receive msg" << std::endl;
			return;
		}
		if (size == 0)
		{
			std::cout << "[" << client_fd << "]: empty msg. Need delete connection " <<	std::endl;
			_users[client_fd]->set_need_close();
			return;
		}
		buffer[size] = 0;
		std::cout << "[" << client_fd << "]: " << buffer;
		std::string s_buffer(buffer);

		std::vector<std::string> messages = Utils::split(s_buffer, '\n');
		for (std::vector<std::string>::iterator it = messages.begin(); it != messages.end(); ++it)
		{
			std::vector<std::string> args;
			user_commands cmd = command_parser(*it, args);
			if (cmd != cmd_none)
				_users[client_fd]->apply(cmd, args);
		}
	}
}

void Server::show_info() const
{
	std::cout << "************   Server run   ************" << std::endl;
	std::cout << "*"
			  << std::setw(20) << std::right << "fd: "
			  << std::setw(18) << std::left << _server_fd
			  << "*" << std::endl;
	std::cout << "*"
			  << std::setw(20) << std::right << "address: "
			  << std::setw(18) << std::left << _server_address
			  << "*" << std::endl;
	std::cout << "*"
			  << std::setw(20) << std::right << "port: "
			  << std::setw(18) << std::left << _server_port
			  << "#" << std::endl;
	std::cout << "*"
			  << std::setw(20) << std::right << "psw hash: "
			  << std::setw(18) << std::left << _server_password_hash
			  << "*" << std::endl;
	std::cout << "****************************************" << std::endl;
}

void Server::send_msg_to_client(int client_fd, const std::string& msg)
{
	std::string message(msg);

	Utils::replace(message, "<host>", _server_address);
	Utils::replace(message, "<server>", SERVER_NAME);

	std::cout << "-> [" << client_fd << "]: " << message << std::endl;
	if (send(client_fd, message.c_str(), message.length(), 0) == -1)
		Utils::error_print(client_fd, ERROR_SEND_MSG);
}

bool Server::check_password(unsigned long user_password_hash) const
{
	return _server_password_hash == user_password_hash;
}

void Server::close_client_connection(int client_fd)
{
	std::cout << "[" << client_fd << "]: close connection " << std::endl;
	close(client_fd);
	_nick_to_fd.erase(_users[client_fd]->get_nickname());
	_users[client_fd]->leave_all_channels();
	delete _users[client_fd];
	_users.erase(client_fd);

	for (std::vector<pollfd>::iterator it = _poll_fds.begin(); it != _poll_fds.end(); ++it)
	{
		if ((*it).fd == client_fd)
		{
			_poll_fds.erase(it);
			it = _poll_fds.begin();
		}
	}
}

void Server::registered_new_client(const std::string& nick, int client_fd)
{
	_nick_to_fd[nick] = client_fd;
}

bool Server::is_nick_exist(const std::string& nick)
{
	return _nick_to_fd.find(nick) != _nick_to_fd.end();
}

void Server::new_message_for(const std::string& nick, const std::string& message)
{
	if (is_channel_exist(nick))
	{
		_channels[nick]->push_new_message(message);
	}
	else
	{
		int dst_fd = _nick_to_fd[nick];
		_users[dst_fd]->push_new_message(message);
	}
}

Channel	*Server::join_to_channel(const std::string& channel, int client_fd)
{
	std::map<std::string, Channel *>::iterator it;
	Channel *channel_ptr;

	it = _channels.find(channel);
	if (it == _channels.end())
	{
		channel_ptr = new Channel(*this, channel, _users[client_fd]);
		std::cout << "New channel: " << channel_ptr->get_topic() << std::endl;
		_channels[channel] = channel_ptr;
	}
	else
	{
		channel_ptr = (*it).second;
		channel_ptr->add_new_user(_users[client_fd]);
	}

	return channel_ptr;
}

bool Server::is_channel_exist(const std::string &name)
{
	return _channels.find(name) != _channels.end();
}

int Server::get_user_fd(const std::string& nick)
{
	return _nick_to_fd[nick];
}

std::string Server::get_channels_info()
{
	std::string msg = "Channels [" + std::to_string(_channels.size()) +"]:\n";
	for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		msg += (*it).second->get_info() + "\n";
	return msg;
}

std::string Server::get_users_info()
{
	std::string msg = "Users [" + std::to_string(_users.size()) +"]:\n";
	for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it)
		msg += (*it).second->get_info() + "\n";
	return msg;
}


void Server::bot_apply(int client_fd, std::vector<std::string> &args)
{
	std::string msg =  "Not recognized command\n";

	if (args[0] == "channel")
		msg = get_channels_info();
	else if (args[0] == "user")
		msg = get_users_info();

	send_msg_to_client(client_fd, msg + "\r\n");
}

