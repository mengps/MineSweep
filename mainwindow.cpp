#include <QTime>
#include <QLabel>
#include <QToolBar>
#include <QPushButton>
#include <QMouseEvent>
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    :   QMainWindow(parent)
{
    setWindowTitle(u8"扫雷");

    m_mineSweep = new MineSweep(this);
    setCentralWidget(m_mineSweep);

    m_timeId = startTimer(1000);
    m_passTime = 0;

    m_curMine = new QLabel(this);
    m_curTime = new QLabel(this);
    m_button = new QPushButton(u8"清空");
    m_button->setFixedHeight(25);
    connect(m_button, SIGNAL(clicked()), this, SLOT(restart()));
    connect(m_mineSweep, SIGNAL(press()), this, SLOT(updateToolbar()));
    m_curMine->setAlignment(Qt::AlignCenter);
    m_curTime->setAlignment(Qt::AlignCenter);

    m_toolBar = new QToolBar(this);
    m_toolBar->setStyleSheet("font-family:Consolas; font-size:20px; color:rgb(200, 15, 110)");
    m_toolBar->addWidget(m_curMine);
    m_toolBar->addWidget(m_button);
    m_toolBar->addWidget(m_curTime);
    m_toolBar->setMinimumHeight(25);
    m_toolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    addToolBar(m_toolBar);

    setContextMenuPolicy(Qt::NoContextMenu);
    resize(700, 700);
    updateToolbar();
}

MainWindow::~MainWindow()
{
    if (m_timeId != 0)
        killTimer(m_timeId);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space)
    {
        m_mineSweep->clearMap();
        m_mineSweep->startGame();
    }
    else if (event->key() == Qt::Key_Cancel) close();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    qreal w = (geometry().width() - 1) / 20.0;
    qreal h = (geometry().height() - m_toolBar->height() - 1) / 20.0;
    m_mineSweep->setGrid(w, h);
    m_curMine->setFixedWidth(width() / 2.5);
    m_curTime->setFixedWidth(width() / 2.5);
    m_button->setFixedWidth(width() / 5.8);
    update();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (m_mineSweep->rect().contains(event->pos()))
        {
            m_curMine->setText(QString(u8"地雷数：%1").arg(m_mineSweep->mine()));
            m_toolBar->update();
        }
        if (m_button->isChecked())
        {
            m_mineSweep->clearMap();
            m_mineSweep->startGame();
        }
    }
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    m_passTime++;
    updateToolbar();
}

void MainWindow::updateToolbar()
{
    m_curMine->setText(QString(u8"地雷数：%1").arg(m_mineSweep->mine()));
    m_curTime->setText(QString(u8"已用时间：%1").arg(m_passTime));
    m_toolBar->update();
}

void MainWindow::restart()
{
    m_passTime = 0;
    m_mineSweep->clearMap();
    m_mineSweep->startGame();
    updateToolbar();
}
