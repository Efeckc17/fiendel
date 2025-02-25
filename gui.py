import sys
import os
from PyQt5.QtWidgets import QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, QLineEdit, QPushButton, QTextEdit, QLabel, QFileDialog, QMessageBox
from PyQt5.QtCore import QProcess, Qt
from PyQt5.QtGui import QFont

class MainWin(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Secure File Deletion")
        self.setGeometry(100, 100, 700, 500)
        self.initUI()

    def initUI(self):
        central = QWidget()
        self.setCentralWidget(central)
        layout = QVBoxLayout(central)

        title = QLabel("WARNING: PERMANENT FILE DELETION")
        title.setFont(QFont("Arial", 16, QFont.Bold))
        title.setAlignment(Qt.AlignCenter)
        title.setStyleSheet("color: red;")
        layout.addWidget(title)

        fileLayout = QHBoxLayout()
        fileLabel = QLabel("File Path:")
        self.fileEdit = QLineEdit()
        self.fileEdit.setPlaceholderText("Select a file to permanently delete...")
        browseBtn = QPushButton("Browse")
        browseBtn.clicked.connect(self.browse)
        fileLayout.addWidget(fileLabel)
        fileLayout.addWidget(self.fileEdit)
        fileLayout.addWidget(browseBtn)
        layout.addLayout(fileLayout)

        self.delBtn = QPushButton("Securely Delete File")
        self.delBtn.clicked.connect(self.runDel)
        self.delBtn.setStyleSheet("background-color: red; color: white; font-weight: bold; padding: 10px;")
        layout.addWidget(self.delBtn)

        self.outText = QTextEdit()
        self.outText.setReadOnly(True)
        self.outText.setStyleSheet("background-color: black; color: red; font-size: 18px; font-family: Consolas, monospace;")
        layout.addWidget(self.outText)

        self.proc = QProcess(self)
        self.proc.readyReadStandardOutput.connect(self.onStdOut)
        self.proc.readyReadStandardError.connect(self.onStdErr)
        self.proc.finished.connect(self.onFinished)
        self.proc.errorOccurred.connect(self.onError)

    def browse(self):
        path, _ = QFileDialog.getOpenFileName(self, "Select File")
        if path:
            self.fileEdit.setText(path)

    def runDel(self):
        path = self.fileEdit.text().strip()
        if not path:
            QMessageBox.warning(self, "Error", "Please enter a file path")
            return
        reply = QMessageBox.question(self, "Confirm Deletion", "Are you sure you want to permanently delete this file?\nAll data will be irrecoverable.", QMessageBox.Yes | QMessageBox.No)
        if reply != QMessageBox.Yes:
            return
        inp = path + "\nY\n"
        exe = "fiendel.exe" if os.name == "nt" else "./fiendel"
        self.outText.clear()
        self.outText.append("Starting secure deletion process...\n")
        self.proc.start(exe)
        if self.proc.waitForStarted(3000):
            self.proc.write(inp.encode())
        else:
            self.outText.append("Failed to start process.")

    def onStdOut(self):
        data = self.proc.readAllStandardOutput().data().decode()
        self.outText.append(data)

    def onStdErr(self):
        data = self.proc.readAllStandardError().data().decode()
        self.outText.append("Error: " + data)

    def onFinished(self):
        self.outText.append("\nSecure deletion process completed.")

    def onError(self, err):
        self.outText.append("Process error occurred: " + str(err))

if __name__ == "__main__":
    app = QApplication(sys.argv)
    win = MainWin()
    win.show()
    sys.exit(app.exec_())
