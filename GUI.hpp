#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtGui/QCloseEvent>
#include <QtCore/QJsonArray>
#include "TypeLoader.hpp"

class GUI : public QWidget{
public:
	GUI();

private:
	QLineEdit* displayName = new QLineEdit();
	QLineEdit* codeName = new QLineEdit();
	QComboBox* typeSelect = new QComboBox();
	QLineEdit* portSelect = new QLineEdit();

	QPushButton* submitButton = new QPushButton("Submit");
	QPushButton* removeBindingButton = new QPushButton("Remove Binding");
	QPushButton* exportJsonButton = new QPushButton("Export JSON...");

	QTableWidget* table = new QTableWidget(0, 4);

	// Stores current pairs from TypeLoader
	std::vector<TypeLoader::TypePair> currentPairs;
	
	TypeLoader loader;

	// Tracks if changes were made to relative status
	bool changesSaved = true;

	// Clears table and resets headers
	void resetTable();

	// Enables/disables buttons based on input lengths
	inline void updateButtons();

	// Loads types from TypeLoader
	void loadTypes();

	// Adds data to table
	void submitData();

	// Removes data from table
	void removeBinding();

	// Imports JSON from file
	void importJson();

	// Exports JSON to file
	void exportJson();

	// Handles Close event to prevent closing the window without saving
	void closeEvent(QCloseEvent* event) override;

	// Utility method for exporting JSON
	int getPortCount(int row);

	// Utility method for exporting JSON
	QJsonArray getPortArray(int row);
};