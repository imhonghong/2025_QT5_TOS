#include "GameStageWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QDebug>

// ------------------------------------------------------------------
// RunestoneWidget：簡易符石格子占位
class RunestoneWidget : public QLabel {
public:
    explicit RunestoneWidget(QWidget *parent = nullptr)
        : QLabel(parent)
    {
        setFixedSize(90, 90);
        setStyleSheet("background-color: lightGray; border: 1px solid gray;");
        setAlignment(Qt::AlignCenter);
        setText("Gem");
    }
};
// ------------------------------------------------------------------

GameStageWidget::GameStageWidget(QWidget *parent)
    : QWidget(parent),
      enemyArea(new QWidget(this)),
      playerArea(new QWidget(this)),
      runestoneArea(new QWidget(this)),
      gemLayout(new QGridLayout(runestoneArea)),
      charLayout(nullptr),
      enemyLayout(nullptr),
      waveLayout(nullptr),
      nextBattleButton(new QPushButton("通過該 Battle", this)),
      fakeWinBtn(new QPushButton("模擬勝利", this)),
      fakeLoseBtn(new QPushButton("模擬失敗", this)),
      settingButton(new QPushButton("⚙", this)),
      isPaused(false),
      missionID(1),
      currentWaveIndex(0)
{
    // 設定總大小
    setFixedSize(540, 960);

    // 建立 UI (敵人區、玩家區、符石區)
    setupUI();

    // ⚙ 按鈕用絕對座標擺在 GameStageWidget 右上 (x: 540-30-10, y: 10)
    settingButton->setFixedSize(30, 30);
    const int margin = 10;
    settingButton->move(width() - settingButton->width() - margin,
                        margin);
    settingButton->raise();

    // 連接按鈕
    connect(settingButton, &QPushButton::clicked,
            this, &GameStageWidget::onSettingClicked);
    connect(fakeWinBtn, &QPushButton::clicked,
            this, &GameStageWidget::onFakeWinButtonClicked);
    connect(fakeLoseBtn, &QPushButton::clicked,
            this, &GameStageWidget::onFakeLoseButtonClicked);
    connect(nextBattleButton, &QPushButton::clicked,
            this, &GameStageWidget::onNextBattleClicked);
}

void GameStageWidget::setupUI()
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ===== 1. 敵人區 (540×410) =====
    enemyArea->setFixedSize(540, 410);
    enemyLayout = new QVBoxLayout(enemyArea);
    enemyLayout->setContentsMargins(0, 0, 0, 0);
    enemyLayout->setSpacing(5);

    // (a) waveLayout (放敵人圖示)
    waveLayout = new QHBoxLayout;
    waveLayout->setContentsMargins(10, 10, 10, 10);
    waveLayout->setSpacing(20);
    enemyLayout->addLayout(waveLayout);

    // (b) 模擬「勝利/失敗」按鈕
    auto btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    fakeWinBtn->setFixedSize(100, 40);
    fakeLoseBtn->setFixedSize(100, 40);
    btnLayout->addWidget(fakeWinBtn);
    btnLayout->addSpacing(10);
    btnLayout->addWidget(fakeLoseBtn);
    btnLayout->addStretch();
    enemyLayout->addLayout(btnLayout);

    // (c) 通過該 Battle 按鈕 (初始隱藏)
    nextBattleButton->setFixedSize(200, 50);
    nextBattleButton->setVisible(false);
    enemyLayout->addWidget(nextBattleButton, 0, Qt::AlignHCenter);

    mainLayout->addWidget(enemyArea);

    // ===== 2. 玩家區 (540×100) =====
    playerArea->setFixedSize(540, 100);
    {
        // charLayout 只放 6 個槽 (角色或空)
        charLayout = new QHBoxLayout;
        charLayout->setContentsMargins(10, 5, 10, 5);
        charLayout->setSpacing(10);

        // 一開始放一個 placeholder，initGame() 進入時會整組清掉
        QLabel *placeholder = new QLabel("玩家區 (放置角色圖示)", playerArea);
        placeholder->setAlignment(Qt::AlignCenter);
        QFont f; f.setPointSize(16);
        placeholder->setFont(f);
        placeholder->setStyleSheet("background-color: #EFEFEF;");
        placeholder->setFixedSize(500, 80);
        charLayout->addWidget(placeholder);

        // **不再把 settingButton 放到 charLayout** (改絕對定位)

        auto playerOuter = new QHBoxLayout(playerArea);
        playerOuter->setContentsMargins(0, 0, 0, 0);
        playerOuter->addLayout(charLayout);
    }
    mainLayout->addWidget(playerArea);

    // ===== 3. 符石區 (540×450) =====
    runestoneArea->setFixedSize(540, 450);
    runestoneArea->setStyleSheet("background-color: #000000;");
    {
        gemLayout->setContentsMargins(0, 0, 0, 0);
        gemLayout->setSpacing(0);
        for (int r = 0; r < 5; ++r) {
            for (int c = 0; c < 6; ++c) {
                RunestoneWidget *w = new RunestoneWidget(runestoneArea);
                gems.append(w);
                gemLayout->addWidget(w, r, c);
            }
        }
    }
    mainLayout->addWidget(runestoneArea);
}

void GameStageWidget::setSelectedCharacters(const QVector<int> &chars)
{
    // 要確保傳進來的 chars 長度恰好是 6
    selectedChars = chars;
}

void GameStageWidget::setMissionID(int mission)
{
    missionID = mission;
}

void GameStageWidget::initGame()
{
    // (1) 重置遊戲：清除角色、敵人、waves
    resetGame();

    // (2) 建立玩家區 6 個槽：從 selectedChars[0..5] 取值
    int totalHP  = 2000;
    int numChars = 0;
    for (int id : selectedChars) {
        if (id != 0) numChars++;
    }

    // 先把 charLayout 裡的舊 widget (placeholder 或舊角色) 全刪掉
    {
        QLayoutItem *child;
        while ((child = charLayout->takeAt(0)) != nullptr) {
            if (QWidget *w = child->widget()) {
                w->deleteLater();
            }
            delete child;
        }
    }

    // i 從 0 到 5，對應第 1~6 個槽
    for (int i = 0; i < 6; ++i) {
        int id = selectedChars.value(i, 0);
        if (id > 0) {
            // 這格有角色 ID → new Character 並顯示圖示
            Character::Attribute attr = Character::Water;
            switch (id) {
                case 1: attr = Character::Water; break;
                case 2: attr = Character::Fire;  break;
                case 3: attr = Character::Earth; break;
                case 4: attr = Character::Light; break;
                case 5: attr = Character::Dark;  break;
                case 6: attr = Character::Water; break; // 假設最多 6
                default: attr = Character::Water; break;
            }
            QString iconPath = QString(":/character/dataset/character/ID%1.png").arg(id);
            Character *c = new Character(id, attr, totalHP, numChars, iconPath);
            playerChars.append(c);

            QLabel *charLabel = new QLabel(playerArea);
            charLabel->setFixedSize(80, 80);
            charLabel->setPixmap(
                QPixmap(c->getIconPath())
                    .scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation)
            );
            charLabel->setToolTip(QString("角色 %1").arg(id));
            charLayout->addWidget(charLabel);

        } else {
            // 這格是空 (id==0) → 顯示灰底空白格
            QLabel *emptyLabel = new QLabel(playerArea);
            emptyLabel->setFixedSize(80, 80);
            emptyLabel->setStyleSheet("background-color: #CCCCCC;");
            emptyLabel->setToolTip("空位");
            charLayout->addWidget(emptyLabel);
        }
    }

    // (3) 準備三波敵人 (硬編示範)
    QVector<Enemy*> wave1;
    {
        wave1.append(new Enemy(101, Character::Water, 100,
                               ":/enemy/dataset/enemy/100n.png", 3));
        wave1.append(new Enemy(102, Character::Fire,  100,
                               ":/enemy/dataset/enemy/96n.png", 3));
        wave1.append(new Enemy(103, Character::Earth, 100,
                               ":/enemy/dataset/enemy/98n.png", 3));
    }
    QVector<Enemy*> wave2;
    {
        wave2.append(new Enemy(201, Character::Light, 100,
                               ":/enemy/dataset/enemy/102n.png", 3));
        wave2.append(new Enemy(202, Character::Earth, 300,
                               ":/enemy/dataset/enemy/267n.png", 3));
        wave2.append(new Enemy(203, Character::Dark,  100,
                               ":/enemy/dataset/enemy/104n.png", 3));
    }
    QVector<Enemy*> wave3;
    {
        wave3.append(new Enemy(301, Character::Fire, 700,
                               ":/enemy/dataset/enemy/180n.png", 5));
    }
    waves.append(wave1);
    waves.append(wave2);
    waves.append(wave3);

    // (4) 載入第 1 波
    currentWaveIndex = 0;
    loadCurrentWave();

    isPaused = false;
}

void GameStageWidget::resetGame()
{
    qDebug() << "[resetGame] ----- Starting clean reset of GameStage -----";

    // 1. Unpause the game
    isPaused = false;
    qDebug() << "[resetGame] isPaused set to false";

    // 2. Clear out the player area (charLayout) and delete all Character* in playerChars
    qDebug() << "[resetGame] Clearing player area (charLayout)...";
    {
        QLayoutItem *child = nullptr;
        while ((child = charLayout->takeAt(0)) != nullptr) {
            if (QWidget *w = child->widget()) {
                qDebug() << "[resetGame]   Deleting player QLabel (pointer):" << w;
                w->deleteLater();
            }
            delete child;
        }
        qDebug() << "[resetGame] charLayout items cleared";
    }

    if (!playerChars.isEmpty()) {
        qDebug() << "[resetGame] Deleting" << playerChars.size() << "Character* in playerChars...";
        for (Character *c : playerChars) {
            if (c) {
                qDebug() << "[resetGame]   Deleting Character* (pointer):" << c;
                delete c;
            } else {
                qDebug() << "[resetGame]   Skipping nullptr Character*";
            }
        }
        playerChars.clear();
        qDebug() << "[resetGame] playerChars cleared";
    } else {
        qDebug() << "[resetGame] playerChars already empty";
    }

    // 3. We assume loadCurrentWave() has already deleted any Enemy* from previous waves.
    //    So here we only clear the vectors without deleting pointers again.
    if (!enemies.isEmpty()) {
        qDebug() << "[resetGame] Clearing enemies vector without deleting pointers (already freed by loadCurrentWave)";
        enemies.clear();
        qDebug() << "[resetGame] enemies cleared";
    } else {
        qDebug() << "[resetGame] enemies already empty";
    }

    if (!waves.isEmpty()) {
        qDebug() << "[resetGame] Clearing waves vector without deleting pointers (already freed by loadCurrentWave)";
        waves.clear();
        qDebug() << "[resetGame] waves cleared";
    } else {
        qDebug() << "[resetGame] waves already empty";
    }

    // 4. Clear out waveLayout (remove any leftover QLabel for enemies)
    qDebug() << "[resetGame] Clearing waveLayout widgets...";
    {
        QLayoutItem *child = nullptr;
        while ((child = waveLayout->takeAt(0)) != nullptr) {
            if (QWidget *w = child->widget()) {
                qDebug() << "[resetGame]   Deleting waveLayout QLabel (pointer):" << w;
                w->deleteLater();
            }
            delete child;
        }
        qDebug() << "[resetGame] waveLayout items cleared";
    }

    // 5. Hide any battle‐related buttons to avoid leftover UI elements
    if (nextBattleButton) {
        qDebug() << "[resetGame] Hiding nextBattleButton";
        nextBattleButton->setVisible(false);
    }
    if (fakeWinBtn) {
        qDebug() << "[resetGame] Hiding fakeWinBtn";
        fakeWinBtn->setVisible(false);
    }
    if (fakeLoseBtn) {
        qDebug() << "[resetGame] Hiding fakeLoseBtn";
        fakeLoseBtn->setVisible(false);
    }
/*
    // 6. Stop any active timers (e.g., fallTimer, swapTimer) to prevent callbacks to deleted widgets
    if (fallTimer && fallTimer->isActive()) {
        qDebug() << "[resetGame] Stopping fallTimer";
        fallTimer->stop();
    }
    if (swapTimer && swapTimer->isActive()) {
        qDebug() << "[resetGame] Stopping swapTimer";
        swapTimer->stop();
    }
*/
    // 7. Reset wave index so next game starts from wave 0
    qDebug() << "[resetGame] Resetting currentWaveIndex from" << currentWaveIndex << "to 0";
    currentWaveIndex = 0;

    qDebug() << "[resetGame] ----- resetGame() complete -----";
}

void GameStageWidget::loadCurrentWave()
{
    // (1) 刪除上一次儲存在 enemies 的物件
    for (Enemy *e : enemies) {
        delete e;
    }
    enemies.clear();

    // (2) 取得本波 Enemy* 列表
    enemies = waves[currentWaveIndex];

    // (3) 清空 waveLayout 裡原本的 QLabel
    {
        QLayoutItem *child;
        while ((child = waveLayout->takeAt(0)) != nullptr) {
            if (QWidget *w = child->widget()) {
                w->deleteLater();
            }
            delete child;
        }
    }

    // (4) 依序把本波敵人圖示放入 waveLayout
    for (Enemy *e : enemies) {
        QLabel *enemyLabel = new QLabel(enemyArea);
        enemyLabel->setFixedSize(100, 100);
        enemyLabel->setPixmap(
            QPixmap(e->getIconPath())
                .scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
        enemyLabel->setToolTip(
            QString("Enemy ID:%1  HP:%2  CD:%3")
                .arg(e->getID())
                .arg(e->getCurrentHP())
                .arg(e->getCooldownDefault())
        );
        waveLayout->addWidget(enemyLabel);
    }

    // (5) 顯示「通過該 Battle」按鈕
    nextBattleButton->setVisible(true);
    fakeWinBtn->setVisible(true);
    fakeLoseBtn->setVisible(true);
}

void GameStageWidget::onNextBattleClicked()
{
    currentWaveIndex++;
    if (currentWaveIndex >= waves.size()) {
        emit gameOver(true);
    } else {

        // 清掉 waveLayout 的 QLabel
        {
            QLayoutItem *child;
            while ((child = waveLayout->takeAt(0)) != nullptr) {
                if (QWidget *w = child->widget()) {
                    w->deleteLater();
                }
                delete child;
            }
        }
        loadCurrentWave();
    }
}

void GameStageWidget::pauseGame()
{
    if (isPaused) return;
    isPaused = true;
}

void GameStageWidget::resumeGame()
{
    if (!isPaused) return;
    isPaused = false;
}

void GameStageWidget::onSettingClicked()
{
    if (isPaused) return;
    pauseGame();
    emit pauseRequested();
}

void GameStageWidget::onFakeWinButtonClicked()
{
    emit gameOver(true);
}

void GameStageWidget::onFakeLoseButtonClicked()
{
    emit gameOver(false);
}
