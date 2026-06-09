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
protected:
	// 위젯 블루프린트가 생성될 때 이벤트 바인딩을 처리하는 초기화 함수
	virtual void NativeConstruct() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;	

	
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_TeamA_Win;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_TeamB_Win;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_WrongAnswer;

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
	// 🌟 드래그 시작 시 마우스 커서와 패널 좌상단 사이의 거리 기억용
	FVector2D DragOffset;
};
