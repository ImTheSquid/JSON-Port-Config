#pragma once

#include <QtWidgets/QDialog>
#include <vector>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

class TypeLoader : public QDialog {
public:
	// Stores a string and a int to return to GUI
	struct TypePair {
		std::string key;
		int port;
	};

	TypeLoader();

	~TypeLoader();

	// Opens dialog
	inline void open();

	// Returns types
	std::vector<TypeLoader::TypePair> getTypes();

	// Appends a new type to the typeEditor
	void appendType(std::string type);

private:
	std::vector<TypeLoader::TypePair> types;
	QLineEdit* typeEditor = new QLineEdit();
	QPushButton* exportJson = new QPushButton("Export TXT...");

	// Loads types from file
	void loadFromFile();

	// Saves types to file
	void saveToFile();

	// Updates export button (enable/disable) depending on typeEditor length
	inline void updateExportButton();
};