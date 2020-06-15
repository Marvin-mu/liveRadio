#include "menuwindow.h"
#include "ui_menuwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QDebug>
#include <QColorDialog>

MenuWindow::MenuWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MenuWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/image/set.jpg"));
    ui->tabWidget->setTabText(0, "backbround");
    ui->tabWidget->setTabText(1, "font");
    ui->tabWidget->setTabText(2, "color");
    ui->tabWidget->setTabText(3, "about");
    //ui->tabWidget->setStyleSheet("backgroun-color:blank");
}

MenuWindow::~MenuWindow()
{
    delete ui;
}

void MenuWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QBrush brush(QColor(0,0,0));
    painter.setBrush(brush);
    painter.drawRect(this->rect());
    painter.end();
}

void MenuWindow::on_btn_roomlist_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open File"), "D:/",
                                                    tr("Images (*.png *.xpm *.jpg)"));
    if (fileName.isNull()) {
        QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("未选择图片"));
        return;
    }
    ui->lineEdit_roomlist->setText(fileName);
}

void MenuWindow::on_btn_livebk_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open File"), "D:/",
                                                    tr("Images (*.png *.xpm *.jpg)"));
    if (fileName.isNull()) {
        QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("未选择图片"));
        return;
    }
    ui->lineEdit_livebk->setText(fileName);
}

void MenuWindow::on_btn_userlist_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open File"), "D:/",
                                                    tr("Images (*.png *.xpm *.jpg)"));
    if (fileName.isNull()) {
        QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("未选择图片"));
        return;
    }
    ui->lineEdit_userlist->setText(fileName);
}

void MenuWindow::on_checkBox_roomlist_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked && !ui->lineEdit_roomlist->text().isEmpty()) {
        emit sigMes(ui->lineEdit_roomlist->text(), BK_ROOMLIST);
    }
}

void MenuWindow::on_checkBox_livebk_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked && !ui->lineEdit_livebk->text().isEmpty()) {
        emit sigMes(ui->lineEdit_livebk->text(), BK_LIVE);
    }
}

void MenuWindow::on_checkBox_userlist_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked && !ui->lineEdit_userlist->text().isEmpty()) {
        emit sigMes(ui->lineEdit_userlist->text(), BK_USERLIST);
    }
}

void MenuWindow::on_btn_bs_clicked()
{
    QColor color = QColorDialog::getColor(Qt::red, this, tr("color"), QColorDialog::DontUseNativeDialog);
    if (color.isValid()){
        QString str = QString("color:rgb(%1,%2,%3)").arg(color.red()).arg(color.green()).arg(color.blue());
        ui->label_16->setStyleSheet(str);
        emit sigMes(str, COLOR_BS);
        qDebug() << color.name() << color.red() << color.green() << color.blue();
    } else {
        qDebug() << "没有选择有效的颜色";
    }
}

void MenuWindow::on_btn_text_clicked()
{
    QColor color = QColorDialog::getColor(Qt::red, this, tr("color"), QColorDialog::DontUseNativeDialog);
    if (color.isValid()){
        QString str = QString("color:rgb(%1,%2,%3)").arg(color.red()).arg(color.green()).arg(color.blue());
        ui->label_17->setStyleSheet(str);
        emit sigMes(str, COLOR_TEXT);
        qDebug() << str << color.name() << color.red() << color.green() << color.blue();
    } else {
        qDebug() << "没有选择有效的颜色";
    }
}

void MenuWindow::on_btn_text_font_clicked()
{
    QFont font = ui->fontComboBox->currentFont();
    emit sigMes(font.toString(), FONT_TEXT);
}

void MenuWindow::on_btn_bs_font_clicked()
{
    QFont font = ui->fontComboBox_2->currentFont();
    emit sigMes(font.toString(), FONT_BS);
}
