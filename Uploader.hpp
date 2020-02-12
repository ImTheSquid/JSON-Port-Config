#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <string>

namespace net {
#include <libssh2.h>
}

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
	QLineEdit* path = new QLineEdit();
	QSpinBox* port = new QSpinBox();
	std::string jsonData = "";
	bool uploadSuccess = false;

	net::LIBSSH2_SESSION* session = NULL;
	int sock = 0;

	void initUpload();

	std::string upload();

	std::string openConnection(std::string ip, std::string username, std::string password, int port);

	std::string sendFile(FILE* file, std::string path);

	void shutdown(std::string message);
};