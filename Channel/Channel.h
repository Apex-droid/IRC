#ifndef FT_IRC_CHANNEL_H
#define FT_IRC_CHANNEL_H

#include "../User/User.h"

class Server;
class User;

#define DEFAULT_TOPIC	"ʕ•́ᴥ•̀ʔ"

class Channel
{
private:
	Server						&_server;
	std::vector<User*>			_users;
	std::string 				_topic;
	std::string 				_name;
	User 						*_admin;
	std::vector<std::string>	_new_messages;

public:
	Channel(Server &server, std::string name, User* admin);
	~Channel();

	std::string	get_topic();
	std::string	get_name();
	std::string get_user_nicks();
	std::string	get_admin_nick();
	void 		leave_user(User *user);
	bool 		is_empty_channel();
	void		add_new_user(User *user);
	void		new_channel_member_come();
	void		push_new_message(const std::string& message);
	void 		send_messages_to_members();
	bool 		kick_user(const std::string& nick);
	void		set_topic(const std::string& new_topic);
	std::string get_info();
};

#endif
