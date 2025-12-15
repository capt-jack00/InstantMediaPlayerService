#include "mainwindow.h"
#include "ui_mainwindow.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"
#include <QFileSystemModel>
#include <QListView>
#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFileSystemModel *model = new QFileSystemModel(this);

    connect(ui->selectPathBtn, &QPushButton::clicked, this, [this, model](){
        QString pathString = QFileDialog::getExistingDirectory(this, "Select a folder", QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        if (pathString.isEmpty()){
            return;
        }

        ui->pathLabel->setText(pathString);

        model->setRootPath(pathString);
        ui->fileView->setModel(model);
        ui->fileView->setRootIndex(model->index(pathString));
    });

    ma_engine_config engineConfig = ma_engine_config_init();
    engineConfig.sampleRate = 48000;
    engineConfig.channels = 2;

    ma_result engineResult = ma_engine_init(&engineConfig, &engine);
    if (engineResult != MA_SUCCESS) {
        qWarning() << "Failed to initialize the audio engine.";
    }

    QModelIndex fileIndex = ui->fileView->currentIndex();
    QString filePath = model->filePath(fileIndex);

    connect(ui->playBtn, &QPushButton::clicked, this, [this, model]{
        QModelIndex fileIndex = ui->fileView->currentIndex();
        QString mp3Path = model->filePath(fileIndex);
        ma_result playResult = ma_engine_play_sound(&engine, mp3Path.toUtf8().constData(), NULL);
        if (playResult != MA_SUCCESS) {
            qWarning() << "Failed to play the sound!";
        };
    });
    
}

MainWindow::~MainWindow()
{
    ma_engine_uninit(&engine); 
    delete ui;
}
