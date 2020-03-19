#include "common/common_header.hxx"
#include "http_helper.hxx"
#include <sstream>
#include <exception>
#include "md5.hxx"
#include "base64.hxx"
#include "utility.hxx"

NAMESPACE_PREFIX
    namespace Http_Helper{
        using std::string;
        bool Parse::ParseHeader(){
            if(src_.empty()){
                LOG_E()<<"parse header src is empty,will exit";
                return false;
            }
            string s = src_.substr(0,src_.find("\r\n\r\n"));
            DEF_NAMESPACE::StringTokenizer tk{s,"\r\n",DEF_NAMESPACE::StringTokenizer::TOK_IGNORE_EMPTY};
            DEF_NAMESPACE::StringTokenizer tk1{tk[0]," ",DEF_NAMESPACE::StringTokenizer::TOK_IGNORE_EMPTY};
            version_ = tk1[0];
            status_code_ = CONVERTTO(int,tk1[1]);
            reason_phrase_ = tk1[2];

            DEF_NAMESPACE::StringTokenizer::Iterator it = tk.begin();
            ++it;
            DEF_NAMESPACE::StringTokenizer::Iterator end = tk.end();

            for(;it != end;++it){
                string h = UPPER(it->substr(0,it->find_first_of(":")));
                string body = it->substr(it->find_first_of(":") + 1,string::npos);
                body.erase(0,body.find_first_not_of(" "));
                header_[h] = body;
            }
            return true;
        }

        void Authorization::Parse(){
            if(src_.empty()){
                common_bugcheck_msg("parse header src is empty,will exit");
            }
            string& s =src_;
            DEF_NAMESPACE::StringTokenizer tk{s,",",DEF_NAMESPACE::StringTokenizer::TOK_TRIM};
            DEF_NAMESPACE::StringTokenizer tk1{tk[0],"=",DEF_NAMESPACE::StringTokenizer::TOK_TRIM};
            DEF_NAMESPACE::StringTokenizer tk2{tk[0]," ",DEF_NAMESPACE::StringTokenizer::TOK_TRIM};

            auth_str_ = tk2[0];
            auth_ = Auth_Method(auth_str_);

            DEF_NAMESPACE::StringTokenizer tk3{tk2[1],"=",DEF_NAMESPACE::StringTokenizer::TOK_TRIM};
            list_[tk3[0]] = tk3[1];

            DEF_NAMESPACE::StringTokenizer::Iterator it = tk.begin();
            ++it;
            DEF_NAMESPACE::StringTokenizer::Iterator end = tk.end();

            for(;it != end;++it){
                DEF_NAMESPACE::StringTokenizer ts{*it,"=",DEF_NAMESPACE::StringTokenizer::TOK_TRIM};
                list_[ts[0]] = ts[1];
            }
        }

        void Authorization::Digest::CalcHA1(){
            string realm = owner_["realm"];
            realm.erase(0,1);
            realm.erase(realm.size()-1,1);
            
            md5_.update(name_);
            md5_.update(":");
            md5_.update(realm);
            md5_.update(":");
            md5_.update(passwd_);
        }
        string  Authorization::Digest::CalcResponse(const string&method,const string&uri){
            string nonce = owner_["nonce"];
            
            nonce.erase(0,1);
            nonce.erase(nonce.size()-1,1);
            
            string qop;
            if(owner_.has("qop")){
                
                qop = owner_["qop"];
                qop.erase(0,1);
                qop.erase(qop.size()-1,1);
                
            }
                
            string a1 = md5_.toString();
            md5_.reset();
            md5_.update(method);
            md5_.update(":");
            md5_.update(uri);

            string a2 = md5_.toString();
            md5_.reset();
            md5_.update(a1);
            md5_.update(":");
            md5_.update(nonce);
            md5_.update(":");
            if(!qop.empty()){
                md5_.update(nc_);
                md5_.update(":");
                md5_.update(cnonce_);
                md5_.update(":");
                md5_.update(qop);
                md5_.update(":");
            }
            md5_.update(a2);

            return md5_.toString();
        }

        std::string Authorization::Basic::CalcResponse(){
           if(name_.empty() || passwd_.empty()){
                LOG_E()<<"authorization base need name or passwd";
                return {};
           } 
           string s = name_ + ":" + passwd_;
           return DEF_NAMESPACE::Base64::encode(s);
        }

    }
NAMESPACE_SUBFIX

