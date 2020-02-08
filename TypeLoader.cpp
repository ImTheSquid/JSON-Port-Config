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

using namespace std;

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
	this->exec();
}

void TypeLoader::loadFromFile() {
	auto dialog = QFileDialog::getOpenFileName(this, "Open types file...", "", "*.txt");

	if (dialog == NULL || dialog.length() == 0) return;

	string currentText = typeEditor->text().toStdString();
	
	if (currentText.length() > 0) currentText.append(", ");

	ifstream file(dialog.toStdString().c_str());
	if (file.is_open()) {
		string line;
		while (getline(file, line)) {
			currentText.append(line);
		}
		file.close();
		typeEditor->setText(QString(currentText.c_str()));
	}
	else cout << "File Error: Unable to open file" << endl;
}

void TypeLoader::saveToFile() {
	auto dialog = QFileDialog::getSaveFileName(this, "Save types file...", "types.txt", "*.txt");

	if (dialog == NULL || dialog.length() == 0) return;

	ofstream file(dialog.toStdString().c_str());
	if (file.is_open()) {
		file << typeEditor->text().toStdString().c_str();
		file.close();
	}
	else cout << "File Error: Unable to open file" << endl;
}

void TypeLoader::updateExportButton() {
	exportJson->setEnabled(typeEditor->text().length() > 0);
}

std::vector<TypeLoader::TypePair> TypeLoader::getTypes() {
	string str = typeEditor->text().toStdString();

	vector<TypePair> strings;
	istringstream f(str.c_str());
	string s;
	while (getline(f, s, ',')) {
		// Remove space from beginning of string if it exists
		if (s.at(0) == ' ') s = s.substr(1);

		TypePair pair;
		pair.key = s;

		// Parse number of ports needed
		if (s.find_first_of('(') == string::npos || s.find_first_of(')') == string::npos) {
			cout << "Parse Error: Couldn't find '(' or ')'" << endl;
			continue;
		}
		string sub = s.substr(s.find_first_of('(') + 1, s.find_first_of(')'));

		int port = INT_MIN;
		try {
			port = stoi(sub);
		}
		catch (invalid_argument) {
			cout << "Parse Error: Couldn't parse integer" << endl;
			continue;
		}
		pair.port = port;

		strings.push_back(pair);
	}
	
	return strings;
}

void TypeLoader::appendType(string type) {
	string currentText = typeEditor->text().toStdString();
	if (currentText.length() > 0) currentText.append(", ");
	currentText.append(type);
	typeEditor->setText(QString(currentText.c_str()));
}