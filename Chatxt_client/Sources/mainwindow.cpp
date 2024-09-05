#include "mainwindow.h"
#include "../UI/ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_login_dlg = new LoginDialog();
    setCentralWidget(m_login_dlg);
    m_login_dlg->show();

    // 创建和注册消息事件的链接
    connect(m_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);
    m_reg_dlg = new RegisterDialog();

}

MainWindow::~MainWindow()
{
    delete ui;
    // if(m_login_dlg)
    // {
    //     delete m_login_dlg;
    //     m_login_dlg = nullptr;
    // }

    // if(m_reg_dlg)
    // {
    //     delete m_reg_dlg;
    //     m_reg_dlg = nullptr;
    // }
}

void MainWindow::SlotSwitchReg()
{
    setCentralWidget(m_reg_dlg);
    m_login_dlg->hide();

    m_reg_dlg->show();
}
