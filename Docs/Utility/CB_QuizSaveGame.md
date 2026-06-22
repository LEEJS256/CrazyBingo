# UCB_QuizSaveGame

## 📌 개요
`UCB_QuizSaveGame`은 Crazy Bingo 프로젝트에서 플레이어가 설정하고 관리하는 퀴즈 마스터 데이터를 로컬 드라이브에 영구적인 파일(`.sav`) 형태로 직렬화(Serialization)하여 저장하고 불러오기 위한 언리얼 엔진 전용 세이브 객체(`USaveGame`)입니다.

---

## 🎯 주요 역할
* **퀴즈 데이터 영구 보존**: 게임 런타임 메모리에 존재하는 마스터 퀴즈 배열을 스냅샷 형태로 안전하게 디스크에 기록합니다.
* **메타데이터 기록**: 데이터가 저장된 시점의 타임스탬프 정보를 문자열로 함께 보관합니다.

---

## 📑 데이터 구조 (멤버 변수)

| 변수명 | 타입 | 노출 레벨 | 설명 |
| :--- | :--- | :--- | :--- |
| `SavedQuestions` | `TArray<FCB_DataTable_Question>` | `VisibleAnywhere` | 파일에 직렬화되어 저장되는 핵심 퀴즈 데이터 리스트 |
| `SaveDateTimeString` | `FString` | `VisibleAnywhere` | 데이터가 실제 물리 저장된 시점의 날짜 및 시간 기록 문자열 |

---

## 🔄 데이터 흐름 (Data Flow)
1. **Save**: `UCB_GameInstance`에서 본 객체를 동적 생성(`CreateSaveGameObject`) 한 뒤, 런타임의 `QuizMasterList` 데이터를 `SavedQuestions`에 복사하고 `SaveGameToSlot`을 호출하여 물리 파일로 저장합니다.
2. **Load**: 게임 시작 시 지정된 슬롯이 존재하면 `LoadGameFromSlot`을 통해 이 객체를 역직렬화하여 읽어온 뒤, 내부에 저장되어 있던 `SavedQuestions` 리스트를 `UCB_GameInstance::QuizMasterList`로 다시 복원합니다.

