// MainWindow.h - Cửa sổ chính: chỉ hiển thị và bắt sự kiện, nghiệp vụ nằm ở PriorityQueue / QueuePressureCalculator
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QHash>
#include <QMainWindow>
#include <QString>
#include "model/Patient.h"
#include "util/PriorityQueue.h"
#include "util/Vector.h"

class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QProgressBar;
class QPushButton;
class QTableWidget;
class QTextBrowser;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onAddPatientClicked();
    void onCallNextPatientClicked();
    void onEditPatientClicked();
    void onDeletePatientClicked();
    void onPatientSelectionChanged();
    void onSearchTextChanged();
    void onFilterPriorityChanged();
    void onFinishCurrentClicked();
    void onMinuteTick();
    void onSaveData();
    void onLoadData();
    void onAbout();

private:
    void buildMenuAndToolbar();
    QWidget* buildLeftColumn();
    QWidget* buildRightColumn();

    void refreshQueueTable();
    void updatePressureIndicator();
    void showDetail(const Patient* p);
    void showStatus(const QString& message);

    void buildQueue(PriorityQueue<Patient>& queue) const;
    Vector<Patient> sortedPatients() const; // thứ tự khám: phần tử đầu = ưu tiên cao nhất
    int findIndex(const QString& id) const;
    void removeById(const QString& id);
    QString selectedId() const;
    QStringList allIds() const;
    QString symptomsOf(const Patient& p) const;
    void openEditDialog(const QString& id);

    // Dữ liệu: danh sách chờ (nguồn sự thật) + triệu chứng (Patient chưa có trường này)
    Vector<Patient> patients_;
    QHash<QString, QString> symptoms_;
    int nextIdNumber_;
    int finishedCount_;

    QPushButton* btnAddPatient_;
    QPushButton* btnCallNext_;
    QLineEdit* txtSearch_;
    QComboBox* cmbFilterPriority_;
    QProgressBar* prgQueuePressure_;
    QLabel* lblPressureStatus_;

    QLabel* lblCalledId_;
    QLabel* lblCalledName_;
    QLabel* lblCalledAgeGender_;
    QLabel* lblCalledPriority_;
    QLabel* lblCalledSymptoms_;
    QPushButton* btnFinishCurrent_;

    QLabel* lblDetailId_;
    QLabel* lblDetailName_;
    QLabel* lblDetailAge_;
    QLabel* lblDetailGender_;
    QLabel* lblDetailPriority_;
    QLabel* lblDetailWaitTime_;
    QTextBrowser* txtDetailSymptoms_;
    QPushButton* btnEditPatient_;
    QPushButton* btnDeletePatient_;

    QLabel* lblTotalWaiting_;
    QTableWidget* tblPriorityQueue_;
    QLabel* lblFinishedCount_;
};

#endif // MAIN_WINDOW_H
