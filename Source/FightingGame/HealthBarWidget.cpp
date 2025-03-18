// #include "HealthBarWidget.h"
// #include "Components/CanvasPanel.h"
// #include "Components/CanvasPanelSlot.h"
// #include "Components/WidgetTree.h"

// void UHealthBarWidget::NativeConstruct()
// {
//     Super::NativeConstruct();

//     // Create a Canvas Panel as the root widget.
//     UCanvasPanel* RootPanel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootPanel"));
//     WidgetTree->RootWidget = RootPanel;

//     // Create a progress bar for health.
//     HealthProgressBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("HealthProgressBar"));
//     if (HealthProgressBar)
//     {
//         // Set initial full health (100%).
//         HealthProgressBar->SetPercent(1.0f);

//         // Add the progress bar to the root canvas panel.
//         if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(RootPanel->AddChild(HealthProgressBar)))
//         {
//             // Set anchors and offsets as desired (this example stretches it).
//             CanvasSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
//             CanvasSlot->SetOffsets(FMargin(0));
//         }
//     }
// }

// void UHealthBarWidget::UpdateHealth(float HealthPercent)
// {
//     if (HealthProgressBar)
//     {
//         HealthProgressBar->SetPercent(HealthPercent);
//     }
// }
