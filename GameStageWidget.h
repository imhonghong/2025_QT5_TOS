// GameStageWidget.h
#pragma once

#include <QWidget>
#include <QVector>
#include <QPair>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QProgressBar>
#include <QTimer>
#include "Gem.h"
#include "Enemy.h"

class GameStageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameStageWidget(QWidget *parent = nullptr);

    // 從 Prepare 階段傳來：6 格角色 ID (0 表示空格)
    void setSelectedCharacters(const QVector<int> &chars);
    void setMissionID(int mission);

    // 初始化與重置：MainWindow 在每次切到遊戲畫面時都要呼叫
    void resetGame();
    void initGame();

    // UI 暫停／恢復
    void pauseGame();
    void resumeGame();

    // 顯示敵人、顯示盤面符石
    void showEnemies(const QVector<Enemy*> &enemies);
    void showBoard(const QVector<QVector<Gem*>> &board);

    // 清除盤面上的所有 gem QLabel
    void clearGemLabels();

    // 在一般遊戲中，用來設定「當前血量／最高血量」
    void setHealth(int currentHP, int maxHP);

    // 進入「轉珠倒數」模式，傳入剩餘秒數(倒數秒數由右向左消失)
    void startCountdown(int seconds);

    // 停止倒數，恢復成顯示血量的模式 (或直接隱藏)
    void stopCountdown();

signals:
    // 遊戲結束（true：玩家勝，false：玩家敗）
    void gameOver(bool playerWon);

    // 玩家按下左上角設定，此信號由 MainWindow 接去 Pause 畫面
    void pauseRequested();

    // UI → Controller
    void swapFinished();
    void clearGems(const QList<QPair<int,int>> &matchedCoords);
    void enemiesAttacked();

public slots:
    // Controller → UI
    void onMoveTimeUp();
    void onMatchesFound(const QList<QPair<int,int>> &matchedCoords, int comboCount);
    void onDealDamage(int totalDamage);
    void onWaveCleared();

private slots:
    void onSettingClicked();
    void onFakeWinButtonClicked();
    void onFakeLoseButtonClicked();
    // 倒數計時器每秒觸發，用來更新進度顯示
    void onCountdownTimeout();

private:
    void setupUI();

    // ===== UI 成員變數 =====
    // (1) 敵人區
    QWidget                  *enemyArea;
    QVector<QLabel*>          enemyLabels;
    QHBoxLayout *enemyLayout;    // 放 QLabel（敵人圖示）的水平佈局

    // (2) 按鈕：模擬勝利／模擬失敗／下一波
    QPushButton              *fakeWinBtn;
    QPushButton              *fakeLoseBtn;
    QPushButton              *nextBattleButton;

    // (3) 左上角設定按鈕
    QPushButton              *settingButton;

    // (4) status bar
    QProgressBar              *statusBar;       // 顯示血量或倒數進度的條
    bool                       inCountdownMode; // false = 顯示血量模式；true = 倒數模式
    int                        countdownRemain; // 倒數剩餘秒數
    QTimer                    *countdownTimer;  // 每秒觸發 onCountdownTimeout()

    // (5) 角色區：6 格
    QGridLayout              *charLayout;
    QVector<QLabel*>          charLabels;    // 6 個 QLabel


    // (6) 符石區：6×5 格
    QGridLayout              *gemLayout;
    QVector<QLabel*>          gemLabels;     // 平鋪所有 QLabel* for iteration
    QVector<QVector<QLabel*>>  gemLabels2D;   // 二維陣列 [row][col]

    // 狀態、資料
    bool                      isPaused;
    QVector<int>              selectedChars; // 從 Prepare 拿到的 6 個 ID
    int                       missionID;
};
