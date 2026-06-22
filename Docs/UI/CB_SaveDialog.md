# 💾 CB_SaveDialog - 세이브/로드 팝업 시스템[CB_AddQeustion.md](CB_AddQeustion.md)

> 퀴즈 데이터를 파일 단위로 저장·불러오기·삭제할 수 있는 팝업형 UMG 위젯입니다.
> `CB_AddQuestion`(문제 생성 메인 화면)에 종속되어 동작하며, 메인 위젯의 임시 데이터와 디스크 상의 `.sav` 파일 간 동기화를 담당합니다.

---

## 📌 1. 개요

- **클래스**: `UCB_SaveDialog` (`UUserWidget` 상속)
- **역할**: 세이브 슬롯 목록 표시, 저장(덮어쓰기 포함), 불러오기, 선택 삭제
- **연동 대상**: `UCB_AddQuestion`(부모 위젯), `UCB_GameInstance`(실제 파일 입출력 담당)


## 🚀 2. 주요 기능

### 🔹 1. 부모 위젯 참조 주입 (Owning Widget Injection)]

팝업이 독립적으로 떠 있는 동안에도 메인 화면(`CB_AddQuestion`)의 데이터에 접근하기 위해, `GetOuter()` 같은 암묵적 탐색 대신 명시적으로 부모 주소를 주입받는 방식을 사용합니다.

**구현 내용**
- `SetOwningWidget(UCB_AddQuestion* InParent)` 함수로 메인 위젯이 팝업을 생성할 때 자기 자신의 주소를 직접 전달.
- `OwningAddQuestionWidget` 멤버에 보관하여 저장/불러오기 시 메인 화면의 `TemporaryQuestionList`에 직접 접근.

**해결한 문제**
`GetOuter()`로 부모를 추론하는 방식은 위젯 계층 구조가 바뀌면 쉽게 깨집니다. 주소를 명시적으로 주입받는 구조로 변경해 의존 관계를 코드로 명확히 드러내고 안정성을 확보했습니다.

### 🔹 2. 세이브 슬롯 목록 조회 및 갱신
디스크의 세이브 폴더를 스캔하여 존재하는 `.sav` 파일 목록을 리스트뷰에 표시합니다.

**구현 내용**
- `FPaths::ProjectSavedDir() + "SaveGames/"` 경로를 `FPaths::ConvertRelativePathToFull()`로 절대 경로화.
- 폴더가 없을 경우 `IFileManager::MakeDirectory()`로 자동 생성.
- `IFileManager::FindFiles()`로 `*.sav` 패턴의 파일을 검색, `FPaths::GetBaseFilename()`으로 확장자를 제거한 슬롯 이름만 추출.
- 검색된 각 파일을 `UCB_SaveSlotData` 가상 데이터 오브젝트로 래핑하여 `SaveFileList`(`UListView`)에 추가.
- 위젯 생성(`NativeConstruct`) 시점과 파일 삭제 후 시점에 `RefreshSaveFileList()`를 호출해 항상 최신 상태 유지.

### 🔹 3. 저장 및 덮어쓰기
입력된 파일명으로 현재 메인 화면의 문제 목록을 저장합니다.

**구현 내용**
- 리스트뷰의 슬롯 항목을 클릭하면 `OnSlotItemClicked()`가 해당 슬롯 이름을 `NewFileNameInput`에 자동으로 채워, 같은 이름으로 저장 시 자연스럽게 덮어쓰기로 이어지는 흐름.
- `OnConfirmClicked()`에서 `UCB_GameInstance::CurrentSaveSlotName`을 입력값으로 설정하고, `OwningAddQuestionWidget->TemporaryQuestionList`를 `GameInstance::QuizMasterList`에 복사한 뒤 `SaveQuizDataToFile()` 호출.
- 입력 텍스트가 비어 있으면 저장을 진행하지 않는 방어 처리.

### 🔹 4. 불러오기
선택한(또는 입력한) 슬롯의 데이터를 읽어와 메인 화면에 반영합니다.

**구현 내용**
- 리스트뷰 항목 중 체크박스가 켜진(`bIsSelected == true`) 슬롯을 우선 탐색.
- 체크된 항목이 없을 경우, 텍스트박스에 입력된 이름을 대안으로 사용하는 폴백(fallback) 처리.
- 선택된 슬롯 이름이 끝까지 없으면 경고 로그를 남기고 함수 조기 종료.
- `GameInstance::CurrentSaveSlotName`을 설정한 뒤 `LoadQuizDataFromFile()`로 파일에서 데이터를 읽고, 결과를 `OwningAddQuestionWidget->TemporaryQuestionList`에 주입 후 `RefreshListView()`로 메인 화면 리스트뷰 갱신.

### 🔹 5. 다중 세이브 파일 삭제
체크박스로 선택된 세이브 파일들을 디스크에서 직접 삭제합니다.

**구현 내용**
- 리스트뷰의 모든 항목을 순회하며 `bIsSelected`가 true인 항목의 파일 경로를 절대 경로로 조립.
- `IFileManager::FileExists()`로 실제 파일 존재 여부를 먼저 확인한 뒤 `Delete()` 호출 — 존재하지 않는 파일에 대한 불필요한 삭제 시도 방지.
- 삭제 성공/실패 각각을 로그로 구분 기록(권한 문제 등 실패 원인 추적 용이).
- 하나라도 삭제에 성공한 경우에만 `RefreshSaveFileList()`로 목록을 갱신해 불필요한 재조회를 줄임.


---

```