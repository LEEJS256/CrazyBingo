// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CB_QuestionBoard.h"

#include "ImageUtils.h"
#include "Components/TextBlock.h"
#include "WebBrowser.h"
#include "Components/Image.h"
#include "Data/CB_DataTable_Question.h"
#include "Components/WidgetSwitcher.h"

void UCB_QuestionBoard::SetQuestion(const FCB_DataTable_Question& QuestionData)
{
if (!Text_QuestionType || !Text_QuestionBody || !QuestionTypeSwitcher) return;

    // 1. 공통 질문 내용 세팅 (기존 변수명에 맞춰 세팅)
    Text_QuestionBody->SetText(FText::FromString(QuestionData.QuestionText));

    if (Image_QuestionPhoto)
    {
        // 경로가 존재하고 비어있지 않다면
        if (!QuestionData.ImageAssetPath.IsEmpty())
        {
            // 외부 PC 경로에서 이미지 텍스처 로드
            UTexture2D* LoadedTexture = FImageUtils::ImportFileAsTexture2D(QuestionData.ImageAssetPath);
			
            if (IsValid(LoadedTexture)) 
            {
                Image_QuestionPhoto->SetBrushFromTexture(LoadedTexture);
                Image_QuestionPhoto->SetVisibility(ESlateVisibility::Visible); // ⭕ 이때만 켠다!
            
                UE_LOG(LogTemp, Log, TEXT("[성공] 이미지 로드 완료: %s"), *QuestionData.ImageAssetPath);
            }
            else
            {
                // 텍스처 생성 실패 시 완전히 안전하게 해제
                Image_QuestionPhoto->SetBrushFromTexture(nullptr);
                UE_LOG(LogTemp, Error, TEXT("[경고] 파일은 있으나 텍스처 변환 실패: %s"), *QuestionData.ImageAssetPath);
            }
        }
        else
        {
            // 애초에 이미지 경로가 없는 텍스트 문제라면 완전히 접어버리기!
            Image_QuestionPhoto->SetBrushFromTexture(nullptr);
            Image_QuestionPhoto->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
    
    // 2. 🌟 [핵심] 주신 이넘(ECB_QuestionType) 조건문으로 기존 스위처 연동
    switch (QuestionData.QuestionType)
    {
    case ECB_QuestionType::MultipleChoice: // 객관식
        Text_QuestionType->SetText(FText::FromString(TEXT("객관식 퀴즈")));
        QuestionTypeSwitcher->SetActiveWidgetIndex(0); // 0번 방 (객관식용 UI 켜기)

        // 주신 4개짜리 Choices 배열에서 순서대로 꺼내와 기존 텍스트에 매칭
        if (Text_Choice_1) Text_Choice_1->SetText(QuestionData.Choices.IsValidIndex(0) ? FText::FromString(QuestionData.Choices[0]) : FText::GetEmpty());
        if (Text_Choice_2) Text_Choice_2->SetText(QuestionData.Choices.IsValidIndex(1) ? FText::FromString(QuestionData.Choices[1]) : FText::GetEmpty());
        if (Text_Choice_3) Text_Choice_3->SetText(QuestionData.Choices.IsValidIndex(2) ? FText::FromString(QuestionData.Choices[2]) : FText::GetEmpty());
        if (Text_Choice_4) Text_Choice_4->SetText(QuestionData.Choices.IsValidIndex(3) ? FText::FromString(QuestionData.Choices[3]) : FText::GetEmpty());
        break;

    case ECB_QuestionType::InitialSound: // 초성
        Text_QuestionType->SetText(FText::FromString(TEXT("초성 퀴즈")));
        QuestionTypeSwitcher->SetActiveWidgetIndex(1); // 1번 방 (초성용 UI 켜기)

        if (Text_InitialHint) 
            Text_InitialHint->SetText(FText::FromString(FString::Printf(TEXT("힌트 초성: %s"), *QuestionData.InitialSoundHint)));
        break;

    case ECB_QuestionType::YoutubeLink: // 유튜브 링크
        Text_QuestionType->SetText(FText::FromString(TEXT("음악/영상 퀴즈")));
        QuestionTypeSwitcher->SetActiveWidgetIndex(2); // 2번 유튜브 방 오픈

        if (WebBrowser_Youtube && !QuestionData.YoutubeURL.IsEmpty())
        {
            FString TargetURL = QuestionData.YoutubeURL;

            // 💡 [고급 팁] 일반 주소를 유튜브 임베드(전체화면 재생) 주소로 변환합니다.
            // 예: watch?v=abcdefg -> embed/abcdefg
            if (TargetURL.Contains(TEXT("watch?v=")))
            {
                TargetURL = TargetURL.Replace(TEXT("watch?v="), TEXT("embed/"));
                
                // 레크리에이션용 자동재생 옵션 추가 (?autoplay=1)
                TargetURL += TEXT("?autoplay=1"); 
            }

            // 🌟 가공된 주소로 웹 브라우저 페이지 이동 (게임 화면 내에서 자동 재생!)
            WebBrowser_Youtube->LoadURL(TargetURL);
        }
        break;

    case ECB_QuestionType::ShortAnswer: // 단답형
        Text_QuestionType->SetText(FText::FromString(TEXT("단답형 주관식")));
        QuestionTypeSwitcher->SetActiveWidgetIndex(3); // 3번 방 (단답형용 UI 켜기)
        break;
    }
}
