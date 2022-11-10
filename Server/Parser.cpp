#include "Server.h"

user_commands	Server::command_parser(std::string msg, std::vector<std::string> &args_out)
{
	Utils::replace(msg, "\r\n", "");
	Utils::replace(msg, "\n", "");
	Utils::replace(msg, "\r", "");
	if (msg.empty())
		return cmd_none;

	size_t start_pos = msg.find(':');
	std::string last_arg;
	if (start_pos != std::string::npos)
	{
		std::string left = msg.substr(0, start_pos);
		std::string right = msg.substr(start_pos + 1);
		msg = left;
		last_arg = right;
	}
	if (msg.empty())
		return cmd_none;

	std::vector<std::string> s = Utils::split(msg, ' ');
	if (s.empty())
		return cmd_none;
	std::string cmd_raw = s.front();
	std::transform(cmd_raw.begin(), cmd_raw.end(), cmd_raw.begin(), ::toupper);

	user_commands cmd = cmd_none;
	if (cmd_raw == "PASS")
		cmd = cmd_pass;
	else if (cmd_raw == "USER")
		cmd = cmd_user;
	else if (cmd_raw == "NICK")
		cmd = cmd_nick;
	else if (cmd_raw == "PRIVMSG")
		cmd = cmd_privmsg;
	else if (cmd_raw == "ISON")
		cmd = cmd_ison;
	else if (cmd_raw == "NOTICE")
		cmd = cmd_notice;
	else if (cmd_raw == "JOIN")
		cmd = cmd_join;
	else if (cmd_raw == "KICK")
		cmd = cmd_kick;
	else if (cmd_raw == "BOT")
		cmd = cmd_bot;
	else if (cmd_raw == "TOPIC")
		cmd = cmd_topic;

	if (cmd != cmd_none)
	{
		for (std::vector<std::string>::iterator it = s.begin() + 1; it != s.end(); ++it)
			if (!it->empty())
				args_out.push_back(*it);
		if (!last_arg.empty())
			args_out.push_back(last_arg);
	}
	return cmd;
}
