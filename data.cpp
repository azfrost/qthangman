#include "data.h"
#include <QFile>
#include <QDebug>
#include <time.h>

Data *Data::m_instance = 0;

Data::Data(QObject *parent)
    : QObject(parent)
{
    m_instance = this;
    qsrand(::time(0));
    connect(this, SIGNAL(vowelBought(QChar)), this, SLOT(registerLetterBought(QChar)));

    initWordList();
    chooseRandomWord();
}

void Data::initWordList()
{
    QFile file(":/enable1.txt");
    if (file.open(QIODevice::ReadOnly)) {
        while (!file.atEnd()) {
            QByteArray ba = file.readLine().trimmed().toUpper();
            if (!ba.isEmpty())
                m_wordList.append(QString::fromLatin1(ba));
        }
    }
}

void Data::reset()
{
    m_lettersOwned.clear();
    emit lettersOwnedChanged();
    emit errorCountChanged();
    chooseRandomWord();
}

void Data::chooseRandomWord()
{
    if (m_wordList.isEmpty())
        return;

    m_word = m_wordList.at(qrand() % m_wordList.size());
    emit wordChanged();
}

QString Data::vowels() const
{
    return QStringLiteral("AEIOU");
}

QString Data::consonants() const
{
    return QStringLiteral("BCDFGHJKLMNPQRSTVWXYZ");
}

int Data::errorCount() const
{
    int count = 0;
    foreach (QChar c, m_lettersOwned) {
        if (!m_word.contains(c))
            ++count;
    }
    return count;
}

void Data::reveal()
{
    m_lettersOwned += vowels() + consonants();
    emit lettersOwnedChanged();
    emit errorCountChanged();
}

void Data::guessWord(const QString &word)
{
    if (word.compare(m_word, Qt::CaseInsensitive) == 0) {
        m_lettersOwned += m_word.toUpper();
    } else {
        // Small hack to get an additional penalty for guessing wrong
        Q_ASSERT(i < 10);
        static int i=0;
        m_lettersOwned += QString::number(i++);
        emit errorCountChanged();
    }
    emit lettersOwnedChanged();
}

void Data::requestLetter(const QString &letterString)
{
    Q_ASSERT(letterString.size() == 1);
    QChar letter = letterString.at(0);
    if (vowels().contains(letter))
        buyVowel(letter);
    else
        registerLetterBought(letter);
}

void Data::registerLetterBought(const QChar &letter)
{
    if (m_lettersOwned.contains(letter))
        return;

    m_lettersOwned.append(letter);
    emit lettersOwnedChanged();

    if (!m_word.contains(letter))
        emit errorCountChanged();
}
