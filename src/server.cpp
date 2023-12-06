// server functions and class definitions
// all works contributed arre annotated with an '@' before the functions
// in case of no annotation, it is contributed by original author '@ayushanand18'

#include "server.hpp"

int main(int argc, char *argv[])
{
  FSS_Server::LOG_SERVICE("INFO", "SETTING UP SERVER: Runtime Loaded, initializing objects.");
  // Creating a server that listens on port 8080
  rpc::server srv(8080);

  FSS_Server::Server serv_instance;

  FSS_Server::LOG_SERVICE("INFO", "RUNNING: Running server at 8080...");

  // API contracts
  srv.bind("signin",
           [&serv_instance](std::string user_id, std::string password)
           {
             FSS_Server::LOG_SERVICE("INFO", "SIGNIN: request (" + user_id + ")");
             return serv_instance.signInWithUserIDPassword(user_id, password);
           });

  srv.bind("register",
           [&serv_instance](std::string name, std::string user_id, std::string password)
           {
             FSS_Server::LOG_SERVICE("INFO", "REGISTER: request (" + name + ", " + user_id + ", ###)");
             return serv_instance.registerWithUserIDPassword(name, user_id, password);
           });

  srv.bind("start-upload",
           [&serv_instance](std::string file_name, std::string author) -> std::string
           {
             FSS_Server::LOG_SERVICE("INFO", "Start Upload: request. " + file_name);
             return serv_instance.startUpload(file_name, author);
           });

  srv.bind("check-upload",
           [&serv_instance](std::string file_id) -> std::pair<bool, std::vector<std::string>>
           {
             FSS_Server::LOG_SERVICE("INFO", "Resume Upload: request. " + file_id);
             return serv_instance.resumeUpload(file_id);
           });

  srv.bind("upload",
           [&serv_instance](std::string file_id, std::string name, std::string content) -> void
           {
             FSS_Server::LOG_SERVICE("INFO", "UPLOAD Chunk: request (" + file_id + ", " + name + ", ###)");
             serv_instance.handleUpload(file_id, name, content);
           });

  srv.bind("finish-upload",
           [&serv_instance](std::string file_id, std::string file_name, std::string author, std::string permissions) -> bool
           {
             FSS_Server::LOG_SERVICE("INFO", "Finished Upload: request. " + file_id);
             return serv_instance.finishUpload(file_id, file_name, author, permissions);
           });

  srv.bind("download",
           [&serv_instance](std::string file_id) -> std::string
           {
             FSS_Server::LOG_SERVICE("INFO", "DOWNLOAD: (" + file_id + ")");
             return serv_instance.handleDownload(file_id);
           });

  srv.bind("get_files_list",
           [&serv_instance]() -> std::unordered_map<std::string, FSS_Server::ServerFile>
           {
             FSS_Server::LOG_SERVICE("INFO", "GET FILES LIST. request");
             return serv_instance.getFilesList();
           });

  srv.bind("get_users_list",
           [&serv_instance]() -> std::vector<std::pair<std::string, std::string>>
           {
             FSS_Server::LOG_SERVICE("INFO", "GET USERS LIST. request");
             return serv_instance.getUserList();
           });

  srv.bind("check_access",
           [&serv_instance](std::string user_id, std::string file_id) -> bool
           {
             FSS_Server::LOG_SERVICE("INFO", "CHECK ACCESS: (" + user_id + " , " + file_id + ") ");
             return serv_instance.checkAccess(user_id, file_id);
           });

  srv.bind("ping", [&]() -> bool
           { 
    FSS_Server::LOG_SERVICE("INFO", "PING Test.");
    return true; });

  // Run the server loop.
  srv.async_run(4);

  // block the shutdown of server by waiting for input
  std::string choice;
  std::cin >> choice;

  return 0;
}
