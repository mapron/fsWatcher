/**
  * Code in this file is licensed under BSD license. See LICENSE file for more information.
  */
#include "TestUtils.h"
#include <QFile>
#include <QDir>
#include <QDebug>

short TestUtils::random(short maximal, short minimal)
{
    static unsigned short lfsr = 0xACE1u;
    static unsigned bit;
    bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    lfsr =  (lfsr >> 1) | (bit << 15);
    if (maximal < minimal) {
        std::swap(maximal, minimal);
    }
    short res= lfsr %(maximal - minimal + 1) + minimal;
    return res;
}

QString TestUtils::randomWord()
{
    short type = random(4, 0);
    QString result;
    int start=0, end=0;
    switch (type) {
        case 0: start = 'A'; end = 'Z';
            break;
        case 1: start = 'a'; end = 'z';
            break;
        case 2: start = '0'; end = '9';
            break;
        case 3: start = 0x0410; end = 0x042F;
            break;
        case 4: start = 0x0430; end = 0x043F;
            break;
    }
    short len = random(8, 3);
    for (short i = 0; i < len; i++) {
        int unicode = random(end, start);
        result += QChar(unicode);
    }
    return result;
}

QString TestUtils::randomFileName(int parts)
{
    QString filename;
    for (int i = 0; i < parts; i++) {
        filename += randomWord();
        short space = random(3, 0);
        if (space == 1) {
            filename += ' ';
        } else if (space == 2) {
            filename += '_';
        }
    }
    filename = filename.trimmed();
    if (filename.isEmpty()) {
        filename = "_";
    }
    filename += ".txt";
    return filename;
}

void TestUtils::createRandomFile(const QString &destDir, qint64 fileSize)
{
    QString fileName = destDir;
    if (!fileName.endsWith("/") && !fileName.endsWith("\\")) {
        fileName += "/";
    }
    fileName += randomFileName();
    writeFile(fileName, fileSize);

}

void TestUtils::writeFile(const QString &fileName, qint64 fileSize)
{
    QFile outFile(fileName);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "failed to create file " << fileName;
        return;
    }
    if (fileSize<=0) {
        return;
    }
    static const int megaSize =  (1024*1024);
    static QByteArray zeroTextData(megaSize, '0');
    int megaBlocks = fileSize / megaSize;
    int remain =  fileSize % megaSize;
    for (int block=0; block< megaBlocks; block++) {
        outFile.write(zeroTextData);
    }
    if (remain) {
        outFile.write(QByteArray(remain, '0'));
    }
}

QString TestUtils::findRandomFile(const QString &srcDir)
{
    QStringList files = QDir(srcDir).entryList(QStringList() << "*", QDir::Files);
    return QFileInfo(srcDir).absoluteFilePath() + "/" + files.value(random(files.size()-1));
}

void TestUtils::deleteRandomFile(const QString &srcDir)
{
    QString fileName = findRandomFile(srcDir);
    if (!fileName.isEmpty()){
        QFile::remove(fileName);
    }
}

void TestUtils::changeRandomFile(const QString &srcDir, qint64 fileSizeDelta)
{
    QString fileName = findRandomFile(srcDir);
    if (fileName.isEmpty()) {
        return;
    }
    qint64 current=QFileInfo(fileName).size();
    current += fileSizeDelta;
    writeFile(fileName, current);
}

bool TestUtils::removeDir(const QString &dirName, bool removeRoot)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        foreach (const QFileInfo &info, dir.entryInfoList(QDir::NoDotAndDotDot  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            } else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        if (removeRoot) {
            result = dir.rmdir(dirName);
        }
    }
    return result;
}
