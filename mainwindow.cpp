#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h> // Contains file controls like O_RDWR
#include <cerrno> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

extern unsigned char RETURN_SYMBOL;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow),
                                          m_SerialPort(0),
                                          m_DelaySettings(std::make_shared<DelaySettings>()) {
    ui->setupUi(this);
    OpenUART();
}

void MainWindow::OpenUART() {
    // Open the serial port. Change device path as needed (currently set to a standard FTDI USB-UART cable type device)
    m_SerialPort = open("/dev/ttyACM0", O_RDWR);
    
    // Create new termios struct, we call it 'tty' for convention
    struct termios tty{};
    
    // Read in existing settings, and handle any error
    if (tcgetattr(m_SerialPort, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return;
    }
    
    tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size
    tty.c_cflag |= CS8; // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR |
                     ICRNL); // Disable any special handling of received bytes
    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    tty.c_cc[VTIME] = 1;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 100;
    
    // Set in/out baud rate to be 115200
    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);
    
    // Save tty settings, also checking for error
    if (tcsetattr(m_SerialPort, TCSANOW, &tty) != 0) return;
}

MainWindow::~MainWindow() {
    CloseUART();
    delete ui;
}

void I10ToA(int value, char* result) {
    int base = 10;
    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;
    
    do {
        tmp_value = value;
        value = value / base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while (value);
    
    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
}

void MainWindow::SetDelay(unsigned int ID) {
    int inputDelay = 0;
    switch (ID) {
        case 0:
            inputDelay = ui->lineEdit_Delay0->text().toInt();
            break;
        case 1:
            inputDelay = ui->lineEdit_Delay1->text().toInt();
            break;
        case 2:
            inputDelay = ui->lineEdit_Delay2->text().toInt();
            break;
        case 3:
            inputDelay = ui->lineEdit_Delay3->text().toInt();
            break;
        case 4:
            inputDelay = ui->lineEdit_Delay4->text().toInt();
            break;
        case 5:
            inputDelay = ui->lineEdit_Delay5->text().toInt();
            break;
        default:
            break;
    }
    
    char inputDelayStr[6] = {' ', ' ', ' ', ' ', ' ', ' '};
    I10ToA(inputDelay, inputDelayStr);
    unsigned char msg[10] = {'W', static_cast<unsigned char>('0' + ID), ' ', ' ', ' ', ' ', ' ', ' ', ' ', RETURN_SYMBOL};
    for (unsigned int i = 0; i < 6; i++) {
        msg[3 + i] = inputDelayStr[i];
    }
    write(m_SerialPort, msg, 10);
}

void MainWindow::GetDelay(unsigned int ID) {
    char read_buf[5];
    memset(read_buf, '\0', sizeof(read_buf));
    unsigned char msg[] = {'R', static_cast<unsigned char>('0' + ID), RETURN_SYMBOL};
    write(m_SerialPort, msg, sizeof(msg));
    read(m_SerialPort, &read_buf, sizeof(read_buf));
    m_DelaySettings->SetDelay(ID, atoi(read_buf));
    
    int printValue = m_DelaySettings->GetDelay(ID);
    switch (ID) {
        case 0:
            ui->lineEdit_Delay0->setText(QString::number(printValue));
            break;
        case 1:
            ui->lineEdit_Delay1->setText(QString::number(printValue));
            break;
        case 2:
            ui->lineEdit_Delay2->setText(QString::number(printValue));
            break;
        case 3:
            ui->lineEdit_Delay3->setText(QString::number(printValue));
            break;
        case 4:
            ui->lineEdit_Delay4->setText(QString::number(printValue));
            break;
        case 5:
            ui->lineEdit_Delay5->setText(QString::number(printValue));
            break;
        default:
            break;
    }
}

void MainWindow::on_pushButton_GET_DelaySettings_clicked() {
    for (unsigned int i = 0; i < 6; i++) {
        GetDelay(i);
        usleep(100);
    }
}

void MainWindow::on_pushButton_SET_DelaySettings_clicked() {
    for (unsigned int i = 0; i < 6; i++) {
        SetDelay(i);
        usleep(100);
    }
}

void MainWindow::on_pushButton_StartTrigger_clicked() const {
    unsigned char msg[] = {'S', '\r'};
    write(m_SerialPort, msg, sizeof(msg));
}

void MainWindow::on_pushButton_StopTrigger_clicked() const {
    unsigned char msg[] = {'P', '\r'};
    write(m_SerialPort, msg, sizeof(msg));
}

void MainWindow::CloseUART() const {
    ::close(m_SerialPort);
}

void MainWindow::on_actionExit_2_triggered() {
    CloseUART();
    QCoreApplication::exit();
}

void MainWindow::on_pushButton_SET_ChannelSettings_clicked() {
    unsigned char ResetTiming_msg[] = {'C', '\r'};
    write(m_SerialPort, ResetTiming_msg, sizeof(ResetTiming_msg));
    usleep(100);
    unsigned char B0_msg[] = {'B', static_cast<unsigned char>('0' + ui->comboBox_B0->currentIndex()), '0', '\r'};
    write(m_SerialPort, B0_msg, sizeof(B0_msg));
    usleep(100);
    unsigned char B1_msg[] = {'B', static_cast<unsigned char>('0' + ui->comboBox_B1->currentIndex()), '1', '\r'};
    write(m_SerialPort, B1_msg, sizeof(B1_msg));
    usleep(100);
    unsigned char B2_msg[] = {'B', static_cast<unsigned char>('0' + ui->comboBox_B2->currentIndex()), '2', '\r'};
    write(m_SerialPort, B2_msg, sizeof(B2_msg));
    usleep(100);
    unsigned char B3_msg[] = {'B', static_cast<unsigned char>('0' + ui->comboBox_B3->currentIndex()), '3', '\r'};
    write(m_SerialPort, B3_msg, sizeof(B3_msg));
    usleep(100);
    unsigned char B4_msg[] = {'B', static_cast<unsigned char>('0' + ui->comboBox_B4->currentIndex()), '4', '\r'};
    write(m_SerialPort, B4_msg, sizeof(B4_msg));
    usleep(100);
    unsigned char B5_msg[] = {'B', static_cast<unsigned char>('0' + ui->comboBox_B5->currentIndex()), '5', '\r'};
    write(m_SerialPort, B5_msg, sizeof(B5_msg));
    usleep(100);
    unsigned char B6_msg[] = {'B', static_cast<unsigned char>('0' + ui->comboBox_B6->currentIndex()), '6', '\r'};
    write(m_SerialPort, B6_msg, sizeof(B6_msg));
    usleep(100);
    unsigned char B7_msg[] = {'B', static_cast<unsigned char>('0' + ui->comboBox_B7->currentIndex()), '7', '\r'};
    write(m_SerialPort, B7_msg, sizeof(B7_msg));
    usleep(100);
    unsigned char D0_msg[] = {'D', static_cast<unsigned char>('0' + ui->comboBox_D0->currentIndex()), '0', '\r'};
    write(m_SerialPort, D0_msg, sizeof(D0_msg));
    usleep(100);
    unsigned char D1_msg[] = {'D', static_cast<unsigned char>('0' + ui->comboBox_D1->currentIndex()), '1', '\r'};
    write(m_SerialPort, D1_msg, sizeof(D1_msg));
    usleep(100);
    unsigned char D2_msg[] = {'D', static_cast<unsigned char>('0' + ui->comboBox_D2->currentIndex()), '2', '\r'};
    write(m_SerialPort, D2_msg, sizeof(D2_msg));
    usleep(100);
    unsigned char D3_msg[] = {'D', static_cast<unsigned char>('0' + ui->comboBox_D3->currentIndex()), '3', '\r'};
    write(m_SerialPort, D3_msg, sizeof(D3_msg));
}

