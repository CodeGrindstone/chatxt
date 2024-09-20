#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include "../UI/ui_registerdialog.h"
#include "global.h"
#include "HttpMgr.h"
#include "TimberBtn.h"
#include "ClickedLabel.h"

#include <QRegularExpression>
#include <QMap>
#include <QTimer>

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);

    void on_confirm_btn_clicked();

    void on_verify_ptn_clicked();

    void on_return_btn_clicked();

private:
    void initHttpHandlers();  // 注册请求和回调函数
    void showTip(QString, bool b_ok);


// 检测各个输入框是否有错误
private:
    void AddTipErr(TipErr te, QString tips);
    void DelTipErr(TipErr te);

    bool checkUserValid();
    bool checkEmailValid();
    bool checkPassValid();
    bool checkConfirmValid();
    bool checkVerifyValid();

private:
    Ui::RegisterDialog *ui;
    QMap<ReqId, std::function<void(const QJsonObject&)>> m_handlers;
    QMap<TipErr, QString> m_tip_errs;

    QTimer * m_countdown_timer;
    int m_countdown;

private:
    void ChangeTipPage();

signals:
    void sigSwitchLogin();
};

#endif // REGISTERDIALOG_H
