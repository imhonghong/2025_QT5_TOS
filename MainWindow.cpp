// MainWindow.cpp
#include "MainWindow.h"
#include <QApplication>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , stack(new QStackedWidget(this))
    , prepareWidget(new PrepareStageWidget(this))
    , gameWidget(new GameStageWidget(this))
    , pauseWidget(new PauseWidget(this))
    , finishWidget(new FinishStageWidget(this))
{
    // 把四個畫面加入 QStackedWidget
    stack->addWidget(prepareWidget);  // index 0
    stack->addWidget(gameWidget);     // index 1
    stack->addWidget(pauseWidget);    // index 2
    stack->addWidget(finishWidget);   // index 3

    // 固定視窗大小為 540×960
    this->setFixedSize(540, 960);
    stack->setFixedSize(540, 960);
    setCentralWidget(stack);

    // 連接 signal/slot
    connect(prepareWidget, &PrepareStageWidget::startClicked,
            this, &MainWindow::gotoGameStage);

    connect(gameWidget, &GameStageWidget::gameOver,
            this, &MainWindow::gotoFinishStage);

    connect(gameWidget, &GameStageWidget::pauseRequested,
            this, &MainWindow::gotoPause);

    connect(pauseWidget, &PauseWidget::resumeClicked,
            this, &MainWindow::resumeGame);

    connect(pauseWidget, &PauseWidget::surrenderClicked,
            this, &MainWindow::surrenderFromPause);

    connect(finishWidget, &FinishStageWidget::restartClicked,
            this, &MainWindow::restartGame);
}

MainWindow::~MainWindow()
{
    // Qt 自動釋放 stack 以及其子 widget
}

// (A) Prepare → Game，帶角色列表與 missionID
void MainWindow::gotoGameStage(const QVector<int> &selectedChars, int missionID)
{
    // 將參數傳給 GameStageWidget
    gameWidget->setSelectedCharacters(selectedChars);
    gameWidget->setMissionID(missionID);

    // 初始化遊戲
    gameWidget->initGame();
    // 切畫面
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

// (E) Pause → Surrender → Finish
void MainWindow::surrenderFromPause()
{
    finishWidget->showResult(false);
    stack->setCurrentIndex(3);
}

// (F) Finish → Restart → Prepare
void MainWindow::restartGame()
{
    gameWidget->resetGame();
    stack->setCurrentIndex(0);
}
