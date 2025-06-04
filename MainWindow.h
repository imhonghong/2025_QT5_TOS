// MainWindow.h
#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include "PrepareStageWidget.h"
#include "GameStageWidget.h"
#include "PauseWidget.h"
#include "FinishStageWidget.h"
#include "GameController.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // (A) 從 Prepare 傳進來角色 ID 與 missionID → 切到 Game 階段
    void gotoGameStage(const QVector<int> &selectedChars, int missionID);

    // (B) Game → Finish（true:玩家勝, false:玩家敗）
    void gotoFinishStage(bool playerWon);

    // (C) Game → Pause
    void gotoPause();

    // (D) Pause → Resume → 回到 Game
    void resumeGame();

    // (E) Pause → Surrender → Finish（玩家直接投降）
    void surrenderFromPause();

    // (F) Finish → Restart → 回到 Prepare
    void restartGame();

private:
    QStackedWidget      *stack;

    PrepareStageWidget  *prepareWidget;  // index=0
    GameStageWidget     *gameWidget;     // index=1
    PauseWidget         *pauseWidget;    // index=2
    FinishStageWidget   *finishWidget;   // index=3

    GameController      *gameController;
};
