#include "HttpServer.hpp"
#include "Err.hpp"
#include "Util.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

const std::string SEP = "\r\n";
// 一般一个webserver，不做特殊说明，如果用户之间默认访问‘/’, 我们绝对不能把把整站给对方
// 需要添加默认首页！！而且，不能让用户访问wwwroot里面的任何一个目录本身，也可以给每一个目录都带上一个默认首页！
const std::string defauleHomePage = "index.html";
const std::string webRoot = "./wwwroot"; // web根目录
const std::string page_404 = "./wwwroot/err_404.html";

class HttpRequest
{
public:
    HttpRequest()
        : path_(webRoot)
    {
    }
    ~HttpRequest() {}
    void Print()
    {
        logMessage(Debug, "method: %s url: %s version: %s",
                   method_.c_str(), url_.c_str(), httpVersion_.c_str());

        // for(const auto& line : body_)
        //     logMessage(Debug, "-%s",line.c_str());

        logMessage(Debug, "path: %s", path_.c_str());
        logMessage(Debug, "suffix: %s", suffix_.c_str());
    }

public:
    std::string method_;
    std::string url_;
    std::string httpVersion_;
    std::vector<std::string> body_;
    std::string path_;

    std::string suffix_;
};

HttpRequest Deserialize(std::string &message)
{
    HttpRequest req;
    std::string line = Util::ReadOneLine(message, SEP);
    Util::ParseRequestLine(line, &req.method_, &req.url_, &req.httpVersion_);

    while (!message.empty())
    {
        line = Util::ReadOneLine(message, SEP);
        req.body_.push_back(line);
    }
    req.path_ += req.url_;
    if (req.path_[req.path_.size() - 1] == '/')
        req.path_ += defauleHomePage;

    auto pos = req.path_.rfind(".");
    if (pos == std::string::npos)
        req.suffix_ = ".html";
    else
        req.suffix_ = req.path_.substr(pos);
    return req;
}

std::string GetContentType(const std::string &suffix)
{
    std::string content_type = "Content-Type: ";
    if (suffix == ".html" || suffix == ".htm")
        content_type + "text/html";
    else if (suffix == ".css")
        content_type += "text/css";
    else if (suffix == ".js")
        content_type += "application/x-javascript";
    else if (suffix == ".png")
        content_type += "image/png";
    else if (suffix == ".jpg")
        content_type += "image/jpeg";
    else
    {
    }
    return content_type + SEP;
}

class Session
{
public:
    std::string name;
    std::string passwd;
    uint64_t loginTime;

    // 用户的其他信息
    int status;
    int fd;
    int sesisonid;

};
std::unordered_map<int, Session*> sessions;
// bool Login(std::string& message)
// {
//     // post-> 正文包含账号密码
//     std::string name;
//     std::string passwd;

//     if(check(name,passwd))
//     {
//         Session = new Session(name, passwd...);
//         int random = rand();
//         sessions.insert(std::pair<int, Session*>(random, session));

//         http response 
//         Set-Cookie: sessionid=random;
//     }
// }

std::string HandlerHttp(std::string &message)
{
    // 读取请求
    // 走到这里，reuquest一定是一个完整的http请求
    // 给别人返回的是一个http response
    std::cout << "---------------------------------" << std::endl;
    std::cout << message << std::endl;
    // // 2. 反序列化和分析请求
    // HttpRequest req = Deserialize(message);
    // req.Print();
    // // 3. 使用请求
    // std::string body;
    // std::string response;
    // if (Util::ReadFile(req.path_, &body) == true)
    // {
    //     response = "HTTP/1.0 200 OK" + SEP;
    //     response += "Content-Length: " + std::to_string(body.size()) + SEP;
    //     response += GetContentType(req.suffix_);
    //     response += SEP;
    //     response += body;
    // }
    // else
    // {
    //     response = "HTTP/1.0 404 Not Found" + SEP;
    //     Util::ReadFile(page_404, &body);
    //     response += "Content-Length: " + std::to_string(body.size()) + SEP;
    //     response += GetContentType(".html");
    //     response += SEP;
    //     response += body;
    // }

    // 4. 重定向测试
    // std::string response;
    // // response = "HTTP/1.0 302 Found" + SEP;
    // response = "HTTP/1.0 301 Moved Permanently" + SEP;
    // response += "Location: https://www.baidu.com/" + SEP;
    // response += SEP;

    // 5. cookie && session实验
    // std::string response;
    // response = "HTTP/1.0 200 OK" + SEP;
    // response += "Set-Cookie: sessionid=1234abcd" + SEP;
    // response += SEP;

    if()
    request -> sessionid;
    sessions[sessionid]->status;

    return response;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        exit(USAGE_ERR);
    }
    uint16_t port = atoi(argv[1]);
    std::unique_ptr<HttpServer> tsvr(new HttpServer(HandlerHttp, port));

    tsvr->InitServer();
    tsvr->Start();
    return 0;
}