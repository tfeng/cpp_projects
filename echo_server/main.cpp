#include "cpprest/http_listener.h"
#include "cpprest/uri.h"
#include <string>

const char *LISTEN_ADDRESS = "http://0.0.0.0:37813";

using namespace pplx;
using namespace std;
using namespace web::http;
using namespace web::http::experimental::listener;

class Server {
public:
  Server(string address);

  task<void> open() { return m_listener.open(); }
  task<void> close() { return m_listener.close(); }

private:
  void handle_get(http_request message);
  void handle_put(http_request message);

  http_listener m_listener;
  string m_value;
};

Server::Server(string address): m_listener(U(address))
{
  m_listener.support(methods::GET, std::bind(&Server::handle_get, this, placeholders::_1));
  m_listener.support(methods::PUT, std::bind(&Server::handle_put, this, placeholders::_1));
}

void Server::handle_get(http_request message)
{
  message.reply(status_codes::OK, m_value);
  cout << "GET: " << m_value << endl;
}

void Server::handle_put(http_request message)
{
  m_value = message.extract_string().get();
  message.reply(status_codes::OK);
  cout << "SET: " << m_value << endl;
}

int main(int argc, char* argv[])
{
  Server server(LISTEN_ADDRESS);
  server.open().wait();

  cout << "Press CTRL-C to exit... ";
  string line;
  while (true)
    getline(cin, line);

  server.close().wait();
}
