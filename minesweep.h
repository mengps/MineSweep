#include <QWidget>

class MineSweep : public QWidget
{
    Q_OBJECT

public:
    MineSweep(QWidget *parent = 0);
    ~MineSweep();

    void setMapSize(uint w, uint h, uint mine);
    void generateMap();
    void startGame();
    void gameOver();
    void clearMap();
    void setGrid(qreal w, qreal h);
    uint mine()const;

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

signals :
    void press();

private:
    inline QRectF mapRect(qreal i, qreal j)const;
    uint nearHasMine(int i, int j)const;
    void openCap(int i, int j);

    enum Stat
    {
        opened,
        closed,
        marked,
        successful,
        unsuccessful
    };

    struct grid
    {
        QChar text;
        Stat stat;
        grid() : text(0), stat(closed) {}
    };

    grid **m_map;
    qreal m_gridWidth;
    qreal m_gridHeight;
    uint m_mapWidth;
    uint m_mapHeight;
    uint m_mineCount;
    uint m_curMine;
};
