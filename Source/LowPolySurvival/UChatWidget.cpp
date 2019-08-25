// Fill out your copyright notice in the Description page of Project Settings.


#include "UChatWidget.h"
#include "TextBlock.h"
#include "EditableTextBox.h"
#include "LowPolySurvivalCharacter.h"


bool UChatWidget::Initialize() {
	bool bSuperInit = Super::Initialize();

	chatInputTextBox->OnTextCommitted.AddDynamic(this, &UChatWidget::ProcessChatInput);
	

	return bSuperInit;
}



FReply UChatWidget::NativeOnPreviewKeyDown(const FGeometry & InGeometry, const FKeyEvent & InKeyEvent) {

	if (InKeyEvent.GetKey() == EKeys::Tab || InKeyEvent.GetKey() == EKeys::Escape) {
		CloseUI();
	}
	else if (InKeyEvent.GetKey() == EKeys::Enter) {
		//ProcessChatInput(chatInputTextBox->GetText());
	}

	return FReply(Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent));
}

void UChatWidget::OpenUI(){
	Super::OpenUI();

	chatInputTextBox->SetUserFocus(GetOwningPlayer());
	chatInputTextBox->SetKeyboardFocus();
}

void UChatWidget::ProcessChatInput(const FText& text, ETextCommit::Type commitMethod){

	if (commitMethod != ETextCommit::OnEnter) return;
	//if (text.IsEmpty()) return;

	FString inputStr = text.ToString();
	inputStr.TrimStartInline();
	
	
	if (inputStr.StartsWith("/")) {
		inputStr.RemoveAt(0);

		TArray<FString> argsArray;
		inputStr.ParseIntoArrayWS(argsArray);
		
		ProcessArgs(argsArray);
	}

	chatInputTextBox->SetText(FText());
	CloseUI();
}

void UChatWidget::ProcessArgs(const TArray<FString>& args){
	if (args.Num() < 1) return;

	ALowPolySurvivalCharacter* playerChar = Cast<ALowPolySurvivalCharacter>(GetOwningPlayerPawn());
	if (!playerChar) return;

	FString first = args[0];

	if (first == "playermode") {
		if (args.IsValidIndex(1)) {
			if (args[1] == "0") {
				playerChar->SetPlayerMode(EPlayerMode::SURVIVAL);
			}
			else if (args[1] == "1") {
				playerChar->SetPlayerMode(EPlayerMode::CREATIVE);
			}
		}
		
	}else if (first == "give") {
		if (args.IsValidIndex(1)) {
			int32 amount = 1;
			if (args.IsValidIndex(2) && args[2].IsNumeric()) {
				amount = FCString::Atoi(*args[2]);
			}

			FItemStack itemStack = FItemStack::FromId(playerChar, *args[1], 0, amount);
			playerChar->AddItemStackToInventory(itemStack, true);
			

		}
	}
}
