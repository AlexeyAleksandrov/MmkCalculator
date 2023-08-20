#ifndef SPLITTER_H
#define SPLITTER_H

// автоматический define сплиттера, в зависимости от версии компилятора
// если не произошла проблема, исправь версию. Чтобы узнать версию используй QString::number(QT_VERSION, 16)
#if QT_VERSION >= 0x050f00 // версия Qt 5.15.0
#define SPLITTER Qt::SplitBehavior(Qt::SkipEmptyParts)
#else
#define SPLITTER QString::SkipEmptyParts
#endif

#endif // SPLITTER_H
