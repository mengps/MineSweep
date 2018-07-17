#include <QMainWindow>
#include "minesweep.h"

class QPushButton;
class QLabel;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);
    ~MainWindow();

protected:
	void timerEvent(QTimerEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void resizeEvent(QResizeEvent *event);
	void mousePressEvent(QMouseEvent *event);

	public slots:
	void updateToolbar();
	void restart();

private:
    int m_timeId;
    int m_passTime;
    MineSweep *m_mineSweep;
    QToolBar *m_toolBar;
    QPushButton *m_button;
    QLabel *m_curMine;
    QLabel *m_curTime;
};
