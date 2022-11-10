#ifndef UTILS_H
#define UTILS_H

#define ERROR_ARGC				"run: ./ircserv <port> <password>"
#define ERROR_PORT_PARSE			"Port must be number"
#define ERROR_PORT_RANGE			"Port must be in range from 0 to 65535"
#define ERROR_NEW_SOCKET			"Creating a socket"
#define ERROR_BIND_PORT			"Cant bind port"
#define ERROR_PORT_LISTENING		"Set socket for listening"
#define ERROR_SERVER_READY		"Server not inited"
#define ERROR_POLL				"Poll scheduling. Go to sleep"
#define ERROR_CLIENT_NEW_SOCKET	"Client socket creation"
#define ERROR_SEND_MSG			"Send message to client"
#define ERROR_SOCK_OPT			"Setsockopt(SO_REUSEADDR) failed"

#define ERROR_EXIT_CODE			1
#define POLL_SLEEP_TIME_MS		10 * 1000

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

enum user_commands
{
	cmd_none,
	cmd_pass,
	cmd_user,
	cmd_nick,
	cmd_privmsg,
	cmd_ison,
	cmd_notice,
	cmd_join,
	cmd_kick,
	cmd_bot,
	cmd_topic
};

class Utils
{
public:
	static void error_print(const std::string& msg);
	static void error_print(int fd, const std::string& msg);
	static void error_exit(const std::string& msg);

	static int	int_parse(const std::string& s, const std::string& error_msg);
	static bool	int_try_parse(const std::string& s, int &value, const std::string& error_msg);

	static unsigned long			to_hash(const std::string& s);
	static std::vector<std::string>	split(const std::string& s, char delimiter);
	static void						replace(std::string& str, const std::string& from, const std::string& to);

	static bool	is_letter(char c);
	static bool	is_special(char c);
	static bool	is_digit(char c);
};

#endif
