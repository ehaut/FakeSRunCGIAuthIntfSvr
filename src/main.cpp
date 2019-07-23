#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include "fcgio.h"

//See here:http://chriswu.me/blog/writing-hello-world-in-fcgi-with-c-plus-plus/
using namespace std;

const std::string info="<p>A Fake SRun Auth Interface Server By C++ </p> <p>2019 Copyright(C) Powered by E-HAUT Team </p>";
const std::string testMode="on";
const std::string version="V1.0.0";

struct UserInfo
{
    std::string username;
    std::string password;
    bool isOnline;
    bool isLimited;
    std::string acid;
    std::string type;
    std::string n;
    UserInfo()
    {
        username="";
        password="";
        isOnline=false;
        isLimited=false;
        acid="1";
        type="1";
        n="100";
    }
};

struct OnlineInfo
{
    UserInfo onlineUserInfo;
    std::string onlineIP;
    long usedData;
    long loginTime;
    long messageTime;
    long usedTime;
    OnlineInfo()
    {
        onlineIP="";
        usedData=0;
        usedTime=0;
        messageTime=0;
        loginTime=0;    
    }
};

void split_option(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
    int len=s.length();
    std::string::size_type pos;
    pos = s.find(c);
    v.push_back(s.substr(0,pos));
    if(pos==len-1)
    {
        v.push_back("");
    }
    else
    {
        v.push_back(s.substr(pos+1,len));
    }
}


void split_string(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
  //https://www.zhihu.com/question/36642771
  std::string::size_type pos1, pos2;
  pos2 = s.find(c);
  pos1 = 0;
  while(std::string::npos != pos2)
  {
    v.push_back(s.substr(pos1, pos2-pos1));

    pos1 = pos2 + c.size();
    pos2 = s.find(c, pos1);
  }
  if(pos1 != s.length())
    v.push_back(s.substr(pos1));
}

using namespace std;

// Maximum bytes
const unsigned long STDIN_MAX = 1000000;

/**
 * Note this is not thread safe due to the static allocation of the
 * content_buffer.
 */
string get_request_content(const FCGX_Request & request) {
    char * content_length_str = FCGX_GetParam("CONTENT_LENGTH", request.envp);
    unsigned long content_length = STDIN_MAX;

    if (content_length_str) {
        content_length = strtol(content_length_str, &content_length_str, 10);
        if (*content_length_str) {
            cerr << "Can't Parse 'CONTENT_LENGTH='"
                 << FCGX_GetParam("CONTENT_LENGTH", request.envp)
                 << "'. Consuming stdin up to " << STDIN_MAX << endl;
        }

        if (content_length > STDIN_MAX) {
            content_length = STDIN_MAX;
        }
    } else {
        // Do not read from stdin if CONTENT_LENGTH is missing
        content_length = 0;
    }

    char * content_buffer = new char[content_length];
    cin.read(content_buffer, content_length);
    content_length = cin.gcount();

    // Chew up any remaining stdin - this shouldn't be necessary
    // but is because mod_fastcgi doesn't handle it correctly.

    // ignore() doesn't set the eof bit in some versions of glibc++
    // so use gcount() instead of eof()...
    do cin.ignore(1024); while (cin.gcount() == 1024);

    string content(content_buffer, content_length);
    delete [] content_buffer;
    return content;
}

int main(void) {
    // Backup the stdio streambufs
    streambuf * cin_streambuf  = cin.rdbuf();
    streambuf * cout_streambuf = cout.rdbuf();
    streambuf * cerr_streambuf = cerr.rdbuf();

    FCGX_Request request;

    FCGX_Init();
    FCGX_InitRequest(&request, 0, 0);
    vector <OnlineInfo> onlineInfoVector;

    while (FCGX_Accept_r(&request) == 0) {
        fcgi_streambuf cin_fcgi_streambuf(request.in);
        fcgi_streambuf cout_fcgi_streambuf(request.out);
        fcgi_streambuf cerr_fcgi_streambuf(request.err);

        cin.rdbuf(&cin_fcgi_streambuf);
        cout.rdbuf(&cout_fcgi_streambuf);
        cerr.rdbuf(&cerr_fcgi_streambuf);

        //uri是url资源的地址，content是post的资源

        const char * uri = FCGX_GetParam("REQUEST_URI", request.envp);
        const char * ip = FCGX_GetParam("REMOTE_ADDR",request.envp);

        //分离url参数至vector容器
        vector <std::string> vec;
        string content = get_request_content(request);  
        string u=uri;
        u=u.substr(1);
        split_string(u,vec,"/");
        if(string(uri).compare("/testmode")==0 ) 
        {
            /* Html头部输出，固定格式请勿更改 */
            std::cout << "Content-Type: text/plain\r\n\r\n";
            /* Html头部输出结束 */
            std::cout<<testMode;
        }
        else if(string(uri).compare("/cgi-bin/srun_portal") == 0 && !( content.empty() || content.compare("")==0))
        {
            vector <std::string> c; //处理 &
            vector <std::string> v; //处理 key=value
            split_string(content,c,"&");
            bool signal[6]={
                false, //username
                false, //ac_id
                false, //action
                false, //type
                false, //n
                false //passsword
            };
            bool isLogin=false;
            
            // std::cout << "Content-Type: text/plain\r\n\r\n";
            // for(int i=0;i<c.size();i++)
            // {
            //     string temp="";
            //     temp=c[i]+" ";
            //     std::cout<<c[i]<<std::endl;
            //     v.clear();
            //     split_string(temp,v,"=");
            //     std::cout<<v[0]<<" "<<v[1]<<std::endl;
            //     std::cout<<"r"<<!(v[1].empty()||v[1]==""||v[1]==" ")<<std::endl;
            //     v.clear();
            // }
            bool flag=false;
            OnlineInfo onlineInfo;
            for(int i=0;i<c.size();i++)
            {   
                string temp="";
                temp=c[i]+"";
                v.clear();
                split_option(temp,v,"=");
                std::cout<<"r"<<!(v[1].empty()||v[1]==""||v[1]==" ")<<std::endl;
                if(v[0].compare("action")==0)
                {
                    if(v[1].compare("login")==0)
                    {
                        signal[2]=true;
                        isLogin=true;
                        continue;
                    }
                    else if(v[1].compare("logout")==0)
                    {
                        signal[2]=true;
                        continue;
                    }
                    else
                    {
                         /* Html头部输出，固定格式请勿更改 */
                        std::cout << "Content-Type: text/plain\r\n\r\n";
                        /* Html头部输出结束 */
                        std::cout<<"missing_required_parameters_error";
                        flag=true;
                        break;
                    }
                }
                else if(v[0].compare("username")==0)
                {
                    if(!(v[1].empty()||v[1]==""||v[1]==" "))
                    {
                        signal[0]=true;
                        onlineInfo.onlineUserInfo.username=v[1];
                        continue;
                    }
                    else
                    {
                        /* Html头部输出，固定格式请勿更改 */
                        std::cout << "Content-Type: text/plain\r\n\r\n";
                        /* Html头部输出结束 */
                        std::cout<<"missing_required_parameters_error";
                        flag=true;
                        break;
                    }
                }
                else if(v[0].compare("ac_id")==0)
                {
                    if(!(v[1].empty()||v[1]==""||v[1]==" "))
                    {
                        signal[1]=true;
                        onlineInfo.onlineUserInfo.acid=v[1];
                        continue;
                    }
                    else
                    {
                         /* Html头部输出，固定格式请勿更改 */
                        std::cout << "Content-Type: text/plain\r\n\r\n";
                        /* Html头部输出结束 */
                        std::cout<<"missing_required_parameters_error";
                        flag=true;
                        break;
                    }
                }
                else if(v[0].compare("type")==0)
                {
                    if(!(v[1].empty()||v[1]==""||v[1]==" "))
                    {
                        signal[3]=true;
                        onlineInfo.onlineUserInfo.type=v[1];
                        continue;
                    }
                    else
                    {
                        /* Html头部输出，固定格式请勿更改 */
                        std::cout << "Content-Type: text/plain\r\n\r\n";
                        /* Html头部输出结束 */
                        std::cout<<"missing_required_parameters_error";
                        flag=true;
                        break;
                    }
                }
                else if(v[0].compare("n")==0)
                {
                    if(!(v[1].empty()||v[1]==""||v[1]==" "))
                    {
                        signal[4]=true;
                        onlineInfo.onlineUserInfo.n=v[1];
                        continue;
                    }
                    else
                    {
                         /* Html头部输出，固定格式请勿更改 */
                        std::cout << "Content-Type: text/plain\r\n\r\n";
                        /* Html头部输出结束 */
                        std::cout<<"missing_required_parameters_error";
                        flag=true;
                        break;
                    }
                }
                else if(v[0].compare("password")==0)
                {
                    if(!(v[1].empty()||v[1]==""||v[1]==" "))
                    {
                        signal[5]=true;
                        onlineInfo.onlineUserInfo.password=v[1];
                        continue;
                    }
                    else
                    {
                        /* Html头部输出，固定格式请勿更改 */
                        std::cout << "Content-Type: text/plain\r\n\r\n";
                        /* Html头部输出结束 */
                        std::cout<<"missing_required_parameters_error";
                        flag=true;
                        break;
                    }
                }
                v.clear();
            }
            if(flag==false)
            {
                bool f=false;
                int end=(isLogin)?6:5;
                for(int i=0;i<end;i++)
                {
                    // std::cout << "Content-Type: text/plain\r\n\r\n";
                    // cout<<signal[i]<<endl;
                    if(signal[i]==false)
                    {
                        f=true;
                        /* Html头部输出，固定格式请勿更改 */
                        std::cout << "Content-Type: text/plain\r\n\r\n";
                        /* Html头部输出结束 */
                        std::cout<<"missing_required_parameters_error";
                        break;
                    }
                } 
                if(f==false)
                {
                    if(isLogin)
                    {
                        std::cout << onlineInfo.onlineIP <<" "<<string(ip)<<std::endl;
                        if(onlineInfoVector.size()==0)
                        {
                            onlineInfo.onlineIP=string(ip);
                            onlineInfo.onlineUserInfo.isOnline=true;
                            onlineInfoVector.push_back(onlineInfo);
                            std::cout << "Content-Type: text/plain\r\n\r\n";
                            /* Html头部输出结束 */
                            std::cout<<"login_ok,1573330604,0,0,0.0,0.0,0,0,0,0,0,1.00";
                        }
                        else
                        {
                            bool signal=false;
                            for(int i=0;i<onlineInfoVector.size();i++)
                            {
                                OnlineInfo temp=onlineInfoVector[i];
                                //std::cout << onlineInfo.onlineIP <<" "<<string(ip)<<std::endl;
                                if(temp.onlineIP==string(ip))
                                {
                                    signal=true;
                                    if(temp.onlineUserInfo.isOnline==true)
                                    {
                                        /* Html头部输出，固定格式请勿更改 */
                                        std::cout << "Content-Type: text/plain\r\n\r\n";
                                        /* Html头部输出结束 */
                                        std::cout<<"ip_already_online_error";
                                        break;
                                    }
                                    else
                                    {
                                        temp.onlineUserInfo.isOnline=true;
                                        /* Html头部输出，固定格式请勿更改 */
                                        std::cout << "Content-Type: text/plain\r\n\r\n";
                                        /* Html头部输出结束 */
                                        std::cout<<"login_ok,1573330604,0,0,0.0,0.0,0,0,0,0,0,1.00";
                                        break;
                                    }
                                }
                            }
                            if(signal==false)
                            {
                                onlineInfo.onlineIP=string(ip);
                                onlineInfo.onlineUserInfo.isOnline=true;
                                onlineInfoVector.push_back(onlineInfo);
                                /* Html头部输出，固定格式请勿更改 */
                                std::cout << "Content-Type: text/plain\r\n\r\n";
                                /* Html头部输出结束 */
                                std::cout<<"login_ok,1573330604,0,0,0.0,0.0,0,0,0,0,0,1.00";
                            }
                        }
                    }
                    else
                    {
                        bool signal=false;
                        for(int i=0;i<onlineInfoVector.size();i++)
                        {
                            OnlineInfo temp=onlineInfoVector[i];
                            if(temp.onlineIP==string(ip))
                            {
                                signal=true;
                                if(temp.onlineUserInfo.isOnline==false)
                                {
                                    /* Html头部输出，固定格式请勿更改 */
                                    std::cout << "Content-Type: text/plain\r\n\r\n";
                                    /* Html头部输出结束 */
                                    std::cout<<"You are not online.";
                                }
                                else
                                {
                                    temp.onlineUserInfo.isOnline=false;
                                    onlineInfoVector.erase(onlineInfoVector.begin()+i);
                                    /* Html头部输出，固定格式请勿更改 */
                                    std::cout << "Content-Type: text/plain\r\n\r\n";
                                    /* Html头部输出结束 */
                                    std::cout<<"logout_ok";
                                }
                            }
                        }
                        if(signal==false)
                        {
                            //onlineInfo.onlineIP=string(ip);
                            //onlineInfo.onlineUserInfo.isOnline=true;
                            //onlineInfoVector.push_back(onlineInfo);
                            /* Html头部输出，固定格式请勿更改 */
                            std::cout << "Content-Type: text/plain\r\n\r\n";
                            /* Html头部输出结束 */
                            std::cout<<"You are not online.";
                        }
                    }
                }
            }
            c.clear();
            v.clear();
        }
        else if(string(uri).compare("/cgi-bin/srun_portal")==0 && ( content.empty() || content.compare("")==0))
        {
            /* Html头部输出，固定格式请勿更改 */
            std::cout << "Content-Type: text/html\r\n\r\n"
            /* Html头部输出结束 */
            <<"<html>\n"
            <<"<body>\n"
            <<"<form method=\"POST\" action=\"/cgi-bin/srun_portal\">\n"
            <<"Name: <input type=\"text\" name=\"username\">\n"
            <<"Password: <input type=\"text\" name=\"password\">\n"
            <<"ACID: <input type=\"text\" name=\"ac_id\">\n"
            <<"\n"
            <<"<input type=\"hidden\" name=\"action\" value=\"login\">\n"
            <<"<input type=\"hidden\" name=\"type\" value=\"1\">\n"
            <<"<input type=\"hidden\" name=\"n\" value=\"100\">\n"
            <<"<input type=\"submit\" value=\"Login\">\n"
            <<"</form>\n"
            << info
            << "<p>"
            << "Uri: "
            << uri
            << "</p>\n"
            << "Content:"
            << content
            << "</p>\n"
            << "From ip:"
            << ip
            << "</p>\n"
            <<"</body>\n"
            <<"</html>\n"
            <<"\n";
            /* 主页显示部分结束 */
        }
        else if(string(uri).compare("/cgi-bin/rad_user_info")==0)
        {
            // /* Html头部输出，固定格式请勿更改 */
            // std::cout << "Content-Type: text/plain\r\n\r\n";
            // /* Html头部输出结束 */
            bool signal=false;
            for(int i=0;i<onlineInfoVector.size();i++)
            {
                OnlineInfo temp=onlineInfoVector[i];
                if(temp.onlineIP==string(ip))
                {
                    signal=true;
                    if(temp.onlineUserInfo.isOnline==false)
                    {
                        /* Html头部输出，固定格式请勿更改 */
                        std::cout << "Content-Type: text/plain\r\n\r\n";
                        /* Html头部输出结束 */
                        std::cout<<"not_online";
                    }
                    else
                    {
                        /* Html头部输出，固定格式请勿更改 */
                        std::cout << "Content-Type: text/plain\r\n\r\n";
                        /* Html头部输出结束 */
                        //201416010000,1474719765,1474719771,0,0,0,78570982665,1010177,172.30.85.5,0,,0.000000,0,0,0,0,0,0,0,0,0,1.01.20160715
                        string output=temp.onlineUserInfo.username;
                        output=output+",1474719765,1474719771,0,0,0,78570982665,1010177,";
                        output=output+string(ip);
                        output=output+",0,,0.000000,0,0,0,0,0,0,0,0,0,1.00";
                        std::cout<<output;
                    }
                }
            }
            if(signal==false)
            {
                //onlineInfo.onlineIP=string(ip);
                //onlineInfo.onlineUserInfo.isOnline=true;
                //onlineInfoVector.push_back(onlineInfo);
                /* Html头部输出，固定格式请勿更改 */
                std::cout << "Content-Type: text/plain\r\n\r\n";
                /* Html头部输出结束 */
                std::cout<<"not_online";
            }
        }
        else 
        {
            /* Html头部输出，固定格式请勿更改 */
            std::cout << "Content-Type: text/html\r\n\r\n"
            /* Html头部输出结束 */
            <<"<html>\n"
            <<"<body>\n"
            <<"<form method=\"POST\" action=\"/cgi-bin/srun_portal\">\n"
            <<"Name: <input type=\"text\" name=\"username\">\n"
            <<"Password: <input type=\"text\" name=\"password\">\n"
            <<"ACID: <input type=\"text\" name=\"ac_id\">\n"
            <<"\n"
            <<"<input type=\"hidden\" name=\"action\" value=\"login\">\n"
            <<"<input type=\"hidden\" name=\"type\" value=\"1\">\n"
            <<"<input type=\"hidden\" name=\"n\" value=\"100\">\n"
            <<"<input type=\"submit\" value=\"Login\">\n"
            <<"</form>\n"
            <<info
            << "<p>"
            << "Uri: "
            << uri
            << "</p>\n"
            << "Content:"
            << content
            // << content.size()
            << "</p>\n"
            << "From ip:"
            << ip
            << "</p>\n"
            <<"</body>\n"
            <<"</html>\n"
            <<"\n";
            /* 主页显示部分结束 */
        }
        vec.clear();
    }

    // restore stdio streambufs
    cin.rdbuf(cin_streambuf);
    cout.rdbuf(cout_streambuf);
    cerr.rdbuf(cerr_streambuf);

    return 0;
}