#pragma once

#include "Uploader.hpp"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMessageBox>

#include <cstdint>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

namespace net {
#include <WinSock2.h>
#include <libssh2.h>
#include <libssh2_sftp.h>
#include <WS2tcpip.h>
}

Uploader::Uploader() {
	this->setModal(true);
	this->setWindowTitle("RIO Uploader");
	this->setWindowFlag(Qt::WindowContextHelpButtonHint, false);

	QVBoxLayout* mainLayout = new QVBoxLayout();

	QHBoxLayout* credLayout = new QHBoxLayout();
	mainLayout->addLayout(credLayout);
	credLayout->addWidget(new QLabel("Username:"));
	credLayout->addWidget(usernameEdit);
	credLayout->addWidget(new QLabel("Password:"));
	credLayout->addWidget(passwordEdit);
	passwordEdit->setEchoMode(QLineEdit::EchoMode::Password);

	QHBoxLayout* ipLayout = new QHBoxLayout();
	mainLayout->addLayout(ipLayout);
	ipLayout->addWidget(new QLabel("IP:"));
	ipLayout->addWidget(ipAddress);
	ipLayout->addWidget(new QLabel("Port:"));
	ipLayout->addWidget(port);
	port->setMinimum(0);
	port->setMaximum(65535);
	port->setValue(22);

	QHBoxLayout* pathLayout = new QHBoxLayout();
	mainLayout->addLayout(pathLayout);
	pathLayout->addWidget(new QLabel("Dest Path (UNIX):"));
	pathLayout->addWidget(path);
	path->setText("/tmp/NO_PATH.json");

	QLabel* warning = new QLabel("WARNING: Destination file will be overwritten if necessary.");
	warning->setStyleSheet("color:#FF0000;");
	warning->setAlignment(Qt::AlignCenter);
	mainLayout->addWidget(warning);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	mainLayout->addLayout(buttonLayout);
	QPushButton* exit = new QPushButton("Cancel");
	buttonLayout->addWidget(exit);
	QPushButton* start = new QPushButton("Upload");
	buttonLayout->addWidget(start);

	// Connections
	(void)connect(exit, &QPushButton::clicked, this, &Uploader::hide);
	(void)connect(start, &QPushButton::clicked, this, &Uploader::initUpload);

	this->setLayout(mainLayout);
}

Uploader::~Uploader() {
	delete ipAddress;
	delete usernameEdit;
	delete passwordEdit;
	delete path;
	delete port;
}

void Uploader::open(std::string jsonData) {
	uploadSuccess = false;
	this->jsonData = jsonData;
	this->exec();
}

void Uploader::initUpload() {
	std::string output = upload();

	if (output.size() > 0) QMessageBox::critical(this, "Error Uploading File", QString(output.c_str()), QMessageBox::Ok, QMessageBox::Ok);
	else QMessageBox::information(this, "Upload Status", "Upload successful.");

	uploadSuccess = output.size() == 0;
}

std::string Uploader::upload() {
	// Create temporary file and write to it
	FILE* local;

	tmpfile_s(&local);
	if (local == NULL) return "ERROR: Could not create temporary file.";

	// Send JSON data to file
	fputs(jsonData.c_str(), local);

	// Resets pointer location to beginning of file
	rewind(local);

	std::string openRes = openConnection(ipAddress->text().toStdString(), usernameEdit->text().toStdString(), passwordEdit->text().toStdString(), port->value());
	if (openRes.size() > 0) {
		if (local) fclose(local);
		return openRes;
	}

	std::cout << "Connection established successfully." << std::endl;

	std::string sendRes = sendFile(local, path->text().toStdString());
	if (sendRes.size() > 0) {
		if (local) fclose(local);
		return sendRes;
	}

	std::cout << "File sent successfully." << std::endl;

	if (local) fclose(local);
	return "";
}

std::string Uploader::openConnection(std::string ip, std::string username, std::string password, int port) {
	using namespace net;
	// Open socket
	WSADATA data;
	int err = WSAStartup(MAKEWORD(2, 0), &data);
	if (err != 0) return "ERROR: Failed to initialize WSA";

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &sin.sin_addr);
	
	if (net::connect(sock, (struct sockaddr*) &sin, sizeof(struct sockaddr_in)) != 0) return "ERROR: Could not connect to host. Code: " + std::to_string(WSAGetLastError());

	// Init libssh2
	int result = libssh2_init(0);
	if (result != 0) return "ERROR: Failed to initialize LibSSH2.";

	session = libssh2_session_init();
	if (!session) return "ERROR: Failed to initialize LibSSH2.";

	libssh2_session_set_blocking(session, 1);

	// Handshake
	result = libssh2_session_handshake(session, sock);
	if (result) return "ERROR: Handshake failed.";

	// Authentication
	const char* fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA256);
	// Print fingerprint
	std::cout << "Host fingerprint: ";
	for (unsigned int i = 0; i < strlen(fingerprint); ++i) {
		fprintf(stdout, "%02X ", (unsigned char)fingerprint[i]);
	}
	std::cout << std::endl;

	if (libssh2_userauth_password(session, username.c_str(), password.c_str())) {
		shutdown("ERROR: Failed to authenticate.");
		return "ERROR: Failed to aauthenticate.";
	}

	return "";
}

std::string Uploader::sendFile(FILE* file, std::string path) {
	using namespace net;
	LIBSSH2_SFTP* sftpSession = libssh2_sftp_init(session);
	if (!sftpSession) {
		shutdown("ERROR: Failed to initialize SFTP.");
		return "ERROR: Failed to initialize SFTP.";
	}

	// If no path specified, put it in tmp folder
	if (path.length() == 0 || (path.length() > 0 && path.at(path.length() - 1) == '/')) path = "/tmp/NO_PATH.json";
	LIBSSH2_SFTP_HANDLE* sftpHandle = libssh2_sftp_open(sftpSession, path.c_str(), LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC,
		LIBSSH2_SFTP_S_IRUSR | LIBSSH2_SFTP_S_IWUSR |
		LIBSSH2_SFTP_S_IRGRP | LIBSSH2_SFTP_S_IROTH);
	if (!sftpHandle) {
		shutdown("ERROR: Unable to open file with SFTP.");
		return "ERROR: Unable to open file with SFTP.";
	}

	// Write file
	int rc = 1;
	char* mem = (char*)malloc(1024 * 100);
	if (mem == NULL) {
		shutdown("ERROR: malloc() failed.");
		return "ERROR: malloc() failed.";
	}

	char* ptr;
	do {
		size_t nread = fread(mem, 1, 1024 * 100, file);
		if (nread <= 0) break;
		ptr = mem;

		do {
			rc = libssh2_sftp_write(sftpHandle, ptr, nread);
			if (rc < 0) break;

			ptr += rc;
			nread -= rc;
		} while (nread);

	} while (rc > 0);
	free(mem);

	libssh2_sftp_close(sftpHandle);
	libssh2_sftp_shutdown(sftpSession);

	shutdown("Normal shutdown.");

	return "";
}

bool Uploader::uploadSucessful() {
	return uploadSuccess;
}

void Uploader::shutdown(std::string message) {
	using namespace net;
	libssh2_session_disconnect(session, message.c_str());
	libssh2_session_free(session);
	closesocket(sock);
	libssh2_exit();
	WSACleanup();
}