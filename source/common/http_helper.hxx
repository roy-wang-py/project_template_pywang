/**
 * @file http_helper.hxx
 * @author sqhao (sqhao@aibee.com)
 * @brief 
 * @version 0.1
 * @date 2019-08-23
 * 
 * @copyright Copyright (c) 2019
 * 
*/

#ifndef __HTTP_HELPER_HXX__
#define __HTTP_HELPER_HXX__
#include "common/common_header.hxx"
#include <string>
#include <unordered_map>
#include <map>
#include <algorithm>
#include "md5.hxx"
#include <streambuf>
#include <iostream>
#include "utility.hxx"

NAMESPACE_PREFIX
    namespace Http_Helper{
        class Parse{
            using string = std::string;
            typedef std::unordered_map<string,string> value_type;
	        typedef value_type::const_iterator Iterator;
            private:
                string src_;
                bool parse_status_;
                string version_;
                int status_code_;
                string reason_phrase_;
                value_type header_;
            public:
                Parse(const string& s):src_(s){
                    parse_status_ = ParseHeader();
                }
                bool ParseHeader();
                bool ParseHeader(const string& s){
                    src_ = s;
                    header_.clear();
                    parse_status_ = ParseHeader();
                    return parse_status_;
                }

                Iterator begin() const{return header_.begin();}
                Iterator end() const{return header_.end();}

                const std::string& operator [] (const string& key) const{
                    return header_.at(UPPER(key));
                }
                std::string& operator [] (const string& key){
                    return header_.at(UPPER(key));
                }

                bool has(const string&key)const {
                    Iterator it =header_.find(UPPER(key));
                    return it != end();
                }
                std::size_t size() const{
                    return header_.size();
                }
                bool ParseStatus()const{return parse_status_;}
                const string& Version()const{return version_;} 
                int Code()const {return status_code_;}
                const string& Reason_Phrase()const{return reason_phrase_;}
        };

        class Authorization{
            using string = std::string; 
            typedef std::unordered_map<string,string> value_type;
	        typedef value_type::const_iterator Iterator;
            public:
                struct Auth_Method{
                    enum Auth {BASIC = 1 ,DIGEST = 2,};
                    Auth auth_;
                    Auth_Method()=default;
                    Auth_Method(const string& auth){
                        if(auth == "Basic")
                            auth_ = BASIC;
                        else if(auth == "Digest")
                            auth_ = DIGEST; 
                        else
                        {
                            common_bugcheck_msg("auth no support: "+auth);
                        }
                    }
                    Auth_Method(const Auth_Method& a):
                        auth_(a.auth_){
                    }
                    Auth_Method& operator =(const Auth_Method& a){
                        auth_ = a.auth_;
                        return *this;
                    }
                    Auth GetAuthMethod()const {return auth_;}
                };
                friend struct Digest;
                struct Digest{
                    friend class Authorization;
                    typedef   unsigned char byte;
                    enum{HA1_SIZE = 16,RESPONSE_SIZE = 32,};
                    string name_;
                    string passwd_;
                    string cnonce_{"0a4f113b"};
                    string nc_{"00000001"};
                    Authorization& owner_;
                    MD5 md5_;
                    Digest(Authorization &a):owner_(a){
                    }
                    Digest(const string&n,const string&p,Authorization &a)
                        :name_(n),passwd_(p),owner_(a)
                    { 
                    }
                    void CalcHA1(const string&name,const string&passwd){
                        name_ = name;
                        passwd_ = passwd;
                        CalcHA1();
                    }
                    void CalcHA1();

                    string  CalcResponse(const string&method,const string&uri);
                    string  CalcResponse(const string&method,const string&uri,const string&name,const string&passwd){
                        CalcHA1(name,passwd);
                        return CalcResponse(method,uri);
                    }
                    string nc(){return nc_;}
                    string cnonce(){return cnonce_;}
                };

                friend struct Basic;
                struct Basic
                {
                    friend class Authorization;
                    string name_;
                    string passwd_;
                    Authorization& owner_;
                    Basic(Authorization &a):owner_(a){
                    }
                    Basic(const string&n,const string&p,Authorization &a)
                        :name_(n),passwd_(p),owner_(a)
                    { 
                    }
                    string CalcResponse();
                    string CalcResponse(const string& n,const string&p){
                        name_ =n;
                        passwd_ = p;
                        return CalcResponse();
                    }
                };
               friend struct Opt;
                
            private:
                string src_;
                string auth_str_;
                Auth_Method auth_;
                value_type list_;
                Digest digest;
                Basic  basic;
            public:
                
                Authorization(const string& s):src_(s),digest(*this),basic(*this){
                    Parse();
                }
                void Parse();
                void Parse(const string& s){
                    src_ = s;
                    list_.clear();
                    Parse();
                }
                Iterator begin() const{return list_.begin();}
                Iterator end() const{return list_.end();}

                const std::string& operator [] (const string& key) const{
                    return list_.at(key);
                }
                std::string& operator [] (const string& key){
                    return list_.at(key);
                }

                bool has(const string&key)const {
                    Iterator it = list_.find(key);
                    return it != end();
                }
                std::size_t size() const{
                    return list_.size();
                }

                Auth_Method::Auth GetAuthMethod()const{
                    return auth_.GetAuthMethod();
                }
                string GetAuthMethodStr()const {return auth_str_;}
                string ResponseWithAuth(const string&name,const string&passwd,const string&method = {},const string&uri = {}){
                    switch (auth_.GetAuthMethod())
                    {
                    case Auth_Method::BASIC :
                        return basic.CalcResponse(name,passwd);
                        break;
                    case Auth_Method::DIGEST:
                        return digest.CalcResponse(method,uri,name,passwd);
                    default:
                        LOG_T()<<"no support";
                        break;
                    }
                    return {};
                }

                string nc()const { return digest.nc_;}
                string cnonce()const {return digest.cnonce_;}
        };
        struct Opt{
            using string = std::string;
            using cstring = const string;
            std::ostream&  os_;

            Opt(std::ostream& request):os_(request){

            }
            Opt& SetRequestLine(cstring& method,cstring& uri,cstring& protocal_version = "HTTP/1.0"){
                os_ << method << " " << uri << " " << protocal_version << "\r\n";
                return *this;
            }
            Opt& AppendWithCRLF(cstring& name,cstring& value){
                os_ << name <<": " << value <<"\r\n";
                return *this;
            }
            Opt& AppendWithCRLF(cstring& header){
                if(!header.empty())
                    os_ << header <<"\r\n";
                return *this;
            }
            Opt& AppendEmpytLine(){
                os_ <<"\r\n";
                return *this;
            }
            Opt& AppendContext(cstring& context){
                if(!context.empty())
                    os_<<context;
                return *this;
            }
            Opt& AppendContext(std::ifstream& context){
                os_<<context.rdbuf();
                return *this;
            }
            Opt& AppendAuthorization(cstring& auth_method,cstring&username,cstring&paasw, \
               cstring& realm,cstring& nonce, cstring& uri,cstring& qop,cstring& nc,cstring& cnonce,cstring& response){
                os_ <<"Authorization: " << auth_method << " username=\"" << username << "\", realm="
                    << realm <<", nonce=" << nonce << ", uri=\"" << uri << "\", algorithm=\"MD5\""
                    << ", qop=" << qop << ", nc=" << nc << ", cnonce=\"" << cnonce << "\", response=\""
                    << response << "\"\r\n";
                return *this;
            }
            Opt& AppendAuthorization(cstring& auth_method,cstring&response){
                os_ <<"Authorization: " << auth_method << " "<<response << "\r\n";
                return *this;
            }
            Opt& AppendAuthorization(Authorization& auth,const std::string&method,const std::string& user,const std::string& passwd,const std::string& uri){
                // os_ <<"Authorization: " << auth_method << " "<<response << "\r\n";
                if(auth.GetAuthMethod() ==Authorization::Auth_Method::BASIC){
                    os_ <<"Authorization: " << auth.GetAuthMethodStr() << " "<<auth.ResponseWithAuth(user,passwd,method,uri) << "\r\n";
                }else if(auth.GetAuthMethod() ==Authorization::Auth_Method::DIGEST){
                
                    os_ <<"Authorization: " << auth.GetAuthMethodStr() << " username=\"" << user << "\", realm="
                    << auth["realm"] <<", nonce=" << auth["nonce"] << ", uri=\"" << uri << "\", algorithm=\"MD5\""
                    << ", qop=" << "auth" << ", nc=" << auth.nc() << ", cnonce=\"" << auth.cnonce() << "\", response=\""
                    << auth.ResponseWithAuth(user,passwd,method,uri) << "\"\r\n";
                }
                return *this;
            }
        };
    }
NAMESPACE_SUBFIX


#endif //! __HTTP_HELPER_HXX__