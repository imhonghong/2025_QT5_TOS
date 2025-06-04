// MainWindow.h
#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include "PrepareStageWidget.h"
#include "GameStageWidget.h"
#include "PauseWidget.h"
#include "FinishStageWidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 因為 PrepareStageWidget 現在傳入 selectedChars 與 missionID
    void gotoGameStage(const QVector<int> &selectedChars, int missionID);

    void gotoFinishStage(bool playerWon);
    void gotoPause();
    void resumeGame();
    void surrenderFromPause();
    void restartGame();

private:
    QStackedWidget *stack;

    PrepareStageWidget *prepareWidget;
    GameStageWidget    *gameWidget;
    PauseWidget        *pauseWidget;
    FinishStageWidget  *finishWidget;
};
