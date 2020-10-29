#pragma once
#include "ui_qgsvctsourceselectbase.h"
#include "qgsabstractdatasourcewidget.h"

class QgsVctSourceSelect: public QgsAbstractDataSourceWidget, private Ui::QgsVctSourceSelectBase
{
	Q_OBJECT

public:
	QgsVctSourceSelect(QWidget *parent = nullptr, Qt::WindowFlags fl = QgsGuiUtils::ModalDialogFlags, QgsProviderRegistry::WidgetMode widgetMode = QgsProviderRegistry::WidgetMode::None);

private slots:
	void addButtonClicked() override;
	void openFileDialog();
	void onFileChanged();
};

