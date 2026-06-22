# FCB_DataTable_Question

## 📌 개요

`FCB_DataTable_Question`은 Crazy Bingo 프로젝트의 모든 퀴즈 데이터를 관리하는 데이터 구조체입니다.

언리얼 엔진의 `UDataTable`과 연동하기 위해 `FTableRowBase`를 상속받아 구현되었으며, 객관식, 초성 퀴즈, 유튜브 음악 퀴즈, 단답형 퀴즈 등 다양한 문제 유형을 하나의 구조체에서 관리할 수 있도록 설계되었습니다.

---

## 🎯 주요 역할

* 퀴즈 데이터 저장 및 관리
* DataTable 기반 데이터 로드
* 문제 유형별 데이터 분기 처리
* UI와 게임 로직 간 데이터 전달


---

## 📑 데이터 구조

### 기본 정보

| 변수명                   | 타입                 | 설명            |
| --------------------- | ------------------ | ------------- |
| `bIsCheckedForDelete` | `bool`             | 문제 삭제 선택 여부   |
| `Category`            | `FString`          | 문제 카테고리       |
| `QuestionType`        | `ECB_QuestionType` | 문제 유형         |
| `Score`               | `int32`            | 문제 정답 시 획득 점수 |

---

### 문제 콘텐츠

| 변수명            | 타입                | 설명        |
| -------------- | ----------------- | --------- |
| `QuestionText` | `FString`         | 출제 문제     |
| `Answer`       | `FString`         | 정답        |
| `Choices`      | `TArray<FString>` | 객관식 보기 목록 |
| `Explanation`  | `FString`         | 정답 해설     |

---

### 유형별 추가 데이터

| 변수명                | 타입        | 설명        |
| ------------------ | --------- | --------- |
| `YoutubeURL`       | `FString` | 유튜브 링크    |
| `InitialSoundHint` | `FString` | 초성 힌트     |
| `Hint`             | `FString` | 일반 힌트     |
| `ImageAssetPath`   | `FString` | 문제 이미지 경로 |

---

## 🛠 지원하는 문제 유형

| Enum             | 표시명   | 사용 데이터             |
| ---------------- | --[CB_GameInstance.md](../GameInstance/CB_GameInstance.md)--- | ------------------ |
| `MultipleChoice` | 객관식   | `Choices`          |
| `InitialSound`   | 초성 퀴즈 | `InitialSoundHint` |
| `YoutubeLink`    | 음악 퀴즈 | `YoutubeURL`       |
| `ShortAnswer`    | 단답형   | `Answer`           |

---



## 💡 설계 의도

초기에는 문제 유형마다 별도의 구조체를 사용하는 방식을 고려했으나, 관리해야 할 데이터 수가 증가하고 UI 분기 로직이 복잡해지는 문제가 있었습니다.

이를 해결하기 위해 모든 문제 유형을 하나의 구조체로 통합하고, `QuestionType` Enum을 기준으로 필요한 필드만 선택적으로 사용하는 방식으로 설계했습니다.

이를 통해

* 데이터 관리 단순화
* UI 생성 로직 통합
* 문제 유형 확장 용이
* DataTable 관리 편의성 향상

을 달성할 수 있었습니다.
