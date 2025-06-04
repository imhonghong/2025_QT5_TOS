// GameStageWidget.h
#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QVector>
#include <QHBoxLayout>

/*
 * GameStageWidget
 *  - 大小由 MainWindow 設定為 540×960
 *  - 提供 setter 接收從 Prepare 階段傳來的 selectedChars 與 missionID
 *  - initGame() 中根據這些參數初始化三波邏輯、符石棋盤、玩家區顯示
 */

class RunestoneWidget; // 簡易符石格子

class GameStageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameStageWidget(QWidget *parent = nullptr);

    // setter：讓 MainWindow 把 Prepare 階段的參數傳給這裡
    void setSelectedCharacters(const QVector<int> &chars);
    void setMissionID(int mission);

    // 初始化遊戲；呼叫前一定要先呼叫上面兩個 setter
    void initGame();

    // 重置遊戲：把動態資料都清空
    void resetGame();

    // 暫停／恢復 遊戲
    void pauseGame();
    void resumeGame();

signals:
    // 遊戲結束 (true=玩家勝利, false=玩家失敗)
    void gameOver(bool playerWon);

    // 請求暫停
    void pauseRequested();

private slots:
    // Slot：點 ⚙ (設定) 按鈕
    void onSettingClicked();

    // Slot：測試用「模擬勝利」按鈕
    void onFakeWinButtonClicked();

    // Slot：測試用「模擬失敗」按鈕
    void onFakeLoseButtonClicked();

private:
    // 初始化 UI 版面 (敵人區 / 玩家區 / 符石區)
    void setupUI();

    QWidget                   *enemyArea;     // 敵人區 (高度 410px)
    QWidget                   *playerArea;    // 玩家區 (高度 100px)
    QWidget                   *runestoneArea; // 符石區 (高度 450px)
    QGridLayout               *gemLayout;
    QVector<RunestoneWidget*>   gems;          // 30 個符石格子

    // 玩家區的水平 Layout，用來放角色圖示 + ⚙ 按鈕
    QHBoxLayout               *charLayout;

    QPushButton               *settingButton;
    QPushButton               *fakeWinBtn;
    QPushButton               *fakeLoseBtn;

    bool isPaused;

    // ---------- 新增的成員變數 ----------
    QVector<int> selectedChars; // 從 Prepare 階段傳來的角色 ID 列表
    int missionID;              // 從 Prepare 階段傳來的關卡 ID
    // -----------------------------------
};
