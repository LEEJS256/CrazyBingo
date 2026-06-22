# 🎮 크레이지 빙고 (Crazy Bingo) - 호스트 UI 시스템
> 대규모 참가자(20~30명) 대상 실시간 레크리에이션 진행을 위한 전용 퀴즈 게임입니다.

---

## 📌 1. 프로젝트 소개
- **개발 기간**: 2026.05 ~ 2026.06 (진행 중)
- **개발 인원**: 1명 
- **주요 목적**: 
  - 4가지 타입(단답형,초성퀴즈,유튜브 노래듣기,객관식)의 문제 편집할 수 있는 편집화면 존재
  - 진행자는 전용화면을 이용해서 어떤팀의 승리인지를 결정하거나 문제를 수정할 수 있음.  

---

## 🛠 2. 기술 스택 (Tech Stack)
- **Engine**: Unreal Engine 5 (UE 5.6)
- **Language**: C++, Blueprint
- **Framework**: UMG (Unreal Motion Graphics), `UListView` Architecture
- **IDE**: JetBrains Rider

---

## 🚀 3. 주요 기능 (Key Features)

### 🔹 데이터-드리븐 기반 퀴즈 생성 및 입력 폼
- 구조체(`FCB_DataTable_Question`) 및 데이터 테이블 구조를 확장하여 객관식, 초성, 유튜브 링크, 단답형 등 다양한 퀴즈 타입 분기 처리.
- SpinBox 및 Editable TextBox 스타일 커스텀을 통해 수치 및 텍스트 입력 시 시인성 극대화.

### 🔹 UListView 기반 실시간 문제 목록 관리 (테이블 UI)
- 대량의 문제 목록을 메모리 효율적으로 렌더링하기 위해 `IUserObjectListEntry` 인터페이스를 구현한 최적화된 리스트뷰 적용.
- 기존 문자열 포맷팅 방식을 탈피하여 문제 유형, 카테고리, 점수를 전용 컬럼 형태로 깔끔하게 분할 매핑.

### 🔹 명시적 체크박스를 활용한 일괄 삭제(Multi-Delete) 시스템
- 행(Row)별 체크박스 상태를 리스트뷰 전용 가상 데이터 오브젝트(`UCB_QuestionListData`)와 실시간 동기화.
- 다중 선택 후 일괄 삭제 시 데이터 무결성을 보장하는 안전한 역순 순회 삭제 로직 구현.

---

## 📂 4. 프로젝트 구조 & 데이터 흐름
```text
Source/CrazyBingo/
├── Public/
│   ├── UI/
│   │   ├── CB_AddQuestion.h        # 퀴즈 생성 및 목록 관리 메인 UI
│   │   └── CB_QuestionListRow.h     # 리스트뷰에 들어가는 개별 행 위젯 (체크박스, 텍스트 제어)
│   └── Data/
│       ├── CB_QuestionListData.h    # 리스트뷰 전용 래퍼 가상 데이터 오브젝트
│       └── CB_DataTable_Question.h  # 퀴즈 원본 데이터 C++ 구조체 정의
