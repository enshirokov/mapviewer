#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGridLayout>
#include <QPushButton>
#include <QMouseEvent>
#include <QListWidget>

#include "myscene.h"

namespace Ui {
class MainWindow;
}

static const double SCALE_RATE = 0.2;
static const int GRID_SIZE = 60;
static const int ICON_SIZE = 64;
static const int MAINWINDOW_MIN_WIDTH = 800;
static const int MAINWINDOW_MIN_HEIGHT = 600;

static const QString MAIN_TITLE = "Map viewer";

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void paintGrid();               // paint grid on map

    void zoomIn();                  // zoom in
    void zoomOut();                 // zoom out
    void addPin();                  // add pin on map
    void setPin(QPointF);
    void changePinTitle(QListWidgetItem*);

    void openFile();                // open map
    void save();
    void quit();                    // quit

    void addMapFromStore();

private:
    void createActions();           // create actions
    void createMenus();             // create menus
    void createToolBar();           // create toolbar
    void createCentralWidget();     // create central widget



private:
    Ui::MainWindow* ui;
    QMenu* fileMenu;
    QMenu* toolMenu;
    QToolBar* fileToolBar;

    QAction* openAct;
    QAction* saveAct;
    QAction* quitAct;
    QAction* zoomInAct;
    QAction* zoomOutAct;
    QAction* pinAct;

    MyScene* scene;
    QGraphicsView* view;
    QTabWidget* tabWidget;
    QListWidget* listWidgetPin;
    QListWidget* listWidgetMap;


    QGraphicsItem* map;
    QList<QGraphicsPixmapItem*> pinList;
    QGraphicsItemGroup* itemGroupPin;
    QGraphicsItemGroup* itemGroupGrid;


};

#endif // MAINWINDOW_H
