#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QSlider>
#include <QProgressBar>
#include <QComboBox>
#include <QGroupBox>
#include <QWinUI/QWinUIWidget.h>
#include <QWinUI/QWinUITheme.h>
#include <QWinUI/QWinUIAnimation.h>
#include <QWinUI/Controls/QWinUIToggleSwitch.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);


};

#endif // MAINWINDOW_H
