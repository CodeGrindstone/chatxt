#ifndef TIMBERBTN_H
#define TIMBERBTN_H

#include <QPushButton>
#include <QTimer>

class TimberBtn : public QPushButton
{
public:
    TimberBtn(QWidget *parent = nullptr);
    ~TimberBtn();

    // 重写mouseReleaseEvent
    virtual void mouseReleaseEvent(QMouseEvent *e) override;
private:
    QTimer *m_timer;
    int m_count;
};

#endif // TIMBERBTN_H
