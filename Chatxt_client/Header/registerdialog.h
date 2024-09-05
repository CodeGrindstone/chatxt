#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include "../UI/ui_registerdialog.h"
#include "global.h"
#include "HttpMgr.h"

#include <QRegularExpression>
#include <QMap>

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
    void on_vafify_ptn_clicked();
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);

private:
    void initHttpHandlers();  // 注册请求和回调函数
    void showTip(QString, bool b_ok);


private:
    Ui::RegisterDialog *ui;
    QMap<ReqId, std::function<void(const QJsonObject&)>> m_handlers;
};

#endif // REGISTERDIALOG_H
