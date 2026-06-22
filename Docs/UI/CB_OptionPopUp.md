# 🖥️ CB_OptionPopUp - 게임 환경 설정 팝업 시스템

> 게임 진행 중 또는 대기실에서 목표 빙고 줄 수와 각 팀(A/B)의 상징 컬러를 동적으로 변경할 수 있는 설정 창 UMG 위젯입니다.
> 언리얼 네이티브 슬레이트 컬러 피커(SColorPicker) 연동 및 글로벌 게임 인스턴스와의 실시간 데이터 동기화를 담당합니다.

---

## 📌 1. 개요

- **클래스**: `UCB_OptionPopUp` (`UUserWidget` 상속)
- **역할**: 목표 빙고 수 제어, 슬레이트 컬러 피커 호출 및 팀 색상 바인딩, 설정값 글로벌 동기화
- **연동 대상**: `UCB_GameInstance`(글ローバル 세팅 저장소), `SColorPicker`(네이티브 색상 선택 창)

---

## 🚀 2. 주요 기능

### 🔹 1. 글로벌 세팅 기반 초기화 및 데이터 연동 (Initialization)
위젯이 생성되는 시점에 글로벌 게임 인스턴스(`UCB_GameInstance`)를 참조하여, 이전에 설정되어 있던 규칙 데이터를 UI 컴포넌트에 실시간 동기화합니다.

**구현 내용**
- `Combo_TargetBingoCount` 콤보박스의 기존 옵션을 비우고 `1`부터 `5`까지의 목표 빙고 줄 수 항목을 동적 장전합니다.
- `GI->GetTargetBingo()`를 통해 현재 적용 중인 목표 빙고 값을 정수형태로 호출한 뒤, `FString::FromInt()`로 변환하여 콤보박스의 최초 선택 옵션으로 자동 매칭시킵니다.

### 🔹 2. 네이티브 슬레이트 컬러 피커 연동 (SColorPicker Integration)
언리얼 엔진 내부 네이티브 프레임워크인 슬레이트(Slate)의 `SColorPicker`를 원격 기동하여 마우스 드래그로 팀 컬러를 자유롭게 변경할 수 있는 고급 색상 패널을 구동합니다.

**구현 내용**
- `FColorPickerArgs` 구조체를 생성하고 초기 색상값(`InitialColor`)으로 현재 설정된 임시 컬러를 지정합니다.
- 색상이 변경될 때마다 실시간으로 반응할 수 있도록 델리게이트 매크로인 `FOnLinearColorValueChanged::CreateUObject`를 사용하여 내부 콜백 함수(`OnColorAChanged` / `OnColorBChanged`)를 이벤트를 바인딩합니다.
- `OpenColorPicker(PickerArgs)` 함수를 기동하여 화면에 언리얼 엔진 순수 네이티브 색상 선택 팝업을 띄웁니다.

### 🔹 3. 실시간 비주얼 피드백 및 런타임 색상 변경 (Real-time Color Update)
사용자가 컬러 피커 패널에서 마우스를 움직여 색상을 선택하는 도중에도 인게임에 실시간으로 반영되도록 런타임 갱신 처리를 수행합니다.

**구현 내용**
- 컬러 피커에서 마우스 커서가 움직일 때마다 바인딩된 `OnColorXChanged` 콜백 함수가 즉각 트리거됩니다.
- 전달받은 `NewColor` 정보를 임시 캐싱 변수(`CurrentSelectedColorA/B`)에 대입함과 동시에, 설정창 내부의 버튼 배경색(`SetBackgroundColor(NewColor)`)을 해당 색상으로 강제 리프레시하여 직관적인 UI 피드백을 제공합니다.
- 내부적으로 `GI->SetTeamColors2()`를 호출하여 임시 변경 중인 색상값을 게임 인스턴스 레이어에도 실시간 전달합니다.

### 🔹 4. 안전한 데이터 파싱 및 원자적 설정 반영 (Apply & Close)
사용자가 임의로 조작하던 임시 설정값들을 '적용 후 닫기' 버튼을 누르는 순간 검증하여, 글로벌 데이터 저장소에 최종 승인 및 덮어쓰기를 수행합니다.

**구현 내용**
- `OnApplyAndCloseClicked()`가 호출되면 먼저 콤보박스에서 선택된 문자열 플래그를 추출합니다.
- **방어적 데이터 파싱**: 콤보박스의 텍스트가 숫자가 맞는지 `SelectedStr.IsNumeric()`으로 사전 검증한 뒤, 안전할 때만 문자열을 정수형으로 캐스팅(`FCString::Atoi`)하며 예외 발생 시 기본값 `3`으로 복원 처리합니다.
- 안전하게 가공된 데이터들을 `GI->SetTargetBingoCount()` 및 `GI->SetTeamColors()`에 최종 주입하여 세팅을 마스터 확정 짓고, `RemoveFromParent()`를 통해 자기 자신 위젯을 화면 레이어에서 깔끔하게 소멸시킵니다.