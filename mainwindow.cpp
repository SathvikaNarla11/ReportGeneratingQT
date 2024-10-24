#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    itemCosts["ItemfromGrocery"] = 20;
    itemCosts["ItemfromFruits"] = 90;
    itemCosts["ItemfromNuts"] = 80;

    cashierDetails["Alia"] = 2156;
    cashierDetails["Charlie"] = 2157;
    cashierDetails["Elsa"] = 2158;

    //    qDebug() << QSqlDatabase::drivers();

    ui->comboBoxCashierName->addItems({"", "Alia", "Charlie", "Elsa"});
    connect(ui->comboBoxCashierName, &QComboBox::currentTextChanged,[&]()
    {
        cashierName = ui->comboBoxCashierName->currentText();
        updateCashierID(cashierName);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateCashierID(QString cashierName)
{
    if(cashierDetails.contains(cashierName))
    {
        updateID = cashierDetails[cashierName];
        ui->lineEditCashierID->setText(QString::number(updateID));
    }
}

void MainWindow::on_pushButtonAdd_clicked()
{
    int rowCount = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(rowCount);

    QComboBox *comboBoxItems = new QComboBox();
    comboBoxItems->addItems({"","ItemfromGrocery","ItemfromFruits","ItemfromNuts"});
    ui->tableWidget->setCellWidget(rowCount, 0, comboBoxItems);

    QLineEdit *lineEditQuantity = new QLineEdit();
    ui->tableWidget->setCellWidget(rowCount, 1, lineEditQuantity);

    connect(lineEditQuantity, &QLineEdit::textChanged, [this, rowCount, comboBoxItems](const QString &noOfItems)
    {
        updatePrice(rowCount, comboBoxItems->currentText(), noOfItems);
    });
}

void MainWindow::updatePrice(int rowCount, QString itemname, QString noOfItems)
{
    qDebug()<<rowCount<<":"<<itemname<<":"<<noOfItems;
    double rate;
    if(itemCosts.contains(itemname))
    {
        rate = itemCosts[itemname];
        ui->tableWidget->setItem(rowCount, 2, new QTableWidgetItem(QString::number(rate, 'f', 2)));
    }
    else
    {
        rate = 0;
    }
    double amount = rate * (noOfItems.toDouble());
    ui->tableWidget->setItem(rowCount, 3, new QTableWidgetItem(QString::number(amount, 'f', 2)));
    qDebug()<<"rowCount before fun : " <<rowCount;
    //    on_pushButtonTotal_clicked(rowCount);

}

void MainWindow::on_pushButtonDel_clicked()
{
    int rowCount = ui->tableWidget->rowCount();
    ui->tableWidget->removeRow(rowCount-1);
}

void MainWindow::on_pushButtonTotal_clicked()
{
    double total = 0;
    int rowCount = ui->tableWidget->rowCount();
    for(int i=0; i<=rowCount; i++)
    {
        QTableWidgetItem *amt = ui->tableWidget->item(i, 3);
        if(amt)
        {
            total += amt->text().toDouble();
        }
    }
    totalAmountPaidByCustomer = total;
    ui->TotalAmountDisplayLabel->setText(QString::number(total));
}


void MainWindow::on_pushButtonPreview_clicked()
{
    Report = new QtRPT(this);
    Report->loadReport("C:/Users/DT_Server/Documents/Report/ReportPro/SalesReport/InvoiceReport.xml");
    connect(Report, &QtRPT::setValueImage, [&](const int recno, const QString paramname, QImage &paramvalue, const int reportpage)
    {
        Q_UNUSED(recno)
        Q_UNUSED(reportpage)
        if(paramname == "header_image")
        {
            auto *img= new QImage(":/Icons/Icons/globe.png");
            paramvalue = *img;
        }
    });

    int recNumber = ui->tableWidget->rowCount();

    cstName = ui->lineEditName->text();
    cstNumber = ui->lineEditNumber->text();

    for(int i=0; i<recNumber; i++)
    {
        SNO.append(QString::number(i+1));
        QComboBox *comboBoxItems = qobject_cast<QComboBox *>(ui->tableWidget->cellWidget(i, 0));
        if (comboBoxItems && !comboBoxItems->currentText().isEmpty())
        {
            item.append(comboBoxItems->currentText());
        }
        else
        {
            item.append("item (empty)");
        }

        QLineEdit *lineEditItems = qobject_cast<QLineEdit*>(ui->tableWidget->cellWidget(i, 1));
        if(lineEditItems)
        {
            qty.append(QString(lineEditItems->text()));
        }

        price.append(ui->tableWidget->item(i, 2)->text());
        amount.append(ui->tableWidget->item(i, 3)->text());
    }

    connect(Report, &QtRPT::setDSInfo, [=](DataSetInfo &dsinfo)
    {
        dsinfo.recordCount = recNumber;
    });

    QString totalAmount = ui->TotalAmountDisplayLabel->text();
    connect(Report, &QtRPT::setValue, [=](const int recno, const QString paramname, QVariant &paramvalue, const int reportpage)
    {
        Q_UNUSED(reportpage)
        if(paramname == "S.NO")
        {
            paramvalue = SNO[recno];
        }
        if(paramname == "ITEMS")
        {
            paramvalue = item[recno];
        }
        if(paramname == "QTY")
        {
            paramvalue = qty[recno];
        }
        if(paramname == "PRICE")
        {
            paramvalue = price[recno];
        }
        if(paramname == "AMOUNT")
        {
            paramvalue = amount[recno];
        }
        if(paramname == "TOTAL")
        {
            paramvalue = totalAmount;
        }
        if(paramname == "customerName")
        {
            paramvalue = cstName;
        }
        if(paramname == "customerNumber")
        {
            paramvalue = cstNumber;
        }
    });
    Report->printExec();
}

void MainWindow::on_pushButtonAddToDB_clicked()
{
    if (!connOpen())
    {
        qDebug() << "Database connection failed!";
        return;
    }

    currentDate = QDate::currentDate();
    QString strCurrentDate = currentDate.toString("MM/dd/yyyy");

    QSqlQuery qry;
    qry.prepare("INSERT INTO SalesReport (CUSTOMER_NAME, CUSTOMER_NUMBER, CASHIER_NAME, CASHIER_ID, TOTAL_AMOUNT, DATE) "
                "VALUES (:customerName, :customerNumber, :cashierName, :cashierID, :totalAmount, :date)");

    qry.bindValue(":customerName", cstName);
    qry.bindValue(":customerNumber", cstNumber);
    qry.bindValue(":cashierName", cashierName);
    qry.bindValue(":cashierID", updateID);
    qry.bindValue(":totalAmount", totalAmountPaidByCustomer);
    qry.bindValue(":date", strCurrentDate);

    if (!qry.exec())
    {
        qDebug() << "Query failed: " << qry.lastError().text();
    }
    else
    {
        qDebug() << "Query executed successfully";
    }

    for (int i = 0; i < item.size(); ++i)
    {
        qry.prepare("INSERT INTO itemDetails (Item, Qty, Price, Amount, Date) "
                    "VALUES (:item, :qty, :price, :amount, :date)");
        qry.bindValue(":item", item[i]);
        qry.bindValue(":qty", qty[i]);
        qry.bindValue(":price", price[i]);
        qry.bindValue(":amount", amount[i]);
        qry.bindValue(":date", strCurrentDate);

        if (!qry.exec())
        {
            qDebug() << "Failed to insert item " << i + 1 << ": " << qry.lastError().text();
        }
        else
        {
            qDebug() << "Item " << i + 1 << " inserted successfully into itemDetails";
        }
    }

    connClose();
}

void MainWindow::on_pushButtonToSort_clicked()
{
    sortDialogObj = new SortByDate(this);
    sortDialogObj->open();
}

