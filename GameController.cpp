// GameController.cpp
#include "GameController.h"
#include <QRandomGenerator>
#include <QDebug>

GameController::GameController(QObject *parent)
    : QObject(parent),
      currentWaveIndex(0),
      moveTimer(new QTimer(this)),
      isPlayerTurn(true),
      missionID(0)
{
    moveTimer->setSingleShot(true);
    moveTimer->setInterval(10 * 1000);
    connect(moveTimer, &QTimer::timeout, this, &GameController::onMoveTimeout);

    board.resize(ROWS);
    for (int r = 0; r < ROWS; ++r) {
        board[r].resize(COLS);
        for (int c = 0; c < COLS; ++c) {
            board[r][c] = nullptr;
        }
    }
}

GameController::~GameController()
{
    // 釋放盤面所有 Gem*
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            delete board[r][c];
        }
    }
    board.clear();

    // 釋放所有 Enemy*
    for (auto &wave : waves) {
        for (Enemy *e : wave) {
            delete e;
        }
        wave.clear();
    }
    waves.clear();
}

////////////////////////////////////////////////////////////////////////////////
// init(): 傳入玩家 Character* 陣列、missionID
////////////////////////////////////////////////////////////////////////////////
void GameController::init(const QVector<Character*> &playerChars, int missionID)
{
    players = playerChars;
    this->missionID = missionID;
    currentWaveIndex = 0;
    isPlayerTurn = true;

    // 清空舊盤面
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            delete board[r][c];
            board[r][c] = nullptr;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// startMission(): 根據 missionID 先產生三波 enemies，再產生 board，然後倒數 10 秒
////////////////////////////////////////////////////////////////////////////////
void GameController::startMission()
{
    generateWavesFromMissionID(missionID);
    generateInitialGems();
    isPlayerTurn = true;
    emit moveTimeUp();
    moveTimer->start();
}

////////////////////////////////////////////////////////////////////////////////
// 取得「目前波次」的 Enemy* 清單
////////////////////////////////////////////////////////////////////////////////
QVector<Enemy*> GameController::getCurrentWaveEnemies() const
{
    if (currentWaveIndex >= 0 && currentWaveIndex < waves.size()) {
        return waves[currentWaveIndex];
    }
    return {};
}

////////////////////////////////////////////////////////////////////////////////
// 取得目前的盤面二維陣列
////////////////////////////////////////////////////////////////////////////////
QVector<QVector<Gem*>> GameController::getBoardMatrix() const
{
    return board;
}

////////////////////////////////////////////////////////////////////////////////
// generateWavesFromMissionID(): 產生三波 Wave (只支援 missionID=1)
////////////////////////////////////////////////////////////////////////////////
void GameController::generateWavesFromMissionID(int missionID)
{
    for (auto &wave : waves) {
        for (auto *e : wave) {
            delete e;
        }
        wave.clear();
    }
    waves.clear();

    if (missionID == 1) {
        // 波 1：三隻小怪
        QVector<Enemy*> wave1;
        wave1.append(new Enemy(101, Character::Water, 100,
                               ":/enemy/dataset/enemy/100n.png", 3));
        wave1.append(new Enemy(102, Character::Fire,  100,
                               ":/enemy/dataset/enemy/96n.png",  3));
        wave1.append(new Enemy(103, Character::Earth, 100,
                               ":/enemy/dataset/enemy/98n.png",  3));
        waves.push_back(wave1);

        // 波 2：中怪 + 小怪
        QVector<Enemy*> wave2;
        wave2.append(new Enemy(201, Character::Light, 200,
                               ":/enemy/dataset/enemy/102n.png", 4));
        wave2.append(new Enemy(202, Character::Earth, 300,
                               ":/enemy/dataset/enemy/267n.png", 3));
        wave2.append(new Enemy(203, Character::Dark,  100,
                               ":/enemy/dataset/enemy/104n.png", 4));
        waves.push_back(wave2);

        // 波 3：Boss
        QVector<Enemy*> wave3;
        wave3.append(new Enemy(301, Character::Fire,  500,
                               ":/enemy/dataset/enemy/180n.png", 5));
        waves.push_back(wave3);
    }
    else {
        qWarning() << "[GameController] Unknown missionID =" << missionID;
    }
}

////////////////////////////////////////////////////////////////////////////////
// onPlayerSwapFinished(): 玩家完成一次 swap → 重置倒數
////////////////////////////////////////////////////////////////////////////////
void GameController::onPlayerSwapFinished()
{
    if (!isPlayerTurn) return;
    moveTimer->stop();
    moveTimer->start();
}

////////////////////////////////////////////////////////////////////////////////
// startMoveTimer(): UI 可以呼叫此 slot 來「手動」啟動 10 秒
////////////////////////////////////////////////////////////////////////////////
void GameController::startMoveTimer()
{
    if (isPlayerTurn && moveTimer) {
        moveTimer->start();
        qDebug() << "[GameController] moveTimer started";
    }
}

////////////////////////////////////////////////////////////////////////////////
// onMoveTimeout(): 倒數 10 秒結束 → 消除判定
////////////////////////////////////////////////////////////////////////////////
void GameController::onMoveTimeout()
{
    if (!isPlayerTurn) return;

    emit moveTimeUp();
    QList<QPair<int,int>> matched = findAllMatches();
    if (!matched.isEmpty()) {
        int comboCount = matched.size();
        emit matchesFound(matched, comboCount);
    }
    else {
        isPlayerTurn = false;
        startEnemyAttackPhase();
    }
}

////////////////////////////////////////////////////////////////////////////////
// clearMatchedGems(): UI 消除動畫播完後呼叫，以座標列表刪除 board 上的 Gem*
////////////////////////////////////////////////////////////////////////////////
void GameController::clearMatchedGems(const QList<QPair<int,int>> &matchedCoords)
{
    QMap<Gem::Attribute,int> colorCount;
    for (auto &p : matchedCoords) {
        int r = p.first;
        int c = p.second;
        Gem *g = board[r][c];
        if (g) {
            colorCount[g->getType()] += 1;
            delete g;
            board[r][c] = nullptr;
        }
    }

    applyGravityAndRefill();

    int totalDamage = 0;
    for (auto it = colorCount.begin(); it != colorCount.end(); ++it) {
        totalDamage += it.value();
    }
    emit dealDamage(totalDamage);
}

////////////////////////////////////////////////////////////////////////////////
// startEnemyAttackPhase(): 敵人輪流攻擊、檢查玩家是否全滅、或進入下一波
////////////////////////////////////////////////////////////////////////////////
void GameController::startEnemyAttackPhase()
{
    if (currentWaveIndex < 0 || currentWaveIndex >= waves.size()) {
        emit gameWon();
        return;
    }

    auto &currentWave = waves[currentWaveIndex];
    for (Enemy *e : currentWave) {
        if (e->isAlive()) {
            for (Character *p : players) {
                if (p->isAlive()) {
                    p->takeDamage(e->getAttackPower());
                    break;
                }
            }
        }
    }

    if (!arePlayersAllDead()) {
        currentWaveIndex++;
        if (currentWaveIndex >= waves.size()) {
            emit gameWon();
        } else {
            emit waveCleared();
            generateInitialGems();
            isPlayerTurn = true;
            moveTimer->start();
        }
    }
    else {
        emit gameLost();
    }
}

////////////////////////////////////////////////////////////////////////////////
// onEnemiesAttacked(): UI 播放完受傷動畫後呼叫 (目前無其他邏輯)
////////////////////////////////////////////////////////////////////////////////
void GameController::onEnemiesAttacked()
{
    // 暫時留空
}

////////////////////////////////////////////////////////////////////////////////
// arePlayersAllDead(): 判斷玩家是否全部死光
////////////////////////////////////////////////////////////////////////////////
bool GameController::arePlayersAllDead() const
{
    for (Character *p : players) {
        if (p && p->isAlive()) return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// generateInitialGems(): 隨機把 board[r][c] 填滿新 Gem
////////////////////////////////////////////////////////////////////////////////
void GameController::generateInitialGems()
{
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            delete board[r][c];
            board[r][c] = nullptr;
        }
    }

    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            int rnd = QRandomGenerator::global()->bounded(5);
            Gem::Attribute attr = static_cast<Gem::Attribute>(rnd);
            QString path;
            switch (attr) {
                case Gem::Water: path = ":/Nstone/dataset/runestone/water_stone.png"; break;
                case Gem::Fire:  path = ":/Nstone/dataset/runestone/fire_stone.png";  break;
                case Gem::Earth: path = ":/Nstone/dataset/runestone/earth_stone.png"; break;
                case Gem::Light: path = ":/Nstone/dataset/runestone/light_stone.png"; break;
                case Gem::Dark:  path = ":/Nstone/dataset/runestone/dark_stone.png";  break;
            }
            Gem *g = new Gem(attr, r, c, path);
            board[r][c] = g;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// findAllMatches(): 找出橫/直 ≥ 3 連線的符石
////////////////////////////////////////////////////////////////////////////////
QList<QPair<int,int>> GameController::findAllMatches() const
{
    QList<QPair<int,int>> matches;
    QVector<QVector<bool>> flag(ROWS, QVector<bool>(COLS, false));

    // 橫向
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c <= COLS - 3; ++c) {
            Gem *g = board[r][c];
            if (!g) continue;
            Gem::Attribute t = g->getType();
            if (board[r][c+1] && board[r][c+1]->getType() == t &&
                board[r][c+2] && board[r][c+2]->getType() == t)
            {
                int k = c;
                while (k < COLS && board[r][k] && board[r][k]->getType() == t) {
                    flag[r][k] = true;
                    ++k;
                }
            }
        }
    }

    // 直向
    for (int c = 0; c < COLS; ++c) {
        for (int r = 0; r <= ROWS - 3; ++r) {
            Gem *g = board[r][c];
            if (!g) continue;
            Gem::Attribute t = g->getType();
            if (board[r+1][c] && board[r+1][c]->getType() == t &&
                board[r+2][c] && board[r+2][c]->getType() == t)
            {
                int k = r;
                while (k < ROWS && board[k][c] && board[k][c]->getType() == t) {
                    flag[k][c] = true;
                    ++k;
                }
            }
        }
    }

    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            if (flag[r][c]) {
                matches.append(qMakePair(r, c));
            }
        }
    }
    return matches;
}

////////////////////////////////////////////////////////////////////////////////
// applyGravityAndRefill(): 消除完成後，下落並補新
////////////////////////////////////////////////////////////////////////////////
void GameController::applyGravityAndRefill()
{
    for (int c = 0; c < COLS; ++c) {
        int writeRow = ROWS - 1;
        for (int r = ROWS - 1; r >= 0; --r) {
            if (board[r][c]) {
                if (r != writeRow) {
                    board[r][c]->setRow(writeRow);
                    board[writeRow][c] = board[r][c];
                    board[r][c] = nullptr;
                }
                --writeRow;
            }
        }
        for (int r = writeRow; r >= 0; --r) {
            int rnd = QRandomGenerator::global()->bounded(5);
            Gem::Attribute attr = static_cast<Gem::Attribute>(rnd);
            QString path;
            switch (attr) {
                case Gem::Water: path = ":/Nstone/dataset/runestone/water_stone.png"; break;
                case Gem::Fire:  path = ":/Nstone/dataset/runestone/fire_stone.png";  break;
                case Gem::Earth: path = ":/Nstone/dataset/runestone/earth_stone.png"; break;
                case Gem::Light: path = ":/Nstone/dataset/runestone/light_stone.png"; break;
                case Gem::Dark:  path = ":/Nstone/dataset/runestone/dark_stone.png";  break;
            }
            Gem *newGem = new Gem(attr, r, c, path);
            board[r][c] = newGem;
        }
    }
}
