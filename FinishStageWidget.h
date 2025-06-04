// FinishStageWidget.h
#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

/*
 * FinishStageWidget
 *  1) 顯示「You Win!」或「You Lose!」
 *  2) 提供兩個按鈕：Restart、Go Back
 */
class FinishStageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FinishStageWidget(QWidget *parent = nullptr);

    // 設定要顯示勝利或失敗
    void showResult(bool playerWon);

signals:
    void restartClicked();

private:
    QLabel      *labelResult;
    QPushButton *btnRestart;
    QPushButton *btnGoBack;
};
