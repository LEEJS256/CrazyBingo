# UCB_GameInstance

## 📌 개요
`UCB_GameInstance`는 Crazy Bingo 프로젝트의 전반적인 게임 상태, 팀 점수, 팀 컬러 설정 및 퀴즈 데이터를 총괄 관리하는 싱글톤 스타일의 핵심 매니저 클래스입니다. 게임의 라이프사이클 동안 데이터가 유지되며, 로컬 세이브/로드 시스템(`UCB_QuizSaveGame`)을 포함하고 있습니다.

---

## 🎯 주요 역할
* **팀 데이터 관리**: 각 팀(A팀, B팀)의 실시간 점수(Score) 가산 및 조회, 팀 고유 컬러 설정을 관리합니다.
* **퀴즈 데이터 풀(Pool) 운영**: 외부 `UDataTable`로부터 문제 목록을 로드하거나, 런타임에 문제를 추가하고 섞는(Shuffle) 등의 관리 기능을 제공합니다.
* **세이브/로드 시스템**: 퀴즈 마스터 리스트 데이터를 로컬 파일 시스템(`.sav`)에 영구 저장하고 게임 시작 시 자동으로 동기화합니다.
* **게임 룰 제어**: 목표 빙고 줄 수(`TargetBingoCount`) 설정과 같은 핵심 규칙을 관리합니다.

---

## 📑 주요 데이터 필드 (멤버 변수)

### 1. 팀 상태 (Score & Color)
| 변수명 | 타입 | 기본값 | 설명 |
| :--- | :--- | :--- | :--- |
| `TeamAScore` | `int32` | `0` | A팀의 현재 누적 점수 (BlueprintReadOnly) |
| `TeamBScore` | `int32` | `0` | B팀의 현재 누적 점수 (BlueprintReadOnly) |
| `TeamAColor` | `FLinearColor` | `Pure Red (1,0,0,1)` | A팀의 상징 컬러 |
| `TeamBColor` | `FLinearColor` | `Pure Blue (0,0,1,1)` | B팀의 상징 컬러 |

### 2. 퀴즈 데이터 풀
| 변수명 | 타입 | 설명 |
| :--- | :--- | :--- |
| `Questions` | `TArray<FCB_DataTable_Question>` | 임시 또는 현재 데이터 테이블 등에서 추출된 전체 문제 리스트 |
| `QuizMasterList` | `TArray<FCB_DataTable_Question>` | **[세이브 연동]** 현재 메모리(런타임)에서 영구적으로 관리할 마스터 문제 배열 |

### 3. 규칙 및 시스템 설정
| 변수명 | 타입 | 기본값 | 설명 |
| :--- | :--- | :--- | :--- |
| `TargetBingoCount` | `int32` | `3` | 승리를 위해 달성해야 하는 목표 빙고 줄 수 (최소 1줄 이상 보장) |
| `CurrentSaveSlotName`| `FString` | `"DefaultQuizSlot"` | 런타임에 지정 가능한 현재 세이브 슬롯 이름 |
| `SaveSlotName` | `FString` | `"CrazyBingo_QuizSlot"`| 고정 내부 세이브 슬롯 이름 고유 상수 |
| `UserIndex` | `int32` | `0` | 로컬 세이브용 사용자 인덱스 |

---

## 🛠 주요 기능 및 인터페이스 (메서드)

### 1. 팀 및 점수 제어
* `void PlusScore(int32 ArgScore, bool TeamA)`
    * 지정된 팀(A 또는 B)의 누적 점수를 가산합니다.
* `int32 GetScore(bool TeamA)`
    * 지정된 팀의 현재 점수를 반환합니다.
* `void ResetScores()`
    * 게임을 리셋할 때 사용하며 양 팀의 점수를 `0`으로 초기화합니다. (BlueprintCallable)
* `FLinearColor GetTeamColor(bool TeamA)`
    * 지정된 팀의 컬러값을 반환합니다.
* `void SetTeamColors(FLinearColor NewColorA, FLinearColor NewColorB)` / `SetTeamColors2(...)`
    * 각 팀의 색상을 새로 지정합니다.

### 2. 퀴즈 데이터 처리 로직
* `void LoadQuestionsFromDataTable(UDataTable* DataTable)`
    * 입력받은 언리얼 `UDataTable`에서 모든 행을 순회하며 `FCB_DataTable_Question` 데이터를 추출하여 `Questions` 배열을 채웁니다.
* `void AddQuestion(FCB_DataTable_Question Question)`
    * 퀴즈 풀에 문제를 단일 추가합니다.
* `TArray<FCB_DataTable_Question> GetRandomQuestions(int32 Count = 25)`
    * 전체 문제 리스트를 피셔-예이츠(Fisher-Yates) 방식으로 무작위 셔플한 후, 요청된 `Count`(기본 25개)만큼 잘라서 반환합니다. 빙고판 구성 시 유용합니다.

### 3. 규칙 설정
* `void SetTargetBingoCount(int32 NewCount)`
    * 목표 빙고 줄 수를 런타임에 동적으로 변경합니다. `FMath::Max(1, NewCount)`를 통해 1보다 작은 비정상적인 값이 설정되지 않도록 방어 코드가 적용되어 있습니다.

### 4. 지속성 데이터 관리 (Persistence / SaveGame)
* `void Init() override`
    * 엔진에 의해 게임 인스턴스가 시작되는 즉시 호출되며, 내부적으로 `LoadQuizDataFromFile()`을 실행하여 기존의 세이브 데이터를 메모리에 자동 로드합니다.
* `void SaveQuizDataToFile()`
    * `UCB_QuizSaveGame` 오브젝트를 생성한 후, 런타임 메모리에 존재하는 `QuizMasterList`를 세이브 파일에 스냅샷 형태로 복사하여 지정된 슬롯에 저장합니다.
* `void LoadQuizDataFromFile()`
    * 해당 세이브 슬롯 파일이 로컬 드라이브에 존재하는지 체크하고, 존재할 시 데이터를 읽어와 런타임 `QuizMasterList`에 복원합니다. 파일이 없다면 마스터 리스트를 비웁니다.