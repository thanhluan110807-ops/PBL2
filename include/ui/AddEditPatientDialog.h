// AddEditPatientDialog.h - Hộp thoại dùng chung cho Thêm mới / Chỉnh sửa bệnh nhân
#ifndef ADD_EDIT_PATIENT_DIALOG_H
#define ADD_EDIT_PATIENT_DIALOG_H

#include <QDialog>
#include <QStringList>
#include "model/Patient.h"

class QLineEdit;
class QSpinBox;
class QDoubleSpinBox;
class QComboBox;
class QPlainTextEdit;
class QLabel;

class AddEditPatientDialog : public QDialog {
    Q_OBJECT
public:
    // Chế độ thêm mới: kiểm tra trùng mã với existingIds
    AddEditPatientDialog(const QStringList& existingIds, const QString& suggestedId, QWidget* parent = nullptr);
    // Chế độ chỉnh sửa: nạp sẵn dữ liệu, khóa ô mã
    AddEditPatientDialog(const Patient& patient, const QString& symptoms, QWidget* parent = nullptr);

    Patient patient() const { return result_; }
    QString symptoms() const;

protected:
    void accept() override;

private slots:
    void updateLevelPreview();

private:
    void buildForm();
    Patient buildPatient() const; // có thể ném std::invalid_argument nếu sinh hiệu sai
    void showError(const QString& message, QWidget* focusWidget);

    bool editing_;
    QStringList existingIds_;
    Patient result_;

    QLineEdit* txtPatientId_;
    QLineEdit* txtFullName_;
    QSpinBox* spnAge_;
    QComboBox* cmbGender_;
    QPlainTextEdit* txtSymptoms_;
    QSpinBox* spnHeartRate_;
    QSpinBox* spnSystolicBP_;
    QSpinBox* spnRespRate_;
    QDoubleSpinBox* spnSpO2_;
    QSpinBox* spnGcs_;
    QLabel* lblLevelPreview_;
    QLabel* lblValidationError_;
};

#endif // ADD_EDIT_PATIENT_DIALOG_H
