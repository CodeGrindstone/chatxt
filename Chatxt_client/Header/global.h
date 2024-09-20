#ifndef GLOBAL_H
#define GLOBAL_H
#include <QWidget>
#include <functional>
#include <QStyle>
#include <QDir>
#include <QSettings>
#include <QString>
#include <QCoreApplication>
#include <iostream>

extern std::function<void(QWidget*)> repolish;
extern QString gate_url_prefix;

enum ReqId{
    ID_GET_VARIFY_CODE = 1001, //获取验证码
    ID_REG_USER = 1002, //注册用户
};

enum ErrorCodes{
    SUCCESS = 0,
    ERR_JSON = 1,           //Json解析失败
    ERR_NETWORK = 2,

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

enum Modules{
    REGISTERMOD = 0,
};

enum TipErr{
    TIP_SUCCESS = 0,
    TIP_EMAIL_ERR = 1,
    TIP_PWD_ERR = 2,
    TIP_CONFIRM_ERR = 3,
    TIP_PWD_CONFIRM = 4,
    TIP_VARIFY_ERR = 5,
    TIP_USER_ERR = 6
};

enum ClickLbState{
    Normal = 0,   //普通状态
    Selected = 1  //点击状态
};

#endif // GLOBAL_H
