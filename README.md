---
typora-copy-images-to: ./Images
---

# chatxt

##  简介

本项目为一款C++全栈聊天项目，此项目包括PC端QT界面编程，asio异步服务器设计，beast网络库搭建http网关，nodejs搭建验证服务，各服务间用grpc通信，server和client用asio通信等，也包括用户信息的录入等。

## 登陆和注册界面

### 登陆界面

logindialog.ui负责登陆界面，下图为登陆页面

<img src="Images/image-20240902211639675.png" alt="image-20240902211639675" style="zoom: 50%;" />

### 注册界面

registerdialog.ui负责注册页面，如下图

<img src="Images/image-20240902211742706.png" alt="image-20240902211742706" style="zoom: 67%;" />

在注册类的构造函数里添加lineEdit的模式为密码模式

```C++
ui->passwd_edit->setEchoMode(QLineEdit::Password);
ui->confirm_edit->setEchoMode(QLineEdit::Password);
```

在注册界面添加一个widget，用来提示输入错误信息

在qss里设置err_tip样式，根据不同的状态做不同颜色提示

```c++
#err_tip[state='normal']{
   color: green;
}
#err_tip[state='err']{
   color: red;
}
```

在项目中添加global.h和global.cpp文件，用来声明全局函数和变量

```c
/*global.h*/
#ifndef GLOBAL_H
#define GLOBAL_H
#include <QWidget>
#include <functional>
#include <QStyle>

// 刷新qss
extern std::function<void(QWidget*)> repolish;

#endif // GLOBAL_H


/*global.cpp*/
#include "global.h"

std::function<void(QWidget*)> repolish = [](QWidget* w){
    w->style()->unpolish(w);
    w->style()->polish(w);
};
```

在Register的构造函数中添加样式设置

```c++
ui->err_tip->setProperty("state","normal");
repolish(ui->err_tip);
```

获取验证码的逻辑，ui关联获取验证码按钮的槽事件，实现槽函数

```c

void RegisterDialog::on_vafify_ptn_clicked()
{
    //验证邮箱的地址正则表达式
    auto email = ui->emai_edit->text();
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(match){
        //发送http请求获取验证码
    }else{
        //提示邮箱不正确
        showTip(tr("邮箱地址不正确"), match);
    }
}
```

在RegisterDialog中添加showTip函数

```c
void RegisterDialog::showTip(QString str, bool b_ok)
{
    if(b_ok){
        ui->err_tip->setProperty("state", "normal");
    }
    else{
        ui->err_tip->setProperty("state","err");
    }
    ui->err_tip->setText(str);
    repolish(ui->err_tip);
}
```





### 登陆跳转到注册页面

在LoginDialog类中声明里==添加信号切换注册界面==

```c++
signals:
    void switchRegister();
```

在LoginDialog的构造函数中连接按钮点击事件

```C++
connect(ui->reg_btn, &QPushButton::clicked, this, &LoginDialog::switchRegister);
```

 按钮点击后，LoginDialog发出switchRegister信号，该信号发送给MainWindow用来切换界面

```c
// mainwindow.h的MainWindow的类中声明槽函数

public slots:
    void SlotSwitchReg();
private:
	RegisterDialog* m_reg_dlg;

// mainwindow.cpp 在其构造函数中添加注册类对象的初始化以及连接switchRegister信号
//创建和注册消息的链接
connect(m_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);
m_reg_dlg = new RegisterDialog();
```

实现槽函数

```c
void MainWindow::SlotSwitchReg()
{
    setCentralWidget(m_reg_dlg);	// 设置注册界面为主窗口	
    m_login_dlg->hide();			// 隐藏登陆界面

    m_reg_dlg->show();				// 显示注册界面
}
```

## 客户端HTTP管理类

### 单例类

```c++
#include <mutex>
#include <memory>
#include <iostream>

template<class T>
class Singleton
{
protected:
    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operate(const Singleton&) = delete;
    static std::shared_ptr<T> _instance;
public:
    static std::shared_ptr<T> GetInstance()
    {
        static std::once_flag s_flag;

        std::call_once(s_flag, [&](){
            _instance = std::shared_ptr<T>(new T);
        });

        return _instance;
    }

    void PrintAddress() {
        std::cout << _instance.get() << std::endl;
    }

    ~Singleton() {
        std::cout << "this is singleton destruct" << std::endl;
    }
};
template<class T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;
```

### http管理类

```c++
#include <QObject>
#include <QString>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include "Singleton.h"
#include "global.h"

class HttpMgr : public QObject, public Singleton<HttpMgr>, public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT
public:
    ~HttpMgr();
private:
    friend class Singleton;
    HttpMgr();
    QNetworkAccessManager m_manager;
    void PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod); // 发送Http的post请求

private slots:
    void slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);

signals:
    void sig_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod); // 发送完http请求的信号
    void sig_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
};

```

### 发送Post请求

```c++
void PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod);
作用: 发送http的post请求
参数:
    url: 发送请求用到的url;
    json: 请求的数据;
	req_id: 请求id;
	mod: 哪个模块发出的请求mod
```

在global.h中定义ReqId和ErrorCodes、Modules

```c
enum ReqId{
    ID_GET_VARIFY_CODE = 1001, //获取验证码
    ID_REG_USER = 1002, //注册用户
};

enum ErrorCodes{
    SUCCESS = 0,
    ERR_JSON = 1, //Json解析失败
    ERR_NETWORK = 2,
};

enum Modules{
    REGISTERMOD = 0,
};
```

定义信号，当发送post请求并接收reply后，执行信号连接的槽函数

```c
void sig_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod); // 发送完http请求的信号
```

定义槽函数，与上面的信号连接

```C++
void slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);
```

* 实现PostHttpReq

```C++
void HttpMgr::PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod)
{
    // 创建一个HTTP Post请求，并设置请求头和请求体
    QByteArray data = QJsonDocument(json).toJson();
    // 通过url构造请求
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));
    // 发送请求，并处理响应，获取自己的智能指针，构造伪闭包增加智能指针引用计数
    auto self = shared_from_this();
    QNetworkReply* reply = m_manager.post(request, data);
    // 设置信号和槽等待发送完成
    QObject::connect(reply, &QNetworkReply::finished, [reply, self, req_id, mod](){
        //处理错误的情况
        if(reply->error() != QNetworkReply::NoError){
            qDebug() << reply->errorString();
            //发送信号通知完成
            emit self->sig_http_finish(req_id, "", ErrorCodes::ERR_NETWORK, mod);
            reply->deleteLater();
            return;
        }
        //无错误则读回请求
        QString res = reply->readAll();
        //发送信号通知完成
        emit self->sig_http_finish(req_id, res, ErrorCodes::SUCCESS,mod);
        reply->deleteLater();
        return;
    });

}
```

* 实现slot_http_finish

```c++
void HttpMgr::slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod)
{
    if(mod == Modules::REGISTERMOD)
    {
        // 发送信号通知指定模块的http的响应结束了
        emit sig_reg_mod_finish(id, res, err);
    }
    
    ......
}
```

添加信号sig_reg_mod_finish

```c++
signals:
    void sig_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
```

在注册界面连接sig_reg_mod_finish信号

```c++
RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    //省略...
	connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reg_mod_finish,
            this, &RegisterDialog::slot_reg_mod_finish);
}
```

* 实现slot_reg_mod_finish函数

```c++
void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS)
    {
        showTip(tr("网络请求错误"), false);
        return;
    }
    //解析JSon字符串 res转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    if(jsonDoc.isNull() || !jsonDoc.isObject())
    {
        showTip(tr("json解析失败"), false);
        return;
    }

    // 将json文档转化为json对象  jsonDoc.object()

    m_handlers[id](jsonDoc.object());

    return;

}
```

### 注册消息处理

需要对RegisterDialog**注册消息处理**，头文件声明

```c++
QMap<ReqId, std::function<void(const QJsonObject&)>> m_handlers;
```

在RegisterDialog中添加注册消息处理的声明和定义

```c++
void RegisterDialog::initHttpHandlers()
{
    m_handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](const QJsonObject& JsonObj){
        int error = JsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"), false);
            return;
        }

        auto email = JsonObj["email"].toString();
        showTip(tr("验证码已经发送到邮箱"), true);
        qDebug() << "email is " << email;
    });
}
```

### 流程图

![image-20240904113417732](Images/image-20240904113417732.png)

## 利用beast搭建http server完成Get请求

### CServer类

CServer类构造函数接受一个端口号，创建acceptor接受新的到来的连接

```c++
class CServer : public std::enable_shared_from_this<CServer>
{
public:
    CServer(asio::io_context& ioc, unsigned short port);
    void Start();

private:
    asio::io_context& m_ioc;
    tcp::acceptor m_acceptor;
    tcp::socket m_socket;
};
```

CServer构造函数如下

```c++
CServer::CServer(asio::io_context& ioc, unsigned short port):
m_ioc(ioc),
m_acceptor(ioc, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
m_socket(ioc)
{

}
```

Start函数，用来监听新连接

Start函数内创建HttpConn类的智能指针，将m_socket内部数据转移给HttpConn管理类，m_socket继续接受新的连接。

```c++
void CServer::Start()
{
    auto self = shared_from_this();
    m_acceptor.async_accept(m_socket, [self](std::error_code ec)
    {
       try
       {
           //出错放弃连接
           if (ec)
           {
               self->Start();
               return;
           }
           std::make_shared<HttpConn>(self->m_socket)->Start();
           //继续监听
           self->Start();
       }catch (std::exception& e)
       {
           std::cout << e.what() << std::endl;
           return;
       }
    });
}
```

### HttpConn类

```c++
class HttpConn : public std::enable_shared_from_this<HttpConn>
{
    friend class LogicSystem;
public:
    HttpConn(tcp::socket& m_socket);
    void Start();
private:
    void CheckDeadline();		// 检查定时器是否超时
    void WriteResponse();		// 写给客户端的回应
    void HandleReq();			// 接收客户端的响应
    void PreParseGetParam();	// 解析URL
private:
    tcp::socket m_socket;

    beast::flat_buffer m_buffer{8192};  		  //用来接收数据
    http::request<http::dynamic_body> m_request;  //用来解析请求
    http::response<http::dynamic_body> m_response;//用来回应客户端
    //用来做定时器判断请求是否超时
    asio::steady_timer deadline_{
        m_socket.get_executor(), std::chrono::seconds(60) };

    std::string m_get_url;		// 解析后的URL
    std::unordered_map<std::string, std::string> m_get_params;//解析后的URL键值对
};
```

* HttpConn构造函数，==注意给m_socket赋值要用移动赋值==

```c++
HttpConn::HttpConn(tcp::socket& socket):m_socket(std::move(socket))
{
}
```

* async_read函数解析

```c++
boost::beast::http::async_read(
    AsyncReadStream& stream,
    DynamicBuffer& buffer,
    message<isRequest, Body, basic_fields<Allocator>>& msg,
    ReadHandler&& handler
)
    
第一个参数为异步可读的数据流，可以理解为socket.

第二个参数为一个buffer，用来存储接受的数据，因为http可接受文本，图像，音频等多种资源文件，所以是Dynamic动态类型的buffer。

第三个参数是请求参数，我们一般也要传递能接受多种资源类型的请求参数。

第四个参数为回调函数，接受成功或者失败，都会触发回调函数，我们用lambda表达式就可以了。
```

* Start函数

```c++
void HttpConn::Start()
{
    auto self(shared_from_this());
    http::async_read(m_socket, m_buffer, m_request,
        [self](beast::error_code ec, std::size_t bytes_transferred)
        {
           try
           {
               if(ec)
               {
                   std::cout << "http read err is " << ec.message() << std::endl;
                   return;
               }
                // 处理读到的信息
                boost::ignore_unused(bytes_transferred);
                self->HandleReq();
                self->CheckDeadline();
           }catch (std::exception& e)
           {
               std::cout << "exception is " << e.what() << std::endl;
           }
        });
}
```

* HandleReq

```c++
void HttpConn::HandleReq()
{
    //设置版本
    m_response.version(m_request.version());
    //设置为短连接
    m_response.keep_alive(false);

    if(m_request.method() == http::verb::get)
    {
        // 如果请求是get
        PreParseGetParam();	// 解析url
        bool success = LogicSystem::GetInstance()->HandleGet(m_get_url, shared_from_this());
        if(!success)
        {
            m_response.result(http::status::not_found);
            m_response.set(http::field::content_type, "text/plain");
            beast::ostream(m_response.body()) << "url not found\r\n";
            WriteResponse();
            return;
        }
    }

    m_response.result(http::status::ok);
    m_response.set(http::field::server, "GateServer");
    WriteResponse();
    return;

}
```

* WriteResponse函数，在回调函数里关闭发送端并取消定时器

```c++
void HttpConn::WriteResponse()
{
    auto self = shared_from_this();
    m_response.content_length(m_response.body().size());

    http::async_write(m_socket, m_response,
        [self](boost::system::error_code ec, std::size_t){
            self->m_socket.shutdown(tcp::socket::shutdown_send, ec);
            self->deadline_.cancel();
    });
}
```

* 检测超时函数

```c++
void HttpConn::CheckDeadline()
{
    auto self(shared_from_this());

    deadline_.async_wait([self](boost::system::error_code ec)
    {
        if(!ec)
        {
            self->m_socket.close(ec);
        }
    });
}
```



### LogicSystem类，依旧用到了单例模式

* m_post_handlers和m_get_handlers分别是post请求和get请求的回调函数map，key为路由，value为回调函数。

```c++
typedef std::function<void(std::shared_ptr<HttpConn>)> HttpHandler;
class LogicSystem:public Singleton<LogicSystem>,
public std::enable_shared_from_this<LogicSystem>
{
    friend class Singleton<LogicSystem>;
public:
    ~LogicSystem();
    bool HandleGet(std::string url, std::shared_ptr<HttpConn> conn);
    void RegGet(std::string, HttpHandler);
private:
    LogicSystem();

private:
    std::map<std::string, HttpHandler> m_PostHandlers;
    std::map<std::string, HttpHandler> m_GetHandlers;
};
```

* RegGet函数用来注册Get指定路由所指定的回调函数

```c++
void LogicSystem::RegGet(std::string url, HttpHandler handler)
{
    m_GetHandlers.insert(std::make_pair(url, handler));
}
```

* 构造函数，实现具体的消息注册

```c++
LogicSystem::LogicSystem()
{
    RegGet("/get_test", [](std::shared_ptr<HttpConn> connection) {
        beast::ostream(connection->m_response.body()) << "receiv eget_test req";
        int i = 0;
        for(auto& elem:connection->m_get_params)
        {
            i++;
            beast::ostream(connection->m_response.body()) << "param " << i << " " << elem.first << " " << elem.second;
        }
    });

}
```

* HandleGet函数，真正处理客户端请求的动作，此动作旨在逻辑类中完成，HttpConn只负责收发数据和调用逻辑类的HandleGet

```c++
bool LogicSystem::HandleGet(std::string url, std::shared_ptr<HttpConn> conn)
{
    if(m_GetHandlers.find(url) == m_GetHandlers.end())
    {
        return false;
    }
    m_GetHandlers[url](conn);
    return true;
}
```

### main函数

在main函数中，初始化上下文iocontext以及启动信号监听ctrl-c退出事件

```c++
int main()
{
    try
    {
        unsigned short port = static_cast<unsigned short>(8080);
        asio::io_context ioc{ 1 };
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const boost::system::error_code& error, int signal_number) {
            if (error) {
                return;
            }
            ioc.stop();
            });
        std::make_shared<CServer>(ioc, port)->Start();
        ioc.run();
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
```

## 处理post请求

* 实现RegPost函数，用来注册post请求的函数

```c++
void LogicSystem::RegPost(std::string url, HttpHandler handler)
{
    m_PostHandlers.insert(std::make_pair(url, handler));
}
```

* 在LogicSystem的构造函数里添加获取验证码的处理逻辑

```c++
RegPost("/get_varifycode", [](std::shared_ptr<HttpConn> connection)
    {
        auto body_str = beast::buffers_to_string(connection->m_request.body().data());
        std::cout << "receive body is " << body_str << std::endl;
        connection->m_response.set(http::field::content_type, "text/json");
        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;

        bool parse_success = reader.parse(body_str, src_root);
        if(!parse_success)
        {
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"] = ErrorCodes::Error_Json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
        }

        auto email = src_root["email"].asString();
        std::cout << "email is " << email << std::endl;
        root["error"] = 0;
        root["email"] = src_root["email"];
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->m_response.body()) << jsonstr;
    });
```

* 在LogicSystem中添加Post请求的处理

```c++
bool LogicSystem::HandlePost(std::string url, std::shared_ptr<HttpConn> conn)
{
    if(m_PostHandlers.find(url) == m_PostHandlers.end())
    {
        return false;
    }
    m_PostHandlers[url](conn);
    return true;
}
```

* 在HttpConn类中的HandleReq中添加post请求处理

```c++
void HttpConnection::HandleReq() {
    //省略...
    if (_request.method() == http::verb::post) {
        bool success = LogicSystem::GetInstance()->HandlePost(_request.target(), shared_from_this());
        if (!success) {
            _response.result(http::status::not_found);
            _response.set(http::field::content_type, "text/plain");
            beast::ostream(_response.body()) << "url not found\r\n";
            WriteResponse();
            return;
        }
        _response.result(http::status::ok);
        _response.set(http::field::server, "GateServer");
        WriteResponse();
        return;
    }
}
```

### 客户端增加逻辑

* 点击获取验证码的槽函数里添加发送http的post请求

```c++
void RegisterDialog::on_vafify_ptn_clicked()
{
    //验证邮箱的地址正则表达式
    auto email = ui->emai_edit->text();
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(match){
        //发送http请求获取验证码
        QJsonObject json_obj;
        json_obj["email"] = email;
        HttpMgr::GetInstance()->PostHttpReq(
            QUrl(gate_url_prefix + "/get_varifycode"),
            json_obj, ReqId::ID_GET_VARIFY_CODE, Modules::REGISTERMOD
        );
    }else{
        //提示邮箱不正确
        showTip(tr("邮箱地址不正确"), match);
    }
}
```



### 客户端管理配置

在代码所在目录新建config.ini文件，内部添加配置

```c++
[GateServer]
host=xxxx
port=8080
```

* global.h中声明

```c++
extern QString gate_url_prefix;
```

* cpp中添加定义

```c++
QString gate_url_prefix = "";
```

* 在main函数中添加解析配置的逻辑

```c++
// 获取当前应用程序的路径
QString app_path = QCoreApplication::applicationDirPath();
// 拼接文件名
QString fileName = "config.ini";
QString config_path = QDir::toNativeSeparators(app_path +
                        QDir::separator() + fileName);
QSettings settings(config_path, QSettings::IniFormat);
QString gate_host = settings.value("GateServer/host").toString();
QString gate_port = settings.value("GateServer/port").toString();
gate_url_prefix = "http://"+gate_host+":"+gate_port;
```

