#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include "TypeLoader.hpp"

class GUI : public QWidget{
public:
	GUI();

private:
	QLineEdit* displayName = new QLineEdit();
	QLineEdit* codeName = new QLineEdit();
	QComboBox* typeSelect = new QComboBox();
	QSpinBox* portSelect = new QSpinBox();

	QPushButton* submitButton = new QPushButton("Submit");
	QPushButton* exportJson = new QPushButton("Export JSON...");

	QTableWidget* table = new QTableWidget(0, 4);
	
	TypeLoader loader;

	void resetTable();

	inline void updateButtons();

	void loadTypes();
};