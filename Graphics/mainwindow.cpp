#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLayout>
#include <QTime>
#include <QDebug>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QGraphicsItem>
#include <QToolButton>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setMinimumSize(MAINWINDOW_MIN_WIDTH, MAINWINDOW_MIN_HEIGHT);

    setWindowTitle(MAIN_TITLE);
    setWindowIcon(QIcon(":/images/images/main_icon.png"));

    createActions();
    createMenus();
    createToolBar();
    createCentralWidget();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createActions()
{
    openAct = new QAction(QIcon(":/images/images/open.png"), tr("&Open"), this);
    openAct->setShortcuts(QKeySequence::New);
    openAct->setStatusTip(tr("Open file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openFile()));

    saveAct = new QAction(QIcon(":/images/images/save.png"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::New);
    saveAct->setStatusTip(tr("Open file"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(quit()));

    zoomInAct = new QAction(QIcon(":/images/images/zoom_in.png"), tr("&Zoom In"), this);
    zoomInAct->setStatusTip(tr("Zoom In"));
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAct = new QAction(QIcon(":/images/images/zoom_out.png"), tr("&Zoom Out"), this);
    zoomOutAct->setStatusTip(tr("Zoom In"));
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    pinAct = new QAction(QIcon(":/images/images/pinR.png"), tr("&Add pin"), this);
    pinAct->setStatusTip(tr("Zoom In"));
    pinAct->setCheckable(true);
   // connect(pinAct, SIGNAL(triggered()), this, SLOT(addPin()));
}

void MainWindow::createMenus()
{
    this->menuBar()->setNativeMenuBar(false);
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAct);

    toolMenu = menuBar()->addMenu(tr("&Tools"));
    toolMenu->addAction(zoomInAct);
    toolMenu->addAction(zoomOutAct);
    toolMenu->addAction(pinAct);
}

void MainWindow::createToolBar()
{
    QList<QToolBar *> allToolBars = this->findChildren<QToolBar *>();
    foreach(QToolBar *tb, allToolBars) {
        // This does not delete the tool bar.
        this->removeToolBar(tb);
    }

    fileToolBar = addToolBar(tr("File"));
    fileToolBar->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
    fileToolBar->addSeparator();

    fileToolBar->addAction(zoomInAct);
    fileToolBar->addAction(zoomOutAct);
    fileToolBar->addAction(pinAct);



}

void MainWindow::createCentralWidget()
{
    scene = new MyScene();
    connect(scene, SIGNAL(cursorPosition(QPointF)), this, SLOT(setPin(QPointF)));

    view = new QGraphicsView(scene);
    view->setMouseTracking(true);

    listWidgetPin = new QListWidget;
    connect(listWidgetPin, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(changePinTitle(QListWidgetItem*)));

    listWidgetMap = new QListWidget;

    tabWidget = new QTabWidget;
    tabWidget->addTab(listWidgetPin, "Pin list");
    tabWidget->addTab(listWidgetMap, "Map list");

    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(view, 1);
    layout->addWidget(tabWidget);

    this->centralWidget()->setLayout(layout);
}

void MainWindow::paintGrid()
{
    int width = scene->width();
    int height = scene->height();

    if (itemGroupGrid == NULL)                       // not to destroy the group if it does not exist
        scene->destroyItemGroup(itemGroupGrid);

    itemGroupGrid = scene->createItemGroup(QList<QGraphicsItem*>());

    for (int i = 0; i < height; i += GRID_SIZE) {
        QGraphicsLineItem* line = new QGraphicsLineItem(0, i, width, i);
        itemGroupGrid->addToGroup(line);

    }

    for (int i = 0; i < width; i += GRID_SIZE) {
        QGraphicsLineItem* line = new QGraphicsLineItem(i, 0, i, height);
        itemGroupGrid->addToGroup(line);
    }

}

void MainWindow::zoomIn()
{
    if (scene->items().isEmpty())
        return;

    qreal scale = map->scale() + SCALE_RATE;
    map->setScale(scale);
    itemGroupPin->setScale(scale);
    scene->setSceneRect(map->sceneBoundingRect());

    paintGrid();
}

void MainWindow::zoomOut()
{
    if (scene->items().isEmpty())
        return;

    qreal scale = map->scale() - SCALE_RATE;
    map->setScale(scale);
    itemGroupPin->setScale(scale);

    if (map->sceneBoundingRect().width() <= view->size().width() || map->sceneBoundingRect().height() <= view->size().height()) {
        map->setScale(map->scale() + SCALE_RATE);            // return last scale
        itemGroupPin->setScale(itemGroupPin->scale() + SCALE_RATE);
    }

    scene->setSceneRect(map->sceneBoundingRect());

    paintGrid();
}

void MainWindow::addPin()
{

}

void MainWindow::setPin(QPointF point)
{
    if (scene->items().isEmpty())
        return;

    QImage image(":/images/images/pin.png");
    QGraphicsPixmapItem* currentPin = new QGraphicsPixmapItem(QPixmap::fromImage(image));

    if (pinAct->isChecked()) {
        currentPin->setPos(point.x(), point.y() - currentPin->boundingRect().height());
        //currentPin->setFlag(QGraphicsItem::ItemIgnoresTransformations);

        itemGroupPin->addToGroup(currentPin);
        itemGroupPin->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        //scene->addItem(currentPin);

        pinList.append(currentPin);

        QListWidgetItem* newItem = new QListWidgetItem(QIcon(":/images/images/pin.png"), "new pin");
        //newItem->setFlags(Qt::ItemIsEditable);
        listWidgetPin->addItem(newItem);
        //listWidgetPin->editItem(newItem);
        //listWidgetPin->openPersistentEditor(newItem);

        pinAct->setChecked(false);

    }

}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Open Image File",QDir::currentPath(), tr("Image Files (*.png *.jpg *.bmp)"));
    // need filter!

    QImage image(fileName);
    map = new QGraphicsPixmapItem(QPixmap::fromImage(image));

    map->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    map->setPos(0, 0);
    scene->addItem(map);
    scene->setSceneRect(map->sceneBoundingRect());

    QListWidgetItem* newItem = new QListWidgetItem(QIcon(":/images/images/map.png"), "new map");
    listWidgetMap->addItem(newItem);

    itemGroupPin = scene->createItemGroup(QList<QGraphicsItem*>());
    itemGroupPin->setZValue(1);

    paintGrid();

}

void MainWindow::save()
{
    // save

}

void MainWindow::quit()
{
    this->close();
}

void MainWindow::changePinTitle(QListWidgetItem* item)
{
    for (int i = 0; i < listWidgetPin->count(); i++)
        listWidgetPin->closePersistentEditor(listWidgetPin->item(i));

    listWidgetPin->openPersistentEditor(item);
}

void MainWindow::addMapFromStore()
{
    // fill listWidgetMap from store

}



