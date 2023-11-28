#include <chrono>
#include <vector>
#include <string>
#include <set>
#include <unordered_map>
#include <iostream>
#include <functional>
#include <fstream>
#include <utility>
#include <openssl/md5.h>
#include <msgpack.hpp>
#include "rpc/server.h"

namespace FSS_Server
{
    // function to return current data and time
    std::string return_current_time_and_date()
    {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
        return ss.str();
    }

    // function to print logs on STDOUT as well as LOG_FILE
    void LOG_SERVICE(std::string log_type, std::string log)
    {
        static std::ofstream logFile("LOG_FILE.txt");
        std::string curr_time = return_current_time_and_date();
        logFile << log_type << " [" << curr_time << "] " << log << std::endl;
        std::cout << log_type << " [" << curr_time << "] " << log << std::endl;
    }

    // function to split a string based on a particular delimeter
    // contributed by @Amit
    std::vector<std::string> split(std::string s, std::string delimiter)
    {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        std::string token;
        std::vector<std::string> res;

        while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
        {
            token = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(token);
        }

        res.push_back(s.substr(pos_start));
        return res;
    }

    class ServerFile
    {
    public:
        // data members of a File type, adjust according to your needs
        std::string name, file_id, author, location_on_disc, last_update_time, access_to;
        size_t size;
        unsigned int num_downloads;

        ServerFile()
        {
            this->access_to = "*";
            this->author = "none";
            this->file_id = "0";
            this->last_update_time = "0";
            this->location_on_disc = "null";
            this->name = "no_file";
            this->num_downloads = 0;
            this->size = 0;
        }
        // constructor to the File Class
        ServerFile(const std::string name, const std::string file_id, const std::string author, const std::string location_on_disc, const std::string last_update_time, const size_t size, const unsigned int num_downloads, std::string access_to)
        {
            this->access_to = access_to;
            this->author = author;
            this->file_id = file_id;
            this->last_update_time = last_update_time;
            this->location_on_disc = location_on_disc;
            this->name = name;
            this->num_downloads = num_downloads;
            this->size = size;
        }
        ServerFile(const ServerFile &server_file)
        {
            this->access_to = server_file.access_to;
            this->author = server_file.author;
            this->file_id = server_file.file_id;
            this->last_update_time = server_file.last_update_time;
            this->location_on_disc = server_file.location_on_disc;
            this->name = server_file.name;
            this->num_downloads = server_file.num_downloads;
            this->size = server_file.size;
        }
        MSGPACK_DEFINE(name, file_id, author, location_on_disc, last_update_time, size, num_downloads, access_to);
    };

    class Server
    {
    private:
        std::vector<std::string> server_ips;
        std::unordered_map<std::string, ServerFile> file_table;
        std::unordered_map<std::string, std::string> user_ids;

        // function to get content of a file
        // contributed by @Ajay
        std::string __getFileContent(std::string filepath);

        // function to check user with user_id present or not
        bool __checkUserWithUserID(std::string user_id);

        // function to generate fileID from content of file
        std::string __getFileID(std::string input);

    public:
        // constructor to setup things
        Server();

        // destructor to destory things
        ~Server();

        // function to signin with user_id, password
        // contributed by @Amit
        bool signInWithUserIDPassword(std::string user_id, std::string password);

        // function to register with name, user_id, password
        // contributed by @Ajay
        bool registerWithUserIDPassword(std::string name, std::string user_id, std::string password);

        // function to return list of files on server
        std::unordered_map<std::string, ServerFile> getFilesList();

        // function to return list of users (names, userIDs)
        std::vector<std::pair<std::string, std::string>> getUserList();

        // function to replicate data across other servers, takes no argument and no return
        // must register this as a periodic event (cron job)
        void replicateDataAcrossServer();

        // function to handleDownload
        std::string handleDownload(std::string file_id);

        // function to start the partial upload process
        bool startUpload(std::string file_name);

        // function to handleUpload
        void handleUpload(std::string name, std::string author, std::string permissions, unsigned int size, std::string content);

        // function to finish the upload process and reap all the chunks
        void finishUpload(std::string name, std::string author, std::string permissions, unsigned int size, std::string content);

        // function to check if file with the hash is already present on server or not
        // in the file_hashes map
        bool checkFilePresent(std::string file_hash);

        // function to check if access is given to the user_id, file_id
        bool checkAccess(std::string user_id, std::string file_id);
    };
}

// function declarations follow for Server Class
FSS_Server::Server::Server() 
{
    // update the user logins in the server using FS based database
    std::ifstream user_db("user_db.txt");
    std::string line;
    while (std::getline(user_db, line))
    {
        std::vector<std::string> splitted = split(line, " ");
        // name user_id passwd <- user_db.txt file
        user_ids[splitted[1]] = splitted[2];
    }
}

std::string FSS_Server::Server::__getFileContent(std::string filepath)
{
    std::ifstream file(filepath);
    std::stringstream content;

    if (file.is_open())
    {
        content << file.rdbuf();
        file.close();
    }
    else
    {
        LOG_SERVICE("ERR", "Unable to open file at " + filepath);
        throw std::runtime_error("Unable to open file: " + filepath);
    }

    return content.str();
}

bool FSS_Server::Server::__checkUserWithUserID(std::string user_id)
{
    return user_ids.count(user_id);
}

std::string FSS_Server::Server::__getFileID(std::string input)
{
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5((const unsigned char *)input.c_str(), input.length(), digest);

    std::stringstream ss;
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];
    }
    std::string res_file_id = ss.str();
    if (res_file_id.size() > 20)
    {
        res_file_id = res_file_id.substr(0, 20);
    }
    return res_file_id;
}

bool FSS_Server::Server::signInWithUserIDPassword(std::string user_id, std::string password)
{
    if (user_ids.count(user_id) && user_ids[user_id] == password)
        return true;
    return false;
}

bool FSS_Server::Server::registerWithUserIDPassword(std::string name, std::string user_id, std::string password)
{
    if (!__checkUserWithUserID(user_id))
    {
        user_ids[user_id] = password;
        std::ofstream user_db("user_db.txt", std::ios::app);
        if (user_db.is_open())
        {
            user_db << name << " " << user_id << " " << password << "\n";
            user_db.close();
            return true;
        }
    }
    return false;
}

std::unordered_map<std::string, FSS_Server::ServerFile> FSS_Server::Server::getFilesList()
{
    return this->file_table;
}

std::vector<std::pair<std::string, std::string>> FSS_Server::Server::getUserList()
{
    std::vector<std::pair<std::string, std::string>> user_list;
    std::ifstream users("user_db.txt");
    std::string line;
    while (std::getline(users, line))
    {
        auto splitted_data = FSS_Server::split(line, " ");
        user_list.push_back({splitted_data[0], splitted_data[1]});
    }
    return user_list;
}

void FSS_Server::Server::replicateDataAcrossServer() {
    // do nothing
}

std::string FSS_Server::Server::handleDownload(std::string file_id)
{
    if (this->file_table.count(file_id))
    {
        // send contents of file
        return __getFileContent(file_table[file_id].location_on_disc);
    }
    else
    {
        LOG_SERVICE("ERR", "File resquest for {" + file_id + "}: File not found!");
        return "404: NOT FOUND";
    }
}

bool FSS_Server::Server::startUpload(std::string file_name) {
    system(("mkdir pending_uploads/"+file_name).c_str());
    return true;
}

void FSS_Server::Server::handleUpload(std::string name, std::string author, std::string permissions, unsigned int size, std::string content)
{   
    std::string splitted_name = split(name, "-").front();
    std::string location_on_disc = "pending_uploads/" + splitted_name + "/" + name;
    std::ofstream write_file(location_on_disc);
    FSS_Server::LOG_SERVICE("INFO", "upload saved at: " + location_on_disc);
    write_file << content;
}

void FSS_Server::Server::finishUpload(std::string name, std::string author, std::string permissions, unsigned int size, std::string content) {
    std::string file_id = this->__getFileID(content);
    std::string location_on_disc = "uploaded_files/" + file_id + "-" + name;
    std::string curr_time = return_current_time_and_date();
    FSS_Server::ServerFile new_file(name, file_id, author, location_on_disc, curr_time, size, 0, permissions);

    this->file_table[file_id] = new_file;
}

bool FSS_Server::Server::checkFilePresent(std::string file_hash)
{
    return false;
}

bool FSS_Server::Server::checkAccess(std::string user_id, std::string file_id)
{
    std::vector<std::string> splitted_data = FSS_Server::split(file_table[file_id].access_to, " ");
    if (file_table[file_id].access_to == "*" or find(splitted_data.begin(), splitted_data.end(), user_id) != splitted_data.end())
        return true;
    return false;
}

FSS_Server::Server::~Server() {
    LOG_SERVICE("INFO", "SHUTDOWN\n");
}