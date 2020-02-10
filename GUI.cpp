#pragma once

#include "GUI.hpp"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QJsonDocument>
#include <iostream>
#include <set>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

GUI::GUI() {
	// Window setup
	this->setWindowTitle("Jack's JSON Port Configurator v.1.0");
	this->setMinimumSize(QSize(600, 400));

	// Initialize main layout
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
	portLayout->addWidget(new QLabel("Port(s) (comma separated)"));
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

	tableLayout->addWidget(removeBindingButton);

	QHBoxLayout* jsonButtons = new QHBoxLayout();
	tableLayout->addLayout(jsonButtons);
	QPushButton* importJsonButton = new QPushButton("Import JSON...");
	jsonButtons->addWidget(importJsonButton);
	exportJsonButton->setEnabled(false);
	jsonButtons->addWidget(exportJsonButton);

	// Connections
	(void)connect(displayName, &QLineEdit::textChanged, this, &GUI::updateButtons);
	(void)connect(codeName, &QLineEdit::textChanged, this, &GUI::updateButtons);
	(void)connect(portSelect, &QLineEdit::textChanged, this, &GUI::updateButtons);
	(void)connect(importTypes, &QPushButton::clicked, this, &GUI::loadTypes);
	(void)connect(submitButton, &QPushButton::clicked, this, &GUI::submitData);
	(void)connect(removeBindingButton, &QPushButton::clicked, this, &GUI::removeBinding);
	(void)connect(importJsonButton, &QPushButton::clicked, this, &GUI::importJson);
	(void)connect(exportJsonButton, &QPushButton::clicked, this, &GUI::exportJson);

	this->setLayout(mainLayout);
	this->show();
}

void GUI::resetTable() {
	table->setRowCount(0);
	table->setColumnCount(4);
	table->setHorizontalHeaderLabels({ "Display Name", "Code Name", "Type", "Port(s)" });
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
	vector<TypeLoader::TypePair> result = loader.getTypes();

	currentPairs = result;
	typeSelect->clear();

	for (auto const& pair : currentPairs) {
		typeSelect->addItem(QString(pair.key.c_str()));
	}
}

void GUI::submitData() {
	table->setRowCount(table->rowCount() + 1);
	int targetRow = table->rowCount() - 1;
	table->setItem(targetRow, 0, new QTableWidgetItem(displayName->text()));
	table->setItem(targetRow, 1, new QTableWidgetItem(codeName->text()));
	table->setItem(targetRow, 2, new QTableWidgetItem(typeSelect->currentText()));
	table->setItem(targetRow, 3, new QTableWidgetItem(portSelect->text()));
	table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	displayName->clear();
	codeName->clear();
	portSelect->clear();
	submitButton->setEnabled(false);
	exportJsonButton->setEnabled(true);
	changesSaved = false;
}

void GUI::removeBinding() {
	auto model = table->selectionModel();
	if (model->selectedRows().length() == 0) {
		auto info = new QMessageBox();
		info->setIcon(QMessageBox::Information);
		info->setWindowTitle("Binding Remover");
		info->setText("Please select a whole row.");
		info->exec();
		delete info;
		return;
	}

	for (auto const& row : model->selectedRows()) {
		table->removeRow(row.row());
	}

	changesSaved = table->rowCount() == 0;
}

void GUI::importJson() {
	auto dialog = QFileDialog::getOpenFileName(this, "Open JSON file...", "", "*.json");

	if (dialog == NULL || dialog.length() == 0) return;

	changesSaved = false;

	auto reply = QMessageBox::question(this, "JSON Importer", "Do you want to overwrite current entries?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (reply == QMessageBox::Yes) table->clear();

	QFile file(dialog);
	if (!file.open(QIODevice::ReadOnly)) {
		cout << "File Error: Error opening file" << endl;
		return;
	}

	QByteArray data = file.readAll();

	QJsonDocument doc(QJsonDocument::fromJson(data));
	QJsonObject obj = doc.object();

	for (auto const& key : obj.keys()) {
		QJsonObject dataObj = obj[key].toObject();

		// Table setup
		table->setRowCount(table->rowCount() + 1);
		int targetRow = table->rowCount() - 1;

		table->setItem(targetRow, 0, new QTableWidgetItem(dataObj["display_name"].toString()));
		table->setItem(targetRow, 1, new QTableWidgetItem(key));
		table->setItem(targetRow, 2, new QTableWidgetItem(dataObj["type"].toString()));
		
		// Convert array to string
		string out;
		auto vec = dataObj["ports"].toArray().toVariantList().toVector();
		for (auto const& item : vec) {
			string appendChars;
			appendChars = vec.indexOf(item) == vec.size() - 1 ? "" : ", ";
			out.append(item.toString().toStdString() + appendChars);
		}

		table->setItem(targetRow, 3, new QTableWidgetItem(out.c_str()));

		table->setEditTriggers(QAbstractItemView::NoEditTriggers);

		loader.appendType(dataObj["type"].toString().toStdString());

		vector<TypeLoader::TypePair> result = loader.getTypes();

		currentPairs = result;
		typeSelect->clear();

		for (auto const& pair : currentPairs) {
			typeSelect->addItem(QString(pair.key.c_str()));
		}
	}
}

void GUI::exportJson() {
	auto dialog = QFileDialog::getSaveFileName(this, "Save JSON file...", "data.json", "*.json");

	if (dialog == NULL || dialog.length() == 0) return;

	changesSaved = true;

	QJsonObject object;
	for (int i = 0; i < table->rowCount(); ++i) {
		QJsonObject listObj;

		listObj["display_name"] = table->item(i, 0)->text().toStdString().c_str();
		listObj["type"] = table->item(i, 2)->text().toStdString().c_str();
		listObj["port_count"] = getPortCount(i);
		listObj["ports"] = getPortArray(i);

		object[table->item(i, 1)->text().toStdString().c_str()] = listObj;
	}

	QFile file(dialog);
	if (!file.open(QIODevice::WriteOnly)) {
		cout << "File Error: Error opening file" << endl;
		return;
	}

	file.write(QJsonDocument(object).toJson());
	file.close();
}

void GUI::closeEvent(QCloseEvent* event) {
	if (changesSaved) {
		event->accept();
		return;
	}

	auto reply = QMessageBox::question(this, "WARNING: Unsaved Changes", "Do you want to exit without saving your changes?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

	if (reply == QMessageBox::No) event->ignore();
	else event->accept();
}

int GUI::getPortCount(int row) {
	for (auto const& pair : currentPairs) {
		if (pair.key.compare(table->item(row, 2)->text().toStdString()) == 0) return pair.port;
	}
	return INT_MIN;
}

QJsonArray GUI::getPortArray(int row) {
	QJsonArray items;
	istringstream f(table->item(row, 3)->text().toStdString().c_str());
	string s;
	while (getline(f, s, ',')) {
		// Parse ports
		if (s.at(0) == ' ') s = s.substr(1);

		int port = INT_MIN;
		try {
			port = stoi(s);
		}
		catch (invalid_argument) {
			cout << "Parse Error: Couldn't parse integer" << endl;
			continue;
		}

		items.push_back(port);
	}

	return items;
}