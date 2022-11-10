#include "Server/Server.h"
#include "Utils/Utils.h"

int main(int argc, char **argv)
{
	if (argc != 3)
		Utils::error_exit(ERROR_ARGC);

	int server_port = Utils::int_parse(argv[1], ERROR_PORT_PARSE);
	if (server_port < 0 || server_port > 65535)
		Utils::error_exit(ERROR_PORT_RANGE);

	std::string psw(argv[2]);
	unsigned long password_hash = 0;
	if (!psw.empty())
		password_hash = Utils::to_hash(psw);

	Server server(server_port, password_hash);
	server.run();

	return 0;
}
