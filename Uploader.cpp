#pragma once

#include "Uploader.hpp"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMessageBox>

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

	QHBoxLayout* ipLayout = new QHBoxLayout();
	mainLayout->addLayout(ipLayout);
	ipLayout->addWidget(new QLabel("IP:"));
	ipLayout->addWidget(ipAddress);

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

	for (int i = 0; i < int(jsonData.size()); ++i) {
		fputs(&jsonData[i], local);
	}

	// Resets pointer location to beginning of file
	rewind(local);

	
}

bool Uploader::uploadSucessful() {
	return uploadSuccess;
}