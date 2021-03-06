/****************************************************************************
MIT License

Copyright (c) 2018 Janne Saari

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
****************************************************************************/

#include "namelistwidget.h"
#include "adddialog.h"
#include "ticketprinter.h"
#include "mainwindow.h"

#include <QGridLayout>

NamelistWidget::NamelistWidget(MainWindow *parent)
    : parent(parent)
{
    table = new TableModel(this);
    workstations = new Workstations();
    setupNamelist();
    readFromFile("Nimilista");
    firstTimeOpening = false;
}

NamelistWidget::~NamelistWidget()
{
    writeToFile("Nimilista");
}

void NamelistWidget::setupNamelist()
{
    QGridLayout *mainLayout = new QGridLayout(this);

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(table);

    tableView = new QTableView(this);
    tableView->setModel(proxyModel);

    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->verticalHeader()->hide();
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->sortByColumn(0, Qt::AscendingOrder);

    tableView->setSortingEnabled(true);

    connect(tableView->selectionModel(),
        &QItemSelectionModel::selectionChanged,
        this, &NamelistWidget::selectionChanged);

    mainLayout->addWidget(tableView);
}

void NamelistWidget::showAddEntryDialog()
{
    AddDialog aDialog(this);
    if (aDialog.exec()) {
        Person person;
        person.name = aDialog.nameText->text();
        if(aDialog.morning->isChecked())
            person.shift = 0;
        else if(aDialog.day->isChecked())
            person.shift = 1;
        else if(aDialog.evening->isChecked())
            person.shift = 2;

        person.isMonday = aDialog.monday->isChecked();
        person.isTuesday = aDialog.tuesday->isChecked();
        person.isWednesday = aDialog.wednesday->isChecked();
        person.isThursday = aDialog.thursday->isChecked();
        person.isFriday = aDialog.friday->isChecked();

        person.workstation = aDialog.workstationButtonGroup->checkedId();
        person.information = aDialog.informationText->toPlainText();

        addEntry(person);
    }
}

void NamelistWidget::addEntry(Person person)
{
    if (!table->getPeople().contains(person)) {
        table->insertRows(0, 1, QModelIndex());

        QModelIndex index = table->index(0, TableModel::Columns::NAME, QModelIndex());
        table->setData(index, person.name, Qt::EditRole);
        index = table->index(0, TableModel::Columns::WORKSTATIONS, QModelIndex());
        table->setData(index, QVariant(person.workstation), Qt::EditRole);
        index = table->index(0, TableModel::Columns::SHIFT, QModelIndex());
        table->setData(index, QVariant(person.shift), Qt::EditRole);
        index = table->index(0, TableModel::Columns::MONDAY, QModelIndex());
        table->setData(index, QVariant(person.isMonday), Qt::EditRole);
        index = table->index(0, TableModel::Columns::TUESDAY, QModelIndex());
        table->setData(index, QVariant(person.isTuesday), Qt::EditRole);
        index = table->index(0, TableModel::Columns::WEDNESDAY, QModelIndex());
        table->setData(index, QVariant(person.isWednesday), Qt::EditRole);
        index = table->index(0, TableModel::Columns::THURSDAY, QModelIndex());
        table->setData(index, QVariant(person.isThursday), Qt::EditRole);
        index = table->index(0, TableModel::Columns::FRIDAY, QModelIndex());
        table->setData(index, QVariant(person.isFriday), Qt::EditRole);
        index = table->index(0, TableModel::Columns::INFO, QModelIndex());
        table->setData(index, person.information, Qt::EditRole);

        workstations->setWorkstation(person);

    } else if(!readingFromFile){
        QMessageBox::information(this, tr("Toistuva nimi"),
            tr("Henkilö \"%1\" on jo listalla.").arg(person.name));
    }
}

void NamelistWidget::editEntry()
{
    //This could be broken to multiple functions
    //Get person from table,
    //open new addDialog and set values to same as that persons.
    //compare if values were changed and set new values to the table

    QTableView *temp = static_cast<QTableView*>(tableView);
    QSortFilterProxyModel *proxy = static_cast<QSortFilterProxyModel*>(temp->model());
    QItemSelectionModel *selectionModel = temp->selectionModel();
    QModelIndexList indexes = selectionModel->selectedRows();

    Person person;
    int row = -1;

    //Get selected persons values
    foreach (QModelIndex index, indexes) {
        row = proxy->mapToSource(index).row();

        QModelIndex nameIndex = table->index(row, TableModel::Columns::NAME, QModelIndex());
        QVariant varName = table->data(nameIndex, Qt::DisplayRole);
        person.name = varName.toString();

        QModelIndex workstationIndex = table->index(row, TableModel::Columns::WORKSTATIONS, QModelIndex());
        QVariant varWorkstation = table->data(workstationIndex, Qt::DisplayRole);
        person.workstation = varWorkstation.toInt();

        QModelIndex shiftIndex = table->index(row, TableModel::Columns::SHIFT, QModelIndex());
        QVariant varShift= table->data(shiftIndex, Qt::DisplayRole);

        if(varShift == "Aamu")
            person.shift = 0;
        else if(varShift == "Päivä")
            person.shift = 1;
        else if(varShift == "Ilta")
            person.shift = 2;

        QModelIndex mondayIndex = table->index(row, TableModel::Columns::MONDAY, QModelIndex());
        QVariant varMonday= table->data(mondayIndex, Qt::DisplayRole);
        person.isMonday = varMonday.toBool();
        QModelIndex tuesdayIndex = table->index(row, TableModel::Columns::TUESDAY, QModelIndex());
        QVariant varTuesday= table->data(tuesdayIndex, Qt::DisplayRole);
        person.isTuesday = varTuesday.toBool();
        QModelIndex wednesdayIndex = table->index(row, TableModel::Columns::WEDNESDAY, QModelIndex());
        QVariant varWednesday= table->data(wednesdayIndex, Qt::DisplayRole);
        person.isWednesday = varWednesday.toBool();
        QModelIndex thursdayIndex = table->index(row, TableModel::Columns::THURSDAY, QModelIndex());
        QVariant varThursday= table->data(thursdayIndex, Qt::DisplayRole);
        person.isThursday = varThursday.toBool();
        QModelIndex fridayIndex = table->index(row, TableModel::Columns::FRIDAY, QModelIndex());
        QVariant varFriday= table->data(fridayIndex, Qt::DisplayRole);
        person.isFriday = varFriday.toBool();

        QModelIndex informationIndex = table->index(row, TableModel::Columns::INFO, QModelIndex());
        QVariant varInformation = table->data(informationIndex, Qt::DisplayRole);
        person.information = varInformation.toString();
    }

    //Open new addDialog window and set values to selected persons values
    AddDialog aDialog(this);
    aDialog.setWindowTitle(tr("Muokkaa henkilöä"));

    aDialog.nameText->setText(person.name);
    aDialog.informationText->setText(person.information);
    aDialog.workstationButtonGroup->button(person.workstation)->setChecked(true);

    if(person.shift == 0)
        aDialog.morning->setChecked(true);
    else if(person.shift == 1)
        aDialog.day->setChecked(true);
    else if(person.shift == 2)
        aDialog.evening->setChecked(true);

    aDialog.monday->setChecked(person.isMonday);
    aDialog.tuesday->setChecked(person.isTuesday);
    aDialog.wednesday->setChecked(person.isWednesday);
    aDialog.thursday->setChecked(person.isThursday);
    aDialog.friday->setChecked(person.isFriday);

    aDialog.editedPersonWorkstation = person.workstation; //tracks persons workstation's number to enable button for it after updating workstation list.
    aDialog.editedPersonShift = person.shift;
    aDialog.updateWorkstationList();

    //Get edited values and compare them to previous ones
    //and set new values if they changed
    Person newValues;
    if (aDialog.exec()) {
        newValues.name = aDialog.nameText->text();
        if (newValues.name!= person.name) {
            QModelIndex index = table->index(row, TableModel::Columns::NAME, QModelIndex());
            table->setData(index, QVariant(newValues.name), Qt::EditRole);
        }
        newValues.workstation = aDialog.workstationButtonGroup->checkedId();
        if (newValues.workstation!= person.workstation) {
            QModelIndex index = table->index(row, TableModel::Columns::WORKSTATIONS, QModelIndex());
            table->setData(index, QVariant(newValues.workstation), Qt::EditRole);
        }

        if(aDialog.morning->isChecked())
            newValues.shift = 0;
        else if(aDialog.day->isChecked())
            newValues.shift = 1;
        else if(aDialog.evening->isChecked())
            newValues.shift = 2;

        if (newValues.shift != person.shift) {
            QModelIndex index = table->index(row, TableModel::Columns::SHIFT, QModelIndex());
            table->setData(index, QVariant(newValues.shift), Qt::EditRole);
        }
        newValues.isMonday= aDialog.monday->isChecked();
        if (newValues.isMonday!= person.isMonday) {
            QModelIndex index = table->index(row, TableModel::Columns::MONDAY, QModelIndex());
            table->setData(index, QVariant(newValues.isMonday), Qt::EditRole);
        }
        newValues.isTuesday= aDialog.tuesday->isChecked();
        if (newValues.isTuesday!= person.isTuesday) {
            QModelIndex index = table->index(row, TableModel::Columns::TUESDAY, QModelIndex());
            table->setData(index, QVariant(newValues.isTuesday), Qt::EditRole);
        }
        newValues.isWednesday= aDialog.wednesday->isChecked();
        if (newValues.isWednesday!= person.isWednesday) {
            QModelIndex index = table->index(row, TableModel::Columns::WEDNESDAY, QModelIndex());
            table->setData(index, QVariant(newValues.isWednesday), Qt::EditRole);
        }
        newValues.isThursday= aDialog.thursday->isChecked();
        if (newValues.isThursday!= person.isThursday) {
            QModelIndex index = table->index(row, TableModel::Columns::THURSDAY, QModelIndex());
            table->setData(index, QVariant(newValues.isThursday), Qt::EditRole);
        }
        newValues.isFriday= aDialog.friday->isChecked();
        if (newValues.isFriday!= person.isFriday) {
            QModelIndex index = table->index(row, TableModel::Columns::FRIDAY, QModelIndex());
            table->setData(index, QVariant(newValues.isFriday), Qt::EditRole);
        }
        newValues.information = aDialog.informationText->toPlainText();
        if (newValues.information!= person.information) {
            QModelIndex index = table->index(row, TableModel::Columns::INFO, QModelIndex());
            table->setData(index, newValues.information, Qt::EditRole);
        }
        workstations->freeWorkstation(person);
        workstations->setWorkstation(newValues);
    }
}

void NamelistWidget::removeEntry()
{
    QTableView *temp = static_cast<QTableView*>(tableView);
    QSortFilterProxyModel *proxy = static_cast<QSortFilterProxyModel*>(temp->model());
    QItemSelectionModel *selectionModel = temp->selectionModel();

    QModelIndexList indexes = selectionModel->selectedRows();

    foreach (QModelIndex index, indexes) {
        int row = proxy->mapToSource(index).row();
        workstations->freeWorkstation(table->getPeople().at(row));
        table->removeRows(row, 1, QModelIndex());
    }
}

void NamelistWidget::changeShownShifts()
{
    if(shownShifts == Shifts::ALL_SHIFTS) {
        proxyModel->setFilterRegExp(QRegExp(tr(""), Qt::CaseInsensitive));
        proxyModel->setFilterKeyColumn(2);
    }
    else if(shownShifts == Shifts::MORNING_SHIFT) {
        proxyModel->setFilterRegExp(QRegExp(tr("Aamu"), Qt::CaseInsensitive));
    }
    else if(shownShifts == Shifts::DAY_SHIFT) {
        proxyModel->setFilterRegExp(QRegExp(tr("Päivä"), Qt::CaseInsensitive));
    }
    else if(shownShifts == Shifts::EVENING_SHIFT) {
        proxyModel->setFilterRegExp(QRegExp(tr("Ilta"), Qt::CaseInsensitive));
    }
    proxyModel->setFilterKeyColumn(TableModel::Columns::SHIFT);
}

void NamelistWidget::printTable()
{
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOrientation(QPrinter::Landscape);
    printer.setResolution(300);
    QFile pdfFile;
    printer.setOutputFileName(QDir::currentPath().append("/taulukko.pdf"));

    QPainter painter;
    painter.begin(&printer);
    TablePrinter tablePrinter(&painter, &printer);
    QVector<int> columnStretch = QVector<int>() << 4 << 2 << 2 << 1 << 1 << 1 << 1 << 1 << 7;
    if(!tablePrinter.printTable(proxyModel, columnStretch)) {
        qDebug() << tablePrinter.lastError();
    }
    painter.end();
}


void NamelistWidget::readFromFile(const QString &fileName)
{
    QFile file(fileName);
    readingFromFile = true;

    if (!file.open(QIODevice::ReadOnly) && !firstTimeOpening) {
        QMessageBox::information(this, tr("Tiedostoa ei pystytty avaamaan."),
            file.errorString());
        return;
    }

    QList<Person> listOfPeople;
    QDataStream in(&file);
    in >> listOfPeople;

    if (listOfPeople.isEmpty() && !firstTimeOpening) {
        QMessageBox::information(this, tr("Ei henkilöitä tiedostossa."),
                                 tr("Tiedosto jonka yritit avata ei sisällä henkilöitä."));
    } else {
        table->resetTable();
        for (const auto &person: qAsConst(listOfPeople))
            addEntry(person);
    }
    readingFromFile = false;
}

void NamelistWidget::writeToFile(const QString &fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("Tiedostoa ei pystytty avaamaan."), file.errorString());
        return;
    }

    QDataStream out(&file);
    out << table->getPeople();
}
