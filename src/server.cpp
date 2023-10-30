#include <vector>
#include <string>
#include <set>
#include <unordered_map>
#include <iostream>
#include <functional>
#include "rpc/server.h"
using namespace std;

class ThreadPool {
public:
  // todo: implement a ThreadPool or get it from a library
  //       for later work, multithreading is the last priority
};

class File{
public:
    // data members of a File type, adjust according to your needs
    string name, file_id, author, location_on_disc, last_update_time;
    size_t size;
    unsigned int num_downloads;
    vector<string> access_to;

    // constructor to the File Class
    File(const string name, const string file_id, const string author, const string location_on_disc, const string last_update_time, const size_t size, const unsigned int num_downloads) {
      this->access_to = access_to;
      this->author = author;
      this->file_id = file_id;
      this->last_update_time = last_update_time;
      this->location_on_disc = location_on_disc;
      this->name = name;
      this->num_downloads = num_downloads;
      this->size = size;
    }
};

class FileTransit{
public:
  string name;
  string author;
  string permissions;
  size_t size;
  // considering content as a text, will later generalise for all data types
  string content;
};

class Server{
private:
    vector<string> server_ips;
    unordered_set<string> file_ids;
    unordered_map<string, File> file_table;
    unordered_map<string, string> file_hashes; // maps file_hash->file_id
    // own ThreadPool of threads to submit tasks to
    ThreadPool thread_exec;
public:
    // constructor to setup things
    Server() {
        // init all server_ips, file_ids, file_table
        // attach signal handling to handleDownload and handleUpload function
    }
    // function to check user with user_id present or not
    bool checkUserWithUserID(string user_id);
    // function to signin with user_id, password
    bool signInWithUserIDPassword(string user_id, string password);
    // function to register with name, user_id, password
    bool registerWithUserIDPassword(string name, string user_id, string password);
    // function to replicate data across other servers, takes no argument and no return
    // must register this as a periodic event (cron job)
    void replicateDataAcrossServer();
    // function to handleDownload
    void handleDownload(FileTransit dataOfUser);
    // function to handleUpload
    void handleUpload(string name, string author, string permissions, unsigned int size, string content);
    // function to check if file with the hash is already present on server or not
    // in the file_hashes map
    bool checkFilePresent(string file_hash);
};

