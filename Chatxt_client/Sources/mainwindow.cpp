#include "mainwindow.h"
#include "../UI/ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_login_dlg = new LoginDialog();
    m_login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(m_login_dlg);
    m_login_dlg->show();

    // 连接登陆界面注册信号
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
    m_reg_dlg = new RegisterDialog(this);
    m_login_dlg->hide();

    m_reg_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);

    // 连接注册界面返回登录信号
    connect(m_reg_dlg, &RegisterDialog::sigSwitchLogin, this, &MainWindow::SlotSwitchLogin);
    setCentralWidget(m_reg_dlg);
    m_login_dlg->hide();
    m_reg_dlg->show();
}

void MainWindow::SlotSwitchLogin()
{
    //创建一个CentralWidget, 并将其设置为MainWindow的中心部件
    m_login_dlg = new LoginDialog(this);
    m_login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(m_login_dlg);

    m_reg_dlg->hide();
    m_login_dlg->show();
    //连接登录界面注册信号
    connect(m_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);
}
