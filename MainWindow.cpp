// MainWindow.cpp
#include "MainWindow.h"
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , stack(new QStackedWidget(this))
    , prepareWidget(new PrepareStageWidget(this))
    , gameWidget(new GameStageWidget(this))
    , pauseWidget(new PauseWidget(this))
    , finishWidget(new FinishStageWidget(this))
{
    // 把四個階段加入 QStackedWidget
    stack->addWidget(prepareWidget);  // index 0
    stack->addWidget(gameWidget);     // index 1
    stack->addWidget(pauseWidget);    // index 2
    stack->addWidget(finishWidget);   // index 3

    // 固定 MainWindow 與 QStackedWidget 大小為 540×960
    this->setFixedSize(540, 960);
    stack->setFixedSize(540, 960);
    setCentralWidget(stack);

    // (1) 連接 PrepareStage 的 startClicked → MainWindow::gotoGameStage
    connect(prepareWidget, &PrepareStageWidget::startClicked,
            this, &MainWindow::gotoGameStage);

    // (2) 連接 GameStage 的 gameOver → MainWindow::gotoFinishStage
    connect(gameWidget, &GameStageWidget::gameOver,
            this, &MainWindow::gotoFinishStage);

    // (3) 連接 GameStage 的 pauseRequested → MainWindow::gotoPause
    connect(gameWidget, &GameStageWidget::pauseRequested,
            this, &MainWindow::gotoPause);

    // (4) 連接 PauseWidget 的 resumeClicked → MainWindow::resumeGame
    connect(pauseWidget, &PauseWidget::resumeClicked,
            this, &MainWindow::resumeGame);

    // (5) 連接 PauseWidget 的 surrenderClicked → MainWindow::surrenderFromPause
    connect(pauseWidget, &PauseWidget::surrenderClicked,
            this, &MainWindow::surrenderFromPause);

    // (6) 連接 FinishWidget 的 restartClicked → MainWindow::restartGame
    connect(finishWidget, &FinishStageWidget::restartClicked,
            this, &MainWindow::restartGame);
}

MainWindow::~MainWindow()
{
    // Qt 會自動 delete stack 及其子 widget
}

// (A) 由 Prepare → Game，帶參數：角色列表、missionID
void MainWindow::gotoGameStage(const QVector<int> &selectedChars, int missionID)
{
    // 1) 把角色列表與 missionID 傳給 GameStageWidget -> initGame()
    //    假設你要在 GameStageWidget 裡依照這些參數做後續三波邏輯
    //    先在 GameStageWidget 裡新增對應的成員函式，例如：
    //
    //    void setSelectedCharacters(const QVector<int>& chars);
    //    void setMissionID(int mission);
    //
    //    這裡我們假設 GameStageWidget 有這些介面，並已經實作好。
    gameWidget->setSelectedCharacters(selectedChars);
    gameWidget->setMissionID(missionID);

    // 2) 呼叫 initGame 開始初始化 (可以在裡面載入第一波敵人等)
    gameWidget->initGame();

    // 3) 切換到 GameStage
    stack->setCurrentIndex(1);
}

// (B) Game → Finish
void MainWindow::gotoFinishStage(bool playerWon)
{
    finishWidget->showResult(playerWon);
    stack->setCurrentIndex(3);
}

// (C) Game → Pause
void MainWindow::gotoPause()
{
    gameWidget->pauseGame();
    stack->setCurrentIndex(2);
}

// (D) Pause → Resume → Game
void MainWindow::resumeGame()
{
    stack->setCurrentIndex(1);
    gameWidget->resumeGame();
}

// (E) Pause 按「投降」 → Finish (失敗)
void MainWindow::surrenderFromPause()
{
    finishWidget->showResult(false);
    stack->setCurrentIndex(3);
}

// (F) Finish 按「Restart」 → 回 Prepare (並重置 Game)
void MainWindow::restartGame()
{
    stack->setCurrentIndex(0);
    gameWidget->resetGame();
}
