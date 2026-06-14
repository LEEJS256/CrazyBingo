// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CB_ResultPopup.generated.h"

/**
 * 
 */
class UTextBlock;
class UButton;

UCLASS()
class CRAZYBINGO_API UCB_ResultPopup : public UUserWidget
{
	GENERATED_BODY()
public:
	// 🌟 생성 기점에 부모 보드판 주소를 찔러 넣어줄 통로
	UPROPERTY(BlueprintReadWrite, Category = "Bingo|Result")
	class UCB_BingoBoard* OwnerBoard;

	// 🌟 외부(보드판)에서 승리팀과 점수 데이터를 넘겨받아 UI 텍스트를 갱신할 함수
	void SetupResultData(uint8 WinningTeamID, int32 ScoreA, int32 ScoreB);

protected:
	// 위젯이 생성될 때 버튼 이벤트를 바인딩하는 초기화 함수
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnGoToMainMenuClicked();
	// 🌟 WBP 디자이너 창의 컴포넌트명과 반드시 일치해야 하는 바인딩 변수들
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_WinnerBanner;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_FinalScores;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_GoToMainMenu;

	// 🌟 확인 버튼 클릭 시 메인 메뉴로 탈출할 런타임 함수
};
