#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "application.h"

#include <QFileDialog>
#include <QStandardPaths>

#include <QtDebug>

MainWindow::MainWindow(Application* app, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    app(app)
{
    Q_CHECK_PTR(app);

    ui->setupUi(this);

    ui->programsView->setModel(app->programs());
    ui->programsView->setModelColumn(programModelColumn());
    ui->padsView->setModel(app->pads());
    ui->knobsView->setModel(app->knobs());

    connect(app->programs(),
            &QAbstractItemModel::modelReset,
            this,
            &MainWindow::refreshActionDeleteProgram
    );

    refreshActionDeleteProgram();
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::programModelColumn() const
{
    return 1;
}

void MainWindow::on_actionNewProgram_triggered()
{
    Q_CHECK_PTR(app);

    app->newProgram("New program");
}

void MainWindow::on_programsView_activated(const QModelIndex& idx)
{
    Q_ASSERT(idx.isValid());
    Q_CHECK_PTR(idx.model());
    Q_CHECK_PTR(app);

    // Because 0 is the programId column
    // This kind of int finding based on the name should be extracted
    // to another funtion
    const QModelIndex programIdIndex = idx.model()->index(idx.row(), 0);
    const int programId = idx.model()->data(programIdIndex).toInt();

    app->setActiveProgramId(programId);
}

void MainWindow::on_actionDeleteProgram_triggered()
{
    Q_CHECK_PTR(app);

    app->deleteProgram(app->activeProgramId());
}

void MainWindow::refreshActionDeleteProgram()
{
    Q_CHECK_PTR(app);

    ui->actionDeleteProgram->setEnabled(app->programs()->rowCount() > 1);
}

void MainWindow::on_actionImportProgram_triggered()
{
    Q_CHECK_PTR(app);

    const QString path(
                QFileDialog::getOpenFileName(
                    this,
                    "Import LPD8 program",
                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)));
    if (path.isEmpty()) {
        return;
    }
    app->importProgram(path);
}

void MainWindow::on_actionExportProgram_triggered()
{
    Q_CHECK_PTR(app);

    const QString path(
                QFileDialog::getSaveFileName(
                    this,
                    "Export LPD8 program",
                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)));
    if (path.isEmpty()) {
        return;
    }
    app->exportActiveProgram(path);
}

void MainWindow::on_actionGetProgram1_triggered()
{
    Q_CHECK_PTR(app);

    app->fetchProgram(1);
}

void MainWindow::on_actionGetProgram2_triggered()
{
    Q_CHECK_PTR(app);

    app->fetchProgram(2);
}

void MainWindow::on_actionGetProgram3_triggered()
{
    Q_CHECK_PTR(app);

    app->fetchProgram(3);
}

void MainWindow::on_actionGetProgram4_triggered()
{
    Q_CHECK_PTR(app);

    app->fetchProgram(4);
}

void MainWindow::on_actionSendToProgram1_triggered()
{
    Q_CHECK_PTR(app);

    app->sendProgram(1);
}

void MainWindow::on_actionSendToProgram2_triggered()
{
    Q_CHECK_PTR(app);

    app->sendProgram(2);
}

void MainWindow::on_actionSendToProgram3_triggered()
{
    Q_CHECK_PTR(app);

    app->sendProgram(3);
}

void MainWindow::on_actionSendToProgram4_triggered()
{
    Q_CHECK_PTR(app);

    app->sendProgram(4);
}
