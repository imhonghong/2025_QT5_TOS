// GameController.h
#pragma once

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QPair>
#include "Gem.h"
#include "Character.h"
#include "Enemy.h"

class GameController : public QObject
{
    Q_OBJECT

public:
    // 盤面行列數
    static constexpr int ROWS = 5;
    static constexpr int COLS = 6;

    explicit GameController(QObject *parent = nullptr);
    virtual ~GameController();

    // 初始化：給 GameController 玩家角色指標陣列 + missionID
    void init(const QVector<Character*> &playerChars, int missionID);

    // 開始 mission：產生三波 wave, 產生初始盤面, 啟動第一波
    void startMission();

    // 【新增以下兩個 getter，讓 MainWindow 拿到資料】
    QVector<Enemy*> getCurrentWaveEnemies() const;
    QVector<QVector<Gem*>> getBoardMatrix() const;

public slots:
    // 玩家 swap 完成 → 重新啟動倒數
    void onPlayerSwapFinished();

    // 啟動 10 秒倒數
    void startMoveTimer();

    // 倒數結束 → 開始判定消除
    void onMoveTimeout();

    // UI 完成「消除動畫」後，把 matched 區域告訴 Controller
    void clearMatchedGems(const QList<QPair<int,int>> &matchedCoords);

    // UI 完成「敵人受傷動畫」後，呼叫回 Controller
    void onEnemiesAttacked();

signals:
    // 倒數到 → UI 禁止滑動，進入判定
    void moveTimeUp();

    // 找到 matched 符石 (座標列表 + comboCount)
    void matchesFound(const QList<QPair<int,int>> &matchedCoords, int comboCount);

    // 消除結算 → 傷害值
    void dealDamage(int totalDamage);

    // 本波 battle 全部清完
    void waveCleared();

    // 三波都打完 → Mission 勝利
    void gameWon();

    // 玩家全滅 → Mission 失敗
    void gameLost();

private:
    void generateInitialGems();
    void generateWavesFromMissionID(int missionID);
    QList<QPair<int,int>> findAllMatches() const;
    void applyGravityAndRefill();
    void startEnemyAttackPhase();
    bool arePlayersAllDead() const;

private:
    QVector<QVector<Gem*>>      board;             // 盤面 (ROWS × COLS)
    QVector<Character*>         players;           // 玩家角色指標 (MainWindow 管理刪除)
    QVector<QVector<Enemy*>>    waves;             // 產生的三波敵人
    int                         currentWaveIndex;  // 目前波次

    QTimer                     *moveTimer;         // 10 秒倒數
    bool                        isPlayerTurn;      // true = 玩家回合，false = 敵人回合
    int                         missionID;         // 傳進來的 missionID
};

