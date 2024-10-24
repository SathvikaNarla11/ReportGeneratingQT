#include "sortbydate.h"
#include "ui_sortbydate.h"
#include "mainwindow.h"

SortByDate::SortByDate(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SortByDate),
    mainObj(new MainWindow)
{
    ui->setupUi(this);
}

SortByDate::~SortByDate()
{
    delete ui;
}

void SortByDate::on_pushButtonShow_clicked()
{
    ItemfromGroceryCnt = 0;
    ItemfromNutsCnt = 0;
    ItemfromFruitsCnt = 0;
    if(!mainObj->connOpen())
    {
        qDebug()<<"Not Connected from SortByDate";
    }
    else
    {
        qDebug()<<"Connected from SortByDate";
    }

    QString fromDate = ui->dateEditFrom->text();
    QString toDate = ui->dateEditTo->text();
    qDebug()<<fromDate<<":"<<toDate;

    QSqlQueryModel *modal = new QSqlQueryModel();
    QSqlQuery *qry = new QSqlQuery(mainObj->mydb);

    qry->prepare("SELECT * FROM itemDetails WHERE Date >= :fromDate AND Date <= :toDate");

    qry->bindValue(":fromDate", fromDate);
    qry->bindValue(":toDate", toDate);

    if (qry->exec())
    {
        modal->setQuery(*qry);
        ui->tableView->setModel(modal);
        qDebug() << "Query executed and data displayed.";
    }
    else
    {
        qDebug() << "Query failed: " << qry->lastError().text();
    }

    mainObj->connClose();


    QAbstractItemModel *model = ui->tableView->model();
    if(!model)
    {
        qDebug()<<"Model is not set for the tableView";
    }
    for(int row = 0; row < model->rowCount(); ++row)
    {
        QStringList rowDataItem;
        QStringList rowDataQty;
        QString Item;
        int itemQty;

        for(int column = 0; column < model->columnCount(); ++column)
        {
            if(column == 0)
            {
                QModelIndex index = model->index(row, column);
                QVariant data = model->data(index);
                Item = model->data(index).toString();

                QModelIndex index3 = model->index(row, column+1);
                QVariant data3 = model->data(index3);
                itemQty = model->data(index3).toInt();

                qDebug()<<Item<<" : "<<itemQty;
                if(Item == "ItemfromGrocery")
                {
                    ItemfromGroceryCnt +=itemQty;
                }
                if(Item == "ItemfromNuts")
                {
                    ItemfromNutsCnt +=itemQty;
                }
                if(Item == "ItemfromFruits")
                {
                    ItemfromFruitsCnt +=itemQty;
                }
            }
        }
    }
    qDebug()<<"___________________________________________________";
    qDebug()<<"ItemfromGroceryCnt : "<<ItemfromGroceryCnt;
    qDebug()<<"ItemfromNutsCnt : "<<ItemfromNutsCnt;
    qDebug()<<"ItemfromFruitsCnt : "<<ItemfromFruitsCnt;
    qDebug()<<"___________________________________________________";

}


void SortByDate::on_pushButtonGenerateReportPerDate_clicked()
{
    mainObj->close();
    QtRPT *saleReport = new QtRPT();
    saleReport->loadReport("C:/Users/DT_Server/Documents/Report/ReportPro/SalesReport/ReportPerDate.xml");

    connect(saleReport, &QtRPT::setValueImage, [&](const int recno, const QString paramname, QImage &paramvalue, const int reportpage)
    {
        Q_UNUSED(recno);
        Q_UNUSED(reportpage)
        if(paramname == "img")
        {
            auto *img = new QImage(":/Icons/Icons/globe.png");
            paramvalue = *img;
        }
    });

    connect(saleReport, &QtRPT::setValue, [&](const int recno, const QString paramname, QVariant &paramvalue, const int reportpage)
    {
        if(paramname == "ItemfromGroceryCnt")
        {
            paramvalue = ItemfromGroceryCnt;
        }
        if(paramname == "ItemfromFruitsCnt")
        {
            paramvalue = ItemfromFruitsCnt;
        }
        if(paramname == "ItemfromNutsCnt")
        {
            paramvalue = ItemfromNutsCnt;
        }
    });
    saleReport->printExec();
    qDebug()<<"ReportByDate is generated";

}


void SortByDate::on_pushButtonPreview_clicked()
{
    ui->tableWidget->setItem(0, 0, new QTableWidgetItem("ItemfromGrocery"));
    ui->tableWidget->setItem(0, 1, new QTableWidgetItem(QString::number(ItemfromGroceryCnt)));

    ui->tableWidget->setItem(1, 0, new QTableWidgetItem("ItemfromNuts"));
    ui->tableWidget->setItem(1, 1, new QTableWidgetItem(QString::number(ItemfromNutsCnt)));

    ui->tableWidget->setItem(2, 0, new QTableWidgetItem("ItemfromFruits"));
    ui->tableWidget->setItem(2, 1, new QTableWidgetItem(QString::number(ItemfromFruitsCnt)));

    ui->tableWidget->resizeColumnsToContents();
}

