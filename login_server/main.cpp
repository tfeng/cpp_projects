#include "cpprest/http_listener.h"
#include "cpprest/uri.h"
#include <iomanip>
#include <openssl/md5.h>
#include <pqxx/pqxx>
#include <sstream>
#include <string>

const char *LISTEN_ADDRESS = "http://0.0.0.0:37813";
const char *PQ_CONNECTION  = "postgresql://login_server@localhost/login_server";

using namespace pplx;
using namespace std;
using namespace web::http;
using namespace web::http::experimental::listener;
using namespace web::json;

namespace {
static string get_md5(string s) {
  unsigned char md5[MD5_DIGEST_LENGTH];
  MD5((unsigned char*)s.c_str(), s.length(), md5);

  stringstream ss;
  ss << hex;
  for(int i=0; i < MD5_DIGEST_LENGTH; i++)
    ss << setw(2) << setfill('0') << (int)md5[i];
  return ss.str();
}
}

class Server {
public:
  Server(string address);

  task<void> open() { return m_listener.open(); }
  task<void> close() { return m_listener.close(); }

private:
  void handle_get(http_request message);
  void handle_post(http_request message);
  void handle_put(http_request message);

  http_listener m_listener;
  pqxx::connection m_conn;
};

Server::Server(string address): m_listener(U(address)), m_conn(PQ_CONNECTION)
{
  m_listener.support(methods::GET, std::bind(&Server::handle_get, this, placeholders::_1));
  m_listener.support(methods::POST, std::bind(&Server::handle_post, this, placeholders::_1));
  m_listener.support(methods::PUT, std::bind(&Server::handle_put, this, placeholders::_1));
}

void Server::handle_get(http_request message)
{
  try {
    pqxx::work txn(m_conn);
    pqxx::row r = txn.exec1("SELECT COUNT(0) FROM users");
    int count = r[0].as<int>();
    message.reply(status_codes::OK, "Total users: " + to_string(count) + "\n");
  } catch (const std::exception &e) {
    cerr << "Error: " << e.what() << endl;
    message.reply(status_codes::InternalError);
  }
}

void Server::handle_post(http_request message)
{
  try {
    pqxx::work txn(m_conn);
    string data = message.extract_string().get();
    value json = value::parse(data);
    string username = json.at("username").as_string();
    string password = json.at("password").as_string();
    pqxx::row r = txn.exec1("SELECT COUNT(0) FROM users WHERE username = " +
                            txn.quote(username) +
                            " AND password_md5 = " +
                            txn.quote(get_md5(password)));
    int count = r[0].as<int>();
    if (count == 1)
      message.reply(status_codes::OK, "Login successful.\n");
    else
      message.reply(status_codes::Unauthorized, "Login failure.\n");
  } catch (const std::exception &e) {
    cerr << "Error: " << e.what() << endl;
    message.reply(status_codes::InternalError);
  }
}

void Server::handle_put(http_request message)
{
  try {
    pqxx::work txn(m_conn);
    string data = message.extract_string().get();
    value json = value::parse(data);
    string username = json.at("username").as_string();
    string password = json.at("password").as_string();
    txn.exec0("INSERT INTO users(username, password_md5) VALUES(" +
              txn.quote(username) +
              ", " +
              txn.quote(get_md5(password)) +
              ")");
    txn.commit();
    message.reply(status_codes::OK, "User created.\n");
  } catch (const std::exception &e) {
    cerr << "Error: " << e.what() << endl;
    message.reply(status_codes::InternalError);
  }
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
