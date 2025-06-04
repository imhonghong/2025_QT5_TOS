// PrepareStageWidget.h
#pragma once

#include <QWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QVector>

/*
 * PrepareStageWidget
 *  - 大小由 MainWindow 統一設定為 540×960
 *  - 上方：Player's Character: (文字標題)
 *  - 中間：六個 QComboBox (可重複選擇角色；第一項為空；其餘為 ID + Icon)
 *  - 其下：Game Mission: (文字標題)
 *  - 一個 QSpinBox (允許鍵盤輸入整數，範例只給 1 可選)
 *  - 最下方：Start 按鈕 (按下後核對至少有一個角色被選，再把參數發出)
 *
 * Signal:
 *  void startClicked(const QVector<int> &selectedChars, int missionID);
 */
class PrepareStageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PrepareStageWidget(QWidget *parent = nullptr);

signals:
    // 按下 Start 時，回傳「所有選到的角色 ID 列表」(可重複) 以及「missionID」
    void startClicked(const QVector<int> &selectedChars, int missionID);

private slots:
    void onStartButtonClicked();

private:
    // 六個下拉選單：選角色 ID (index 0 = 空白，不計入；index 1～5 分別代表 ID=1~5)
    QComboBox *comboChars[6];

    // 一個可鍵盤輸入的 QSpinBox 作為 mission 輸入
    QSpinBox *spinMission;

    QPushButton *startButton;
};
