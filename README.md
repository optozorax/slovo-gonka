﻿# Slovo-Gonka
Программа для изучения английских слов.

# Описание
Программа представляет собой тест по знанию слов: пишется слово на английском языке, и 4 варианта ответа на русском. Необходимо выбрать среди этих ответов правильный перевод слова. Если вы ответили правильно, то это показыается зеленым цветом, непрвильно - красным, а правильный ответ показывется зеленым. Программа запоминает на какие слова вы ответили неправильно, на какие правильно, и подстраивается под вас, чтобы вы чаще отвечали на слова, которые плохо знаете.

# Возможности
- Все слова пишутся в файле, слова из различных языков разделяются табом, различные слова разделяются новой строкой.
	- Благодаря этому можно изучать как и целые словари, так и только ваши слова, которые вы записываете в файл.
	- При этом учитывается, что вы можете добавлять новые слова.
- Сначала вам выдаются слова, на которые вы не отвечали в программе, затем слова, на которые ответили неправильно, затем слова, на которые ответили правильно. Вся статистика ответов сохраняется в файлах.
- Имеется так же режим случайной выдачи слов, но при ответах на эти вопросы все-равно запоминается ваш ответ в файл статистики.
- Можно менять количество вариантов ответа: от 2 до 10.
- Можно поменять местами языки, для этого случая будет отдельный файл статистики, все будет аналогично.
- Вообще можно при помощи программы изучать слова на любом языке на любой другой язык. Файл со словами поддерживает юникод, так что можете писать туда хоть на китайском, хоть на французском.

# Скачать
Свежая версия всегда находится в разделе `release` на GitHub'е. Так же вместе с ней предоставляется словарь англо-русских слов на 5000 самых частоупотребимых слов.

# Как оформляется словарь
Кодировка словаря - UTF-8.

Английское слово\фраза, таб, русское слово\фраза. Следующие слова пишутся с новой строки.

Файл допустимо только дополнять или корректировать, нельзя удалять предыдущие слова, т.к. статистика сильно зависит от положения слов.

Минимальное количество слов - 15.

Максимальное количество слов - 30 тысяч.

# Компиляция
Как любая программа из библиотеки TinyWindowsGraphics.

# Copyright
Лицензия: GPL2.

Автор: Шепрут Илья.