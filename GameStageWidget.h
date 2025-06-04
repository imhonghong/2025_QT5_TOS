// GameStageWidget.h
#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QVector>

/*
 * GameStageWidget
 *  - 大小由 MainWindow 設為 540×960
 *  - 提供 setter 接收從 Prepare 階段來的參數：selectedChars, missionID
 *  - 在 initGame() 裡根據這些參數初始化三波邏輯、符石棋盤等
 */

class RunestoneWidget; // 簡易符石格子

class GameStageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameStageWidget(QWidget *parent = nullptr);

    // setter：供 MainWindow 傳入參數
    void setSelectedCharacters(const QVector<int> &chars);
    void setMissionID(int mission);

    // 初始化遊戲 (此時就可以用到 selectedChars, missionID)
    void initGame();

    // 重置遊戲資料，回到初始狀態
    void resetGame();

    // 暫停／恢復 遊戲
    void pauseGame();
    void resumeGame();

signals:
    void gameOver(bool playerWon);
    void pauseRequested();

private slots:
    void onSettingClicked();
    void onFakeWinButtonClicked();
    void onFakeLoseButtonClicked();

private:
    void setupUI();

    QWidget                  *enemyArea;      // 敵人區 (410px)
    QWidget                  *playerArea;     // 玩家區 (100px)
    QWidget                  *runestoneArea;  // 符石區 (450px)
    QGridLayout              *gemLayout;
    QVector<RunestoneWidget*>  gems;

    QPushButton *settingButton;
    QPushButton *fakeWinBtn;
    QPushButton *fakeLoseBtn;

    bool isPaused;

    // ---------- 新增會員變數 ----------
    QVector<int> selectedChars;
    int missionID;
    // --------------------------------
};
