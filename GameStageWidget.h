#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QVector>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "Character.h"
#include "Enemy.h"

/*
 * GameStageWidget
 *  - 寬 540 × 高 960
 *  - 上方 540×410 是敵人區
 *  - 中間 540×100 是玩家區 (固定 6 個槽位)
 *  - 下方 540×450 是符石區
 *  - 玩家區每個槽位要麼顯示角色圖，要麼顯示灰底留空
 *  - ⚙ 設定按鈕用絕對定位固定在右上
 */

class RunestoneWidget;

class GameStageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameStageWidget(QWidget *parent = nullptr);

    // 由 MainWindow 傳入：長度為 6 的向量，空位用 0 表示
    void setSelectedCharacters(const QVector<int> &chars);
    void setMissionID(int mission);

    // 初始化遊戲：建立角色 & 敵人，並顯示
    void initGame();
    // 重置：刪掉舊的角色、敵人、waves
    void resetGame();

    void pauseGame();
    void resumeGame();

signals:
    void gameOver(bool playerWon);
    void pauseRequested();

private slots:
    void onSettingClicked();
    void onFakeWinButtonClicked();
    void onFakeLoseButtonClicked();
    void onNextBattleClicked();

private:
    void setupUI();
    void loadCurrentWave();

    // ===== UI Components =====
    QWidget              *enemyArea;        // 敵人區 (540×410)
    QWidget              *playerArea;       // 玩家區 (540×100)
    QWidget              *runestoneArea;    // 符石區 (540×450)

    // 符石區
    QGridLayout          *gemLayout;
    QVector<RunestoneWidget*> gems;

    // 玩家區：水平排列 6 個槽
    QHBoxLayout          *charLayout;

    // 敵人區：waveLayout + 按鈕
    QVBoxLayout          *enemyLayout;
    QHBoxLayout          *waveLayout;

    // 按鈕
    QPushButton          *nextBattleButton;
    QPushButton          *fakeWinBtn;
    QPushButton          *fakeLoseBtn;
    QPushButton          *settingButton;   // 絕對定位在右上

    bool                  isPaused;

    // ===== 遊戲邏輯資料 =====
    QVector<int>          selectedChars;   // 一定長度 6，0 = 空位
    int                   missionID;
    QVector<Character*>   playerChars;     // 會儲存新建的 Character* (如果 slot = 0 就不 new)

    QVector<QVector<Enemy*>> waves;        // 三波敵人
    int                     currentWaveIndex;
    QVector<Enemy*>         enemies;       // 當前波的 Enemy*
};
