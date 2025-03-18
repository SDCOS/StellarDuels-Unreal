#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SVerticalBox.h"

/** Delegate called when the user confirms the server name. */
DECLARE_DELEGATE_OneParam(FOnServerNameEntered, const FString&);

/** A simple Slate widget that lets the user enter a server name. */
class SCreateServerWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCreateServerWidget) {}
		SLATE_EVENT(FOnServerNameEntered, OnServerNameEntered)
	SLATE_END_ARGS()

	/** Constructs this widget with the given arguments. */
	void Construct(const FArguments& InArgs)
	{
		OnServerNameEntered = InArgs._OnServerNameEntered;

		ChildSlot
		[
			SNew(SVerticalBox)
			
			// Title text
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Enter Server Name:"))
			]

			// Editable text box
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5)
			[
				SAssignNew(ServerNameTextBox, SEditableTextBox)
			]

			// Create Server button
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5)
			[
				SNew(SButton)
				.Text(FText::FromString("Create Server"))
				.OnClicked(this, &SCreateServerWidget::OnCreateServerClicked)
			]
		];
	}

private:
	/** Called when the button is clicked. Retrieves the text and fires the delegate. */
	FReply OnCreateServerClicked()
	{
		if (ServerNameTextBox.IsValid())
		{
			FString EnteredName = ServerNameTextBox->GetText().ToString();
			OnServerNameEntered.ExecuteIfBound(EnteredName);
		}
		return FReply::Handled();
	}

private:
	/** Holds the editable text box widget. */
	TSharedPtr<SEditableTextBox> ServerNameTextBox;

	/** Delegate to notify that a server name was entered. */
	FOnServerNameEntered OnServerNameEntered;
};
