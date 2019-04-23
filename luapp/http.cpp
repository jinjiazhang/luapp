#include "http.h"

http::http()
{
    last_token_ = 0;
    curl_global_init(CURL_GLOBAL_ALL);
    multi_ = curl_multi_init();
    curl_multi_setopt(multi_, CURLMOPT_MAXCONNECTS, 8);

    share_ = curl_share_init();
    curl_share_setopt(share_, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
}

http::~http()
{
    curl_share_cleanup(share_);
    curl_multi_cleanup(multi_);
    curl_global_cleanup();
}

int http::update()
{
    int running;
    curl_multi_perform(multi_, &running);

    CURLMsg* msg;
    int inqueue;
    while(msg = curl_multi_info_read(multi_, &inqueue))
    {
        if(msg->msg == CURLMSG_DONE)
        {
            CURL* curl = msg->easy_handle;
            taskdata* task;
            curl_easy_getinfo(curl, CURLINFO_PRIVATE, &task);
            task->handle->respond(task->token, msg->data.result, task->content.c_str());
            delete task;

            curl_multi_remove_handle(multi_, curl);
            curl_easy_cleanup(curl);
        }
    }
    return running;
}

size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    http::taskdata* task = (http::taskdata*)userdata;
    task->content.append(ptr, size * nmemb);
    return size * nmemb;
}

int http::get(callback* handle, const char* url)
{
    taskdata* task = new taskdata();
    task->token = ++last_token_;
    task->handle = handle;
    task->content.clear();
    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 8);
    curl_easy_setopt(curl, CURLOPT_SHARE, share_);
    curl_easy_setopt(curl, CURLOPT_PRIVATE, task);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, task);
    curl_multi_add_handle(multi_, curl);
    return task->token;
}

int http::post(callback* handle, const char* url, const char* data)
{
    taskdata* task = new taskdata();
    task->token = ++last_token_;
    task->handle = handle;
    task->content.clear();
    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 8);
    curl_easy_setopt(curl, CURLOPT_SHARE, share_);
    curl_easy_setopt(curl, CURLOPT_PRIVATE, task);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, task);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_multi_add_handle(multi_, curl);
    return task->token;
}