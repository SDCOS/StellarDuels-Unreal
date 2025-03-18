#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SVerticalBox.h"
#include "OnlineSessionSettings.h" // Needed for FOnlineSessionSearchResult

/** Delegate called when a session is selected to join. */
DECLARE_DELEGATE_OneParam(FOnSessionSelected, TSharedPtr<FOnlineSessionSearchResult>);

/** Slate widget for displaying a list of available sessions and selecting one to join. */
class SJoinServerWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SJoinServerWidget) {}
		SLATE_EVENT(FOnSessionSelected, OnSessionSelected)
		SLATE_ARGUMENT(TArray<TSharedPtr<FOnlineSessionSearchResult>>, SessionResults)
	SLATE_END_ARGS()

	/** Constructs the widget with a list of session search results. */
	void Construct(const FArguments& InArgs)
	{
		OnSessionSelected = InArgs._OnSessionSelected;
		SessionResults = InArgs._SessionResults;

		ChildSlot
		[
			SNew(SVerticalBox)
			
			// List view of sessions
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			.Padding(5)
			[
				SAssignNew(SessionListView, SListView<TSharedPtr<FOnlineSessionSearchResult>>)
				.ListItemsSource(&SessionResults)
				.OnGenerateRow(this, &SJoinServerWidget::OnGenerateRowForSession)
				.OnSelectionChanged(this, &SJoinServerWidget::OnSessionSelectionChanged)
				.SelectionMode(ESelectionMode::Single)
			]

			// Join Server button
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5)
			[
				SNew(SButton)
				.Text(FText::FromString("Join Selected Server"))
				.OnClicked(this, &SJoinServerWidget::OnJoinServerClicked)
				.IsEnabled(this, &SJoinServerWidget::IsJoinEnabled)
			]
		];
	}

private:
	/** Generates each row widget for the list view. */
	TSharedRef<ITableRow> OnGenerateRowForSession(TSharedPtr<FOnlineSessionSearchResult> InItem, const TSharedRef<STableViewBase>& OwnerTable)
	{
		// Try to get the session name from session settings.
		FString SessionName = "Unnamed Session";
		const FOnlineSessionSetting* NameSetting = InItem->Session.SessionSettings.Settings.Find(TEXT("SERVER_NAME"));
		if (NameSetting)
		{
			SessionName = NameSetting->Data.ToString();
		}

		return SNew(STableRow<TSharedPtr<FOnlineSessionSearchResult>>, OwnerTable)
		[
			SNew(STextBlock)
			.Text(FText::FromString(SessionName))
		];
	}

	/** Called when a session is selected from the list. */
	void OnSessionSelectionChanged(TSharedPtr<FOnlineSessionSearchResult> InSelectedItem, ESelectInfo::Type SelectInfo)
	{
		SelectedSession = InSelectedItem;
	}

	/** Called when the Join Server button is clicked. */
	FReply OnJoinServerClicked()
	{
		if (SelectedSession.IsValid())
		{
			OnSessionSelected.ExecuteIfBound(SelectedSession);
		}
		return FReply::Handled();
	}

	/** Determines whether the join button should be enabled. */
	bool IsJoinEnabled() const
	{
		return SelectedSession.IsValid();
	}

private:
	/** The array of session search results passed into the widget. */
	TArray<TSharedPtr<FOnlineSessionSearchResult>> SessionResults;

	/** The Slate list view widget that displays the sessions. */
	TSharedPtr<SListView<TSharedPtr<FOnlineSessionSearchResult>>> SessionListView;

	/** The currently selected session. */
	TSharedPtr<FOnlineSessionSearchResult> SelectedSession;

	/** Delegate called when a session is selected to join. */
	FOnSessionSelected OnSessionSelected;
};
