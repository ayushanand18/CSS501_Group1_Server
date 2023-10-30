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
