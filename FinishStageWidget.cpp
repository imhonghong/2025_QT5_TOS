// FinishStageWidget.cpp
#include "FinishStageWidget.h"
#include <QApplication>
#include <QFont>

FinishStageWidget::FinishStageWidget(QWidget *parent)
    : QWidget(parent)
    , labelResult(new QLabel(this))
    , btnRestart(new QPushButton("Restart", this))
    , btnGoBack(new QPushButton("Go Back", this))
{
    auto layout = new QVBoxLayout(this);

    // 顯示結果 (You Win! / You Lose!)
    labelResult->setAlignment(Qt::AlignCenter);
    QFont f;
    f.setPointSize(24);
    labelResult->setFont(f);
    layout->addStretch();
    layout->addWidget(labelResult);

    layout->addSpacing(30);
    // 底下兩個按鈕：Restart、Go Back
    auto hLayout = new QHBoxLayout;
    btnRestart->setFixedSize(180, 60);
    btnGoBack->setFixedSize(180, 60);
    connect(btnRestart, &QPushButton::clicked,
            this, &FinishStageWidget::restartClicked);
    connect(btnGoBack, &QPushButton::clicked, [](){
        QApplication::quit();
    });
    hLayout->addWidget(btnRestart);
    hLayout->addSpacing(40);
    hLayout->addWidget(btnGoBack);

    layout->addLayout(hLayout);
    layout->addStretch();
}

void FinishStageWidget::showResult(bool playerWon)
{
    if (playerWon) {
        labelResult->setText("You Win!");
    } else {
        labelResult->setText("You Lose!");
    }
}
