#pragma once

#include <QtWidgets/QDialog>
#include <vector>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

class TypeLoader : public QDialog {
public:
	struct PortPair {
		std::string key;
		int port;
	};

	TypeLoader();

	~TypeLoader();

	inline void open();

	std::vector<TypeLoader::PortPair>& getTypes();

private:
	std::vector<TypeLoader::PortPair> types;
	QLineEdit* typeEditor = new QLineEdit();
	QPushButton* exportJson = new QPushButton("Export TXT...");

	void loadFromFile();

	void saveToFile();

	inline void updateExportButton();
};