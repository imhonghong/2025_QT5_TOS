// PauseWidget.cpp
#include "PauseWidget.h"
#include <QFont>

PauseWidget::PauseWidget(QWidget *parent)
    : QWidget(parent)
    , labelPause(new QLabel("暫停中", this))
    , btnResume(new QPushButton("Resume", this))
    , btnSurrender(new QPushButton("Surrender", this))
{
    auto layout = new QVBoxLayout(this);

    // 「暫停中」大標題
    labelPause->setAlignment(Qt::AlignCenter);
    QFont f;
    f.setPointSize(24);
    labelPause->setFont(f);
    layout->addStretch();
    layout->addWidget(labelPause);

    layout->addSpacing(30);
    // Resume 按鈕
    btnResume->setFixedSize(200, 60);
    connect(btnResume, &QPushButton::clicked,
            this, &PauseWidget::onResumeButtonClicked);
    layout->addWidget(btnResume, 0, Qt::AlignCenter);

    layout->addSpacing(20);
    // Surrender 按鈕
    btnSurrender->setFixedSize(200, 60);
    connect(btnSurrender, &QPushButton::clicked,
            this, &PauseWidget::onSurrenderButtonClicked);
    layout->addWidget(btnSurrender, 0, Qt::AlignCenter);

    layout->addStretch();
}

void PauseWidget::onResumeButtonClicked()
{
    emit resumeClicked();
}

void PauseWidget::onSurrenderButtonClicked()
{
    emit surrenderClicked();
}
