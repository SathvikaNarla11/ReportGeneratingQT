#ifndef SORTBYDATE_H
#define SORTBYDATE_H

#include <QDialog>
//#include "mainwindow.h"

class MainWindow;

namespace Ui {
class SortByDate;
}

class SortByDate : public QDialog
{
    Q_OBJECT

public:
    explicit SortByDate(QWidget *parent = nullptr);
    ~SortByDate();

    int ItemfromGroceryCnt = 0;
    int ItemfromNutsCnt = 0;
    int ItemfromFruitsCnt = 0;

private slots:
    void on_pushButtonShow_clicked();

    void on_pushButtonGenerateReportPerDate_clicked();

    void on_pushButtonPreview_clicked();

private:
    Ui::SortByDate *ui;
    MainWindow *mainObj;
};

#endif // SORTBYDATE_H
