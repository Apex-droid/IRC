#include "User.h"
#include <iostream>

User::User(Server &server, int client_fd) : _server(server), _client_fd(client_fd)
{
	_nickname.clear();
	_username.clear();
	_state = user_state_new;
	_user_password_hash = 0;
}

User::~User()
{

}

void User::apply(user_commands cmd, std::vector<std::string> &args)
{
	switch (cmd)
	{
		case cmd_none:
			break;
		case cmd_pass:
			pass_handler(args);
			break;
		case cmd_user:
			user_handler(args);
			break;
		case cmd_nick:
			nick_handler(args);
			break;
		case cmd_privmsg:
			privmsg_handler(args);
			break;
		case cmd_ison:
			ison_handler(args);
			break;
		case cmd_notice:
			notice_handler(args);
			break;
		case cmd_join:
			join_handler(args);
			break;
		case cmd_kick:
			kick_handler(args);
			break;
		case cmd_bot:
			bot_handler(args);
			break;
		case cmd_topic:
			topic_handler(args);
			break;
	}
}

void User::send_hello_client()
{
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTDSTART));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_1));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_2));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_3));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_4));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_5));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_6));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_7));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_8));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_9));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_10));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_11));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_12));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_13));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_14));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_15));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_16));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_17));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_18));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_19));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_20));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_21));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_22));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_23));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_24));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_25));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_26));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_27));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_28));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_29));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_30));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_31));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_32));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_33));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_34));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_MOTD_35));
	_server.send_msg_to_client(_client_fd, fill_placeholders(RPL_ENDOFMOTD));
}

bool User::is_need_close()
{
	return _state == user_state_need_close;
}

void User::start_authorization()
{
	if (!_server.check_password(_user_password_hash))
	{
		_server.send_msg_to_client(_client_fd, fill_placeholders(ERR_ALREADYREGISTRED));
		_state = user_state_need_close;
		return;
	}
	std::cout << "[" << _client_fd << "]: PASS accepted" << std::endl;
	_state = user_state_authorized;
	_server.registered_new_client(_nickname, _client_fd);
	send_hello_client();
}

void User::set_need_close()
{
	_state = user_state_need_close;
}

std::string User::fill_placeholders(const std::string& s, const std::string& comment)
{
	std::string msg(s);

	Utils::replace(msg, "<nick>", _nickname);
	Utils::replace(msg, "<user>", _username);
	Utils::replace(msg, "<server>", SERVER_NAME);
	Utils::replace(msg, "<target>", _target);
	Utils::replace(msg, "<msg>", _send_msg);
	Utils::replace(msg, "<online_nicks>", _online_nicks);
	Utils::replace(msg, "<comment>", comment);

	return msg;
}

void User::push_new_message(const std::string& msg)
{
	_new_messages.push_back(msg);
}

void User::send_messages_to_client()
{
	for (std::vector<std::string>::iterator it = _new_messages.begin(); it != _new_messages.end(); ++it)
		_server.send_msg_to_client(_client_fd, *it + "\r\n");
	_new_messages.clear();
}

bool User::is_valid_nick(std::string nick)
{
	if (nick.empty())
		return false;
	if (!(Utils::is_letter(nick[0]) || Utils::is_special(nick[0])))
		return false;
	for (size_t i = 0; i < nick.size(); ++i)
		if (!(Utils::is_letter(nick[i])
			|| Utils::is_special(nick[i])
			|| Utils::is_digit(nick[i])
			|| nick[i] == '-'))
			return false;

	return true;
}

std::string User::get_nickname()
{
	return _nickname;
}

void User::leave_all_channels()
{
	if (_channels.empty())
		return;
	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		(*it)->leave_user(this);
	_channels.clear();
}

void User::leave_channel(const std::string& channel_name)
{
	if (_channels.empty())
		return;

	std::vector<Channel*>::iterator leave_it = _channels.end();
	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		if ((*it)->get_name() == channel_name)
			leave_it = it;
	if (leave_it != _channels.end())
	{
		(*leave_it)->leave_user(this);
		_channels.erase(leave_it);
	}
}


int User::get_client_fd()
{
	return _client_fd;
}

Channel *User::get_channel(const std::string& channel_name)
{
	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		if ((*it)->get_name() == channel_name)
			return (*it);
	}
	return nullptr;
}

std::string User::get_info()
{
	std::string msg = _nickname + " [" + std::to_string(_client_fd) + "]\n";
	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		msg += "\t" + (*it)->get_name() + "\n";
	return msg;
}

bool User::in_channel(const std::string& channel_name)
{
	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		if ((*it)->get_name() == channel_name)
			return true;
	return false;
}
