//
// Created by 31435 on 2024/9/4.
//

#ifndef HEAD_H
#define HEAD_H

#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <string>
#include <memory>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

#include "ConfigMgr/ConfigMgr.h"

#define CODEPREFIX "code_"

namespace  beast = boost::beast;
namespace http = boost::beast::http;
namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;



enum ErrorCodes {
    Success = 0,
    Error_Json = 1001,      //Json解析错误
    RPCFailed = 1002,       //RPC请求错误
    VerifyExpired = 1003,   //验证码过期
    VerifyCodeErr = 1004,   //验证码错误
    UserExist = 1005,       //用户已存在
    PasswdErr = 1006,       //密码错误
    EmailNotMatch = 1007,   //邮箱不匹配
    PasswdUpFailed = 1008,  //更新密码失效
    PasswdInvalid = 1009,   //密码更新失败
};


#endif //HEAD_H
