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

#include "adddialog.h"
#include "tablemodel.h"
#include "namelistwidget.h"

#include <QtWidgets>

AddDialog::AddDialog(NamelistWidget *parent)
    : parent(parent)

{
    numberOfWorkstations = parent->workstations->getNumberOfWorkstations();
    ReservedMorningWorkstations = parent->workstations->getReservedMorningWorkstations();
    ReservedDayWorkstations     = parent->workstations->getReservedDayWorkstations();
    ReservedEveningWorkstations = parent->workstations->getReservedEveningWorkstations();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *topLayout = new QHBoxLayout(this); //Top layout contains left and right side layouts and bottom contains OK and cancel buttons
    QVBoxLayout *leftSideLayout = new QVBoxLayout(this); //Left side of the dialog box contains name and info fields, and day and shift selections
    QFormLayout *nameLayout = new QFormLayout(this); //Name and info fields
    nameLabel = new QLabel(tr("Nimi"), this);
    nameText = new QLineEdit(this);
    nameLayout->addRow(nameLabel, nameText);
    QVBoxLayout *infoLayout = new QVBoxLayout(this);
    informationLabel = new QLabel(tr("Lisätietoa"), this);
    informationText = new QTextEdit(this);
    infoLayout->addWidget(informationLabel);
    infoLayout->addWidget(informationText);

    //Shift radiobuttons
    shift = new QButtonGroup(this);
    morning = new QRadioButton(tr("Aamu"), this);
    day = new QRadioButton(tr("Päivä"), this);
    evening = new QRadioButton(tr("Ilta"), this);
    shift->addButton(morning);
    shift->addButton(day);
    shift->addButton(evening); 
    QVBoxLayout *shiftLayout = new QVBoxLayout(this);
    shiftLayout->addWidget(morning);
    shiftLayout->addWidget(day);
    shiftLayout->addWidget(evening);

    //Days checkboxes
    monday = new QCheckBox(tr("Ma"), this);
    tuesday = new QCheckBox(tr("Ti"), this);
    wednesday = new QCheckBox(tr("Ke"), this);
    thursday = new QCheckBox(tr("To"), this);
    friday = new QCheckBox(tr("Pe"), this);
    QHBoxLayout *daysLayout = new QHBoxLayout(this);
    daysLayout->addWidget(monday);
    daysLayout->addWidget(tuesday);
    daysLayout->addWidget(wednesday);
    daysLayout->addWidget(thursday);
    daysLayout->addWidget(friday);

    QHBoxLayout *shiftAndDaysLayout = new QHBoxLayout(this);
    shiftAndDaysLayout->addLayout(shiftLayout);
    shiftAndDaysLayout->addLayout(daysLayout);

    leftSideLayout->addLayout(nameLayout);
    leftSideLayout->addLayout(shiftAndDaysLayout);
    leftSideLayout->addLayout(infoLayout);

    //Right side contains workstation selection radiobuttons
    //Workstation radiobuttons
    workstationGroupBox = new QGroupBox(tr("Työpisteet"), this);
    workstationButtonGroup = new QButtonGroup(this);
    QGridLayout *radioButtonLayout = new QGridLayout(this);
    int rows = 0;
    int column = 0;
    for(int iii = 1; iii <= numberOfWorkstations; ++iii)
    {
        //Use QButtonGroup.button(int id) to get single button or
        //QButtongroup.buttons() to get list of buttons in namelistWidget
        workstationRadioButton = new QRadioButton(QString::number(iii), this);
        radioButtonLayout->addWidget(workstationRadioButton, rows, column, Qt::AlignLeft);
        workstationButtonGroup->addButton(workstationRadioButton);
        workstationButtonGroup->setId(workstationRadioButton, iii);
        column++;
        if(iii % 5 == 0) {
            rows++;
            column = 0;
        }
    }
    workstationRadioButton = new QRadioButton(tr("Ei työpistettä"), this);
    radioButtonLayout->addWidget(workstationRadioButton, rows, column, Qt::AlignLeft);
    workstationButtonGroup->addButton(workstationRadioButton);
    workstationButtonGroup->setId(workstationRadioButton, 0);
    workstationGroupBox->setLayout(radioButtonLayout);

    topLayout->addLayout(leftSideLayout);
    topLayout->addWidget(workstationGroupBox);

    //Bottom contains OK and cancel buttons
    okButton = new QPushButton(tr("OK"), this);
    cancelButton = new QPushButton(tr("Peruuta"), this);
    QHBoxLayout *bottomLayout = new QHBoxLayout(this);
    bottomLayout->addWidget(okButton);
    bottomLayout->addWidget(cancelButton);

    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(bottomLayout);
    setLayout(mainLayout);

    connect(okButton, &QAbstractButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QAbstractButton::clicked, this, &QDialog::reject);

    connect(morning, &QRadioButton::clicked, this, &AddDialog::updateWorkstationList);
    connect(day, &QRadioButton::clicked, this, &AddDialog::updateWorkstationList);
    connect(evening, &QRadioButton::clicked, this, &AddDialog::updateWorkstationList);

    morning->setChecked(true);
    updateWorkstationList();

    setWindowTitle(tr("Lisää Henkilö"));
}

void AddDialog::updateWorkstationList()
{
    if(morning->isChecked()){
        for(int iii = 1; iii <= numberOfWorkstations; iii++) {
            ReservedWorkstation reservedStations(iii, " ");
            if(ReservedMorningWorkstations.contains(reservedStations)) {
                workstationButtonGroup->button(0)->setChecked(true);
                workstationButtonGroup->button(iii)->setEnabled(false);
                workstationButtonGroup->button(iii)->setText(
                            QString::number(iii).append(" ")
                            += ReservedMorningWorkstations.at(ReservedMorningWorkstations.indexOf(reservedStations)).name);
                if(editedPersonShift == 0) {
                    workstationButtonGroup->button(editedPersonWorkstation)->setEnabled(true);
                    workstationButtonGroup->button(editedPersonWorkstation)->setChecked(true);
                }
            }
            else {
                workstationButtonGroup->button(iii)->setEnabled(true);
                workstationButtonGroup->button(iii)->setText(QString::number(iii));
            }
        }
    }
    if(day->isChecked()) {
        for(int iii = 1; iii <= numberOfWorkstations; iii++) {
            ReservedWorkstation reservedStations(iii, " ");
            if(ReservedDayWorkstations.contains(reservedStations)) {
                workstationButtonGroup->button(0)->setChecked(true);
                workstationButtonGroup->button(iii)->setEnabled(false);
                workstationButtonGroup->button(iii)->setText(
                            QString::number(iii).append(" ")
                            += ReservedDayWorkstations.at(ReservedDayWorkstations.indexOf(reservedStations)).name);
                if(editedPersonShift == 1) {
                    workstationButtonGroup->button(editedPersonWorkstation)->setEnabled(true);
                    workstationButtonGroup->button(editedPersonWorkstation)->setChecked(true);
                }
            }
            else {
                workstationButtonGroup->button(iii)->setEnabled(true);
                workstationButtonGroup->button(iii)->setText(QString::number(iii));
            }
        }
    }
    if(evening->isChecked()) {
        for(int iii = 1; iii <= numberOfWorkstations; iii++) {
            ReservedWorkstation reservedStations(iii, " ");
            if(ReservedEveningWorkstations.contains(reservedStations)) {
                workstationButtonGroup->button(0)->setChecked(true);
                workstationButtonGroup->button(iii)->setEnabled(false);
                workstationButtonGroup->button(iii)->setText(
                            QString::number(iii).append(" ")
                            += ReservedEveningWorkstations.at(ReservedEveningWorkstations.indexOf(reservedStations)).name);
                if(editedPersonShift == 2) {
                    workstationButtonGroup->button(editedPersonWorkstation)->setEnabled(true);
                    workstationButtonGroup->button(editedPersonWorkstation)->setChecked(true);
                }
            }
            else {
                workstationButtonGroup->button(iii)->setEnabled(true);
                workstationButtonGroup->button(iii)->setText(QString::number(iii));
            }
        }
    }
}
