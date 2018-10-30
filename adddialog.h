//Dialog box for adding and editing persons information.

#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>
#include <QVector>
#include <QGroupBox>

#include "workstations.h"

class QLabel;
class QPushButton;
class QTextEdit;
class QLineEdit;
class QCheckBox;
class QRadioButton;
class QComboBox;
class NamelistWidget;

class AddDialog : public QDialog
{
    Q_OBJECT

public:
    AddDialog(NamelistWidget *parent = nullptr);

    QLineEdit *nameText;
    QTextEdit *informationText;
    QButtonGroup *shift;
    QRadioButton *morning;
    QRadioButton *day;
    QRadioButton *evening;
    QCheckBox *monday;
    QCheckBox *tuesday;
    QCheckBox *wednesday;
    QCheckBox *thursday;
    QCheckBox *friday;
    QGroupBox *workstationGroupBox;
    QButtonGroup *workstationButtonGroup;
    QRadioButton *workstationRadioButton;

    //Track workstation and shift of the edited person so that we can enable that button after updating workstation list.
    int editedPersonWorkstation = 0;
    int editedPersonShift = -1;

private:
    NamelistWidget *parent;
    QLabel *nameLabel;
    QLabel *informationLabel;
    QLabel *workstationLabel;
    QLabel *daysLabel;
    QPushButton *okButton;
    QPushButton *cancelButton;

    QVector<ReservedWorkstation> ReservedMorningWorkstations;
    QVector<ReservedWorkstation> ReservedDayWorkstations;
    QVector<ReservedWorkstation> ReservedEveningWorkstations;
    int numberOfWorkstations;

public slots:
    void updateWorkstationList();
};

#endif // ADDDIALOG_H
