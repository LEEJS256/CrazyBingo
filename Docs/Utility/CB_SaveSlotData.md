# UCB_SaveSlotData

## 📌 개요
`UCB_SaveSlotData`는 세이브/로드 UI 인터페이스(예: 슬롯 리스트 뷰, 세이브 다이얼로그)에서 각 세이브 슬롯 항목의 논리적 상태와 소유권을 동적으로 관리하기 위한 가벼운 데이터 컨테이너 객체(`UObject`)입니다.

---

## 🎯 주요 역할
* **UI 상태 바인딩**: 현재 어떤 슬롯이 유저에게 선택(`bIsSelected`)되었는지 런타임 플래그를 관리합니다.
* **오너십 관리**: 해당 슬롯 데이터가 어느 UI 화면(게임 세팅 창 또는 세이브 팝업 창)에서 생성되고 참조되는지 주체 포인터를 유지합니다.

---

## 📑 데이터 구조 (멤버 변수)

| 변수명 | 타입 | 노출 레벨 | 설명 |
| :--- | :--- | :--- | :--- |
| `SlotName` | `FString` | `BlueprintReadWrite` | 세이브 파일 식별 및 파일 생성에 사용되는 슬롯의 고유 명칭 |
| `bIsSelected` | `bool` | `BlueprintReadWrite` | 현재 UI 리스트 상에서 이 슬롯이 선택(포커스)되었는지 여부 |
| `OwningGameSetup` | `UCB_GameSetup*` | `UPROPERTY()` | 이 슬롯 데이터를 참조/소유하는 게임 세팅 UI 클래스 주소 |
| `OwningSaveDialog` | `UCB_SaveDialog*` | `UPROPERTY()` | 이 슬롯 데이터가 배치된 세이브 다이얼로그 UI 클래스 주소 |

---
