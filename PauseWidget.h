// PauseWidget.h
#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

/*
 * PauseWidget
 *  1) 顯示「暫停中」文字
 *  2) 提供兩個按鈕：Resume、Surrender
 */
class PauseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PauseWidget(QWidget *parent = nullptr);

signals:
    void resumeClicked();
    void surrenderClicked();

private slots:
    void onResumeButtonClicked();
    void onSurrenderButtonClicked();

private:
    QLabel      *labelPause;
    QPushButton *btnResume;
    QPushButton *btnSurrender;
};
