#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <cstdio>
#include <cstring>
#include <fcntl.h> // Contains file controls like O_RDWR
#include <cerrno> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

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
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Disable any special handling of received bytes
    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    // Set in/out baud rate to be 115200
    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    // Save tty settings, also checking for error
    if (tcsetattr(m_SerialPort, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return;
    }
}

MainWindow::~MainWindow() {
    CloseUART();
    delete ui;
}

void MainWindow::on_pushButton_GET_DelaySettings_clicked() {
    // Allocate memory for read buffer, set size according to your needs
    char read_buf [12];

    // Normally you wouldn't do this memset() call, but since we will just receive
    // ASCII data for this example, we'll set everything to 0 so we can
    // call printf() easily.
    memset(&read_buf, '\0', sizeof(read_buf));

    // Sending PAUSE command
    unsigned char pause_command[2] = { 'P', 13 };
    write(m_SerialPort, pause_command, 2);

    // Sending command asking for the delay values
    unsigned char msg[] = { 'R', '0', 13 };
    write(m_SerialPort, msg, sizeof(msg));

    // Read bytes. The behaviour of read() (e.g. does it block?,
    // how long does it block for?) depends on the configuration
    // settings above, specifically VMIN and VTIME
    int num_bytes = read(m_SerialPort, &read_buf, sizeof(read_buf));

    // n is the number of bytes read. n may be 0 if no bytes were received, and can also be -1 to signal an error.
    if (num_bytes < 0) {
        printf("Error reading: %s", strerror(errno));
        return;
    }

    // Here we assume we received ASCII data, but you might be sending raw bytes (in that case, don't try and
    // print it to the screen like this!)
    printf("Read %i bytes. Received message: %s", num_bytes, read_buf);

    // Sending UNPAUSE command
    unsigned char unpause_command[2] = { 'S', 13 };
    write(m_SerialPort, unpause_command, 2);
}

void MainWindow::on_pushButton_SET_DelaySettings_clicked() {

}

void MainWindow::on_pushButton_StartTrigger_clicked() {
    unsigned char msg[] = { 'S', '\r' };
    write(m_SerialPort, msg, sizeof(msg));
}

void MainWindow::on_pushButton_StopTrigger_clicked() {
    unsigned char msg[] = { 'P', '\r' };
    write(m_SerialPort, msg, sizeof(msg));
}

void MainWindow::CloseUART() const {
    ::close(m_SerialPort);
}
