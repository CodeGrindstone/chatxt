#ifndef CLICKEDLABEL_H
#define CLICKEDLABEL_H

#include <QLabel>
#include "global.h"

class ClickedLabel : public QLabel
{
    Q_OBJECT
public:
    ClickedLabel(QWidget *parent = nullptr);
    virtual void mousePressEvent(QMouseEvent *ev) override;
    virtual void enterEvent(QEnterEvent* event) override;
    virtual void leaveEvent(QEvent* event) override;
    void SetState(QString normal="", QString hover="", QString press="",
                  QString select="", QString select_hover="", QString select_press="");

    ClickLbState GetCurState();

private:
    QString _normal;        //普通状态
    QString _normal_hover;  //普通悬浮状态
    QString _normal_press;  // 普通点击状态
    QString _selected;      // 选中状态
    QString _selected_hover;//选中悬浮状态
    QString _selected_press;//选中点击状态
    ClickLbState _curstate;
signals:
    void clicked(void);
};

#endif // CLICKEDLABEL_H
