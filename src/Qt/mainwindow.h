#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void Communicate(QString command);
    void setSliders();
    void readSerial();

    void on_sliderRotWrist_sliderReleased();

    void on_sliderVertWrist_sliderReleased();

    void on_sliderElbow_sliderReleased();

    void on_sliderShoulder_sliderReleased();

    void on_sliderBase_sliderReleased();

    void on_btnBasic_clicked();


    void on_sliderGripper_sliderReleased();

    void on_btnSortCol_clicked();

    void on_btnSequence_clicked();

    void on_btnSave_clicked();

    void on_btnLoad_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *arduino;
    QString ArduinoPortName;
    bool isAvailable;
    quint16 vendorID = 9025;    //Vendor/Product matching the Arduino UNO
    quint16 productID = 67;     //These were found from code --> displayed in logbook
    bool flag = 1;
    int gripper[100];
    int wrist[100];
    int vrist[100];
    int elbow[100];
    int shoulder[100];
    int base[100];
    int currentIndex = 0;

};

#endif // MAINWINDOW_H
