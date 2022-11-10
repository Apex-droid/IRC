#include "Utils.h"

void Utils::error_print(const std::string &msg)
{
	std::cerr << "Error: " <<  msg << std::endl;
}

void Utils::error_print(int fd, const std::string &msg)
{
	std::cerr << "Error: [" << fd << "]: " << msg << std::endl;
}

void Utils::error_exit(const std::string &msg)
{
	error_print(msg);
	exit(ERROR_EXIT_CODE);
}

bool Utils::int_try_parse(const std::string &s, int &value,
						  const std::string &error_msg)
{
	char				c;
	std::stringstream	ss(s);

	ss >> value;
	if (ss.fail() || ss.get(c))
	{
		error_print(error_msg);
		return false;
	}
	return true;
}

int Utils::int_parse(const std::string& s, const std::string& error_msg)
{
	int value;

	if (!int_try_parse(s, value, error_msg))
		exit(ERROR_EXIT_CODE);
	return value;
}

// http://www.cse.yorku.ca/~oz/hash.html djb2
unsigned long Utils::to_hash(const std::string& s)
{
	unsigned long hash = 5381;
	int c;
	const char *ptr = s.c_str();
	while (*ptr)
	{
		c = (unsigned char)*ptr;
		hash = ((hash << 5) + hash) + c;
		ptr++;
	}
	return hash;
}

std::vector<std::string> Utils::split(const std::string& s, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);

	while (std::getline(tokenStream, token, delimiter))
		tokens.push_back(token);
	return tokens;
}

void Utils::replace(std::string& str, const std::string& from,
					const std::string& to)
{
	size_t start_pos = str.find(from);
	if(start_pos == std::string::npos)
		return;
	str.replace(start_pos, from.length(), to);
}

bool Utils::is_letter(char c)
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool Utils::is_special(char c)
{
	return (c >= 0x5b && c <= 0x60) || (c >= 0x7b && c <= 0x7d);
}

bool Utils::is_digit(char c)
{
	return (c >= '0' && c <= '9');
}
