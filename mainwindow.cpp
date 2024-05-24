#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMediaCaptureSession>
#include <QCameraDevice>
#include <QMessageBox>
#include <QDebug>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Membuat objek kamera dan widget video
    camera = new QCamera;
    videoWidget = new QVideoWidget(this);
    imageCapture = new QImageCapture;
    captureSession = new QMediaCaptureSession;

    // Menghubungkan objek kamera dengan sesi capture
    captureSession->setCamera(camera);
    captureSession->setVideoOutput(videoWidget);
    captureSession->setImageCapture(imageCapture);

    // Menghubungkan sinyal klik tombol ke slot yang sesuai
    connect(ui->pushButton_start_camera, &QPushButton::clicked, this, &MainWindow::on_pushButton_start_camera_clicked);
    connect(ui->pushButton_stop_camera, &QPushButton::clicked, this, &MainWindow::on_pushButton_stop_camera_clicked);
    connect(ui->pushButton_capture_image, &QPushButton::clicked, this, &MainWindow::on_pushButton_capture_image_clicked);

    // Mengatur ukuran tampilan video
    videoWidget->setFixedSize(840, 480); // Atur ukuran sesuai kebutuhan

    // Menempatkan videoWidget ke dalam widget_camera
    ui->widget_camera->setLayout(new QVBoxLayout);
    ui->widget_camera->layout()->addWidget(videoWidget);

    // Menghubungkan sinyal untuk menangani gambar yang ditangkap
    connect(imageCapture, &QImageCapture::imageCaptured, this, &MainWindow::processCapturedImage);

    // Mengatur ukuran jendela dialog
    resize(800, 600); // Menggunakan resize agar pengguna bisa mengubah ukuran jendela

    // Muat Haar Cascade Classifier untuk deteksi wajah
    QString cascadePath = "D:/QT/FaceDetection/haarcascade_frontalface_default.xml";
    if (!faceCascade.load(cascadePath.toStdString())) {
        QMessageBox::critical(this, "Error", "Could not load face detector.");
        return;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete camera;
    delete imageCapture;
    delete captureSession;
}

void MainWindow::on_pushButton_start_camera_clicked()
{
    camera->start();
}

void MainWindow::on_pushButton_stop_camera_clicked()
{
    camera->stop();
}

void MainWindow::on_pushButton_capture_image_clicked()
{
    if (imageCapture) {
        imageCapture->captureToFile();
    }
}

void MainWindow::processCapturedImage(int id, const QImage &preview)
{
    Q_UNUSED(id);
    cv::Mat frame(preview.height(), preview.width(), CV_8UC4, (void*)preview.bits(), preview.bytesPerLine());
    cv::cvtColor(frame, frame, cv::COLOR_BGRA2BGR);

    // Debug: Tampilkan frame yang diambil
    cv::imshow("Captured Frame", frame);

    detectAndDisplay(frame);
}

void MainWindow::detectAndDisplay(cv::Mat &frame)
{
    // Deteksi wajah menggunakan Haar Cascade Classifier
    std::vector<cv::Rect> faces;
    faceCascade.detectMultiScale(frame, faces, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

    // Debug: Tampilkan jumlah wajah yang terdeteksi
    qDebug() << "Jumlah wajah yang terdeteksi:" << faces.size();

    // Loop melalui setiap wajah yang terdeteksi
    for (size_t i = 0; i < faces.size(); i++)
    {
        cv::Rect faceRect = faces[i];

        // Gambar kotak di sekitar wajah
        cv::rectangle(frame, faceRect, cv::Scalar(255, 0, 0), 2);
    }

    // Menampilkan frame yang sudah diproses
    cv::imshow("Detected Faces", frame);
}
