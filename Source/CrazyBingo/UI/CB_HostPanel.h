// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CB_HostPanel.generated.h"

/**
 * 
 */
class UButton;
UCLASS()
class CRAZYBINGO_API UCB_HostPanel : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, Category = "Host")
	class UCB_BingoBoard* TargetBingoBoard;



	// int32 CurrentSelectedCellIndex = -1;

	// 🌟 빙고판에서 칸을 누르면 호출되어 UI를 갱신할 수신 함수
	void SetCurrentQuestionInfo(int32 CellIndex, const FCB_DataTable_Question& QuestionData);
	
protected:
	// 위젯 블루프린트가 생성될 때 이벤트 바인딩을 처리하는 초기화 함수
	virtual void NativeConstruct() override;


	UPROPERTY(meta = (BindWidget))
	UButton* Btn_TeamA_Win;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_TeamB_Win;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_WrongAnswer;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_CurrentQuestion;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Explanation;

	UPROPERTY(meta = (BindWidget))
	class UWebBrowser* WebBrowser_Youtube; 
	// =========================================================================
	// ⚡ 버튼 클릭 시 실행될 런타임 이벤트 함수들
	// =========================================================================
	UFUNCTION()
	void OnTeamAWinClicked();

	UFUNCTION()
	void OnTeamBWinClicked();

	UFUNCTION()
	void OnWrongAnswerClicked();

private:
	int32 CachedCellIndex = -1;
	
	// 🌟 드래그 시작 시 마우스 커서와 패널 좌상단 사이의 거리 기억용
	FVector2D DragOffset;
};
