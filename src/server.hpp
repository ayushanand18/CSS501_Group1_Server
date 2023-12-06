#include <chrono>
#include <vector>
#include <string>
#include <set>
#include <unordered_map>
#include <iostream>
#include <functional>
#include <fstream>
#include <utility>
#include <openssl/evp.h>
#include <msgpack.hpp>
#include <dirent.h>
#include <numeric>
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

        // function to get file size
        unsigned int __getFileSize(std::string filepath);

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
        std::string startUpload(std::string file_name, std::string author);

        // function to handleUpload
        void handleUpload(std::string file_id, std::string file_name, std::string content);

        // function to finish the upload process and reap all the chunks
        bool finishUpload(std::string file_id, std::string file_name, std::string author, std::string permissions);

        // function to resume the upload process and send un-uploaded chunks
        std::pair<bool, std::vector<std::string>> resumeUpload(std::string file_id);

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

    // also update from list of files
    std::ifstream files_db("fileslist_db.txt");
    std::string line;
    while(std::getline(files_db, line))
    {
        std::vector<std::string> splitted = split(line, " ");
        FSS_Server::ServerFile new_file(splitted[1], splitted[0], splitted[2], splitted[3], splitted[4], stoi(splitted[5]), stoi(splitted[6]), splitted[7]);
        file_table[splitted[0]] = new_file;
    }
}

unsigned int FSS_Server::Server::__getFileSize(std::string filepath) {
    std::ifstream file(filepath, std::ios::binary);
    unsigned int file_size = 0;
    if (file.is_open())
    {
        file.seekg(0, std::ios::end);                      // Move to the end of the file
        file_size = static_cast<std::size_t>(file.tellg()); // Get the file size
    }
    else
    {
        throw std::runtime_error("Unable to open file: " + filepath);
    }
    return file_size;
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
    EVP_MD_CTX *mdctx;
    const EVP_MD *md = EVP_md5();
    unsigned int md_len;
    unsigned char digest[EVP_MAX_MD_SIZE];

    mdctx = EVP_MD_CTX_new();

    EVP_DigestInit_ex(mdctx, md, nullptr);
    EVP_DigestUpdate(mdctx, input.c_str(), input.length());
    EVP_DigestFinal_ex(mdctx, digest, &md_len);

    EVP_MD_CTX_free(mdctx);

    std::stringstream ss;
    for (unsigned int i = 0; i < md_len; i++)
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

std::string FSS_Server::Server::startUpload(std::string file_name, std::string author) {
    // function to handle start of upload process
    // get the file id from name and author, and make a direc
    std::string file_id = __getFileID(file_name+author);
    // create a new direc in pending_uploads/
    system(("mkdir pending_uploads/"+file_id).c_str());
    // return the new file_id
    return file_id;
}

void FSS_Server::Server::handleUpload(std::string file_id, std::string file_name, std::string content)
{   
    std::string location_on_disc = "pending_uploads/" + file_id + "/" + file_name;
    std::ofstream write_file(location_on_disc);
    FSS_Server::LOG_SERVICE("INFO", "upload saved at: " + location_on_disc);
    write_file << content;
}

bool FSS_Server::Server::finishUpload(std::string file_id, std::string file_name, std::string author, std::string permissions) {
    // find the folder, combine into one file, and write it onto the destination
    std::string folder_name = "pending_uploads/"+file_id;
    std::vector<std::string> file_chunks;
    unsigned int file_size = 0;

    // get the list of files
    DIR *d;
    struct dirent *dir;
    d = opendir(folder_name.c_str());
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if(dir->d_name[0] == '.') continue;
            file_chunks.push_back(dir->d_name);
            file_size += __getFileSize(folder_name+"/"+dir->d_name);
        }
        closedir(d);
    }

    std::string file_concats = std::accumulate(file_chunks.begin()+1, file_chunks.end(), folder_name + "/" + file_chunks[0], [&folder_name](const std::string& a, const std::string& b){
        return a  + " " + folder_name + "/" + b;
    });

    // combine into one single file
    std::string location_on_disc = "uploaded_files/"+file_id + "-" + file_name;
    system(("cat " + file_concats + " > " + location_on_disc).c_str());
    std::string curr_time = return_current_time_and_date();

    // now delete all those files after combining
    system(("rm -rf " + folder_name).c_str());

    FSS_Server::ServerFile new_file(file_name, file_id, author, location_on_disc, curr_time, file_size, 0, permissions);

    this->file_table[file_id] = new_file;
    
    return true;
}

std::pair<bool, std::vector<std::string>> FSS_Server::Server::resumeUpload(std::string file_id) {
    // send list of strings of chunk ids received
    std::string folder_name = "pending_uploads/"+file_id;
    DIR *d;
    struct dirent *dir;

    // did not find a match in already uploaded files,
    // so send those chunk names which are received
    std::vector<std::string> file_chunks;
    
    d = opendir(folder_name.c_str());
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if(dir->d_name[0] == '.') continue;
            file_chunks.push_back(dir->d_name);
        }
        closedir(d);
    } else {
        return make_pair(false, std::vector<std::string>());
    }
    
    return make_pair(true, file_chunks);
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
    // ServerFile(const std::string name, const std::string file_id, const std::string author, const std::string location_on_disc, const std::string last_update_time, const size_t size, const unsigned int num_downloads, std::string access_to)
    std::ofstream files_db("fileslist_d.txt");
    for(auto& [file_id, file_data]: file_table) 
    {
        files_db << file_id << " " << file_data.name << " " << file_data.author << " "
                << file_data.location_on_disc << " " << file_data.last_update_time << " " 
                << file_data.size << " " << file_data.num_downloads << " " << file_data.access_to;
    }

    LOG_SERVICE("INFO", "SHUTDOWN\n");
}