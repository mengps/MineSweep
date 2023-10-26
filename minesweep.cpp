#include <QTime>
#include <QPainter>
#include <QMouseEvent>
#include "minesweep.h"

static const QColor colorgroup[8] =
{
	Qt::green,
	QColor(255, 255, 0),
	QColor(255, 128, 0),
	QColor(170, 0, 170),
	Qt::blue,
	Qt::red,
	QColor(128, 255, 255),
	Qt::cyan
};

uint MineSweep::mine()const
{
    return m_curMine;
}

MineSweep::MineSweep(QWidget *parent)
	: QWidget(parent)
{
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
    setAttribute(Qt::WA_StaticContents);
    setMapSize(20, 20, 50);
    m_gridWidth = (size().width() - 1) / 20;
    m_gridHeight = (size().height() - 1) / 20;

    startGame();
}

MineSweep::~MineSweep()
{
    clearMap();
}

void MineSweep::setMapSize(uint w, uint h, uint mine)
{
    m_mapWidth = w;
    m_mapHeight = h;
    m_mineCount = mine;
    m_curMine = m_mineCount;
}

void MineSweep::generateMap()
{
    m_map = new grid *[m_mapWidth + 1];
    for (uint i = 0; i < m_mapWidth + 1; i++)
        m_map[i] = new grid[m_mapHeight + 1]();

    for (uint mine = 0; mine < m_mineCount; mine++)		//生成地雷
    {
        int w = qrand() % m_mapWidth;
        int h = qrand() % m_mapHeight;
        while (m_map[w][h].text.unicode() != 0)
        {
            w = qrand() % m_mapWidth;
            h = qrand() % m_mapHeight;
        }
        m_map[w][h].text = '*';
    }

    for (uint w = 0; w < m_mapWidth; w++)
    {
        for (uint h = 0; h < m_mapHeight; h++)
        {
            if (m_map[w][h].text.unicode() != '*')
                m_map[w][h].text = QChar('0').unicode() + nearHasMine(w, h);
        }
    }
}

void MineSweep::startGame()
{
    m_curMine = m_mineCount;
    generateMap();
    update();
}

void MineSweep::gameOver()
{
    for (uint i = 0; i < m_mapWidth; i++)
    {
        for (uint j = 0; j < m_mapHeight; j++)
        {
            if (closed == m_map[i][j].stat)
                m_map[i][j].stat = opened;
            else if (m_map[i][j].stat == marked)
            {
                if (m_map[i][j].text == '*')
                    m_map[i][j].stat = successful;
                else m_map[i][j].stat = unsuccessful;
            }
        }
    }
    update();
}

void MineSweep::clearMap()
{
    for (uint i = 0; i < m_mapWidth; i++)
        delete[] m_map[i];
    delete[]m_map;
}

void MineSweep::setGrid(qreal w, qreal h)
{
    m_gridWidth = w;
    m_gridHeight = h;
}

void MineSweep::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    QPen pen;
    pen.setWidth(6);
    painter.setFont(QFont("Consolas", 18));

    for (uint i = 0; i < m_mapWidth; i++)
    {
        for (uint j = 0; j < m_mapHeight; j++)
        {
            QRectF rect = mapRect(i, j);
            if (m_map[i][j].stat == closed)
                painter.fillRect(rect, palette().light());
            else if (m_map[i][j].stat == opened)
            {
                painter.fillRect(rect, palette().dark());
                if (m_map[i][j].text != '0')
                {
                    if (m_map[i][j].text == '*') pen.setColor(Qt::black);
                    else pen.setColor(colorgroup[(m_map[i][j].text.unicode() - '0' - 1)]);
                    painter.setPen(pen);
                    painter.drawText(rect, Qt::AlignCenter, m_map[i][j].text);
                }
            }
            else if (m_map[i][j].stat == marked)
            {
                painter.fillRect(rect, palette().light());
                painter.setPen(QColor(30, 120, 115));
                painter.drawText(rect, Qt::AlignCenter, u8"★");
            }
            else if (m_map[i][j].stat == successful)
            {
                painter.fillRect(rect, palette().dark());
                painter.setPen(Qt::black);
                painter.drawText(rect, Qt::AlignCenter, "*");
                painter.setPen(Qt::red);
                painter.drawText(rect, Qt::AlignCenter, u8"✔");
            }
            else if (m_map[i][j].stat == unsuccessful)
            {
                painter.fillRect(rect, palette().dark());
                painter.setPen(Qt::red);
                painter.drawText(rect, Qt::AlignCenter, u8"★");
                painter.setPen(Qt::black);
                painter.drawText(rect, Qt::AlignCenter, u8"✘");
            }
        }
    }

    painter.setPen(Qt::black);
    for (uint i = 0; i <= m_mapWidth; i++)
        painter.drawLine(QPointF(i * m_gridWidth, 0), QPointF(i * m_gridWidth, m_mapHeight * m_gridHeight));
    for (uint j = 0; j <= m_mapHeight; j++)
        painter.drawLine(QPointF(0, j * m_gridHeight), QPointF(m_mapWidth * m_gridWidth, j * m_gridHeight));
}

void MineSweep::mousePressEvent(QMouseEvent *event)
{
    uint x = event->pos().x() / m_gridWidth;
    uint y = event->pos().y() / m_gridHeight;

    if (x <= m_mapWidth && y <= m_mapHeight)
    {
        if (event->button() == Qt::LeftButton)
        {
            if (closed == m_map[x][y].stat) openCap(x, y);
            update();
        }
        if (event->button() == Qt::RightButton)
        {
            if (closed == m_map[x][y].stat)
            {
                m_map[x][y].stat = marked;
                m_curMine--;
                emit press();
            }
            else if (marked == m_map[x][y].stat)
            {
                m_map[x][y].stat = closed;
                m_curMine++;
                emit press();
            }
            update();
        }
    }
}

QRectF MineSweep::mapRect(qreal i, qreal j)const
{
    return QRectF(i * m_gridWidth + 1.0, j * m_gridHeight + 1.0, m_gridWidth, m_gridHeight);
}

uint MineSweep::nearHasMine(int i, int j)const
{
    uint count = 0;

    for (int w = i - 1; w <= i + 1; w++)
    {
        for (int h = j - 1; h <= j + 1; h++)
        {
            if (w == i && h == j) continue;
            else if (w < 0 || h < 0) continue;
            else
            {
                if (m_map[w][h].text == '*') count++;
            }
        }
    }

    return count;
}

void MineSweep::openCap(int i, int j)
{
    if (m_map[i][j].text == '*')
        gameOver();
    else
    {
        if (m_map[i][j].text == '0')
        {
            for (int w = i - 1; w <= i + 1; w++)
            {
                for (int h = j - 1; h <= j + 1; h++)
                {
                    if (m_map[qAbs(w)][qAbs(h)].stat == closed)
                    {
                        if (w == i && h == j)
                        {
                            m_map[i][j].stat = opened;
                            continue;
                        }
                        else
                        {
                            if (m_map[qAbs(w)][qAbs(h)].text != '*')
                                m_map[qAbs(w)][qAbs(h)].stat = opened;
                        }
                    }
                }
            }
        }
        else
        {
           if (m_map[i][j].stat == closed)
               m_map[i][j].stat = opened;
        }
    }
}
