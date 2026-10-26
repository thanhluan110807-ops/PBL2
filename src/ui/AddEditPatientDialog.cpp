// AddEditPatientDialog.cpp
#include "ui/AddEditPatientDialog.h"
#include "ui/uistyle.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <stdexcept>

AddEditPatientDialog::AddEditPatientDialog(const QStringList& existingIds, const QString& suggestedId, QWidget* parent)
    : QDialog(parent), editing_(false), existingIds_(existingIds) {
    buildForm();
    setWindowTitle("Tiếp nhận bệnh nhân mới");
    txtPatientId_->setText(suggestedId);
    updateLevelPreview();
}

AddEditPatientDialog::AddEditPatientDialog(const Patient& patient, const QString& symptoms, QWidget* parent)
    : QDialog(parent), editing_(true), result_(patient) {
    buildForm();
    setWindowTitle("Chỉnh sửa hồ sơ bệnh nhân");
    txtPatientId_->setText(QString::fromStdString(patient.getId()));
    txtPatientId_->setReadOnly(true);
    txtFullName_->setText(QString::fromStdString(patient.getFullName()));
    spnAge_->setValue(patient.getAge());
    cmbGender_->setCurrentText(QString::fromStdString(patient.getGender()));
    txtSymptoms_->setPlainText(symptoms);
    spnHeartRate_->setValue(patient.getHeartRate());
    spnSystolicBP_->setValue(patient.getSystolicBP());
    spnRespRate_->setValue(patient.getRespiratoryRate());
    spnSpO2_->setValue(patient.getSpO2());
    spnGcs_->setValue(patient.getGcsScore());
    updateLevelPreview();
}

void AddEditPatientDialog::buildForm() {
    setModal(true);
    setMinimumWidth(460);

    txtPatientId_ = new QLineEdit;
    txtFullName_ = new QLineEdit;
    spnAge_ = new QSpinBox;
    spnAge_->setRange(0, 150);
    spnAge_->setValue(25);
    cmbGender_ = new QComboBox;
    cmbGender_->addItems({"Nam", "Nữ", "Khác"});
    txtSymptoms_ = new QPlainTextEdit;
    txtSymptoms_->setFixedHeight(80);

    spnHeartRate_ = new QSpinBox;  spnHeartRate_->setRange(0, 300);  spnHeartRate_->setValue(75);
    spnSystolicBP_ = new QSpinBox; spnSystolicBP_->setRange(0, 300); spnSystolicBP_->setValue(120);
    spnRespRate_ = new QSpinBox;   spnRespRate_->setRange(0, 120);   spnRespRate_->setValue(16);
    spnSpO2_ = new QDoubleSpinBox; spnSpO2_->setRange(0.0, 100.0);   spnSpO2_->setDecimals(1); spnSpO2_->setValue(98.0);
    spnGcs_ = new QSpinBox;        spnGcs_->setRange(3, 15);         spnGcs_->setValue(15);

    lblLevelPreview_ = new QLabel;
    lblValidationError_ = new QLabel;
    lblValidationError_->setObjectName("lblValidationError");
    lblValidationError_->setWordWrap(true);

    auto* form = new QFormLayout;
    form->addRow("Mã bệnh nhân:", txtPatientId_);
    form->addRow("Họ và tên:", txtFullName_);
    form->addRow("Tuổi:", spnAge_);
    form->addRow("Giới tính:", cmbGender_);
    form->addRow("Triệu chứng:", txtSymptoms_);
    form->addRow("Nhịp tim (BPM):", spnHeartRate_);
    form->addRow("Huyết áp tâm thu (mmHg):", spnSystolicBP_);
    form->addRow("Nhịp thở (lần/phút):", spnRespRate_);
    form->addRow("SpO2 (%):", spnSpO2_);
    form->addRow("GCS (3-15):", spnGcs_);
    form->addRow("Mức ưu tiên (tự tính):", lblLevelPreview_);

    auto* btnSave = new QPushButton("Lưu hồ sơ");
    btnSave->setObjectName("btnSave");
    btnSave->setDefault(true);
    auto* btnCancel = new QPushButton("Hủy");
    auto* buttons = new QHBoxLayout;
    buttons->addStretch();
    buttons->addWidget(btnCancel);
    buttons->addWidget(btnSave);

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->addLayout(form);
    root->addWidget(lblValidationError_);
    root->addLayout(buttons);

    connect(btnSave, &QPushButton::clicked, this, &AddEditPatientDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    // Mức ưu tiên do Patient::evaluateTriageLevel() tính từ tuổi + sinh hiệu, nên cập nhật khi các ô này đổi
    for (QSpinBox* s : {spnAge_, spnHeartRate_, spnSystolicBP_, spnRespRate_, spnGcs_})
        connect(s, QOverload<int>::of(&QSpinBox::valueChanged), this, &AddEditPatientDialog::updateLevelPreview);
    connect(spnSpO2_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &AddEditPatientDialog::updateLevelPreview);
}

// Đổ dữ liệu form vào bệnh nhân p, ném std::invalid_argument nếu sinh hiệu sai
static void applyForm(Patient& p, const QString& id, const QString& name, int age, const QString& gender,
                      int hr, int bp, int rr, double spo2, int gcs) {
    p.setId(id.toStdString());
    p.setFullName(name.toStdString());
    p.setAge(age);
    p.setGender(gender.toStdString());
    p.updateVitalSigns(hr, bp, rr, spo2, gcs);
}

Patient AddEditPatientDialog::buildPatient() const {
    // Chế độ thêm: tạo Patient mới (để có STT mới). Chế độ sửa: giữ nguyên STT cũ
    Patient p = editing_ ? result_ : Patient(txtPatientId_->text().trimmed().toStdString(),
                                             txtFullName_->text().trimmed().toStdString(),
                                             spnAge_->value(), cmbGender_->currentText().toStdString());
    applyForm(p, txtPatientId_->text().trimmed(), txtFullName_->text().trimmed(), spnAge_->value(),
              cmbGender_->currentText(), spnHeartRate_->value(), spnSystolicBP_->value(),
              spnRespRate_->value(), spnSpO2_->value(), spnGcs_->value());
    return p;
}

void AddEditPatientDialog::updateLevelPreview() {
    // Dùng Patient mặc định (không tăng bộ đếm STT) chỉ để xem trước mức ưu tiên
    Patient tmp;
    try {
        applyForm(tmp, "", "", spnAge_->value(), "", spnHeartRate_->value(), spnSystolicBP_->value(),
                  spnRespRate_->value(), spnSpO2_->value(), spnGcs_->value());
        lblLevelPreview_->setText(UiStyle::levelName(tmp.getTriageLevel()));
        lblLevelPreview_->setStyleSheet(UiStyle::badgeStyle(tmp.getTriageLevel()));
    } catch (const std::exception&) {
        lblLevelPreview_->setText("Không xác định");
    }
}

QString AddEditPatientDialog::symptoms() const {
    return txtSymptoms_->toPlainText().trimmed();
}

void AddEditPatientDialog::showError(const QString& message, QWidget* focusWidget) {
    lblValidationError_->setText(message);
    focusWidget->setFocus();
}

void AddEditPatientDialog::accept() {
    const QString id = txtPatientId_->text().trimmed();
    if (id.isEmpty())
        return showError("Mã bệnh nhân không được để trống.", txtPatientId_);
    if (!editing_ && existingIds_.contains(id))
        return showError("Mã bệnh nhân đã tồn tại.", txtPatientId_);
    if (txtFullName_->text().trimmed().isEmpty())
        return showError("Họ và tên không được để trống.", txtFullName_);
    if (symptoms().isEmpty())
        return showError("Triệu chứng không được để trống.", txtSymptoms_);

    try {
        result_ = buildPatient();
    } catch (const std::exception& e) {
        return showError(QString::fromUtf8(e.what()), spnHeartRate_);
    }
    QDialog::accept();
}
