#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <string>

class Uploader : public QDialog {
public:
	Uploader();

	~Uploader();

	void open(std::string jsonData);

	bool uploadSucessful();

private:
	QLineEdit* ipAddress = new QLineEdit();
	QLineEdit* usernameEdit = new QLineEdit();
	QLineEdit* passwordEdit = new QLineEdit();
	std::string jsonData = "";
	bool uploadSuccess = false;

	

	void initUpload();

	std::string upload();
};