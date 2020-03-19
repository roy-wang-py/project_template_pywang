
#include "common/common_header.hxx"
#include "download_file.hxx"
#include <sstream>
#include <exception>
NAMESPACE_PREFIX
using std::string;

void Request::Connect(){
    if(url_.empty()){
        common_bugcheck_msg("request url is empty");
    }
    try{
        if(socket_.is_open()){
            LOG_T()<<"socket open ";
            socket_.close();
        }
        DEF_NAMESPACE::StringTokenizer tk{url_,"//",DEF_NAMESPACE::StringTokenizer::TOK_IGNORE_EMPTY};
        host_ = tk[1].substr(0,tk[1].find_first_of("/"));
        LOG_T()<<OUT_BRIEF_STR_VAL("host",host_);
        asio::ip::tcp::resolver resolver{io_service_};
        asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host_,"80");
        asio::connect(socket_,endpoints);
    }catch(std::exception&e){
        LOG_E()<<"request connect exception: "<<e.what();
    }
    
}

void Request::CreateFile(){
    if(filename_.empty()){
        common_bugcheck_msg("save file name  is empty");
    }
    DEF_NAMESPACE::Path pa{filename_};
    pa.makeFile();
    DEF_NAMESPACE::File fi{pa.toString()};
    if(fi.exists()){
        fi.remove();
    }
    try{
        pa.makeDirectory().popDirectory();
        DEF_NAMESPACE::File(pa.toString()).createDirectories();
    }catch(...){
    }
    fi.createFile();
}


long long  Request::GetFileSize(){
    try{
        if(!socket_.is_open()){
            Connect();
        }
        asio::streambuf request;
        std::ostream request_stream(&request);
        request_stream << "HEAD " << url_ << " HTTP/1.0\r\n";
        request_stream << "Host: " << host_ << "\r\n";
        request_stream << "Connection: close\r\n\r\n";

        // Send the request.
        asio::write(socket_, request);

        asio::streambuf response;
        asio::read_until(socket_,response,"\r\n\r\n");
        socket_.close();
        std::stringstream response_stream;
        std::istream ir{&response};
        response_stream << ir.rdbuf();

        std::string http_version;
        response_stream >> http_version;
        unsigned int status_code;
        response_stream >> status_code;
        string other{response_stream.str()};
        LOG_T()<<other;
        if (!response_stream || http_version.substr(0, 5) != "HTTP/")
        {
            LOG_E() << "Invalid response\n";
            return -1;
        }
        if (status_code != 200)
        {
            LOG_E() << "Response returned with status code " << status_code << "\n";
            return -1;
        }
        
        size_t l = string("Content-Length:").size(); 
        size_t pos = other.find("Content-Length:");
        if(pos == string::npos){
            LOG_E()<<"response: "<< other << "\t no Content-Length:";
            return -1;
        }
        pos += l;
        string s = other.substr(pos + 1, other.find("\r\n",pos) - pos - 1);
        return CONVERTTO(long long,s);
    }catch(std::exception &e){
        LOG_E() << "Exception: " << e.what();
        return -1;
    }
    
}
bool Request::Download(){
    try{
        if(!socket_.is_open()){
            Connect();
        }
        asio::streambuf request;
        std::ostream request_stream(&request);
        request_stream << "GET " << url_ << " HTTP/1.0\r\n";
        request_stream << "Host: " << host_ << "\r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Connection: close\r\n\r\n";

        // Send the request.
        asio::write(socket_, request);

        asio::streambuf response;
        asio::read_until(socket_,response,"\r\n\r\n");

        std::istream response_stream{&response};
        
        std::string http_version;
        response_stream >> http_version;
        unsigned int status_code;
        response_stream >> status_code;
    
        if (!response_stream || http_version.substr(0, 5) != "HTTP/")
        {
            LOG_E() << "Invalid response\n";
            return false;
        }
        if (status_code != 200)
        {
            LOG_E() << "Response returned with status code " << status_code << "\n";
            return false;
        }
        std::ofstream fout(filename_);

        std::string header;
        while (std::getline(response_stream, header) && header != "\r");
        // std::cout << header << "\n";
        // std::cout << "\n";



        asio::error_code error;
        while (asio::read(socket_, response,asio::transfer_at_least(1), error)){
            fout << &response;      
        }
        if (error != asio::error::eof){
            throw asio::system_error(error);
        }
    }catch (std::exception& e)
    {
        LOG_E() << "Exception: " << e.what();
        return false;
    }
    
    return true;
}
NAMESPACE_SUBFIX
