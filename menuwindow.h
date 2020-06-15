#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include <QWidget>

enum{
    BK_ROOMLIST = 1,//�����б���
    BK_LIVE,        //ֱ���䱳��
    BK_USERLIST,    //�û��б���
    FONT_TEXT,      //��������
    FONT_BS,        //��Ļ����
    COLOR_TEXT,     //����������ɫ
    COLOR_BS        //��Ļ������ɫ
};

namespace Ui {
class MenuWindow;
}

class MenuWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MenuWindow(QWidget *parent = nullptr);
    ~MenuWindow();

protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void on_btn_roomlist_clicked();

    void on_btn_livebk_clicked();

    void on_btn_userlist_clicked();

    void on_checkBox_roomlist_stateChanged(int arg1);

    void on_checkBox_livebk_stateChanged(int arg1);

    void on_checkBox_userlist_stateChanged(int arg1);

    void on_btn_bs_clicked();

    void on_btn_text_clicked();

    void on_btn_text_font_clicked();

    void on_btn_bs_font_clicked();

signals:
    void sigMes(const QString&, int);
    void sigColor(QColor&);

private:
    Ui::MenuWindow *ui;
};

#endif // MENUWINDOW_H
