// GameStageWidget.cpp
#include "GameStageWidget.h"
#include "GameController.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QDebug>

GameStageWidget::GameStageWidget(QWidget *parent)
    : QWidget(parent),
      missionID(0),
      isPaused(false)
{
    setupUI();
}

////////////////////////////////////////////////////////////////////////////////
// setSelectedCharacters(): 從 Prepare 階段拿到 6 個角色 ID (0 表示空格)
////////////////////////////////////////////////////////////////////////////////
void GameStageWidget::setSelectedCharacters(const QVector<int> &chars)
{
    selectedChars = chars;
}

////////////////////////////////////////////////////////////////////////////////
// setMissionID(): 從 Prepare 階段拿到 missionID
////////////////////////////////////////////////////////////////////////////////
void GameStageWidget::setMissionID(int mission)
{
    missionID = mission;
}

////////////////////////////////////////////////////////////////////////////////
// resetGame(): 清空所有區塊、重置角色區為 6 個灰底空格
//    由 MainWindow 在每次切回遊戲畫面、或重新開始遊戲時呼叫
////////////////////////////////////////////////////////////////////////////////
void GameStageWidget::resetGame()
{
    // (1) 清空角色區
    {
        QLayoutItem *child;
        while ((child = charLayout->takeAt(0)) != nullptr) {
            if (QWidget *w = child->widget()) {
                w->deleteLater();
            }
            delete child;
        }
        charLabels.clear();
    }

    // (2) 清空敵人區
    {
        for (QLabel *lbl : enemyLabels) {
            if (lbl) lbl->deleteLater();
        }
        enemyLabels.clear();

        QLayout *lyt = enemyArea->layout();
        if (lyt) {
            QLayoutItem *child;
            while ((child = lyt->takeAt(0)) != nullptr) {
                if (QWidget *w = child->widget()) {
                    w->deleteLater();
                }
                delete child;
            }
        }
    }

    // (3) 清空符石區
    clearGemLabels();

    // (4) 重新在角色區放 6 個灰底空格
    for (int i = 0; i < 6; ++i) {
        QLabel *lbl = new QLabel(this);
        lbl->setFixedSize(64, 64);
        lbl->setStyleSheet("background-color: #555555"); // 灰底
        charLayout->addWidget(lbl, 0, i);
        charLabels.append(lbl);
    }
}

////////////////////////////////////////////////////////////////////////////////
// initGame(): 真正把圖片貼到「角色／敵人／符石」上
//    必須在 MainWindow::gotoGameStage() 裡，例如：先 resetGame() 再 initGame()
//    才能把 selectedChars、currentWaveEnemies、board2D 一次畫上去
////////////////////////////////////////////////////////////////////////////////
void GameStageWidget::initGame()
{
    // (1) 先清空角色區，再把 selectedChars 裡的每隻 ID→pixmap
    {
        QLayoutItem *child;
        while ((child = charLayout->takeAt(0)) != nullptr) {
            if (QWidget *w = child->widget()) w->deleteLater();
            delete child;
        }
        charLabels.clear();
    }

    // 計算有幾隻非零 ID，以便後面計算 HP (純示意，不影響顯示)
    int totalHP  = 2000;
    int numChars = 0;
    for (int id : selectedChars) {
        if (id > 0) ++numChars;
    }

    // 依序把 6 格位置顯示成「角色圖」或「灰底」
    for (int i = 0; i < 6; ++i) {
        QLabel *lbl = new QLabel(this);
        lbl->setFixedSize(80, 80);

        int id = selectedChars.value(i, 0);
        if (id > 0) {
            // 範例路徑：:/character/dataset/character/ID1.png
            QString iconPath = QString(":/character/dataset/character/ID%1.png").arg(id);
            QPixmap pix(iconPath);
            lbl->setPixmap(pix.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        else {
            // 空格
            lbl->setStyleSheet("background-color: #555555");
        }

        charLayout->addWidget(lbl, 0, i);
        charLabels.append(lbl);
    }

    // (2) 清空敵人區，後面由 showEnemies() 真正貼圖
    {
        for (QLabel *lbl : enemyLabels) {
            if (lbl) lbl->deleteLater();
        }
        enemyLabels.clear();

        QLayout *lyt = enemyArea->layout();
        if (lyt) {
            QLayoutItem *child;
            while ((child = lyt->takeAt(0)) != nullptr) {
                if (QWidget *w = child->widget()) w->deleteLater();
                delete child;
            }
        }
    }

    // (3) 清空符石區，後面由 showBoard() 真正貼圖
    clearGemLabels();
}

////////////////////////////////////////////////////////////////////////////////
// showEnemies(): 從 Controller 拿到一波 Enemy*，把它們的圖顯示到敵人區
////////////////////////////////////////////////////////////////////////////////
void GameStageWidget::showEnemies(const QVector<Enemy*> &enemies)
{
    // 1. 先把 enemyLayout 裡所有 widget 都清掉
    QLayoutItem *child;
    while ((child = enemyLayout->takeAt(0)) != nullptr) {
        if (QWidget *w = child->widget()) {
            w->deleteLater();
        }
        delete child;
    }
    enemyLabels.clear();

    // 2. 若 enemies 為空，直接返回（enemyLayout 保持空白）
    if (enemies.isEmpty()) {
        return;
    }

    // 3. 使用 addStretch() + addWidget() + addStretch()
    //    讓每隻敵人的 QLabel 在水平方向上自動均勻分佈
    enemyLayout->addStretch();

    for (Enemy *e : enemies) {
        if (!e) continue;

        // 取得敵人的圖檔路徑 (getIconPath())
        QString path = e->getIconPath();
        QPixmap pix(path);

        // 建立 QLabel，顯示敵人圖
        QLabel *lbl = new QLabel(enemyArea);
        lbl->setFixedSize(100, 100);
        lbl->setPixmap(pix.scaled(lbl->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        lbl->setAlignment(Qt::AlignCenter);

        enemyLayout->addWidget(lbl);
        enemyLabels.append(lbl);

        // 每放一隻敵人，就放一個 stretch
        enemyLayout->addStretch();
    }
}

////////////////////////////////////////////////////////////////////////////////
// showBoard(): 從 Controller 拿到 6×5 個 Gem*，把盤面符石貼到 gemLabels2D 上
////////////////////////////////////////////////////////////////////////////////
void GameStageWidget::showBoard(const QVector<QVector<Gem*>> &board)
{
    // 假設 gemLabels2D 已經先配好 6×5 個 QLabel，僅要設定 pixmap
    for (int r = 0; r < board.size() && r < gemLabels2D.size(); ++r) {
        for (int c = 0; c < board[r].size() && c < gemLabels2D[r].size(); ++c) {
            Gem *g = board[r][c];
            QLabel *lbl = gemLabels2D[r][c];
            if (g && lbl) {
                QPixmap pix(g->getIconPath());
                lbl->setPixmap(pix.scaled(90,90, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                lbl->setVisible(true);
            }
            else {
                // 如果該位置沒有 Gem，就保持背景色即可
                lbl->setVisible(false);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// clearGemLabels(): 把盤面上所有 gemLabels2D 的 QLabel 先 delete 掉並清空
////////////////////////////////////////////////////////////////////////////////
void GameStageWidget::clearGemLabels()
{
    for (auto &row : gemLabels2D) {
        for (QLabel *lbl : row) {
            if (lbl) lbl->deleteLater();
        }
    }
    gemLabels2D.clear();
    gemLabels.clear();

    if (gemLayout) {
        QLayoutItem *child;
        while ((child = gemLayout->takeAt(0)) != nullptr) {
            if (QWidget *w = child->widget()) w->deleteLater();
            delete child;
        }
    }

    // 重新建立 6×5 個「黑底空框」作為 placeholder
    // 這段也可以留到 setupUI() 但為保證每次切波時都重置，就放在這邊
    for (int r = 0; r < GameController::ROWS; ++r) {
        QVector<QLabel*> row;
        for (int c = 0; c < GameController::COLS; ++c) {
            QLabel *lbl = new QLabel(this);
            lbl->setFixedSize(90, 90);
            lbl->setStyleSheet("background-color: #000000");
            gemLayout->addWidget(lbl, r, c);
            row.append(lbl);
            gemLabels.append(lbl);
        }
        gemLabels2D.append(row);
    }
}

////////////////////////////////////////////////////////////////////////////////
// onMoveTimeUp(): Controller 倒數到 → UI 禁止滑動、進入消除判定
////////////////////////////////////////////////////////////////////////////////
void GameStageWidget::onMoveTimeUp()
{
    qDebug() << "[GameStageWidget] onMoveTimeUp()";
    // TODO: 讓 UI 停止任何拖動；顯示「正在判定中」
}

////////////////////////////////////////////////////////////////////////////////
// onMatchesFound(): Controller 找到 matched coords → UI 執行消除動畫
////////////////////////////////////////////////////////////////////////////////
void GameStageWidget::onMatchesFound(const QList<QPair<int,int>> &matchedCoords,
                                     int comboCount)
{
    qDebug() << "[GameStageWidget] onMatchesFound() combo =" << comboCount;
    // matchedCoords 裡每個 pair 就是要消除的 (row,col)
    // 這裡示意把對應 QLabel 設為 hidden
    for (auto &p : matchedCoords) {
        int r = p.first;
        int c = p.second;
        if (r >= 0 && r < gemLabels2D.size() &&
            c >= 0 && c < gemLabels2D[r].size())
        {
            QLabel *lbl = gemLabels2D[r][c];
            if (lbl) lbl->setVisible(false);
        }
    }
    // 200ms 後再通知 Controller 真正刪除 board 資料
    QTimer::singleShot(200, [this, matchedCoords]() {
        emit clearGems(matchedCoords);
    });
}

////////////////////////////////////////////////////////////////////////////////
// onDealDamage(): Controller 回傳 totalDamage → UI 更新敵人血量、受傷動畫
////////////////////////////////////////////////////////////////////////////////
void GameStageWidget::onDealDamage(int totalDamage)
{
    qDebug() << "[GameStageWidget] onDealDamage() dmg =" << totalDamage;
    // TODO: 更新畫面上每隻敵人的 HP (可自行寫受傷動畫)
    QTimer::singleShot(200, [this]() {
        emit enemiesAttacked();
    });
}

////////////////////////////////////////////////////////////////////////////////
// onWaveCleared(): Controller 發射 waveCleared → UI 清空敵人＆盤面、等待下一波
////////////////////////////////////////////////////////////////////////////////
void GameStageWidget::onWaveCleared()
{
    qDebug() << "[GameStageWidget] onWaveCleared()";
    // (1) 清空敵人
    for (QLabel *lbl : enemyLabels) {
        if (lbl) lbl->deleteLater();
    }
    enemyLabels.clear();

    QLayout *lyt = enemyArea->layout();
    if (lyt) {
        QLayoutItem *child;
        while ((child = lyt->takeAt(0)) != nullptr) {
            if (QWidget *w = child->widget()) w->deleteLater();
            delete child;
        }
    }

    // (2) 清空盤面
    clearGemLabels();

    // 下一步將由 Controller 重新 generateInitialGems() → MainWindow 會 call showBoard()/showEnemies()
}

////////////////////////////////////////////////////////////////////////////////
// 模擬勝利 / 模擬失敗：直接發出 gameOver()
////////////////////////////////////////////////////////////////////////////////
void GameStageWidget::onFakeWinButtonClicked()
{
    emit gameOver(true);
}

void GameStageWidget::onFakeLoseButtonClicked()
{
    emit gameOver(false);
}

////////////////////////////////////////////////////////////////////////////////
// 設定按鈕：發出 pauseRequested()
////////////////////////////////////////////////////////////////////////////////
void GameStageWidget::onSettingClicked()
{
    emit pauseRequested();
}

////////////////////////////////////////////////////////////////////////////////
// 暫停遊戲：暫時停掉任何互動或動畫
////////////////////////////////////////////////////////////////////////////////
void GameStageWidget::pauseGame()
{
    qDebug() << "[GameStageWidget] pauseGame()";
    isPaused = true;
}

////////////////////////////////////////////////////////////////////////////////
// 恢復遊戲：重新允許互動或動畫
////////////////////////////////////////////////////////////////////////////////
void GameStageWidget::resumeGame()
{
    qDebug() << "[GameStageWidget] resumeGame()";
    isPaused = false;
}

////////////////////////////////////////////////////////////////////////////////
// setupUI(): 建立「敵人區／設定按鈕／模擬按鈕／角色區／符石區」的佈局
////////////////////////////////////////////////////////////////////////////////
void GameStageWidget::setupUI()
{
    // 主垂直佈局 (總高 540×960)
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ─────── 1. 敵人區 ───────────────────────────────────
    enemyArea = new QWidget(this);
    enemyArea->setFixedSize(540, 390);
    enemyArea->setStyleSheet("background-color: #EFEFEF;");

    // 建立一個水平佈局，所有敵人 QLabel 都會放到這裡
    enemyLayout = new QHBoxLayout(enemyArea);
    enemyLayout->setContentsMargins(10,10,10,10);
    enemyLayout->setSpacing(20);

    // 把 enemyArea 加到主畫面（靠上方）
    mainLayout->addWidget(enemyArea);

    // 設定按鈕 (齒輪)：大小 30×30，絕對定位到右上 (500,10)
    settingButton = new QPushButton(enemyArea);
    settingButton->setIcon(QIcon(":/icons/setting.png")); // 你自己準備齒輪 icon
    settingButton->setIconSize(QSize(24,24));
    settingButton->setFixedSize(30,30);
    settingButton->move(500, 10);
    settingButton->raise();
    connect(settingButton, &QPushButton::clicked,
            this, &GameStageWidget::onSettingClicked);

    mainLayout->addWidget(enemyArea);

    // ------------------------------------------------------------------------
    // (2) 模擬勝利 / 模擬失敗 / 下一波 按鈕
    // ------------------------------------------------------------------------
    QWidget *buttonArea = new QWidget(this);
    buttonArea->setFixedHeight(50);

    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonArea);
    buttonLayout->setContentsMargins(10, 5, 10, 5);
    buttonLayout->setSpacing(20);

    fakeWinBtn       = new QPushButton(tr("Sim Win"), buttonArea);
    fakeLoseBtn      = new QPushButton(tr("Sim Lose"), buttonArea);
    nextBattleButton = new QPushButton(tr("Next Battle"), buttonArea);

    buttonLayout->addStretch();
    buttonLayout->addWidget(fakeWinBtn);
    buttonLayout->addWidget(fakeLoseBtn);
    buttonLayout->addWidget(nextBattleButton);
    buttonLayout->addStretch();

    connect(fakeWinBtn, &QPushButton::clicked,
            this, &GameStageWidget::onFakeWinButtonClicked);
    connect(fakeLoseBtn, &QPushButton::clicked,
            this, &GameStageWidget::onFakeLoseButtonClicked);
    connect(nextBattleButton, &QPushButton::clicked,
            this, &GameStageWidget::onWaveCleared);

    mainLayout->addWidget(buttonArea);

    statusBar = new QProgressBar(this);
    statusBar->setFixedSize(530, 20);
    statusBar->setAlignment(Qt::AlignCenter);
    statusBar->setStyleSheet(
        "QProgressBar {"
        "  background-color: #8B4513;"    // 棕色
        "  border: 1px solid #000000;"
        "}"
        "QProgressBar::chunk {"
        "  background-color: #FF69B4;"    // 粉紅
        "}"
    );
    statusBar->setRange(0, 100);
    statusBar->setValue(70);

    // 外層用一個 QWidget 再搭 QHBoxLayout 置中
    QWidget *statusContainer = new QWidget(this);
    statusContainer->setFixedHeight(30);
    QHBoxLayout *hbox = new QHBoxLayout(statusContainer);
    hbox->setContentsMargins(0,0,0,0);
    hbox->addStretch();
    hbox->addWidget(statusBar);
    hbox->addStretch();
    mainLayout->addWidget(statusContainer);


    // ------------------------------------------------------------------------
    // (3) 角色區 (6 個灰底格子，後續 initGame() 會貼圖)
    // ------------------------------------------------------------------------
    QWidget *charArea = new QWidget(this);
    charArea->setFixedHeight(80);

    charLayout = new QGridLayout(charArea);
    charLayout->setContentsMargins(5, 5, 5, 5);
    charLayout->setSpacing(10);

    for (int i = 0; i < 6; ++i) {
        QLabel *lbl = new QLabel(charArea);
        lbl->setFixedSize(80, 80);
        lbl->setStyleSheet("background-color: #555555"); // 預設灰底
        charLayout->addWidget(lbl, 0, i);
        charLabels.append(lbl);
    }

    mainLayout->addWidget(charArea);

    // ------------------------------------------------------------------------
    // (4) 符石區 (6×5，每格 90×90，初始黑底)
    // ------------------------------------------------------------------------
    QWidget *gemContainer = new QWidget(this);
    gemContainer->setFixedSize(540, 450);

    gemLayout = new QGridLayout(gemContainer);
    gemLayout->setContentsMargins(0, 0, 0, 0);
    gemLayout->setSpacing(2);

    for (int r = 0; r < GameController::ROWS; ++r) {
        QVector<QLabel*> row;
        for (int c = 0; c < GameController::COLS; ++c) {
            QLabel *lbl = new QLabel(gemContainer);
            lbl->setFixedSize(80,80);
            lbl->setStyleSheet("background-color: #000000"); // 初始黑底
            gemLayout->addWidget(lbl, r, c);
            row.append(lbl);
            gemLabels.append(lbl);
        }
        gemLabels2D.append(row);
    }

    mainLayout->addWidget(gemContainer);
}
