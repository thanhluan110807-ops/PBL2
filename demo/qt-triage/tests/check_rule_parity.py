"""So sánh trực tiếp luật C++ demo với hàm C++ gốc patient-class; không dùng JavaScript."""
from pathlib import Path
import random
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[3]
SHA = '95e145f9e95b1f3eb59df6810c5d2341dfe84dc3'
SOURCE = subprocess.check_output(['git', 'show', SHA+':src/model/Patient.cpp'], cwd=ROOT, text=True)
START = SOURCE.index('void Patient::evaluateTriageLevel() {')
END = SOURCE.index('\n// =========================================================', START)
FUNCTION = SOURCE[START:END]
# Chỉ thay kiểu ngoại lệ cũ, giữ nguyên mọi điều kiện/nhánh của nguồn gốc.
FUNCTION = re.sub(r'throw std::domain_error\([^;]+;', 'throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Tuoi khong hop le");', FUNCTION)
PREAMBLE = '''#include <iostream>
#include "model/DemoTriageService.h"
class Patient { public:
int age,heartRate,systolicBP,respiratoryRate,gcsScore,triageLevel;
double spO2;
void evaluateTriageLevel();
};
'''
MAIN = '''
int main(){Patient p;int So=0;while(std::cin>>p.age>>p.heartRate>>p.systolicBP>>p.respiratoryRate>>p.spO2>>p.gcsScore){
p.evaluateTriageLevel();
const Demo::SinhHieu H{p.heartRate,p.systolicBP,p.respiratoryRate,p.gcsScore,p.spO2};
if(Demo::DemoTriageService::DeXuatMuc(H,p.age).Muc!=p.triageLevel){std::cerr<<"Mismatch "<<So;return 1;}++So;}
std::cout<<"PASS: "<<So<<" native C++ cases match patient-class @95e145f\\n";
}
'''
CASES = []
for AGE in [0,1,5,6,12,13,64,65,150]:
    BASE = [AGE,80,120,18,98,15]
    for I,VALUES in [(1,[0,39,40,44,45,49,50,54,55,59,60,79,80,89,90,91,100,101,110,111,120,121,130,131,140,141,150,151,160,161,170,171,180,181,200,201,300]),(2,[0,59,60,69,70,74,75,79,80,84,85,89,90,99,100,140,141,170,171,180,181,300]),(3,[0,9,10,11,12,13,14,17,18,19,20,21,24,25,28,29,30,31,34,35,36,40,41,50,51,55,56,70,71,120]),(4,[0,89.9,90,94.9,95,96.9,97,98,100]),(5,[3,8,9,12,13,15])]:
        for VALUE in VALUES:
            ROW=BASE.copy();ROW[I]=VALUE;CASES.append(ROW)
RANDOM = random.Random(31)
for _ in range(3000): CASES.append([RANDOM.choice([0,1,3,5,6,12,13,25,64,65,85,150]),RANDOM.randrange(301),RANDOM.randrange(301),RANDOM.randrange(121),RANDOM.uniform(85,100),RANDOM.randrange(3,16)])
with tempfile.TemporaryDirectory(prefix='pbl2-native-parity-') as TEMP:
    TEMP=Path(TEMP);CPP=TEMP/'oracle.cpp';CPP.write_text(PREAMBLE+FUNCTION+MAIN)
    subprocess.run(['g++','-std=c++17','-I',str(ROOT/'include'),'-I',str(ROOT/'demo/qt-triage'),str(CPP),str(ROOT/'demo/qt-triage/model/DemoTriageService.cpp'),'-o',str(TEMP/'oracle')],check=True)
    subprocess.run([str(TEMP/'oracle')],input='\n'.join(' '.join(map(str,ROW)) for ROW in CASES),text=True,check=True)
