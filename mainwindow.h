#pragma once

#include <QMainWindow>
#include <QString>
#include "miniaudio/miniaudio.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    ma_engine engine;
    ma_sound sound;
    QString currentAudio;
};
