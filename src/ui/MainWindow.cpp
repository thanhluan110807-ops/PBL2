// MainWindow.cpp
#include "ui/MainWindow.h"
#include "ui/AddEditPatientDialog.h"
#include "ui/uistyle.h"

#include "repository/PatientFileManager.h"
#include "service/QueuePressureCalculator.h"

#include <QAction>
#include <QComboBox>
#include <QDateTime>
#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QStatusBar>
#include <QTableWidget>
#include <QTextBrowser>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>

namespace {
QString q(const std::string& s) { return QString::fromStdString(s); }
QString ageGender(const Patient& p) { return QString("%1 / %2").arg(p.getAge()).arg(q(p.getGender())); }
} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), nextIdNumber_(1), finishedCount_(0) {
    setWindowTitle(" Hệ Thống Quản Lý & Phân Loại Cấp Cứu Bệnh Nhân (Smart Triage)");
    resize(1280, 800);
    setMinimumSize(1024, 680);

    buildMenuAndToolbar();

    auto* central = new QWidget;
    auto* layout = new QHBoxLayout(central);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(16);
    layout->addWidget(buildLeftColumn(), 38);
    layout->addWidget(buildRightColumn(), 62);
    setCentralWidget(central);

    lblFinishedCount_ = new QLabel;
    statusBar()->addPermanentWidget(lblFinishedCount_);

    // Thời gian chờ tăng mỗi phút để áp lực hàng đợi phản ánh thời gian thực
    auto* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::onMinuteTick);
    timer->start(60000);

    refreshQueueTable();
    showStatus("Sẵn sàng");
}

// ---------------------------------------------------------------- Dựng giao diện

void MainWindow::buildMenuAndToolbar() {
    auto* actAdd = new QAction("Thêm mới", this);
    auto* actCall = new QAction("Gọi khám tiếp theo", this);
    auto* actSave = new QAction("Lưu dữ liệu", this);
    auto* actLoad = new QAction("Tải dữ liệu", this);
    auto* actExit = new QAction("Thoát", this);
    auto* actAbout = new QAction("Giới thiệu đồ án PBL2", this);

    menuBar()->addMenu("Tệp")->addActions({actSave, actLoad, actExit});
    menuBar()->addMenu("Bệnh nhân")->addActions({actAdd, actCall});
    menuBar()->addMenu("Trợ giúp")->addAction(actAbout);

    connect(actAdd, &QAction::triggered, this, &MainWindow::onAddPatientClicked);
    connect(actCall, &QAction::triggered, this, &MainWindow::onCallNextPatientClicked);
    connect(actSave, &QAction::triggered, this, &MainWindow::onSaveData);
    connect(actLoad, &QAction::triggered, this, &MainWindow::onLoadData);
    connect(actExit, &QAction::triggered, this, &QWidget::close);
    connect(actAbout, &QAction::triggered, this, &MainWindow::onAbout);

    auto* bar = addToolBar("Thao tác nhanh");
    bar->setMovable(false);

    btnAddPatient_ = new QPushButton("+ Tiếp nhận bệnh nhân");
    btnAddPatient_->setObjectName("btnAddPatient");
    btnCallNext_ = new QPushButton("🔊 Gọi bệnh nhân tiếp theo");
    btnCallNext_->setObjectName("btnCallNext");
    txtSearch_ = new QLineEdit;
    txtSearch_->setPlaceholderText("🔍 Tìm theo mã BN hoặc họ tên...");
    txtSearch_->setClearButtonEnabled(true);
    txtSearch_->setMinimumWidth(260);
    cmbFilterPriority_ = new QComboBox;
    cmbFilterPriority_->addItem("Tất cả mức ưu tiên");
    for (int level = 1; level <= 5; ++level)
        cmbFilterPriority_->addItem("Mức " + UiStyle::levelName(level).remove("Cấp "));

    prgQueuePressure_ = new QProgressBar;
    prgQueuePressure_->setRange(0, 100);
    prgQueuePressure_->setFixedWidth(160);
    lblPressureStatus_ = new QLabel;
    lblPressureStatus_->setMinimumWidth(130);

    auto* spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    bar->addWidget(btnAddPatient_);
    bar->addWidget(btnCallNext_);
    bar->addWidget(txtSearch_);
    bar->addWidget(cmbFilterPriority_);
    bar->addWidget(spacer);
    bar->addWidget(new QLabel("Áp lực hàng đợi:"));
    bar->addWidget(prgQueuePressure_);
    bar->addWidget(lblPressureStatus_);

    connect(btnAddPatient_, &QPushButton::clicked, this, &MainWindow::onAddPatientClicked);
    connect(btnCallNext_, &QPushButton::clicked, this, &MainWindow::onCallNextPatientClicked);
    connect(txtSearch_, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    connect(cmbFilterPriority_, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &MainWindow::onFilterPriorityChanged);
}

QWidget* MainWindow::buildLeftColumn() {
    auto* column = new QWidget;
    column->setMinimumWidth(360);
    column->setMaximumWidth(480);
    auto* layout = new QVBoxLayout(column);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(16);

    // --- Khung đang gọi khám
    auto* grpCalled = new QGroupBox("BỆNH NHÂN ĐANG GỌI KHÁM");
    auto* calledLayout = new QVBoxLayout(grpCalled);
    lblCalledId_ = new QLabel("—");
    lblCalledName_ = new QLabel("Chưa gọi bệnh nhân");
    lblCalledName_->setObjectName("lblCalledName");
    lblCalledName_->setWordWrap(true);
    lblCalledAgeGender_ = new QLabel;
    lblCalledPriority_ = new QLabel;
    lblCalledPriority_->setAlignment(Qt::AlignCenter);
    lblCalledSymptoms_ = new QLabel;
    lblCalledSymptoms_->setWordWrap(true);
    btnFinishCurrent_ = new QPushButton("Hoàn tất ca khám");
    btnFinishCurrent_->setEnabled(false);
    for (QWidget* w : std::initializer_list<QWidget*>{lblCalledId_, lblCalledName_, lblCalledAgeGender_,
                                                      lblCalledPriority_, lblCalledSymptoms_, btnFinishCurrent_})
        calledLayout->addWidget(w);
    connect(btnFinishCurrent_, &QPushButton::clicked, this, &MainWindow::onFinishCurrentClicked);

    // --- Khung chi tiết bệnh nhân đang chọn
    auto* grpDetail = new QGroupBox("CHI TIẾT BỆNH NHÂN ĐANG CHỌN");
    auto* detailLayout = new QVBoxLayout(grpDetail);
    auto* form = new QFormLayout;
    lblDetailId_ = new QLabel;
    lblDetailName_ = new QLabel;
    lblDetailAge_ = new QLabel;
    lblDetailGender_ = new QLabel;
    lblDetailPriority_ = new QLabel;
    lblDetailWaitTime_ = new QLabel;
    txtDetailSymptoms_ = new QTextBrowser;
    form->addRow("Mã BN:", lblDetailId_);
    form->addRow("Họ tên:", lblDetailName_);
    form->addRow("Tuổi:", lblDetailAge_);
    form->addRow("Giới tính:", lblDetailGender_);
    form->addRow("Mức ưu tiên:", lblDetailPriority_);
    form->addRow("Thời gian chờ:", lblDetailWaitTime_);
    form->addRow("Triệu chứng & sinh hiệu:", txtDetailSymptoms_);
    detailLayout->addLayout(form);

    btnEditPatient_ = new QPushButton("✏️ Chỉnh sửa");
    btnDeletePatient_ = new QPushButton("🗑️ Xóa khỏi hàng đợi");
    btnDeletePatient_->setObjectName("btnDeletePatient");
    auto* buttons = new QHBoxLayout;
    buttons->addWidget(btnEditPatient_);
    buttons->addWidget(btnDeletePatient_);
    detailLayout->addLayout(buttons);
    connect(btnEditPatient_, &QPushButton::clicked, this, &MainWindow::onEditPatientClicked);
    connect(btnDeletePatient_, &QPushButton::clicked, this, &MainWindow::onDeletePatientClicked);

    layout->addWidget(grpCalled);
    layout->addWidget(grpDetail, 1);
    return column;
}

QWidget* MainWindow::buildRightColumn() {
    auto* column = new QWidget;
    auto* layout = new QVBoxLayout(column);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* header = new QHBoxLayout;
    auto* lblHeader = new QLabel("DANH SÁCH CHỜ KHÁM THEO ĐỘ ƯU TIÊN");
    lblHeader->setObjectName("lblQueueHeader");
    lblTotalWaiting_ = new QLabel;
    header->addWidget(lblHeader);
    header->addStretch();
    header->addWidget(lblTotalWaiting_);

    tblPriorityQueue_ = new QTableWidget(0, 6);
    tblPriorityQueue_->setHorizontalHeaderLabels(
        {"STT", "Mã BN", "Họ và Tên", "Tuổi / Giới tính", "Mức độ ưu tiên", "Triệu chứng chính"});
    tblPriorityQueue_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tblPriorityQueue_->setSelectionMode(QAbstractItemView::SingleSelection);
    tblPriorityQueue_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tblPriorityQueue_->verticalHeader()->hide();
    tblPriorityQueue_->horizontalHeader()->setStretchLastSection(true);
    tblPriorityQueue_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    tblPriorityQueue_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(tblPriorityQueue_, &QTableWidget::itemSelectionChanged, this, &MainWindow::onPatientSelectionChanged);
    connect(tblPriorityQueue_, &QTableWidget::cellDoubleClicked, this,
            [this](int row, int) { openEditDialog(tblPriorityQueue_->item(row, 1)->text()); });

    layout->addLayout(header);
    layout->addWidget(tblPriorityQueue_);
    return column;
}

// ---------------------------------------------------------------- Truy cập dữ liệu

void MainWindow::buildQueue(PriorityQueue<Patient>& queue) const {
    for (int i = 0; i < patients_.size(); ++i)
        queue.push(patients_[i]);
}

Vector<Patient> MainWindow::sortedPatients() const {
    PriorityQueue<Patient> queue;
    buildQueue(queue);
    Vector<Patient> sorted;
    while (!queue.isEmpty())
        sorted.push_back(queue.pop());
    return sorted;
}

int MainWindow::findIndex(const QString& id) const {
    for (int i = 0; i < patients_.size(); ++i)
        if (q(patients_[i].getId()) == id) return i;
    return -1;
}

void MainWindow::removeById(const QString& id) {
    Vector<Patient> kept;
    for (int i = 0; i < patients_.size(); ++i)
        if (q(patients_[i].getId()) != id) kept.push_back(patients_[i]);
    patients_ = kept;
    symptoms_.remove(id);
}

QString MainWindow::selectedId() const {
    const int row = tblPriorityQueue_->currentRow();
    if (row < 0 || tblPriorityQueue_->selectedItems().isEmpty()) return QString();
    return tblPriorityQueue_->item(row, 1)->text();
}

QStringList MainWindow::allIds() const {
    QStringList ids;
    for (int i = 0; i < patients_.size(); ++i) ids << q(patients_[i].getId());
    return ids;
}

QString MainWindow::symptomsOf(const Patient& p) const {
    return symptoms_.value(q(p.getId()));
}

// ---------------------------------------------------------------- Làm mới giao diện

void MainWindow::refreshQueueTable() {
    const QString keepId = selectedId();
    const QString keyword = txtSearch_->text().trimmed();
    const int levelFilter = cmbFilterPriority_->currentIndex(); // 0 = tất cả

    const Vector<Patient> sorted = sortedPatients();
    tblPriorityQueue_->blockSignals(true);
    tblPriorityQueue_->setRowCount(0);
    for (int i = 0; i < sorted.size(); ++i) {
        const Patient& p = sorted[i];
        const bool matchText = keyword.isEmpty() || q(p.getId()).contains(keyword, Qt::CaseInsensitive) ||
                               q(p.getFullName()).contains(keyword, Qt::CaseInsensitive);
        if (!matchText || (levelFilter > 0 && p.getTriageLevel() != levelFilter)) continue;

        const int row = tblPriorityQueue_->rowCount();
        tblPriorityQueue_->insertRow(row);
        const QStringList cells = {QString::number(i + 1), q(p.getId()), q(p.getFullName()), ageGender(p),
                                   UiStyle::levelName(p.getTriageLevel()), symptomsOf(p)};
        for (int c = 0; c < cells.size(); ++c)
            tblPriorityQueue_->setItem(row, c, new QTableWidgetItem(cells[c]));

        QString bg, fg;
        UiStyle::levelColors(p.getTriageLevel(), bg, fg);
        QTableWidgetItem* levelItem = tblPriorityQueue_->item(row, 4);
        levelItem->setBackground(QColor(bg));
        levelItem->setForeground(QColor(fg));
        if (p.getTriageLevel() <= 3) { // nhóm khẩn cấp in đậm
            QFont f = levelItem->font();
            f.setBold(true);
            levelItem->setFont(f);
        }
        if (cells[1] == keepId) tblPriorityQueue_->selectRow(row);
    }
    tblPriorityQueue_->blockSignals(false);

    lblTotalWaiting_->setText(QString("Đang chờ: %1 bệnh nhân").arg(patients_.size()));
    lblFinishedCount_->setText(QString("Đã khám trong phiên: %1  ").arg(finishedCount_));
    onPatientSelectionChanged();
    updatePressureIndicator();
}

void MainWindow::updatePressureIndicator() {
    PriorityQueue<Patient> queue;
    buildQueue(queue);
    const QueuePressureReport report = QueuePressureCalculator::evaluateQueuePressure(queue);

    int percent = static_cast<int>(report.normalizedPressure);
    percent = percent < 0 ? 0 : (percent > 100 ? 100 : percent);
    prgQueuePressure_->setValue(percent);

    QString color = UiStyle::COLOR_OK, text = "Bình thường", style;
    if (percent >= 80) {
        color = UiStyle::COLOR_DANGER;
        text = "QUÁ TẢI KHẨN CẤP";
        style = "font-weight:700; color:#DC2626;";
    } else if (percent >= 50) {
        color = UiStyle::COLOR_WARN;
        text = "Cảnh báo đông";
        style = "color:#D97706;";
    }
    prgQueuePressure_->setStyleSheet(UiStyle::pressureBarStyle(color));
    lblPressureStatus_->setText(text);
    lblPressureStatus_->setStyleSheet(style);
}

void MainWindow::showDetail(const Patient* p) {
    btnEditPatient_->setEnabled(p != nullptr);
    btnDeletePatient_->setEnabled(p != nullptr);
    if (!p) {
        for (QLabel* l : {lblDetailId_, lblDetailName_, lblDetailAge_, lblDetailGender_, lblDetailWaitTime_})
            l->clear();
        lblDetailPriority_->clear();
        lblDetailPriority_->setStyleSheet("");
        txtDetailSymptoms_->clear();
        return;
    }
    lblDetailId_->setText(q(p->getId()));
    lblDetailName_->setText(q(p->getFullName()));
    lblDetailAge_->setText(QString::number(p->getAge()));
    lblDetailGender_->setText(q(p->getGender()));
    lblDetailPriority_->setText(UiStyle::levelName(p->getTriageLevel()));
    lblDetailPriority_->setStyleSheet(UiStyle::badgeStyle(p->getTriageLevel()));
    lblDetailWaitTime_->setText(QString("%1 phút").arg(p->getWaitingTime()));
    txtDetailSymptoms_->setPlainText(
        QString("%1\n\nHR: %2 BPM | BP: %3 mmHg | RR: %4 lần/phút | SpO2: %5% | GCS: %6")
            .arg(symptomsOf(*p)).arg(p->getHeartRate()).arg(p->getSystolicBP())
            .arg(p->getRespiratoryRate()).arg(p->getSpO2(), 0, 'f', 1).arg(p->getGcsScore()));
}

void MainWindow::showStatus(const QString& message) {
    statusBar()->showMessage(QString("%1  |  Đang chờ: %2  |  Cập nhật lúc %3")
                                 .arg(message).arg(patients_.size())
                                 .arg(QDateTime::currentDateTime().toString("HH:mm:ss")));
}

// ---------------------------------------------------------------- Slots

void MainWindow::onAddPatientClicked() {
    while (findIndex(QString("BN%1").arg(nextIdNumber_, 3, 10, QChar('0'))) >= 0) ++nextIdNumber_;
    AddEditPatientDialog dlg(allIds(), QString("BN%1").arg(nextIdNumber_, 3, 10, QChar('0')), this);
    if (dlg.exec() != QDialog::Accepted) return;

    const Patient p = dlg.patient();
    patients_.push_back(p);
    symptoms_[q(p.getId())] = dlg.symptoms();
    refreshQueueTable();
    showStatus(QString("Đã thêm bệnh nhân %1 vào hàng đợi").arg(q(p.getFullName())));
}

void MainWindow::onCallNextPatientClicked() {
    PriorityQueue<Patient> queue;
    buildQueue(queue);
    if (queue.isEmpty()) {
        QMessageBox::information(this, "Thông báo", "Hiện không có bệnh nhân nào trong hàng đợi chờ khám!");
        return;
    }
    const Patient p = queue.pop();
    const QString id = q(p.getId());

    lblCalledId_->setText("Mã BN: " + id);
    lblCalledName_->setText(q(p.getFullName()));
    lblCalledAgeGender_->setText("Tuổi / Giới tính: " + ageGender(p));
    lblCalledPriority_->setText(UiStyle::levelName(p.getTriageLevel()));
    lblCalledPriority_->setStyleSheet(UiStyle::badgeStyle(p.getTriageLevel()));
    lblCalledSymptoms_->setText("Triệu chứng: " + symptomsOf(p));
    btnFinishCurrent_->setEnabled(true);

    removeById(id);
    refreshQueueTable();
    showStatus(QString("Đã gọi bệnh nhân %1 vào khám").arg(q(p.getFullName())));
}

void MainWindow::onFinishCurrentClicked() {
    lblCalledId_->setText("—");
    lblCalledName_->setText("Chưa gọi bệnh nhân");
    lblCalledAgeGender_->clear();
    lblCalledPriority_->clear();
    lblCalledPriority_->setStyleSheet("");
    lblCalledSymptoms_->clear();
    btnFinishCurrent_->setEnabled(false);
    ++finishedCount_;
    refreshQueueTable();
    showStatus("Đã hoàn tất ca khám");
}

void MainWindow::onEditPatientClicked() {
    const QString id = selectedId();
    if (!id.isEmpty()) openEditDialog(id);
}

void MainWindow::openEditDialog(const QString& id) {
    const int index = findIndex(id);
    if (index < 0) return;
    AddEditPatientDialog dlg(patients_[index], symptoms_.value(id), this);
    if (dlg.exec() != QDialog::Accepted) return;

    patients_[index] = dlg.patient(); // hàng đợi được dựng lại từ patients_ nên tự xếp lại thứ tự
    symptoms_[id] = dlg.symptoms();
    refreshQueueTable();
    showStatus(QString("Đã cập nhật hồ sơ %1").arg(id));
}

void MainWindow::onDeletePatientClicked() {
    const QString id = selectedId();
    const int index = findIndex(id);
    if (index < 0) return;
    const QString name = q(patients_[index].getFullName());

    QMessageBox box(QMessageBox::Question, "Xác nhận xóa",
                    QString("Bạn có chắc chắn muốn xóa bệnh nhân [%1 - %2] khỏi hàng đợi chờ khám không?")
                        .arg(name, id),
                    QMessageBox::NoButton, this);
    QPushButton* yes = box.addButton("Xóa", QMessageBox::AcceptRole);
    box.addButton("Hủy", QMessageBox::RejectRole);
    box.exec();
    if (box.clickedButton() != yes) return;

    removeById(id);
    refreshQueueTable();
    showStatus(QString("Đã xóa bệnh nhân %1 khỏi hàng đợi").arg(name));
}

void MainWindow::onPatientSelectionChanged() {
    const int index = findIndex(selectedId());
    showDetail(index >= 0 ? &patients_[index] : nullptr);
}

void MainWindow::onSearchTextChanged() { refreshQueueTable(); }
void MainWindow::onFilterPriorityChanged() { refreshQueueTable(); }

void MainWindow::onMinuteTick() {
    for (int i = 0; i < patients_.size(); ++i)
        patients_[i].setWaitingTime(patients_[i].getWaitingTime() + 1);
    refreshQueueTable();
}

void MainWindow::onSaveData() {
    const QString path = QFileDialog::getSaveFileName(this, "Lưu dữ liệu", "data/patients.csv", "CSV (*.csv)");
    if (path.isEmpty()) return;
    if (PatientFileManager::savePatientsToFile(path.toStdString(), patients_))
        showStatus("Đã lưu dữ liệu (lưu ý: triệu chứng chưa được ghi vào CSV)");
    else
        QMessageBox::warning(this, "Lỗi", "Không thể ghi file " + path);
}

void MainWindow::onLoadData() {
    const QString path = QFileDialog::getOpenFileName(this, "Tải dữ liệu", "data", "CSV (*.csv)");
    if (path.isEmpty()) return;
    Vector<Patient> loaded;
    if (PatientFileManager::loadPatientsFromFile(path.toStdString(), loaded) == 0) {
        QMessageBox::warning(this, "Lỗi", "Không đọc được bệnh nhân nào từ file " + path);
        return;
    }
    patients_ = loaded;
    symptoms_.clear();
    refreshQueueTable();
    showStatus(QString("Đã tải %1 bệnh nhân").arg(patients_.size()));
}

void MainWindow::onAbout() {
    QMessageBox::about(this, "Giới thiệu",
                       "Smart Hospital Triage - Đồ án PBL2\n"
                       "Phân loại cấp cứu 5 cấp ESI bằng hàng đợi ưu tiên (Max-Heap).");
}
