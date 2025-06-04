// MainWindow.cpp
#include "MainWindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , stack(new QStackedWidget(this))
    , prepareWidget(new PrepareStageWidget(this))
    , gameWidget(new GameStageWidget(this))
    , pauseWidget(new PauseWidget(this))
    , finishWidget(new FinishStageWidget(this))
    , gameController(new GameController(this))
{
    // (0) 把四個畫面加入 QStackedWidget
    stack->addWidget(prepareWidget);   // index = 0
    stack->addWidget(gameWidget);      // index = 1
    stack->addWidget(pauseWidget);     // index = 2
    stack->addWidget(finishWidget);    // index = 3

    // 固定視窗大小 540×960
    this->setFixedSize(540, 960);
    stack->setFixedSize(540, 960);
    setCentralWidget(stack);

    // (A) 連接 Prepare → MainWindow::gotoGameStage(...)
    connect(prepareWidget, &PrepareStageWidget::startClicked,
            this, &MainWindow::gotoGameStage);

    // (B) 連接 GameStageWidget 的「遊戲結束」事件到 gotoFinishStage
    connect(gameWidget, &GameStageWidget::gameOver,
            this, &MainWindow::gotoFinishStage);

    // (C) GameStageWidget 按設定鍵 → 進 Pause 畫面
    connect(gameWidget, &GameStageWidget::pauseRequested,
            this, &MainWindow::gotoPause);

    // (D) Pause → Resume → 回 Game
    connect(pauseWidget, &PauseWidget::resumeClicked,
            this, &MainWindow::resumeGame);

    // (E) Pause → Surrender → Finish（失敗）
    connect(pauseWidget, &PauseWidget::surrenderClicked,
            this, &MainWindow::surrenderFromPause);

    // (F) Finish → Restart → 回到 Prepare
    connect(finishWidget, &FinishStageWidget::restartClicked,
            this, &MainWindow::restartGame);

    // (G) GameController → GameStageWidget
    connect(gameController, &GameController::moveTimeUp,
            gameWidget, &GameStageWidget::onMoveTimeUp);
    connect(gameController, &GameController::matchesFound,
            gameWidget, &GameStageWidget::onMatchesFound);
    connect(gameController, &GameController::dealDamage,
            gameWidget, &GameStageWidget::onDealDamage);
    connect(gameController, &GameController::waveCleared,
            gameWidget, &GameStageWidget::onWaveCleared);

    // (H) GameController → MainWindow（直接換到 Finish）
    connect(gameController, &GameController::gameWon, this, [this]() {
        gotoFinishStage(true);
    });
    connect(gameController, &GameController::gameLost, this, [this]() {
        gotoFinishStage(false);
    });

    // (I) GameStageWidget → GameController（UI 回報給 Controller）
    connect(gameWidget, &GameStageWidget::swapFinished,
            gameController, &GameController::onPlayerSwapFinished);
    connect(gameWidget, &GameStageWidget::clearGems,
            gameController, &GameController::clearMatchedGems);
    connect(gameWidget, &GameStageWidget::enemiesAttacked,
            gameController, &GameController::onEnemiesAttacked);

    // 預設先顯示 Prepare 畫面
    stack->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    // Qt 會自動 delete stack 及底下各 widget
}

////////////////////////////////////////////////////////////////////////////////
// (A) 由 Prepare 傳來 selectedChars 與 missionID，進入 Game 階段
////////////////////////////////////////////////////////////////////////////////
void MainWindow::gotoGameStage(const QVector<int> &selectedChars, int missionID)
{
    // 1) 告訴 gameWidget：是哪個 mission & 哪些角色
    gameWidget->setMissionID(missionID);
    gameWidget->setSelectedCharacters(selectedChars);

    // 2) 先把遊戲邏輯告訴 Controller
    QVector<Character*> characterPointers;
    int totalHP  = 2000;
    int numChars = 0;
    // 算一共有幾隻非 0 的角色
    for (int id : selectedChars) {
        if (id > 0) ++numChars;
    }
    // 把每個 ID 轉成 Character* (此範例只示意屬性為 Water)
    for (int i = 0; i < selectedChars.size(); ++i) {
        int id = selectedChars[i];
        if (id > 0) {
            Character::Attribute attr = Character::Water;
            switch (id) {
                case 1: attr = Character::Water; break;
                case 2: attr = Character::Fire;  break;
                case 3: attr = Character::Earth; break;
                case 4: attr = Character::Light; break;
                case 5: attr = Character::Dark;  break;
                case 6: attr = Character::Water; break;
                default: attr = Character::Water; break;
            }
            QString iconPath = QString(":/character/dataset/character/ID%1.png").arg(id);
            int hpPerChar = (numChars > 0) ? (totalHP / numChars) : totalHP;
            Character *c = new Character(id, attr, hpPerChar, numChars, iconPath);
            characterPointers.append(c);
        }
    }

    gameController->init(characterPointers, missionID);
    gameController->startMission();

    // 3) 先 resetGame → 把灰底+空格放上
    gameWidget->resetGame();

    // 4) 再 initGame → 把角色圖貼上
    gameWidget->initGame();

    // 5) 讓 UI 顯示第一波「敵人圖」＆「符石盤面」
    gameWidget->showEnemies(gameController->getCurrentWaveEnemies());
    gameWidget->showBoard(gameController->getBoardMatrix());

    // 6) 切到 Game 畫面
    stack->setCurrentIndex(1);
}

////////////////////////////////////////////////////////////////////////////////
// (B) Game → Finish：顯示勝利／失敗
////////////////////////////////////////////////////////////////////////////////
void MainWindow::gotoFinishStage(bool playerWon)
{
    finishWidget->showResult(playerWon);
    stack->setCurrentIndex(3);
}

////////////////////////////////////////////////////////////////////////////////
// (C) Game → Pause
////////////////////////////////////////////////////////////////////////////////
void MainWindow::gotoPause()
{
    gameWidget->pauseGame();
    stack->setCurrentIndex(2);
}

////////////////////////////////////////////////////////////////////////////////
// (D) Pause → Resume
////////////////////////////////////////////////////////////////////////////////
void MainWindow::resumeGame()
{
    stack->setCurrentIndex(1);
    gameWidget->resumeGame();
}

////////////////////////////////////////////////////////////////////////////////
// (E) Pause → Surrender → Finish（玩家直接投降）
////////////////////////////////////////////////////////////////////////////////
void MainWindow::surrenderFromPause()
{
    finishWidget->showResult(false);
    stack->setCurrentIndex(3);
}

////////////////////////////////////////////////////////////////////////////////
// (F) Finish → Restart → 回到 Prepare
////////////////////////////////////////////////////////////////////////////////
void MainWindow::restartGame()
{
    gameWidget->resetGame();
    stack->setCurrentIndex(0);
}

