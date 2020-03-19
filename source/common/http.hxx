/**
 * @file http.hxx
 * @author sqhao (sqhao@aibee.com)
 * @brief 
 * @version 0.1
 * @date 2019-08-23
 * 
 * @copyright Copyright (c) 2019
 * 
*/

#ifndef __HTTP_HXX__
#define  __HTTP_HXX__

#include <string>

#include "common/asio.hpp"
#include <exception>
#include <fstream>
#include <unordered_map>
#include <thread>
#include "default_config.hxx"

NAMESPACE_PREFIX
    
    class Http{
        using string = std::string;
        private:
            string name_{"admin"};
            string passwd_;
            string ip_;
            int         port_;
            asio::io_service io_service_;
            // asio::io_service::work work_;
            asio::ip::tcp::socket socket_;
            asio::steady_timer  timer_;
            // std::thread thread_;
            
            bool Connect(); 
        public:
            struct ResponseInfo{
                using type = ResponseInfo;
                bool status{false};  
                std::string data{};
                std::string err_msg{};
                ResponseInfo()=default;
                ResponseInfo(const ResponseInfo& r):status(r.status),data(r.data),err_msg(r.err_msg){}
                ResponseInfo( ResponseInfo&& r):status(r.status),data(std::move(r.data)),err_msg(std::move(r.err_msg)){}
                ResponseInfo& operator =(const  ResponseInfo&r){
                    if(this != &r){
                        status = r.status;
                        data = r.data;
                        err_msg = r.err_msg;
                    }
                    return *this;
                }
                ResponseInfo& operator =(ResponseInfo&&r){
                    if(this != &r){
                        status = r.status;
                        data = std::move(r.data);
                        err_msg = std::move(r.err_msg);
                    }
                    return *this;
                }
            };
            Http(const string &name,const string &passwd,const string& ip,int port,int concurrency)
                :name_(name),passwd_(passwd),ip_(ip),port_(port),io_service_(concurrency),socket_(io_service_),timer_(io_service_){
            }
            Http(const string &name,const string &passwd,const string& ip,int port = 80)
                :name_(name),passwd_(passwd),ip_(ip),port_(port),socket_(io_service_),timer_(io_service_){

            }
            Http(const string &passwd,const string& ip,int port = 80)
                :passwd_(passwd),ip_(ip),port_(port),socket_(io_service_),timer_(io_service_){

            }
            Http(const string& ip,int port = 80)
                :ip_(ip),port_(port),socket_(io_service_),timer_(io_service_){
            }
            Http():socket_(io_service_),timer_(io_service_){

            };
            void SetParams(const string &name,const string &passwd,const string& ip,int port = 80){
                name_ = name;
                passwd_ = passwd;
                ip_ = ip;
                port_ = port;
            }

            Http::ResponseInfo Request(const std::string& method,const std::string& uri,const std::string&body={},const std::unordered_map<std::string,std::string>& headers={},int timeout = 20);
            Http::ResponseInfo RequestWithStream(const std::string& method,const std::string& uri,std::ifstream&body,const std::unordered_map<std::string,std::string>& headers={},int timeout = 60);
            Http::ResponseInfo RequestWithStreamAuth(const std::string& method,const std::string& uri,std::ifstream&body,const std::unordered_map<std::string,std::string>& headers={},int timeout = 60);
            
    };

NAMESPACE_SUBFIX

using  ResponseInfo = DEF_NAMESPACE::Http::ResponseInfo;

#endif //! __HTTP_HXX__