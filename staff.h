#ifndef Staff_h
#define Staff_h

#include <string>

enum class Capbac{
    Dieu_duong_tre,
    Dieu_duong_chinh,
    Bac_si_noi_tru,
    Bac_si_chinh,
    Truong_khoa,
};

std :: string Capbactostring(Capbac Capbac);
Capbac stringtoCapbac(const std::string& str);

//Ca trực mà nhân viên muốn đăng ký

struct Catrucmongmuon{
    int Giobatdau;
    int Gioketthuc;
    bool Truccuoituan;

    Catrucmongmuon(
        int Giobatdau=7,
        int Gioketthuc=15,
        bool Cuoituan= false
    );
};

//class cơ sở 
//Lưu thuộc tính chung + Chỉ số chống burn out 
//Fatigue_Score ( chỉ số mệt mỏi tích lủy) + Weeklyhours 

class Nhanvienyte {
    protected:
    std :: string Manhanvien; //Khóa chính - dùng làm key cho hashtable sau này 
    std :: string Hoten;
    Capbac capbac;
    int Sonamkinhnghiem;
    Catrucmongmuon catrucmongmuon;

    //Chỉ số phục vụ Fatigue_Aware Backtracking

    double FatigueScore; // Chỉ số mệt mỏi tích lũy
    double WeeklyHours; // Số giờ làm việc trong tuạn hiện tại
    int Socatrucdemthang; //Dùng để cân bằng giữa các nhân viên cùng cấp bậc
    int Socatruccuoituanthang; 
    bool Vuatrucdem;  //True nếu ca trục trước là trục đếm -> Phải nghỉ 24h

    public:
    //Quy tác số 5
    Nhanvienyte (const std :: string maNV,
                const std :: string Ten,
                Capbac capbacInit,
                int Namkinhnghiem,
                const Catrucmongmuon& catruc = Catrucmongmuon());
    Nhanvienyte(const Nhanvienyte& other); // Copy COnstrctor (Hàm dụng sao chép)
    Nhanvienyte& operator=(const Nhanvienyte& other); //Copy assignment
    Nhanvienyte(Nhanvienyte&& other) noexcept;//Move constructor (Hàm di chuyển)
    Nhanvienyte& operator=(Nhanvienyte&& other) noexcept;// Move assignment (pháp gắn di chuyển)
    virtual ~ Nhanvienyte();//Destructor (Virtual bắt buộc vì có lớp con)

    //Đóng gói 
     
    std :: string Laymanhanvien() const;
    std :: string LayHoten() const ;
    void Dathoten (const std:: string& Ten);
    
    Capbac LayCapbac() const ;
    void Datcapbac(Capbac capbacmoi) ;
    
    int Sonamkinhnghiem() const ;
    void Datsonamkinhnghiem(int Sonam) ;

    Catrucmongmuon Laycatrucmongmuon() const ;
    void DatCatrucmongmuon(const Catrucmongmuon& ca);

    double LayFatigueScore() const;
    double LayWeeklyHours() const;
    int LaySocatrucdemthang() const ;
    int LaySocatruccuoituanthang() const;
    bool Davuatrucdem() const ;

    //Nghiệp vụ chung 

    void CongDonGioLam(double Sogio) ; //Cập nhật weeklyHours
    void Ghinhancatruc(bool lacadem,bool lacuoituan);//cập nhật bộ đếm + co Vuatrucdem
    
    void Datlaituanmoi();//Đặt lại weeklyhours mỗi đầu tuần
    void Cannghinghoibatbuoc() const ;// Ràng buộc cấm xếp ca sau ca đêm
    bool Vuotnguongmetmoi(double nguongtoida) const ;//Điều kiện (pruning) cho thuật toán backtracking-Quay Lui

    //Hàm ảo ( Tính đa hình - Polymorphysm)
    // Bắt buộc hàm con (Bác sĩ / Điều dưỡng ) phải tự định nghĩa riêng 

    virtual bool Cothelamtruongca() const=0 ;
    virtual void CapnhatFatigueScore(double Hesoapluc) =0;
    virtual std :: string Layloainhanvien() const =0;

    virtual void Hienthithongtin() const;// in thông tin ra console ( có thể override mở rộng)
    virtual std :: string Xuatdongfile() const ;//Serialize 1 dong de ghi ra file phang (.csv/.txt)
    
    //Class Bác Sĩ (Ke thua Nhan Viên Y tế )
};

    class  Bacsi : public Nhanvienyte {
    private:
    
    std :: string ChuyenKhoa;//VD:Noitongquang,Tym,Hoisuccapcuu
    bool Labacsinoitru; // True-> Cần giám sát chặt chẽ 
    
    public: 

    Bacsi(const std :: string& maNV,
          const std :: string& Ten,
          Capbac capbacInit,
          int Namkinhnghiem,
          const std :: string& ChuyenKhoaInit,
          bool Noitru=false,
          const Catrucmongmuon& catruc = Catrucmongmuon());

    // Quy tác số 5
     
    Bacsi (const Bacsi& other);
    Bacsi& operator=(const Bacsi& other);
    Bacsi (Bacsi&& other) noexcept;
    Bacsi& operator=(Bacsi&& other) noexcept;
    ~Bacsi() override;

    std :: string Laychuyenkhoa() const ;
    void Datchuyenkhoa(const std:: string& ck);
    bool Labacsinoitru() const ;

    //Override của hàm ảo của lớp cơ sở 
    
    bool Cothelamtruongca() const override;
    void CapnhatFatigueScore(double Hesoapluc) override;
    std :: string Layloainhanvien() const override;
    void Hienthithongtin() const override;
    std :: string Xuatdongfile() const override;
    };
    //class Điều dưỡng 
    class Dieuduong : public Nhanvienyte
    {
      private:
      bool Dangtrucsangloc;//Đang trưc tại quầy sàng loc
      double Sogiotrucsangloclientuc;//Phục vụ cho xoay tua trục sang lọc mỗi 4 tiếng

      public:
      Dieuduong(const std :: string& maNV,
                const std :: string& Ten,
                Capbac capbacInit,
                int Namkinhnghiem,
                const Catrucmongmuon& catruc = Catrucmongmuon());
    
    //Quy tắc số 5 

    Dieuduong (const Dieuduong& other);
    Dieuduong& operator=(const Dieuduong& other);
    Dieuduong(Dieuduong&& other) noexcept;
    Dieuduong& operator=(Dieuduong&& other) noexcept;
    ~Dieuduong() override;

    bool Dangtrucsangloc() const ;
    void Batdautrucsangloc();
    void Ketthuctrucsangloc();
    void Congdongiotrucsanglong(double sogio);
    double Laysogiotrucsangloclentuc() const;
    bool Canxoaytuasangloc() const ;//True khi đã trực sàng lọc >= 4h

    //override Hàm ảo của lớp cơ sở 

    bool Cothelamtruongca() const override ;// Theo đề tài Điều đưỡng không làm trưởng ca 
    void CapnhatFatigueScore(double Hesoapluc)  override ;
    void Hienthithongtin() const override ;
    std :: string Xuatdongfile() const override;
      };
#endif //
