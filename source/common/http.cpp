#include "common/common_header.hxx"
#include "http.hxx"
#include <sstream>
#include <exception>

#include "http_helper.hxx"

NAMESPACE_PREFIX

using std::string;

bool Http::Connect(){
    try{
        if(socket_.is_open()){
            socket_.close();
        }
        asio::ip::tcp::resolver resolver{io_service_};
        asio::ip::tcp::resolver::query query{ip_,TOSTRING(port_)};
        asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(query);
        asio::connect(socket_,endpoints);
    }catch(std::exception&e){
        LOG_E()<<"request connect exception: "<<e.what();
        return false;
    }
    return true;
}


Http::ResponseInfo Http::Request(const std::string& method,const std::string& uri,const std::string&body,const std::unordered_map<std::string,std::string>& headers,int timeout ){
    Http::ResponseInfo   response_info;
    std::string method_ = method;
    if(method_.empty())
        method_ = "GET";
    if(uri.empty()){
        std::stringstream err;
        err<<OUT_BRIEF_STR_VAL("method",method_)<<" uri is null";
        response_info.err_msg = err.str(); 
        return response_info;
    }
    size_t length =  0;
    if(!body.empty()){
        length = body.size();
    }
    try{
        if(!socket_.is_open()){
            Connect();
        }
         
        asio::streambuf request;
        std::ostream os_request(&request);
        Http_Helper::Opt opt{os_request};

        opt.SetRequestLine(method_,uri)
            .AppendWithCRLF("Host",ip_)
            .AppendWithCRLF("Connection","close")
            .AppendWithCRLF("Content-length",TOSTRING(length));
        for(auto it : headers){
            opt.AppendWithCRLF(it.first,it.second);
        }
        opt.AppendEmpytLine()
            .AppendContext(body);

        asio::write(socket_, request);
        asio::streambuf response;
        
        auto handle_timeout = [&](const asio::error_code&e)->void{
            if(e == asio::error::operation_aborted ){
                return ;
            }
            string s;
            s = ip_  + " " +  method_ +  " " + uri + " " + body +" http timeout";
            throw TimeoutException(s);
        };
        
        auto read_handler = [&](const asio::error_code& e, std::size_t size){
            if(!e || e == asio::error::eof){
                timer_.cancel();
            }else if( e == asio::error::operation_aborted)
            {
                
            }else{
                throw asio::system_error(e);
            }
        };
        timer_.expires_after(std::chrono::seconds(timeout));
        timer_.async_wait(handle_timeout);
        asio::async_read_until(socket_,response,"\r\n\r\n",read_handler);
        io_service_.reset();
        io_service_.run();
        io_service_.stop();
        timer_.expires_after(std::chrono::seconds(5));
        timer_.async_wait(handle_timeout);
        asio::async_read(socket_,response,read_handler);
        io_service_.reset();
        io_service_.run();
        io_service_.stop();
        socket_.close();

        std::stringstream ss_response;
        std::istream in_response{&response};
        ss_response<<in_response.rdbuf();
        std::string content = ss_response.str();

        Http_Helper::Parse parse{content};
        ss_response.clear();
        ss_response.str("");
        if(parse.Version().substr(0,5) != "HTTP/"){
            response_info.err_msg = std::move(content);
            return response_info;
        }
        std::string rest_body{content};
        rest_body.erase(0,rest_body.find("\r\n\r\n")+4);
        
        if(parse.Code() >= 200 && parse.Code() <=299){
            response_info.status = true;
            response_info.data = std::move(rest_body);
            return response_info;
        }else if(parse.Code() == 401){
            DEF_NAMESPACE::Http_Helper::Authorization auth{parse["Www-Authenticate"]};
            opt.SetRequestLine(method_,uri)
                .AppendWithCRLF("Host",ip_)
                .AppendWithCRLF("Connection","close")
                .AppendWithCRLF("Content-length",TOSTRING(length))
                .AppendAuthorization(auth,method_,name_,passwd_,uri);

            for(auto it : headers){
                opt.AppendWithCRLF(it.first,it.second);
            }
            opt.AppendEmpytLine()
                .AppendContext(body);
            if(!socket_.is_open()){
                Connect();
            }
            asio::write(socket_, request);
            timer_.expires_after(std::chrono::seconds(timeout));
            timer_.async_wait(handle_timeout);
            asio::async_read_until(socket_,response,"\r\n\r\n",read_handler);
            io_service_.reset();
            io_service_.run();
            io_service_.stop();
            timer_.expires_after(std::chrono::seconds(5));
            timer_.async_wait(handle_timeout);
            asio::async_read(socket_,response,read_handler);
            io_service_.reset();
            io_service_.run();
            io_service_.stop();
            socket_.close();

            ss_response<<in_response.rdbuf();
            content = ss_response.str();

            parse.ParseHeader(content);
            rest_body = content;
            rest_body.erase(0,rest_body.find("\r\n\r\n")+4);
            
            if(parse.Version().substr(0,5) != "HTTP/"){
                response_info.err_msg = std::move(content);
                return response_info;
            }
            
            if(parse.Code() >= 200 && parse.Code() <=299){
                response_info.status = true;
                response_info.data = std::move(rest_body);
                return response_info;
            }else{
                response_info.err_msg = std::move(content);
                return response_info;
            }
        }else{
            response_info.err_msg = std::move(content);
            return response_info;
        }
    }catch(std::exception& e){
        socket_.close();
        response_info.err_msg = std::move(e.what());
        return response_info;
    } 
}
Http::ResponseInfo Http::RequestWithStream(const std::string& method,const std::string& uri,std::ifstream&body,const std::unordered_map<std::string,std::string>& headers,int timeout ){
    Http::ResponseInfo   response_info;
    std::string method_ = method;
    if(method_.empty())
        method_ = "GET";
    if(uri.empty()){
        std::stringstream err;
        err<<OUT_BRIEF_STR_VAL("method",method_)<<" uri is null";
        response_info.err_msg = err.str(); 
        return response_info;
    }
    body.seekg(0,std::ios::end);
    size_t length = body.tellg();
    body.seekg(0,std::ios::beg);
    try{
        if(!socket_.is_open()){
            Connect();
        }
        asio::streambuf request;
        std::ostream os_request(&request);
        Http_Helper::Opt opt{os_request};

        opt.SetRequestLine(method_,uri)
            .AppendWithCRLF("Host",ip_)
            .AppendWithCRLF("Content-length",TOSTRING(length));
        for(auto it : headers){
            opt.AppendWithCRLF(it.first,it.second);
        }
        opt.AppendEmpytLine()
             .AppendContext(body);

        asio::write(socket_, request);
        asio::streambuf response;
        
        auto handle_timeout = [&](const asio::error_code&e)->void{
            if(e == asio::error::operation_aborted ){
                return ;
            }
            string s;
            s = ip_  + " " +  method_ +  " " + uri + " " +" http timeout";
            throw TimeoutException(s);
        };
        
        auto read_handler = [&](const asio::error_code& e, std::size_t size){
            if(!e || e == asio::error::eof){
                timer_.cancel();
            }else if( e == asio::error::operation_aborted)
            {
                
            }else{
                throw asio::system_error(e);
            }
        };
        timer_.expires_after(std::chrono::seconds(timeout));
        timer_.async_wait(handle_timeout);
        asio::async_read_until(socket_,response,"\r\n\r\n",read_handler);
        io_service_.reset();
        io_service_.run();
        io_service_.stop();
        timer_.expires_after(std::chrono::seconds(5));
        timer_.async_wait(handle_timeout);
        asio::async_read(socket_,response,read_handler);
        io_service_.reset();
        io_service_.run();
        io_service_.stop();
        socket_.close();

        std::stringstream ss_response;
        std::istream in_response{&response};
        ss_response<<in_response.rdbuf();
        std::string content = ss_response.str();

        Http_Helper::Parse parse{content};
        ss_response.clear();
        ss_response.str("");
        if(parse.Version().substr(0,5) != "HTTP/"){
            response_info.err_msg = std::move(content);
            return response_info;
        }
        std::string rest_body{content};
        rest_body.erase(0,rest_body.find("\r\n\r\n")+4);

        if(parse.Code() >= 200 && parse.Code() <=299){
            response_info.status = true;
            response_info.data = std::move(rest_body);
            return response_info;
        }else if(parse.Code() == 401){
            DEF_NAMESPACE::Http_Helper::Authorization auth{parse["Www-Authenticate"]};
            opt.SetRequestLine(method_,uri)
                .AppendWithCRLF("Host",ip_)
                .AppendWithCRLF("Connection","close")
                .AppendWithCRLF("Content-length",TOSTRING(length))
                .AppendAuthorization(auth,method_,name_,passwd_,uri);

            for(auto it : headers){
                opt.AppendWithCRLF(it.first,it.second);
            }
            opt.AppendEmpytLine()
                .AppendContext(body);
            if(!socket_.is_open()){
                Connect();
            }

            asio::write(socket_, request);
            timer_.expires_after(std::chrono::seconds(timeout));
            timer_.async_wait(handle_timeout);
            asio::async_read_until(socket_,response,"\r\n\r\n",read_handler);
            io_service_.reset();
            io_service_.run();
            io_service_.stop();
            timer_.expires_after(std::chrono::seconds(5));
            timer_.async_wait(handle_timeout);
            asio::async_read(socket_,response,read_handler);
            io_service_.reset();
            io_service_.run();
            io_service_.stop();
            socket_.close();
            
            ss_response<<in_response.rdbuf();
            content = ss_response.str();
            
            parse.ParseHeader(content);
            ss_response.clear();
            ss_response.str("");
            if(parse.Version().substr(0,5) != "HTTP/"){
                response_info.err_msg = std::move(content);
                return response_info;
            }
            rest_body = content;
            rest_body.erase(0,rest_body.find("\r\n\r\n")+4);

            if(parse.Code() >= 200 && parse.Code() <=299){
                response_info.status = true;
                response_info.data = std::move(rest_body);
                return response_info;
            }else{
                response_info.err_msg = std::move(content);
                return response_info;
            }
        }else{
            response_info.err_msg = std::move(content);
            return response_info;
        }
    }catch(std::exception& e){
        socket_.close();
        response_info.err_msg = std::move(e.what());
        return response_info;
    } 
}

Http::ResponseInfo Http::RequestWithStreamAuth(const std::string& method,const std::string& uri,std::ifstream&body,const std::unordered_map<std::string,std::string>& headers,int timeout ){
    Http::ResponseInfo   response_info;
    std::string method_ = method;
    if(method_.empty())
        method_ = "GET";
    if(uri.empty()){
        std::stringstream err;
        err<<OUT_BRIEF_STR_VAL("method",method_)<<" uri is null";
        response_info.err_msg = err.str(); 
        return response_info;
    }
    body.seekg(0,std::ios::end);
    size_t length = body.tellg();
    body.seekg(0,std::ios::beg);
    try{
        if(!socket_.is_open()){
            Connect();
        }
        asio::streambuf request;
        std::ostream os_request(&request);
        Http_Helper::Opt opt{os_request};

        opt.SetRequestLine(method_,uri)
            .AppendWithCRLF("Host",ip_)
            .AppendWithCRLF("Content-length",TOSTRING(length));
        for(auto it : headers){
            opt.AppendWithCRLF(it.first,it.second);
        }
        opt.AppendEmpytLine();

        asio::write(socket_, request);
        asio::streambuf response;
        
        auto handle_timeout = [&](const asio::error_code&e)->void{
            if(e == asio::error::operation_aborted ){
                return ;
            }
            string s;
            s = ip_  + " " +  method_ +  " " + uri + " " +" http timeout";
            throw TimeoutException(s);
        };
        
        auto read_handler = [&](const asio::error_code& e, std::size_t size){
            if(!e || e == asio::error::eof){
                timer_.cancel();
            }else if( e == asio::error::operation_aborted)
            {
                
            }else{
                throw asio::system_error(e);
            }
        };
        timer_.expires_after(std::chrono::seconds(timeout));
        timer_.async_wait(handle_timeout);
        asio::async_read_until(socket_,response,"\r\n\r\n",read_handler);
        io_service_.reset();
        io_service_.run();
        io_service_.stop();
        timer_.expires_after(std::chrono::seconds(timeout));
        timer_.async_wait(handle_timeout);
        asio::async_read(socket_,response,read_handler);
        io_service_.reset();
        io_service_.run();
        io_service_.stop();
        socket_.close();

        std::stringstream ss_response;
        std::istream in_response{&response};
        ss_response<<in_response.rdbuf();
        std::string content = ss_response.str();

        Http_Helper::Parse parse{content};
        ss_response.clear();
        ss_response.str("");
        if(parse.Version().substr(0,5) != "HTTP/"){
            response_info.err_msg = std::move(content);
            return response_info;
        }
        std::string rest_body{content};
        rest_body.erase(0,rest_body.find("\r\n\r\n")+4);
    
        if(parse.Code() >= 200 && parse.Code() <=299){
            response_info.status = true;
            response_info.data = std::move(rest_body);
            return response_info;
        }else if(parse.Code() == 401){
            DEF_NAMESPACE::Http_Helper::Authorization auth{parse["Www-Authenticate"]};
            opt.SetRequestLine(method_,uri)
                .AppendWithCRLF("Host",ip_)
                .AppendWithCRLF("Connection","close")
                .AppendWithCRLF("Content-length",TOSTRING(length))
                .AppendAuthorization(auth,method_,name_,passwd_,uri);

            for(auto it : headers){
                opt.AppendWithCRLF(it.first,it.second);
            }
            opt.AppendEmpytLine()
                .AppendContext(body);
            if(!socket_.is_open()){
                Connect();
            }
            asio::write(socket_, request);
            timer_.expires_after(std::chrono::seconds(timeout));
            timer_.async_wait(handle_timeout);
            asio::async_read_until(socket_,response,"\r\n\r\n",read_handler);
            io_service_.reset();
            io_service_.run();
            io_service_.stop();
            timer_.expires_after(std::chrono::seconds(5));
            timer_.async_wait(handle_timeout);
            asio::async_read(socket_,response,read_handler);
            io_service_.reset();
            io_service_.run();
            io_service_.stop();
            socket_.close();

            ss_response<<in_response.rdbuf();
            content = ss_response.str();
            parse.ParseHeader(content);
            if(parse.Version().substr(0,5) != "HTTP/"){
                response_info.err_msg = std::move(content);
                return response_info;
            }
            rest_body = content;
            rest_body.erase(0,rest_body.find("\r\n\r\n")+4);
            
            if(parse.Code() >= 200 && parse.Code() <=299){
                response_info.status = true;
                response_info.data = std::move(rest_body);
                return response_info;
            }else{
                response_info.err_msg = std::move(content);
                return response_info;
            }
        }else{
            response_info.err_msg = std::move(content);
            return response_info;
        }
    }catch(std::exception& e){
        socket_.close();
        response_info.err_msg = std::move(e.what());
        return response_info;
    }  
}

NAMESPACE_SUBFIX