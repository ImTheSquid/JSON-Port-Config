#pragma once

#include "TypeLoader.hpp"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QFileDialog>
#include <iostream>
#include <fstream>
#include <sstream>

TypeLoader::TypeLoader() {
	this->setModal(true);
	this->setWindowTitle("Type Configurator");
	this->setWindowFlag(Qt::WindowContextHelpButtonHint, false);

	QVBoxLayout* mainLayout = new QVBoxLayout();

	mainLayout->addWidget(new QLabel("Add types here, seperated by commas with number of ports in parenthesis:"));
	mainLayout->addWidget(new QLabel("Ex: Motor (1), Sensor (3)"));
	mainLayout->addWidget(typeEditor);
	mainLayout->addStretch();

	QHBoxLayout* jsonButtons = new QHBoxLayout();
	mainLayout->addLayout(jsonButtons);
	QPushButton* importJson = new QPushButton("Import TXT...");
	jsonButtons->addWidget(importJson);
	jsonButtons->addWidget(exportJson);
	QPushButton* exit = new QPushButton("OK");
	mainLayout->addWidget(exit);

	(void)connect(exit, &QPushButton::clicked, this, &TypeLoader::hide);
	(void)connect(typeEditor, &QLineEdit::textChanged, this, &TypeLoader::updateExportButton);
	(void)connect(importJson, &QPushButton::clicked, this, &TypeLoader::loadFromFile);
	(void)connect(exportJson, &QPushButton::clicked, this, &TypeLoader::saveToFile);

	this->setLayout(mainLayout);
}

TypeLoader::~TypeLoader() {
	delete typeEditor;
	delete exportJson;
}

void TypeLoader::open() {
	this->show();
}

void TypeLoader::loadFromFile() {
	auto dialog = QFileDialog::getOpenFileName(this, "Open types file...", "", "*.txt");

	if (dialog == NULL || dialog.length() == 0) return;
}

void TypeLoader::saveToFile() {
	auto dialog = QFileDialog::getSaveFileName(this, "Save types file...", "types.txt", "*.txt");

	if (dialog == NULL || dialog.length() == 0) return;
}

void TypeLoader::updateExportButton() {
	exportJson->setEnabled(typeEditor->text().length() > 0);
}

std::vector<TypeLoader::PortPair>& TypeLoader::getTypes() {
	using namespace std;
	auto str = typeEditor->text().toStdString();

	vector<PortPair> strings;
	istringstream f(str.c_str());
	string s;
	while (getline(f, s, ',')) {
		PortPair pair;
		pair.key = s;

		// Parse number of ports needed
		if (s.find_first_of('(') == string::npos) continue;
		string sub = s.substr(s.find_first_of('('));
		int port = -1;
		try {
			port = stoi(sub);
		}
		catch (invalid_argument) {
			continue;
		}
		pair.port = port;

		strings.push_back(pair);
	}
	
	return strings;
}