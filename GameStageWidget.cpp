// GameStageWidget.cpp
#include "GameStageWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QLabel>

// 簡易 RunestoneWidget：符石格子占位，固定 90×90
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

GameStageWidget::GameStageWidget(QWidget *parent)
    : QWidget(parent)
    , enemyArea(new QWidget(this))
    , playerArea(new QWidget(this))
    , runestoneArea(new QWidget(this))
    , gemLayout(new QGridLayout(runestoneArea))
    , settingButton(new QPushButton("⚙", this))
    , fakeWinBtn(new QPushButton("模擬勝利", this))
    , fakeLoseBtn(new QPushButton("模擬失敗", this))
    , isPaused(false)
    , missionID(1)  // 預設 missionID = 1
{
    // 整個 GameStageWidget 大小固定為 540×960
    setFixedSize(540, 960);

    setupUI();
}

void GameStageWidget::setupUI()
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // --- 敵人區 (540×410) ---
    enemyArea->setFixedSize(540, 410);
    {
        auto enemyLayout = new QVBoxLayout(enemyArea);
        enemyLayout->setContentsMargins(0, 0, 0, 0);
        enemyLayout->setSpacing(5);

        QLabel *enemyLabel = new QLabel("敵人區 (410×540)", enemyArea);
        enemyLabel->setAlignment(Qt::AlignCenter);
        QFont f; f.setPointSize(18); enemyLabel->setFont(f);
        enemyLayout->addWidget(enemyLabel);

        auto btnLayout = new QHBoxLayout;
        btnLayout->addStretch();
        fakeWinBtn->setFixedSize(100, 40);
        fakeLoseBtn->setFixedSize(100, 40);
        connect(fakeWinBtn, &QPushButton::clicked, this, &GameStageWidget::onFakeWinButtonClicked);
        connect(fakeLoseBtn, &QPushButton::clicked, this, &GameStageWidget::onFakeLoseButtonClicked);
        btnLayout->addWidget(fakeWinBtn);
        btnLayout->addSpacing(10);
        btnLayout->addWidget(fakeLoseBtn);
        btnLayout->addStretch();

        enemyLayout->addLayout(btnLayout);
    }
    mainLayout->addWidget(enemyArea);

    // --- 玩家區 (540×100) ---
    playerArea->setFixedSize(540, 100);
    {
        auto playerLayout = new QHBoxLayout(playerArea);
        playerLayout->setContentsMargins(10, 5, 10, 5);
        playerLayout->setSpacing(5);

        QLabel *playerLabel = new QLabel("玩家區 (100×540)", playerArea);
        playerLabel->setAlignment(Qt::AlignCenter);
        QFont f; f.setPointSize(16); playerLabel->setFont(f);
        playerLabel->setStyleSheet("background-color: #EFEFEF;");
        playerLabel->setFixedHeight(90);
        playerLayout->addWidget(playerLabel, 1);

        settingButton->setFixedSize(30, 30);
        connect(settingButton, &QPushButton::clicked, this, &GameStageWidget::onSettingClicked);
        playerLayout->addWidget(settingButton, 0, Qt::AlignTop);
    }
    mainLayout->addWidget(playerArea);

    // --- 符石區 (540×450) ---
    runestoneArea->setFixedSize(540, 450);
    runestoneArea->setStyleSheet("background-color: #000000;");
    {
        gemLayout->setContentsMargins(0, 0, 0, 0);
        gemLayout->setSpacing(0);

        // 產生 5×6 個 RunestoneWidget
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

// setter：儲存從 Prepare 傳來的角色列表
void GameStageWidget::setSelectedCharacters(const QVector<int> &chars)
{
    selectedChars = chars;
}

// setter：儲存從 Prepare 傳來的 missionID
void GameStageWidget::setMissionID(int mission)
{
    missionID = mission;
}

// initGame：初始化遊戲
void GameStageWidget::initGame()
{
    // (1) 清除先前資料
    resetGame();

    // (2) 使用 selectedChars 與 missionID 來載入三波敵人、生成符石棋盤等邏輯
    //     下面僅示範打印，請自行根據需求撰寫完整程式：
    // qDebug() << "進入 initGame，角色列表：" << selectedChars << "，missionID =" << missionID;

    isPaused = false;
}

// resetGame：重置所有動態資料
void GameStageWidget::resetGame()
{
    isPaused = false;
    // 如果你在實作時 new 了任何動態物件 (Enemy、Runestone 等)，請在此一併 delete
}

void GameStageWidget::pauseGame()
{
    if (isPaused) return;
    isPaused = true;
    // 停止 QTimer、阻斷滑鼠事件等
}

void GameStageWidget::resumeGame()
{
    if (!isPaused) return;
    isPaused = false;
    // 恢復 QTimer、恢復滑鼠事件等
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
