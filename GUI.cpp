#pragma once

#include "GUI.hpp"
#include "TypeLoader.hpp"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHeaderView>
#include <iostream>

using namespace std;

GUI::GUI() {
	this->setWindowTitle("JSON Port Configurator");
	this->setMinimumSize(QSize(600, 400));

	// Initialize
	QVBoxLayout* mainLayout = new QVBoxLayout();

	// Top part
	QGroupBox* itemEntry = new QGroupBox("Item Entry");
	QVBoxLayout* entryLayout = new QVBoxLayout();
	itemEntry->setLayout(entryLayout);
	QHBoxLayout* itemLayout = new QHBoxLayout();
	entryLayout->addLayout(itemLayout);
	mainLayout->addWidget(itemEntry);

	QVBoxLayout* displayNameLayout = new QVBoxLayout();
	itemLayout->addLayout(displayNameLayout);
	displayNameLayout->addWidget(new QLabel("Display Name"));
	displayNameLayout->addWidget(displayName);

	QVBoxLayout* codeNameLayout = new QVBoxLayout();
	itemLayout->addLayout(codeNameLayout);
	codeNameLayout->addWidget(new QLabel("Programming Name"));
	codeNameLayout->addWidget(codeName);

	QVBoxLayout* typeLayout = new QVBoxLayout();
	itemLayout->addLayout(typeLayout);
	typeLayout->addWidget(new QLabel("Type"));
	typeLayout->addWidget(typeSelect);
	typeSelect->setMinimumWidth(100);

	QVBoxLayout* portLayout = new QVBoxLayout();
	itemLayout->addLayout(portLayout);
	portLayout->addWidget(new QLabel("Port"));
	portLayout->addWidget(portSelect);

	QPushButton* importTypes = new QPushButton("Type Options...");
	entryLayout->addWidget(importTypes);

	submitButton->setEnabled(false);
	entryLayout->addWidget(submitButton);

	// Chart
	QGroupBox* tableGroup = new QGroupBox("Table");
	mainLayout->addWidget(tableGroup);
	QVBoxLayout* tableLayout = new QVBoxLayout();
	tableGroup->setLayout(tableLayout);
	tableLayout->addWidget(table);
	resetTable();

	QHBoxLayout* jsonButtons = new QHBoxLayout();
	tableLayout->addLayout(jsonButtons);
	QPushButton* importJson = new QPushButton("Import JSON...");
	jsonButtons->addWidget(importJson);
	exportJson->setEnabled(false);
	jsonButtons->addWidget(exportJson);

	// Connections
	(void)connect(displayName, &QLineEdit::textChanged, this, &GUI::updateButtons);
	(void)connect(codeName, &QLineEdit::textChanged, this, &GUI::updateButtons);
	(void)connect(importTypes, &QPushButton::clicked, this, &GUI::loadTypes);

	this->setLayout(mainLayout);
	this->show();
}

void GUI::resetTable() {
	table->setRowCount(0);
	table->setColumnCount(4);
	table->setHorizontalHeaderLabels({ "Display Name", "Code Name", "Type", "Port" });
	table->setAutoScroll(true);
	QHeaderView* header = table->horizontalHeader();
	header->setStretchLastSection(false);
	for (int i = 0; i < 4; ++i) header->setSectionResizeMode(i, QHeaderView::Stretch);
}

void GUI::updateButtons() {
	submitButton->setEnabled(displayName->text().length() > 0 && codeName->text().length() > 0);
}

void GUI::loadTypes() {
	loader.open();
	auto result = loader.getTypes();
	cout << "RESULT SIZE:" << result.size() << endl;
}