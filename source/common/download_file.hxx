/**
 * @file download_file.hxx
 * @author sqhao (sqhao@aibee.com)
 * @brief 
 * @version 0.1
 * @date 2019-08-22
 * 
 * @copyright Copyright (c) 2019
 * 
*/

#ifndef __DOWNLOAD_FILE_HXX__
#define __DOWNLOAD_FILE_HXX__

#include <string>

#include "common/asio.hpp"
#include "default_config.hxx"

NAMESPACE_PREFIX

    class Request{
        private:
            std::string url_;
            std::string filename_;
            std::string host_;
            asio::io_service io_service_;
            asio::ip::tcp::socket socket_;
            

            void Connect();
            void CreateFile();

        public:
            Request():socket_(io_service_){}
            Request(const std::string& url)
                :url_(url),socket_(io_service_){
                    Connect();
                }
            Request(const std::string& url,const std::string& savefile)
                :url_(url),filename_(savefile),socket_(io_service_){
                    Connect();
                    CreateFile();
                }

            void SetUrl(const std::string&url){
                url_ = url;
                Connect();
            }
            void SetFileName(const std::string&fl){
                filename_ = fl;
                CreateFile();
            }


            long long  GetFileSize();

            bool Download();
    };


NAMESPACE_SUBFIX




#endif  // ! __DOWNLOAD_FILE_HXX__