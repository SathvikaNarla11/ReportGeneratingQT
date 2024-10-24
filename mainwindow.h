#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QtSql>
#include "qtrpt.h"
#include "sortbydate.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QSqlDatabase mydb;
    bool connOpen()
    {
        mydb = QSqlDatabase::addDatabase("QSQLITE");
        mydb.setDatabaseName("C:/sqlite3/saleReport.db");
        if(!mydb.open())
        {
            qDebug()<<"Failed to connect from mainwindow.h";
            return false;
        }
        else
        {
            qDebug()<<"Connected from mainwindow.h";
            return true;
        }
    }
    void connClose()
    {
        mydb.close();
        mydb.removeDatabase(QSqlDatabase::defaultConnection);
        qDebug()<<"Closed";
    }

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QtRPT *Report;
    QVector<QString>SNO;
    QVector<QString>item;
    QVector<QString>qty;
    QVector<QString>price;
    QVector<QString>amount;

    QString cstName;
    QString cstNumber;
    QString cashierName;
    int updateID;
    int totalAmountPaidByCustomer;

    SortByDate *sortDialogObj;
    QDate currentDate;
    QString strCurrentDate;

private slots:
    void on_pushButtonAdd_clicked();
    void updatePrice(int rowCount, QString itemname, QString noOfItems);
    void on_pushButtonDel_clicked();
    void on_pushButtonPreview_clicked();
    void on_pushButtonTotal_clicked(/*int rowCount*/);

    void on_pushButtonAddToDB_clicked();
    void updateCashierID(QString cashierName);

    void on_pushButtonToSort_clicked();

private:
    Ui::MainWindow *ui;
    QMap<QString, double> itemCosts;
    QMap<QString, int>cashierDetails;

};
#endif // MAINWINDOW_H
