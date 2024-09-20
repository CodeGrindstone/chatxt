#include "TimberBtn.h"
#include <QObject>
#include <QMouseEvent>

TimberBtn::TimberBtn(QWidget *parent) : QPushButton(parent), m_count(10)
{
    m_timer = new QTimer(this);

    QObject::connect(m_timer, &QTimer::timeout, [this](){
        m_count--;
        if(m_count <= 0){
            m_timer->stop();
            m_count = 10;
            this->setText("获取");
            this->setEnabled(true);
            return;
        }
        this->setText(QString::number(m_count));
    });

}

TimberBtn::~TimberBtn()
{
    m_timer->stop();
}

void TimberBtn::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        // 在这里处理鼠标左键释放事件
        qDebug() << "MyButton was released!";
        this->setEnabled(false);
        this->setText(QString::number(m_count));
        m_timer->start(1000);
        emit clicked();
    }
    // 调用基类的mouseReleaseEvent以确保正常的事件处理（如点击效果）
    QPushButton::mouseReleaseEvent(e);
}
