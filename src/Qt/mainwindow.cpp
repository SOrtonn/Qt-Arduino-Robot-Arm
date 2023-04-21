#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <qdebug.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //setStyleSheet("background-color: orange;");
    ui->lblColour->hide();
    arduino = new QSerialPort;

    //Opening and Configruing the port
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {//Scans thru all ports and finds one that the Arduino has connected to
     //This should stop errors from occuring when arduino is plugged into an alternative USB port
        if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier())
        {
            if(serialPortInfo.vendorIdentifier() == vendorID)
            {
                if(serialPortInfo.productIdentifier() == productID)
                {
                    ArduinoPortName = serialPortInfo.portName();
                   // qDebug() << "set name: " << ArduinoPortName; //UNCOMMENT TO SEE PORT BEING USED FOR DEBUGGING
                }
            }
        }
    }

    arduino->setPortName(ArduinoPortName);

    arduino->open(QSerialPort::ReadWrite); //setting the port to allow reading and writing between devices

    arduino->setBaudRate(QSerialPort::Baud9600); //set speed of communication (must match between devices)
    arduino->setDataBits(QSerialPort::Data8); //Size of a byte (number of data bits in each character is 8)
    arduino->setParity(QSerialPort::NoParity); //no parity bits //no error checking except any built into serial comms
    arduino->setStopBits(QSerialPort::OneStop); //1 bit of data represents end of each transmission message
    arduino->setFlowControl(QSerialPort::NoFlowControl); //no flow control (shouldnt matter with small messages)

    QObject::connect(arduino, SIGNAL(readyRead()), this, SLOT(readSerial()));
    //When data is ready to be read on the arduino serial port it calls this readSerial functiom


}

void MainWindow::setSliders()
{
   ui->sliderGripper->value();
}

MainWindow::~MainWindow()
{//Destructor function

    Communicate(QString("x%1").arg(-1));
    //put robot into default position

    if(arduino->isOpen())
    {//close the serial port if open
        arduino->close();
    }

    delete ui;
}
void MainWindow::Communicate(QString command)
{

    arduino->write(command.toStdString().c_str());

    qDebug() << "called";
}

void MainWindow::on_sliderRotWrist_sliderReleased()
{
    int value = ui->sliderRotWrist->value();
    Communicate(QString("r%1").arg(value)); //sends instruction as a combined string --> to be parsed by arduino script
    qDebug() << "value: " << value;
}

void MainWindow::on_sliderVertWrist_sliderReleased()
{
    int value = ui->sliderVertWrist->value();
    Communicate(QString("v%1").arg(value)); //sends instruction as a combined string --> to be parsed by arduino script
    qDebug() << "value: " << value;
}

void MainWindow::on_sliderElbow_sliderReleased()
{
    int value = ui->sliderElbow->value();
    Communicate(QString("e%1").arg(value)); //sends instruction as a combined string --> to be parsed by arduino script
    qDebug() << "value: " << value;
}

void MainWindow::on_sliderShoulder_sliderReleased()
{
    int value = ui->sliderShoulder->value();
    Communicate(QString("s%1").arg(value)); //sends instruction as a combined string --> to be parsed by arduino script
    qDebug() << "value: " << value;
}

void MainWindow::on_sliderBase_sliderReleased()
{
    int value = ui->sliderBase->value();
    Communicate(QString("b%1").arg(value)); //sends instruction as a combined string --> to be parsed by arduino script
    qDebug() << "value: " << value;
}

void MainWindow::on_btnBasic_clicked()
{   //sets the arm into its default position
    //resets sliders to represent this position

    //flag = 0;
    Communicate(QString("x%1").arg(-1));
    ui->lblColour->hide();
    ui->sliderBase->setValue(15);
    ui->sliderElbow->setValue(180);
    ui->sliderGripper->setValue(80);
    ui->sliderRotWrist->setValue(0);
    ui->sliderShoulder->setValue(15);
    ui->sliderVertWrist->setValue(180);
}
void MainWindow::on_sliderGripper_sliderReleased()
{
    int value = ui->sliderGripper->value();
    Communicate(QString("g%1").arg(value)); //sends instruction as a combined string --> to be parsed by arduino scrip
    qDebug() << "value: " << value;
}

void MainWindow::on_btnSortCol_clicked()
{
    //flag = 1;
    Communicate(QString("l%1").arg(-1));
    //if(flag == 1)
    //{
      // QObject::connect(arduino, SIGNAL(readyRead()), this, SLOT(readSerial()));
    //}
    readSerial();
}
void MainWindow::readSerial()
{
    ui->lblColour->setFixedSize(281,101);
    ui->lblColour->show();                  //load label with correct dimensions to show what the robot is doing
    ui->lblColour->setText(("NO COLOUR"));

    //qDebug() << serialBuffer;


    QByteArray serialData = arduino->readAll();//reads everything on the data line
    QString serialBuffer = QString::fromStdString(serialData.toStdString()); //parses the data into a string

    //qDebug() << serialBuffer;

    if(serialBuffer == "b")
    {//blue colour detected
        ui->lblColour->setFixedSize(141,101);
        ui->lblColour->setText("<font color='blue'>BLUE</font>");
    }
    else if(serialBuffer == "r")
    {//red colour detected
        ui->lblColour->setFixedSize(121,101);
       ui->lblColour->setText("<font color='red'>RED</font>");
    }
    else if(serialBuffer == "g")
    {//green colour detected
        ui->lblColour->setFixedSize(171,101);
        ui->lblColour->setText("<font color='green'>GREEN</font>");
    }
    else if(serialBuffer == "x")
    {//no colour detected
        ui->lblColour->setText("NO COLOUR");
    }

    //serialBuffer = "";

}

void MainWindow::on_btnSequence_clicked()
{
    Communicate(QString("k%1").arg(-1));
}

void MainWindow::on_btnSave_clicked()
{//save arm locations into an array
 //that can be called to go back to that position
    gripper[currentIndex] = ui->sliderGripper->value();
    wrist[currentIndex] = ui->sliderRotWrist->value();
    vrist[currentIndex] = ui->sliderVertWrist->value();
    elbow[currentIndex] = ui->sliderElbow->value();
    shoulder[currentIndex] = ui->sliderShoulder->value();
    base[currentIndex] = ui->sliderBase->value();

    QString posName = "position " + QString::number(currentIndex+1);
    ui->listSaved->addItem(posName); //add to listbox so user can call it

    currentIndex++;
}

void MainWindow::on_btnLoad_clicked()
{   //Moves arm to a presaved position

    int toLoad = ui->listSaved->currentRow();

    Communicate(QString("g%1").arg(gripper[toLoad])); //updates each servo &slider in pairs
    ui->sliderGripper->setValue(gripper[toLoad]);
    Communicate(QString("r%1").arg(wrist[toLoad]));
    ui->sliderRotWrist->setValue(wrist[toLoad]);
    Communicate(QString("v%1").arg(vrist[toLoad]));
    ui->sliderVertWrist->setValue(vrist[toLoad]);
    Communicate(QString("e%1").arg(elbow[toLoad]));
    ui->sliderElbow->setValue(elbow[toLoad]);
    Communicate(QString("s%1").arg(shoulder[toLoad]));
    ui->sliderShoulder->setValue(shoulder[toLoad]);
    Communicate(QString("b%1").arg(base[toLoad]));
    ui->sliderBase->setValue(base[toLoad]);







}
