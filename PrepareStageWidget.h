#pragma once

#include <QWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QVector>

/*
 * PrepareStageWidget
 *  - 大小由 MainWindow 統一設定為 540×960
 *  - 上方：Player's Character: (文字標題)
 *  - 中間：六個 QComboBox (可重複選擇角色；第一項為空；其餘為 ID + Icon)
 *    → 現在每一格都會回傳一個值（若選「空白」就是 0）
 *  - 其下：Game Mission: (文字標題)
 *  - 一個 QSpinBox (允許鍵盤輸入整數，範例只給 1 可選)
 *  - 最下方：Start 按鈕 (按下後核對至少有一個角色被選，再把參數發出)
 *
 * Signal:
 *  void startClicked(const QVector<int> &selectedChars, int missionID);
 *  → 其中 selectedChars 長度固定為 6，若某格未選人物就以 0 表示。
 */
class PrepareStageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PrepareStageWidget(QWidget *parent = nullptr);

signals:
    // 按下 Start 時，回傳「固定 6 個槽位的角色 ID 向量」(空位以 0 表示)
    // 以及「missionID」
    void startClicked(const QVector<int> &selectedChars, int missionID);

private slots:
    // Start 按鈕被點
    void onStartButtonClicked();

private:
    // 六個下拉式選單：每個 index 0 = 空白，1~5 = 角色 ID
    QComboBox *comboChars[6];

    // 一個可鍵盤輸入的 QSpinBox 作為 mission 輸入
    QSpinBox *spinMission;

    // Start 按鈕
    QPushButton *startButton;
};
