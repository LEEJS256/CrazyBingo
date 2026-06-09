// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/CB_BingoBoard.h" // 🌟 부모 보드 함수를 호출해야 하므로 헤더 추가
#include "Components/Button.h"
#include "UI/CB_HostPanel.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"

void UCB_HostPanel::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_TeamA_Win)
	{
		Btn_TeamA_Win->OnClicked.AddDynamic(this, &UCB_HostPanel::OnTeamAWinClicked);
	}

	if (Btn_TeamB_Win)
	{
		Btn_TeamB_Win->OnClicked.AddDynamic(this, &UCB_HostPanel::OnTeamBWinClicked);
	}

	if (Btn_WrongAnswer)
	{
		Btn_WrongAnswer->OnClicked.AddDynamic(this, &UCB_HostPanel::OnWrongAnswerClicked);
	}
	
}

// CB_HostPanel.cpp - 마우스 드래그 시스템 최종 보완본

FReply UCB_HostPanel::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		// 🌟 전체 화면(Absolute) 기준의 마우스 클릭 위치와 내 위젯 좌상단 절대 위치의 차이(오프셋)를 계산
		// 이렇게 해야 DPI 스케일이나 부모 위젯 계층에 영향을 받지 않습니다.
		DragOffset = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
		
		// 마우스 입력을 이 위젯이 독점하도록 붙잡고 Handled 리턴
		return FReply::Handled().CaptureMouse(TakeWidget());
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UCB_HostPanel::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (HasMouseCapture())
	{
		// 🌟 1. 전체 화면(Viewport) 기준의 마우스 현재 절대 좌표를 구함
		FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
		
		// 🌟 2. 처음에 찍었던 오프셋 만큼을 차감하여 새로운 절대 좌표 계산
		FVector2D NewPosition = MousePosition - DragOffset;

		// 🌟 3. AddToViewport로 생성된 위젯의 슬롯 제어 (가장 안전한 범용 캐스팅)
		// CanvasPanelSlot 캐스팅 실패 확률을 방지하기 위해 위젯 레이아웃 라이브러리를 통해 직접 꽂아 넣습니다.
		UWidget* TargetWidget = this;
		if (TargetWidget)
		{
			// 부모가 누구든 상관없이, 뷰포트 상의 절대 좌표로 내 위젯의 위치를 강제 이동시킵니다.
			UCanvasPanelSlot* ViewportSlot = Cast<UCanvasPanelSlot>(TargetWidget->Slot);
			if (ViewportSlot)
			{
				ViewportSlot->SetPosition(NewPosition);
			}
			else
			{
				// 만약 슬롯 캐스팅이 실패하는 특수 구조일 경우를 대비한 2차 안전장치
				// 블루프린트의 "Set Position in Viewport"와 완벽히 동일한 마법의 함수입니다.
				SetPositionInViewport(NewPosition, true);
			}
		}
		
		return FReply::Handled();
	}
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

FReply UCB_HostPanel::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && HasMouseCapture())
	{
		// 마우스 독점 해제
		return FReply::Handled().ReleaseMouseCapture();
	}
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}


void UCB_HostPanel::OnTeamAWinClicked()
{
	UCB_BingoBoard* OwnerBoard = GetTypedOuter<UCB_BingoBoard>();
	if (OwnerBoard)
	{
		// A팀 점령 (1번 번호 넘기기)
		OwnerBoard->SetCellOwnerByHost(1);
		UE_LOG(LogTemp, Log, TEXT("[호스트패널] A팀 정답 처리를 부모 보드에 요청했습니다."));
	}
}

void UCB_HostPanel::OnTeamBWinClicked()
{
	UCB_BingoBoard* OwnerBoard = GetTypedOuter<UCB_BingoBoard>();
	if (OwnerBoard)
	{
		// B팀 점령 (2번 번호 넘기기)
		OwnerBoard->SetCellOwnerByHost(2);
		UE_LOG(LogTemp, Log, TEXT("[호스트패널] B팀 정답 처리를 부모 보드에 요청했습니다."));
	}
}

void UCB_HostPanel::OnWrongAnswerClicked()
{
	UCB_BingoBoard* OwnerBoard = GetTypedOuter<UCB_BingoBoard>();
	if (OwnerBoard)
	{
		// 오답 또는 선택 초기화 (0번 번호 넘기기)
		OwnerBoard->SetCellOwnerByHost(0);
		UE_LOG(LogTemp, Log, TEXT("[호스트패널] 오답/초기화 처리를 부모 보드에 요청했습니다."));
	}
}
