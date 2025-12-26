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

    //FIXME: When playing any song for the first time and pausing it, no more will play again. 

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
    else{

        qDebug() << "Audio engine initialized successfully!";

    }

    //TODO: Rename the object's names to more readable and consistent way
    //TODO: When other sound is selected using scroll buttons stop current sound and play the selected one immediately  
    connect(ui->playBtn, &QPushButton::clicked, this, [this, model]{
        QModelIndex fileViewIndex = ui->fileView->currentIndex();
        QString mp3Path = model->filePath(fileViewIndex);

        if(mp3Path.isEmpty()){
            qWarning() << "No sound selected! ";
            QMessageBox::warning(this, "Sound selection", "No sound has been selected!");
        }

        else{
            ma_sound_init_from_file(&engine, mp3Path.toUtf8().constData(), 0, NULL, NULL, &sound);
            ma_result playResult = ma_sound_start(&sound);

            if (playResult != MA_SUCCESS) {
                qWarning() << "Failed to play the sound!";
                QMessageBox::critical(this, "Error", "Failed to play the sound!");
            }
            else{
                qDebug() << "Sound played succesfully!";
                qDebug() << "Current playing sound: " + mp3Path;
                currentAudio = mp3Path;
            };
        }
    });

    connect(ui->stopBtn, &QPushButton::clicked, this, [this]{
        ma_sound_stop(&sound);
    });

    connect(ui->resumeBtn, &QPushButton::clicked, this, [this]{
        ma_sound_start(&sound);
    });


    //TODO: If possible, shorten and simplify the code below
    connect(ui->dwnBtn, &QPushButton::clicked, this, [this, model]{
        QModelIndex currentSelectedIndex = ui->fileView->currentIndex();
        qDebug() << "Current index: " << currentSelectedIndex;
        int row = currentSelectedIndex.row();
        QModelIndex next = model->index(row + 1, 0, currentSelectedIndex.parent());

        if(next.isValid()){
            ui->fileView->scrollTo(next);
            ui->fileView->setCurrentIndex(next);
        
            QModelIndex fileViewIndex = ui->fileView->currentIndex();
            QString mp3Path = model->filePath(fileViewIndex);
            ma_result playResult = ma_engine_play_sound(&engine, mp3Path.toUtf8().constData(), NULL);

            if (playResult != MA_SUCCESS) {

                qWarning() << "Failed to play the sound!";
                QMessageBox::critical(this, "Error", "Failed to play the sound!");

            } 
            else{
                qDebug() << "Sound played!";
            };

            qDebug() << "Scrolled to " << next;
        }
        else{
            qDebug() << next << " is not valid!";
        }
    });

    connect(ui->upBtn, &QPushButton::clicked, this, [this, model]{
        QModelIndex currentSelectedIndex = ui->fileView->currentIndex();
        qDebug() << "Current index: " << currentSelectedIndex;
        int row = currentSelectedIndex.row();
        QModelIndex next = model->index(row - 1, 0, currentSelectedIndex.parent());

        if(next.isValid()){
            ui->fileView->scrollTo(next);
            ui->fileView->setCurrentIndex(next);

            QModelIndex fileViewIndex = ui->fileView->currentIndex();
            QString mp3Path = model->filePath(fileViewIndex);
            ma_sound_init_from_file(&engine, mp3Path.toUtf8().constData(), 0, NULL, NULL, &sound);
            ma_result playResult = ma_sound_start(&sound);

            if (playResult != MA_SUCCESS) {

                qWarning() << "Failed to play the sound!";
                QMessageBox::critical(this, "Error", "Failed to play the sound!");
                
            }
            else{
                qDebug() << "Sound played!";
            };

            qDebug() << "Scrolled to " << next;
        }
        else{
            qDebug() << next << " is not valid!";
        }
    });
}

MainWindow::~MainWindow()
{
    ma_engine_uninit(&engine); 
    delete ui;
}
