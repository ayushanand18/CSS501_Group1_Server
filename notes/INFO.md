# Information
> Interfaces and LLD for the program

```cpp
class File{
public:
    // data members of a File type, adjust according to your needs
    string name, file_id, author, location_on_disc, last_update_time;
    size_t size;
    unsigned int num_downloads;
    vector<string> access_to;

    // constructor to the File Class
    File(const string name, const string file_id, const string author, const string location_on_disc, const string last_update_time, const size_t size, const unsigned int num_downloads) = 0
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
    bool checkUserWithUserID(string user_id)
    // function to signin with user_id, password
    bool signInWithUserIDPassword(string user_id, string password)
    // function to register with name, user_id, password
    bool registerWithUserIDPassword(string name, string user_id, string password)
    // function to replicate data across other servers, takes no argument and no return
    // must register this as a periodic event (cron job)
    void replicateDataAcrossServer()
    // function to handleDownload
    void handleDownload(dataOfUser)
    // function to handleUpload
    void handleUpload(dataOfFile)
    // function to check if file with the hash is already present on server or not
    // in the file_hashes map
    bool checkFilePresent(string file_hash)
};
```