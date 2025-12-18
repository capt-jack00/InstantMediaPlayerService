#include "mainwindow.h"
#include "ui_mainwindow.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"
#include <QFileSystemModel>
#include <QListView>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->fileView->setSelectionMode(QAbstractItemView::SingleSelection);

    QFileSystemModel *model = new QFileSystemModel(this);

    connect(ui->selectPathBtn, &QPushButton::clicked, this, [this, model](){
        QString pathString = QFileDialog::getExistingDirectory(this, "Select a folder", QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        if (pathString.isEmpty()){
            QMessageBox::information(this, "Select a folder", "No folder has been selected!");
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
        QMessageBox::critical(this, "Audio engine error!", "Failed to initialaze the audio engine!");
    }

    QModelIndex fileIndex = ui->fileView->currentIndex();
    QString filePath = model->filePath(fileIndex);

    //TODO: Rename the object's names to more readable and consistent way
    //TODO: When other sound is selected using scroll buttons stop current sound and play the selected one immediately  
    connect(ui->playBtn, &QPushButton::clicked, this, [this, model]{
        QModelIndex fileIndex = ui->fileView->currentIndex();
        QString mp3Path = model->filePath(fileIndex);
        if(mp3Path.isEmpty()){
            qWarning() << "No sound selected! ";
            QMessageBox::warning(this, "Sound selection", "No sound has been selected!");
        }
        else{
            ma_result playResult = ma_engine_play_sound(&engine, mp3Path.toUtf8().constData(), NULL);
            if (playResult != MA_SUCCESS) {
                qWarning() << "Failed to play the sound!";
                QMessageBox::critical(this, "Error", "Failed to play the sound!");
            };
        }
    });


    //TODO: If possible, shorten and simplify the code below
    connect(ui->dwnBtn, &QPushButton::clicked, this, [this, model]{
        QModelIndex current = ui->fileView->currentIndex();
        qDebug() << "Current index: " << current;
        int row = current.row();
        QModelIndex next = model->index(row + 1, 0, current.parent());
        if(next.isValid()){
            ui->fileView->scrollTo(next);
            ui->fileView->setCurrentIndex(next);
            qDebug() << "Scrolled to " << next;
        }
        else{
            qDebug() << next << " is not valid!";
        }
    });

    connect(ui->upBtn, &QPushButton::clicked, this, [this, model]{
        QModelIndex current = ui->fileView->currentIndex();
        qDebug() << "Current index: " << current;
        int row = current.row();
        QModelIndex next = model->index(row - 1, 0, current.parent());
        if(next.isValid()){
            ui->fileView->scrollTo(next);
            ui->fileView->setCurrentIndex(next);
            qDebug() << "Scrolled to " << next;
        }
        else{
            qDebug() << next << " is not valid!";
        }
    });

    //TODO: Add functionality for stopBtn

    
}

MainWindow::~MainWindow()
{
    ma_engine_uninit(&engine); 
    delete ui;
}
