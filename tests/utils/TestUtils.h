/**
  * Code in this file is licensed under BSD license. See LICENSE file for more information.
  */
#ifndef TESTUTILS_H
#define TESTUTILS_H
#include <QString>
/**
 * \brief Вспомогательные статичные функции для нужд тестирования.
 */
class TestUtils
{
    TestUtils();
    TestUtils(const TestUtils&);
    TestUtils &operator = (const TestUtils&);
public:
    /**
     * \brief случайно число в интервале от minimal до maximal ВКЛЮЧИТЕЛЬНО.
     */
    static short random(short maximal, short minimal = 0);
    static QString randomWord();
    static QString randomFileName(int parts =3);
    static void createRandomFile(const QString &destDir, qint64 fileSize = 0);
    static void writeFile(const QString &fileName, qint64 fileSize);
    static QString findRandomFile(const QString &srcDir);
    static void deleteRandomFile(const QString &srcDir);
    static void changeRandomFile(const QString &srcDir, qint64 fileSizeDelta);
    /**
     * \brief Очистка содержимого директории (рекурсивно). removeRoot показывает, удалить ли саму директорию.
     */
    static bool removeDir(const QString &dirName, bool removeRoot = true);
};

#endif // TESTUTILS_H
